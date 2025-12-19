#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUF_SIZE 1024

int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Broadcast message to all clients */
void broadcast(char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] != sender_fd) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Write chat log with timestamp */
void write_log(char *message) {
    pthread_mutex_lock(&file_mutex);
    FILE *fp = fopen("log.txt", "a");
    time_t now = time(NULL);
    fprintf(fp, "[%s] %s", ctime(&now), message);
    fclose(fp);
    pthread_mutex_unlock(&file_mutex);
}

/* Thread function for each client */
void *client_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    char buffer[BUF_SIZE];
    int read_size;

    while ((read_size = recv(sock, buffer, BUF_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';

        write_log(buffer);
        broadcast(buffer, sock);
    }

    /* Remove client */
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] == sock) {
            client_sockets[i] = client_sockets[client_count - 1];
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(sock);
    free(socket_desc);
    return NULL;
}

int main() {
    int sockfd, newfd;
    struct sockaddr_in server, client;
    socklen_t c = sizeof(client);
    pthread_t thread_id;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    listen(sockfd, 5);

    printf("Chat Server started on port %d...\n", PORT);

    while ((newfd = accept(sockfd, (struct sockaddr *)&client, &c))) {
        printf("Accepted new client\n");

        int *new_sock = malloc(sizeof(int));
        *new_sock = newfd;

        pthread_mutex_lock(&clients_mutex);
        client_sockets[client_count++] = newfd;
        pthread_mutex_unlock(&clients_mutex);

        if (pthread_create(&thread_id, NULL, client_handler, (void *)new_sock) < 0) {
            perror("Thread error");
            return 1;
        }
    }

    close(sockfd);
    return 0;
}
