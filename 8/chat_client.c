#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

int sock;

/* Receive messages from server */
void *receive_handler(void *arg) {
    char buffer[BUF_SIZE];
    int read_size;

    while ((read_size = recv(sock, buffer, BUF_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("\n%s", buffer);
        fflush(stdout);
    }
    return NULL;
}

int main() {
    struct sockaddr_in server;
    pthread_t recv_thread;
    char message[BUF_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&server, sizeof(server));

    printf("Connected to chat server\n");

    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    while (fgets(message, BUF_SIZE, stdin)) {
        send(sock, message, strlen(message), 0);
    }

    close(sock);
    return 0;
}
