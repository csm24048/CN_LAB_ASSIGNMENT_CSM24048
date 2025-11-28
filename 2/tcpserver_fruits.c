#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 12345
#define MAX_FRUITS 5
#define MAX_CLIENTS 10

struct Fruit {
    char name[30];
    int quantity;
    char last_sold[64];
};

struct Client {
    char ip[INET_ADDRSTRLEN];
    int port;
};

int unique_client_count = 0;
struct Client clients[MAX_CLIENTS];

int is_new_client(char *ip, int port) {
    for (int i = 0; i < unique_client_count; i++) {
        if (strcmp(clients[i].ip, ip) == 0 && clients[i].port == port)
            return 0;
    }
    strcpy(clients[unique_client_count].ip, ip);
    clients[unique_client_count].port = port;
    unique_client_count++;
    return 1;
}

void update_time(char *buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", t);
}

int main() {
    int server_fd, new_sock;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;
    char buffer[1024];

    // Fruit database
    struct Fruit fruits[MAX_FRUITS] = {
        {"apple", 10, "N/A"},
        {"banana", 15, "N/A"},
        {"orange", 8, "N/A"},
        {"mango", 5, "N/A"},
        {"grape", 12, "N/A"}
    };

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    listen(server_fd, 5);
    printf("Server started on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(clientAddr);
        new_sock = accept(server_fd, (struct sockaddr*)&clientAddr, &addr_size);

        if (new_sock < 0) {
            perror("Accept failed");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(clientAddr.sin_port);

        printf("\nClient connected: %s:%d\n", client_ip, client_port);

        recv(new_sock, buffer, sizeof(buffer), 0);
        printf("Client request: %s\n", buffer);

        char fruit_name[30];
        int qty;
        sscanf(buffer, "%s %d", fruit_name, &qty);

        int found = 0;
        for (int i = 0; i < MAX_FRUITS; i++) {
            if (strcmp(fruits[i].name, fruit_name) == 0) {
                found = 1;
                if (fruits[i].quantity >= qty) {
                    fruits[i].quantity -= qty;
                    update_time(fruits[i].last_sold);
                    sprintf(buffer,
                            "✅ %d %s(s) sold successfully.\nRemaining stock: %d\n",
                            qty, fruit_name, fruits[i].quantity);
                    send(new_sock, buffer, strlen(buffer) + 1, 0);
                    printf("%s", buffer);
                } else {
                    sprintf(buffer, "❌ Not enough %s in stock!\n", fruit_name);
                    send(new_sock, buffer, strlen(buffer) + 1, 0);
                    printf("%s", buffer);
                }
                break;
            }
        }

        if (!found) {
            sprintf(buffer, "❌ Fruit '%s' not found.\n", fruit_name);
            send(new_sock, buffer, strlen(buffer) + 1, 0);
        }

        // Track client
        is_new_client(client_ip, client_port);

        printf("\n🧾 Customers so far:\n");
        for (int i = 0; i < unique_client_count; i++)
            printf("%d) %s:%d\n", i + 1, clients[i].ip, clients[i].port);

        sprintf(buffer, "Total unique customers: %d\n", unique_client_count);
        send(new_sock, buffer, strlen(buffer) + 1, 0);

        close(new_sock);
    }

    close(server_fd);
    return 0;
}
