/* Projekt ISA - ripsniff
 * Anton Firc (xfirca00)
 * 
 * 
 * Zdroje:
 *          https://www.tcpdump.org/sniffex.c
 *          https://www.iana.org/assignments/rip-types/rip-types.xhtml#rip-types-4
 *          
 */
#ifndef RIPSNIFF
#define RIPSNIFF 

#define SUCCESS 0
#define ERR_ARGS 10
#define ERR_IP 11
#define ERR_PACKET 20
#define ERR_PCAP 50


/* Ethernet header */
struct sniff_ethernet {
        u_int8_t  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
        u_int8_t  ether_shost[ETHER_ADDR_LEN];    /* source host address */
        u_short ether_type;                     /* IP? ARP? RARP? etc */
};
#define SIZE_ETHERNET 14
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)

/* IP header */
struct sniff_ip {
        u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
        u_char  ip_tos;                 /* type of service */
        u_short ip_len;                 /* total length */
        u_short ip_id;                  /* identification */
        u_short ip_off;                 /* fragment offset field */
        #define IP_RF 0x8000            /* reserved fragment flag */
        #define IP_DF 0x4000            /* dont fragment flag */
        #define IP_MF 0x2000            /* more fragments flag */
        #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
        u_char  ip_ttl;                 /* time to live */
        u_char  ip_p;                   /* protocol */
        u_short ip_sum;                 /* checksum */
        struct  in_addr ip_src,ip_dst;  /* source and dest address */
};

/* UDP header */
struct udp_hdr {
	unsigned short	uh_sport;		/* source port */
	unsigned short	uh_dport;		/* destination port */
	short	uh_ulen;		/* udp length */
	unsigned short	uh_sum;			/* udp checksum */
};
#define SIZE_UDP_HDR 8

/* RIP header */
struct rip_hdr {
    uint8_t command;
    uint8_t version;
    uint16_t zero;
} rip_hdr;
#define SIZE_RIP_HDR 4

struct rip_v1 {
        uint16_t AFI;           // Address Family Identifier
        uint16_t zero1;         // Must be zero
        struct in_addr IP_addr; // IP Address
        uint32_t zero2;         // Must be zero
        uint32_t zero3;         // Must be zero
        uint32_t metric;        // Metric
} rip_v1;

struct rip_v2 {
        uint16_t AFI;           // Address Family Identifier
        uint16_t r_tag;         // Route Tag / Authentication Type
        struct in_addr IP_addr; // IP Address
        struct in_addr S_mask;  // Subnet Mask
        struct in_addr N_hop;   // Next Hop
        uint32_t metric;        // Metric
} rip_v2;
#define SIZE_RIP 20 //same size for RIPv1 and RIPv2

struct rip_v2_passwd {
        char password[16];
} rip_v2_passwd;

struct rip_v2_md5 {
        uint16_t pkt_len;       // RIP-2 Packet Length
        uint8_t key_ID;         // Key ID;
        uint8_t ADL;            // Auth Data Length
        uint32_t seq_num;       // Sequence Number
        uint32_t zero1;         // Must be zero
        uint32_t zero2;         // Must be zero
} rip_v2_md5;

struct sniff_ipv6 {

   union {

      struct ip6_hdrctl {
         uint32_t ip6_un1_flow; /* 4 bits version, 8 bits TC, 20 bits
                                      flow-ID */
         uint16_t ip6_un1_plen; /* payload length */
         uint8_t ip6_un1_nxt; /* next header */
         uint8_t ip6_un1_hlim; /* hop limit */
      } ip6_un1;

      uint8_t ip6_un2_vfc; /* 4 bits version, top 4 bits
                                      tclass */
   } ip6_ctlun;
   struct in6_addr ip6_src; /* source address */
   struct in6_addr ip6_dst; /* destination address */
};
#define SIZE_IPv6_HDR 40

struct rip_ng {
        struct in6_addr prefix;         // IPv6 Prefix
        uint16_t r_tag;                 // Route Tag
        uint8_t prefix_len;             // Prefix Length
        uint8_t metric;                 // Metric
} rip_ng;

#endif