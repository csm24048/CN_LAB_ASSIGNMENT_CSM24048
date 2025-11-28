// udpclient_chat.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUF_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("10.0.0.1"); // Server IP (h1)

    printf("Connected to UDP Chat/File Server.\n");
    printf("Type messages to chat.\n");
    printf("Type 'sendfile' to send a file.\n");
    printf("Type 'exit' to quit.\n");

    while (1) {
        printf("You: ");
        fgets(buffer, BUF_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline

        sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&servaddr, len);

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        if (strncmp(buffer, "sendfile", 8) == 0) {
            FILE *fp = fopen("file_to_send.txt", "r");
            if (!fp) {
                perror("File not found");
                continue;
            }

            printf("Sending file to server...\n");
            while (fgets(buffer, BUF_SIZE, fp) != NULL) {
                sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&servaddr, len);
                usleep(100000); // small delay
            }
            sendto(sockfd, "EOF", 3, 0, (struct sockaddr*)&servaddr, len);
            fclose(fp);
            printf("File sent successfully.\n");
        } else {
            memset(buffer, 0, BUF_SIZE);
            recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&servaddr, &len);
            printf("Server: %s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}
