/* Projekt ISA - Sniffer RIP sprav
 * Anton Firc (xfirca00)
 * 2018/2019
 * 
 * Zdroje:
 *          https://www.tcpdump.org/sniffex.c
 *          https://www.iana.org/assignments/rip-types/rip-types.xhtml#rip-types-4
 *          
 */
#include <stdio.h>
#include <pcap.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <byteswap.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<net/ethernet.h>
#include<netinet/ip.h> 
#include"myripsniffer.h"

/* Handles SIGINT */
void sig_handle (int sig) {
  printf("Packet sniffing done!\n");
  exit(SUCCESS);
}

void mac_print(const uint8_t *addr) //prints MAC address in user-friendy type
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
         addr[0] & 0xff, addr[1] & 0xff, addr[2] & 0xff,
         addr[3] & 0xff, addr[4] & 0xff, addr[5] & 0xff);
}

/*
 * Process IPv4 packets -> RIPv1/v2
 */
void process_ipv4(const u_char* packet){
    
    const struct sniff_ip *ip;                        /* The IP header */
    int size_ip, size_rip;

    /* define/compute ip header offset 
       taken from sniffex.c*/
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		printf("   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

	/* print source and destination IP addresses */
    printf("\n        IPv4| ");
	printf("SRC: %s", inet_ntoa(ip->ip_src));
	printf(",          DST: %s", inet_ntoa(ip->ip_dst));

    const struct udp_hdr *udp; // UDP header

    udp = (struct udp_hdr*)(packet + SIZE_ETHERNET + size_ip); //locate UDP header in packet
    /* print source and destination UDP port */
    printf("\n         UDP| ");
    printf("SRC Port: %d", ntohs(udp->uh_sport));
    printf(",          DST Port: %d", ntohs(udp->uh_dport));

    const struct rip_hdr *rip;

    rip = (struct rip_hdr*)(packet + SIZE_ETHERNET + size_ip + SIZE_UDP_HDR);   //locate RIP header

    /* check RIP header integrity */
    size_rip = ntohs(ip->ip_len) - (size_ip + SIZE_UDP_HDR);
    if ((size_rip-4)%20 != 0) {
        fprintf(stderr, "Error: Wrong RIP header size %d !\n", size_rip);
    }

     /* print RIP versinon and command */
    printf("\n         RIP| ");
    if (rip->command == 1) 
        printf("Command: Request(1)\n");
    else if (rip->command == 2) 
        printf("Command: Response(2)\n");
    else {
        fprintf(stderr, "Error: Unknown RIP command \"%d\"", rip->command);
        return;
    };

    /* RIPv1 */
    if (rip->version == 1) {
        printf("              Version: RIPv1(1)\n");
        const struct rip_v1 *rip_v1;
        int i = 0;
        while(i < size_rip-sizeof(struct rip_hdr)) {   // loop trough all entries
            rip_v1 = (struct rip_v1*)(packet + SIZE_ETHERNET + size_ip + SIZE_UDP_HDR + SIZE_RIP_HDR + i);
            /* print RIP entry info */
            printf("              Address Family: IP(%d)\n", ntohs(rip_v1->AFI));
            printf("              IP address: %s\n", inet_ntoa(rip_v1->IP_addr));
            printf("              Metric: %d", ntohl(rip_v1->metric));
            i+=SIZE_RIP;
        };
    }
    /* RIPv2 */
    else if (rip->version == 2) {
        printf("              Version: RIPv2(2)\n");
        const struct rip_v2 *rip_v2;
        const struct rip_v2_passwd *rip_v2_passwd;
        const struct rip_v2_md5 *rip_v2_md5;
        int i = 0;
        while(i < size_rip - SIZE_RIP_HDR) {   // loop trough all entries
            printf("\n");
            rip_v2 = (struct rip_v2*)(packet + SIZE_ETHERNET + size_ip + SIZE_UDP_HDR + SIZE_RIP_HDR + i);
            /* authentication entry */
            if (ntohs(rip_v2->AFI) == 65535 && i == 0) {
                if ((ntohs(rip_v2->r_tag) == 2)) {   // Plain-text password
                    rip_v2_passwd = (struct rip_v2_passwd*)(packet + SIZE_ETHERNET + size_ip + SIZE_UDP_HDR + SIZE_RIP_HDR + i + 4);    // 4 -> skip RIP Command and Version + and two zero bytes
                    printf("              Authentication: Simple Password(%d)\n", ntohs(rip_v2->r_tag));
                    printf("              Password: %s", rip_v2_passwd->password);
                }
                else if (ntohs(rip_v2->r_tag) == 3) {  // Cryptographic Hash Function
                    rip_v2_md5 = (struct rip_v2_md5*)(packet + SIZE_ETHERNET + size_ip + SIZE_UDP_HDR + SIZE_RIP_HDR + i + 4);      // 4 -> skip RIP Command and Version + and two zero bytes
                    printf("              Authentication: Keyed Message Digest(%d)\n", ntohs(rip_v2->r_tag));
                    printf("              Digest Offset: %d\n", ntohs(rip_v2_md5->pkt_len));
                    printf("              Key ID: %u\n", rip_v2_md5->key_ID);
                    printf("              Auth Data Len: %u\n", rip_v2_md5->ADL);
                    printf("              Seq Num: %d\n", ntohl(rip_v2_md5->seq_num));

                    rip_v2_passwd = (struct rip_v2_passwd*)(packet + SIZE_ETHERNET + size_ip + SIZE_UDP_HDR + size_rip - SIZE_RIP + 4);
                    size_rip -= SIZE_RIP;   //decrease size of packet because of already used authentication trailer
                    printf("              Authentication Data: ");
                    for (size_t i = 0; i < 16; ++i) printf("%02hhx", rip_v2_passwd->password[i]);
                    printf("\n");
                    
                }
            }
            /* info entry */
            else {
                printf("              Address Family: IP(%d)\n", ntohs(rip_v2->AFI));
                printf("              Route Tag: %d\n", ntohl(rip_v2->r_tag));
                printf("              IP address: %s\n", inet_ntoa(rip_v2->IP_addr));
                printf("              Netmask: %s\n", inet_ntoa(rip_v2->S_mask));
                printf("              Next Hop: %s\n", inet_ntoa(rip_v2->N_hop));
                printf("              Metric: %d\n", ntohl(rip_v2->metric));
            }
            i+=SIZE_RIP; // move to next RIP entry
        };
    }
    else {
        fprintf(stderr, "Error: Unknown RIP version \"%d\"", rip->version);
    };         

}

/*
 * Process IPv6 packet - RIPng
 */
void process_ipv6(const u_char* packet){

    const struct sniff_ipv6 *ip;                        /* The IP header */
    char addr_src[INET6_ADDRSTRLEN];
    char addr_dst[INET6_ADDRSTRLEN];
    int size_rip;

    ip = (struct sniff_ipv6*)(packet + SIZE_ETHERNET);
    

    /* print source and destination IP addresses */
    printf("\n        IPv6| ");
    inet_ntop(AF_INET6, &ip->ip6_src, addr_src, INET6_ADDRSTRLEN);
	printf("SRC: %-30s", addr_src);
    inet_ntop(AF_INET6, &ip->ip6_dst, addr_dst, INET6_ADDRSTRLEN);
	printf("DST: %-30s", addr_dst);

    const struct udp_hdr *udp; // UDP header

    udp = (struct udp_hdr*)(packet + SIZE_ETHERNET + SIZE_IPv6_HDR); //locate UDP header
    /* print UDP source and destination port */
    printf("\n         UDP| ");
    printf("SRC Port: %d", ntohs(udp->uh_sport));
    printf(",          DST Port: %d", ntohs(udp->uh_dport));

    /* check RIP header integrity */
    size_rip = ntohs(ip->ip6_ctlun.ip6_un1.ip6_un1_plen) - (SIZE_UDP_HDR);
    if ((size_rip-4)%20 != 0) {
        fprintf(stderr, "Error: Wrong RIP header size %d !\n", size_rip);
    }

    const struct rip_hdr *rip;

    rip = (struct rip_hdr*)(packet + SIZE_ETHERNET + SIZE_IPv6_HDR + SIZE_UDP_HDR); // locate RIP header
    /* print RIP command */
    printf("\n         RIP| ");
    if (rip->command == 1) 
        printf("Command: Request(1)\n");
    else if (rip->command == 2) 
        printf("Command: Response(2)\n");
    else {
        fprintf(stderr, "Error: Unknown RIP command \"%d\"", rip->command);
        return;
    };
    /* check and print RIP version */
    if (rip->version != 1) {
        fprintf(stderr, "Error: Unknown RIP version \"%d\"", rip->version);
        return;
    }
    printf("              Version: RIPng(1)\n");

    const struct rip_ng *rip_ng;
    char prefix[INET6_ADDRSTRLEN];

    int i = 0;
    while(i < size_rip-SIZE_RIP_HDR) { //loop trough all RIP entries
        rip_ng = (struct rip_ng*)(packet + SIZE_ETHERNET + SIZE_IPv6_HDR + SIZE_UDP_HDR + SIZE_RIP_HDR + i);
        inet_ntop(AF_INET6, &rip_ng->prefix, prefix, INET6_ADDRSTRLEN);
        printf("                 IPv6 Prefix: %-30s", prefix);
        printf("         Route Tag: 0x%04X", ntohs(rip_ng->r_tag));
        printf("         Prefix Length: %-4u", rip_ng->prefix_len);
        printf("         Metric: %u\n", rip_ng->metric);
        i+=SIZE_RIP;
    }
    
    


}

/*
 * Callback function for pcap_loop
 * determines IP version of packet and forwards it to further processing
 */
void callback(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet)
{
    static int count = 1;
    printf("\nPacket number %d:\n", count++);
	
	/* declare pointers to packet headers 
       taken from sniffex.c*/
	const struct ether_header *ethernet;  /* The ethernet header [1] */
	const struct sniff_ip *ip;              /* The IP header */
	const struct sniff_tcp *tcp;            /* The TCP header */
	const char *payload;                    /* Packet payload */

	int size_ip;
	int size_tcp;
	int size_payload;
	
	/* define ethernet header */
	ethernet = (struct ether_header*)(packet);
    /* print source and destination MAC adresses */
    printf("    Ethernet| SRC: ");
    mac_print(ethernet->ether_shost);
    printf(", DST: ");
    mac_print(ethernet->ether_dhost);
    
    /* determine IP version of packet */
    if (ntohs(ethernet->ether_type) == ETHERTYPE_IP) {
        process_ipv4(packet);
    }
    else if (ntohs(ethernet->ether_type) == ETHERTYPE_IPV6) {
        process_ipv6(packet);
    }
    else {
        fprintf(stderr, "Error: Unknown IP type !\n");
    }
    
}

int main(int argc, char** argv) {

    signal (SIGINT, sig_handle);

    if (argc != 3) {                //wrong argument count
        fprintf(stderr, "Error: Wrong argument count!\n");
        fprintf(stderr, "Try %s -i device_name\n", argv[0]);
        return ERR_ARGS;
    }
    if (strcmp(argv[1], "-i")) { // something else than -i used
        fprintf(stderr, "Error: Unknown argument %s, did you mean -i ?\n", argv[1]);
        return ERR_ARGS;
    }
    for (int i = 0; i < strlen(argv[2]); i++) { //check if device name contains alphanumeric characters only
        if (!isalnum(argv[2][i])) {
            fprintf(stderr, "Error: Device name may contain only alphanumeric characters!\n");
            return ERR_ARGS;
            }
    }

    char *dev = argv[2];    // device name
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr;                  // device descriptor
    struct bpf_program fp;		  /* compiled program*/
    bpf_u_int32 pMask;            /* subnet mask */
    bpf_u_int32 pNet;             /* ip address*/
    int i =0;

    // fetch the network address and network mask
    pcap_lookupnet(dev, &pNet, &pMask, errbuf);

    // Now, open device for sniffing
    descr = pcap_open_live(dev, BUFSIZ, 0,-1, errbuf);
    if(descr == NULL)
    {
        printf("pcap_open_live() failed due to [%s]\n", errbuf);
        return ERR_PCAP;
    }

    printf("Sniffing started on device \"%s\"\n", dev);

    // Compile the filter expression
    if(pcap_compile(descr, &fp, "udp portrange 520-521", 0, pNet) == -1)
    {
        printf("\npcap_compile() failed\n");
        return ERR_PCAP;
    }

    // Set the filter compiled above
    if(pcap_setfilter(descr, &fp) == -1)
    {
        printf("\npcap_setfilter() failed\n");
        return ERR_PCAP;
    }

    // For every packet received, call the callback function
    // Maximum limit on packet not set
    pcap_loop(descr, 0, callback, NULL);

    printf("\nDone with packet sniffing!\n"); //TODO unreachable !

    return SUCCESS;
}