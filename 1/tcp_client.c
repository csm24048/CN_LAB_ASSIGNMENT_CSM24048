#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    printf("Client socket created successfully.\n");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1"); // Change if needed

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(1);
    }
    printf("Connected to server.\n");

    strcpy(buffer, "Hi");
    send(sockfd, buffer, strlen(buffer) + 1, 0);
    printf("Sent message: Hi\n");

    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("Server says: %s\n", buffer);

    close(sockfd);
    printf("Connection closed.\n");

    return 0;
}
