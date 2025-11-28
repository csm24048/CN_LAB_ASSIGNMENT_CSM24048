#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    printf("Server socket created successfully.\n");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }
    printf("Binding successful. Listening on port %d...\n", PORT);

    if (listen(sockfd, 1) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(1);
    }

    addr_size = sizeof(clientAddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &addr_size);
    if (newsockfd < 0) {
        perror("Accept failed");
        close(sockfd);
        exit(1);
    }
    printf("Client connected.\n");

    recv(newsockfd, buffer, sizeof(buffer), 0);
    printf("Client says: %s\n", buffer);

    strcpy(buffer, "Hello");
    send(newsockfd, buffer, strlen(buffer) + 1, 0);
    printf("Sent reply: Hello\n");

    close(newsockfd);
    close(sockfd);
    printf("Connection closed.\n");

    return 0;
}
