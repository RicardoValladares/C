#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Headers multiplataforma
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define sockclose(s) closesocket(s)
    typedef SOCKET socket_t;
    #if !defined(INET_NTOP_AVAILABLE)
        const char *win_inet_ntop(int af, const void *src, char *dst, socklen_t size) {
            if (af == AF_INET) {
                struct sockaddr_in srcaddr;
                memset(&srcaddr, 0, sizeof(srcaddr));
                memcpy(&srcaddr.sin_addr, src, sizeof(srcaddr.sin_addr));
                srcaddr.sin_family = af;
                if (WSAAddressToStringA((struct sockaddr*)&srcaddr, sizeof(srcaddr), 
                                      0, dst, (LPDWORD)&size) != 0) {
                    return NULL;
                }
                return dst;
            }
            return NULL;
        }
        #define inet_ntop win_inet_ntop
    #endif
#else
    #include <unistd.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #define sockclose(s) close(s)
    typedef int socket_t;
    #define INVALID_SOCKET (-1)
#endif

#include "handle_login.h"
#include "handle_app.h"
#include "whitelist.h"

#define PORT 8080

int main() {
    #ifdef _WIN32
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            fprintf(stderr, "Failed to initialize Winsock\n");
            return 1;
        }
    #endif

    socket_t server_fd, client_sock;
    struct sockaddr_in addr, client_addr;
    socklen_t addr_len = sizeof(addr);
    char client_ip[INET_ADDRSTRLEN];

    srand(time(NULL));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

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
        if (client_sock == INVALID_SOCKET) {
            perror("accept");
            continue;
        }

        // Obtener IP del cliente
        const char *ip_str = inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        if (!ip_str) {
            // Fallback a inet_ntoa si inet_ntop falla
            const char *tmp = inet_ntoa(client_addr.sin_addr);
            if (tmp) {
                strncpy(client_ip, tmp, INET_ADDRSTRLEN);
            } else {
                strcpy(client_ip, "IP desconocida");
            }
        }

        // Verificar whitelist
        if (!is_ip_whitelisted(client_ip)) {
            const char *forbidden =
                "HTTP/1.1 403 Forbidden\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 20\r\n\r\n"
                "IP no autorizada";
            send(client_sock, forbidden, strlen(forbidden), 0);
            sockclose(client_sock);
            printf("Intento de conexión rechazado desde IP no autorizada: %s\n", client_ip);
            continue;
        }

        char buffer[4096];
        int n = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            sockclose(client_sock);
            continue;
        }
        buffer[n] = '\0';

        char method[8], path[256];
        sscanf(buffer, "%7s %255s", method, path);

        if (strcmp(path, "/login") == 0 && strcmp(method, "POST") == 0) {
            handle_login(client_sock, buffer, client_ip);
            sockclose(client_sock);
            continue;
        }

        if (strcmp(path, "/app") == 0 && strcmp(method, "GET") == 0) {
            handle_app(client_sock, buffer, client_ip);
            sockclose(client_sock);
            continue;
        }

        const char *not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 0\r\n\r\n";
        send(client_sock, not_found, strlen(not_found), 0);
        sockclose(client_sock);
    }

    sockclose(server_fd);
    #ifdef _WIN32
        WSACleanup();
    #endif
    return 0;
}
