#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#define PCKT_LEN 8192

/* Checksum function */
unsigned short checksum(unsigned short *ptr, int nbytes) {
    long sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1)
        sum += *(unsigned char *)ptr;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main() {
    int sock;
    char packet[PCKT_LEN];
    struct iphdr *ip = (struct iphdr *)packet;
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr));
    struct sockaddr_in dest;

    char payload[] = "ROLL_NUMBER_12345";   // ← PUT YOUR ROLL NUMBER HERE

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    memset(packet, 0, PCKT_LEN);

    /* IP Header */
    ip->ihl = 5;
    ip->version = 4;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(payload));
    ip->id = htons(54321);
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    ip->saddr = inet_addr("192.168.1.10");   // source IP
    ip->daddr = inet_addr("192.168.1.20");   // destination IP
    ip->check = checksum((unsigned short *)ip, sizeof(struct iphdr));

    /* TCP Header */
    tcp->source = htons(1234);
    tcp->dest = htons(80);
    tcp->seq = htonl(1);
    tcp->doff = 5;
    tcp->syn = 1;
    tcp->window = htons(65535);

    /* Copy payload */
    memcpy(packet + sizeof(struct iphdr) + sizeof(struct tcphdr),
           payload, strlen(payload));

    /* Destination */
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = ip->daddr;

    sendto(sock, packet,
           sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(payload),
           0, (struct sockaddr *)&dest, sizeof(dest));

    printf("Custom TCP packet sent with roll number payload.\n");
    close(sock);
    return 0;
}
