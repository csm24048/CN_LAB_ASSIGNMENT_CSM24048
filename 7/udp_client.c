#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define BUF_SIZE 1024
#define TIMEOUT_SEC 2

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUF_SIZE];
    socklen_t len = sizeof(servaddr);
    int seq = 1;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("10.0.0.1");  // server IP in Mininet

    while (1) {
        char op[10];
        double x, y;

        printf("\nOperation (sin cos + - * / inv): ");
        scanf("%s", op);
        printf("Operand 1: ");
        scanf("%lf", &x);
        printf("Operand 2 (0 if unused): ");
        scanf("%lf", &y);

        snprintf(buffer, BUF_SIZE, "%d %s %lf %lf", seq, op, x, y);
        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&servaddr, len);

        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        tv.tv_sec = TIMEOUT_SEC;
        tv.tv_usec = 0;

        int rv = select(sockfd + 1, &readfds, NULL, NULL, &tv);

        if (rv == 0) {
            printf("⚠ Packet loss detected (SEQ %d timeout)\n", seq);
        } else {
            recvfrom(sockfd, buffer, BUF_SIZE, 0,
                     (struct sockaddr *)&servaddr, &len);
            int rseq;
            double result;
            sscanf(buffer, "%d %lf", &rseq, &result);

            if (rseq == seq)
                printf("Result: %.6f\n", result);
            else
                printf("⚠ Out-of-order packet detected\n");
        }
        seq++;
    }

    close(sockfd);
    return 0;
}
