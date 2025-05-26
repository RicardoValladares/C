#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "handle_login.h"
#include "handle_app.h"
#include "whitelist.h"

#define PORT 8080

int main() {
    int server_fd, client_sock;
    struct sockaddr_in addr, client_addr;
    socklen_t addr_len = sizeof(addr);
    char client_ip[INET_ADDRSTRLEN];

    srand(time(NULL));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Servidor HTTP escuchando en puerto %d\n", PORT);

    while (1) {
        addr_len = sizeof(client_addr);
        client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        // Obtener IP del cliente
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

        // Verificar whitelist
        if (!is_ip_whitelisted(client_ip)) {
            const char *forbidden =
                "HTTP/1.1 403 Forbidden\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 20\r\n\r\n"
                "IP no autorizada";
            write(client_sock, forbidden, strlen(forbidden));
            close(client_sock);
            printf("Intento de conexión rechazado desde IP no autorizada: %s\n", client_ip);
            continue;
        }

        char buffer[4096];
        int n = read(client_sock, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            close(client_sock);
            continue;
        }
        buffer[n] = '\0';

        char method[8], path[256];
        sscanf(buffer, "%7s %255s", method, path);

        //printf("Cliente: %s Servicio: %s\n", client_ip, path);

        if (strcmp(path, "/login") == 0 && strcmp(method, "POST") == 0) {
            handle_login(client_sock, buffer, client_ip);
            close(client_sock);
            continue;
        }

        if (strcmp(path, "/app") == 0 && strcmp(method, "GET") == 0) {
            handle_app(client_sock, buffer, client_ip);
            close(client_sock);
            continue;
        }

        const char *not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 0\r\n\r\n";
        write(client_sock, not_found, strlen(not_found));
        close(client_sock);
    }

    close(server_fd);
    return 0;
}