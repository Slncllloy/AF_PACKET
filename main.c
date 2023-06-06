#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <unistd.h>

#define LEN_SEGMENT 65535

struct iphdr {
    __u8    ihl:4;
    __u8    version:4;
    __u8    tos;
    __be16  tot_len;
    __be16  id;
    __be16  frag_off;
    __u8    ttl;
    __u8    protocol;
    __be16  check;
    __be32  saddr;
    __be32  daddr;
};
struct udphdr {
    __u16   source;
    __u16   dest;
    __u16   len;
    __u16   check;
};
struct tcphdr {
    __be16 source;
    __be16 dest;
    __be32 seq;
    __be32 ack_seq;
};
unsigned short iphdrlen = 0;

int EthernetPrint(unsigned char* buffer)
{
    struct ethhdr *eth = (struct ethhdr *)(buffer);

    printf("\t______________Ethernet Header________________\n");
    printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
    printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
    printf("\t|-Protocol : %d\n",eth->h_proto);
}
int IPPrint(unsigned char* buffer)
{
    struct sockaddr_in source;
    struct sockaddr_in dest;
    struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = ip->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = ip->daddr;
    
    printf("\t____________________IP____________________\n");
    printf("\t|-Version : %d\n",(unsigned int)ip->version);
    printf("\t|-Internet Header Length : %d DWORDS or %d Bytes\n",(unsigned int)ip->ihl,((unsigned int)(ip->ihl))*4);
    printf("\t|-Type Of Service : %d\n",(unsigned int)ip->tos);
    printf("\t|-Total Length : %d Bytes\n",ntohs(ip->tot_len));
    printf("\t|-Identification : %d\n",ntohs(ip->id));
    printf("\t|-Time To Live : %d\n",(unsigned int)ip->ttl);
    printf("\t|-Protocol : %d\n",(unsigned int)ip->protocol);
    printf("\t|-Header Checksum : %d\n",ntohs(ip->check));
    printf("\t|-Source IP : %s\n", inet_ntoa(source.sin_addr));
    printf("\t|-Destination IP : %s\n",inet_ntoa(dest.sin_addr));
}
int UDPPrint(unsigned char* buffer)
{
    struct iphdr *ip = (struct iphdr *)( buffer + sizeof(struct ethhdr) );
    iphdrlen = ip->ihl*4;
    struct udphdr *udp=(struct udphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
        
    printf("\t____________________UDP______________________\n");
    printf("\t|-Source Port : %d\n" , ntohs(udp->source));
    printf("\t|-Destination Port : %d\n" , ntohs(udp->dest));
    printf("\t|-UDP Length : %d\n" , ntohs(udp->len));
    printf("\t|-UDP Checksum : %d\n" , ntohs(udp->check));
}
int GetData(unsigned char* buffer,ssize_t length,unsigned short iphdrlen)
{
    unsigned char * data = (buffer + iphdrlen + sizeof(struct ethhdr) + sizeof(struct udphdr));
    int remaining_data = length - (iphdrlen + sizeof(struct ethhdr) + sizeof(struct udphdr));
    
    printf("\n");

    printf("%s\n",buffer);

    // for(int i=0;i<remaining_data;i++)
    // {
    //     if(i!=0 && i%16==0)
    //     printf("\n");
    //     printf("%.2X",data[i]);
    // }

    printf("\n");
}
int main()
{
    int socket_af = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    unsigned char *buffer = (unsigned char *) malloc(LEN_SEGMENT);
    struct sockaddr saddr;
    int saddr_len = sizeof (saddr);
    
    while(1)
    {
        ssize_t length=recvfrom(socket_af,
                                buffer,
                                LEN_SEGMENT,
                                0,
                                &saddr,
                                (socklen_t *)&saddr_len);
        EthernetPrint(buffer);
        IPPrint(buffer);
        UDPPrint(buffer);
        GetData(buffer,length,iphdrlen);

        close(socket_af);
        
    }
    
}