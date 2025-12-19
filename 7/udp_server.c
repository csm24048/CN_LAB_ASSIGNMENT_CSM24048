#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

double calculate(char *op, double x, double y) {
    if (strcmp(op, "sin") == 0) return sin(x);
    if (strcmp(op, "cos") == 0) return cos(x);
    if (strcmp(op, "tan") == 0) return tan(x);
    if (strcmp(op, "inv") == 0) return 1.0 / x;
    if (strcmp(op, "+") == 0) return x + y;
    if (strcmp(op, "-") == 0) return x - y;
    if (strcmp(op, "*") == 0) return x * y;
    if (strcmp(op, "/") == 0) return y != 0 ? x / y : NAN;
    return NAN;
}

int main() {
    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("UDP Scientific Calculator Server running...\n");

    while (1) {
        int seq;
        char op[10];
        double x, y;

        recvfrom(sockfd, buffer, BUF_SIZE, 0,
                 (struct sockaddr *)&cliaddr, &len);

        sscanf(buffer, "%d %s %lf %lf", &seq, op, &x, &y);

        double result = calculate(op, x, y);

        snprintf(buffer, BUF_SIZE, "%d %.6f", seq, result);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}
