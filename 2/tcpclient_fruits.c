#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[1024], fruit[30];
    int qty;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1"); // IP of server (h1)

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(1);
    }

    printf("Connected to fruit server.\n");
    printf("Enter fruit name and quantity (e.g., apple 2): ");
    scanf("%s %d", fruit, &qty);

    sprintf(buffer, "%s %d", fruit, qty);
    send(sockfd, buffer, strlen(buffer) + 1, 0);

    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("Server response:\n%s\n", buffer);

    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    close(sockfd);
    return 0;
}
