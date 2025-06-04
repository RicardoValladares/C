#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definiciones multiplataforma
#ifdef _WIN32
    #include <winsock2.h>
    #define sock_write(sock, buf, len) send((sock), (buf), (len), 0)
#else
    #include <unistd.h>
    #define sock_write(sock, buf, len) write((sock), (buf), (len))
#endif

#define TOKEN_EXPIRATION_SECONDS 60

// Variables globales (definidas en otro archivo)
extern char current_token[65];
extern time_t token_timestamp;

int handle_app(int client_sock, const char *buffer, const char *client_ip) {
    printf("Procesando solicitud a /app desde IP: %s\n", client_ip);
    
    const char *auth = strstr(buffer, "Authorization: Bearer ");
    if (!auth) {
        const char *unauth =
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "Content-Length: 23\r\n\r\n"
            "{\"error\":\"Unauthorized\"}";
        sock_write(client_sock, unauth, strlen(unauth));
        printf("Acceso denegado a /app - Falta token (IP: %s)\n", client_ip);
        return -1;
    }

    char token[128] = {0};
    sscanf(auth, "Authorization: Bearer %127s", token);
    char *nl = strpbrk(token, "\r\n");
    if (nl) *nl = '\0';

    // Verificar token y expiración
    time_t ahora = time(NULL);
    if (strcmp(token, current_token) != 0) {
        const char *forbidden =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "Content-Length: 28\r\n\r\n"
            "{\"error\":\"Invalid token\"}";
        sock_write(client_sock, forbidden, strlen(forbidden));
        printf("Acceso denegado - Token inválido (IP: %s)\n", client_ip);
        return -1;
    }

    if (ahora - token_timestamp > TOKEN_EXPIRATION_SECONDS) {
        const char *forbidden =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "Content-Length: 29\r\n\r\n"
            "{\"error\":\"Token expired\"}";
        sock_write(client_sock, forbidden, strlen(forbidden));
        printf("Acceso denegado - Token expirado (IP: %s)\n", client_ip);
        return -1;
    }

    // Procesar parámetros
    char nombre[128] = "No especificado";
    char apellido[128] = "No especificado";
    
    // Buscar parámetros GET
    const char *url_params = strstr(buffer, "GET /app?");
    if (url_params) {
        char *question_mark = strchr(url_params, '?');
        if (question_mark) {
            sscanf(question_mark, "?nombre=%127[^&]&apellido=%127[^ \r\n]", nombre, apellido);
            // Aquí deberías implementar urldecode() para parámetros URL-encoded
        }
    }
    
    // Buscar parámetros POST (form-data)
    const char *content_type = strstr(buffer, "Content-Type: multipart/form-data");
    if (content_type) {
        const char *nombre_start = strstr(buffer, "name=\"nombre\"");
        const char *apellido_start = strstr(buffer, "name=\"apellido\"");
        
        if (nombre_start) {
            const char *value_start = strstr(nombre_start, "\r\n\r\n");
            if (value_start) {
                value_start += 4;
                const char *value_end = strstr(value_start, "\r\n");
                if (value_end) {
                    int len = value_end - value_start;
                    strncpy(nombre, value_start, len < 127 ? len : 127);
                    nombre[len < 127 ? len : 127] = '\0';
                }
            }
        }
        
        if (apellido_start) {
            const char *value_start = strstr(apellido_start, "\r\n\r\n");
            if (value_start) {
                value_start += 4;
                const char *value_end = strstr(value_start, "\r\n");
                if (value_end) {
                    int len = value_end - value_start;
                    strncpy(apellido, value_start, len < 127 ? len : 127);
                    apellido[len < 127 ? len : 127] = '\0';
                }
            }
        }
    }

    printf("Datos recibidos - Nombre: %s, Apellido: %s\n", nombre, apellido);

    // Construir respuesta JSON
    char json[512];
    snprintf(json, sizeof(json), 
        "{\"status\":\"success\",\"message\":\"Acceso concedido\","
        "\"data\":{\"nombre\":\"%s\",\"apellido\":\"%s\"}}", 
        nombre, apellido);

    char response[1024];
    int content_length = strlen(json);
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Connection: close\r\n"
        "Content-Length: %d\r\n\r\n"
        "%s", content_length, json);

    int bytes_sent = sock_write(client_sock, response, strlen(response));
    if (bytes_sent < 0) {
        perror("Error al enviar respuesta");
    } else {
        printf("Respuesta enviada (%d bytes)\n", bytes_sent);
    }
    
    printf("Acceso concedido a /app (IP: %s)\n", client_ip);
    return 0;
}
