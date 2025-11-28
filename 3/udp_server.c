// udpserver_chat.c
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
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }

    printf("UDP Chat/File Server started on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&cliaddr, &len);

        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Client disconnected.\n");
            break;
        } else if (strncmp(buffer, "sendfile", 8) == 0) {
            // File transfer mode
            FILE *fp = fopen("received_from_client.txt", "w");
            if (!fp) {
                perror("File open error");
                continue;
            }
            printf("Receiving file from client...\n");

            while (1) {
                memset(buffer, 0, BUF_SIZE);
                recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&cliaddr, &len);
                if (strncmp(buffer, "EOF", 3) == 0)
                    break;
                fprintf(fp, "%s", buffer);
            }
            fclose(fp);
            printf("File received and saved as 'received_from_client.txt'\n");
        } else {
            printf("Client: %s\n", buffer);
            printf("Server: ");
            fgets(buffer, BUF_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';  // remove newline
            sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&cliaddr, len);
        }
    }

    close(sockfd);
    return 0;
}
