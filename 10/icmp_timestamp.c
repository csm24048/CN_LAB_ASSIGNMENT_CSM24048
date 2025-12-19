#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    for (; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

int main() {
    int sock;
    struct sockaddr_in dest;
    char packet[64];

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    memset(packet, 0, sizeof(packet));

    struct icmphdr *icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_TIMESTAMP;   // Type 13
    icmp->code = 0;
    icmp->un.echo.id = getpid();
    icmp->un.echo.sequence = 1;
    icmp->checksum = checksum(packet, sizeof(packet));

    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr("8.8.8.8"); // Target IP

    sendto(sock, packet, sizeof(packet), 0,
           (struct sockaddr *)&dest, sizeof(dest));

    printf("ICMP Timestamp Request sent.\n");
    close(sock);
    return 0;
}
