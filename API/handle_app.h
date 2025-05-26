#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOKEN_EXPIRATION_SECONDS 60

// Declaración de función
int handle_app(int client_sock, const char *buffer, const char *client_ip);


int handle_app(int client_sock, const char *buffer, const char *client_ip) {
    printf("Procesando solicitud a /app desde IP: %s\n", client_ip);
    
    const char *auth = strstr(buffer, "Authorization: Bearer ");
    if (!auth) {
        const char *unauth =
            "HTTP/1.1 401 Unauthorized\r\n"
            "Content-Length: 0\r\n\r\n";
        write(client_sock, unauth, strlen(unauth));
        printf("Acceso denegado a /app - Falta token de autorización (IP: %s)\n", client_ip);
        return -1;
    }

    char token[128];
    sscanf(auth, "Authorization: Bearer %127s", token);
    char *nl = strpbrk(token, "\r\n");
    if (nl) *nl = '\0';

    // Verificar token y expiración
    time_t ahora = time(NULL);
    if (strcmp(token, current_token) != 0) {
        const char *unauth =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n\r\n";
        write(client_sock, unauth, strlen(unauth));
        printf("Acceso denegado a /app - Token inválido (IP: %s, Token recibido: %s)\n", client_ip, token);
        return -1;
    }

    if (ahora - token_timestamp > TOKEN_EXPIRATION_SECONDS) {
        const char *unauth =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n\r\n";
        write(client_sock, unauth, strlen(unauth));
        printf("Acceso denegado a /app - Token expirado (IP: %s, Token generado hace: %ld segundos)\n", 
               client_ip, (ahora - token_timestamp));
        return -1;
    }

    // Procesar parámetros GET
    char nombre[128] = "No especificado";
    char apellido[128] = "No especificado";
    
    // Buscar parámetros en URL (GET)
    const char *url_params = strstr(buffer, "GET /app?");
    if (url_params) {
        char *question_mark = strchr(url_params, '?');
        if (question_mark) {
            sscanf(question_mark, "?nombre=%127[^&]&apellido=%127[^ \r\n]", nombre, apellido);
            // Decodificar caracteres especiales (ej. %20 -> espacio)
            // Implementar urldecode(nombre) y urldecode(apellido) si es necesario
        }
    }
    
    // Buscar parámetros en form-data (POST)
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
                    if (len > 0 && len < 127) {
                        strncpy(nombre, value_start, len);
                        nombre[len] = '\0';
                    }
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
                    if (len > 0 && len < 127) {
                        strncpy(apellido, value_start, len);
                        apellido[len] = '\0';
                    }
                }
            }
        }
    }

    printf("Datos recibidos - Nombre: %s, Apellido: %s\n", nombre, apellido);

    // Construir respuesta JSON con los parámetros recibidos
    char json[512];
    snprintf(json, sizeof(json), 
        "{\"status\": \"ok\", \"message\": \"Acceso concedido a /app\", "
        "\"datos\": {\"nombre\": \"%s\", \"apellido\": \"%s\"}}", 
        nombre, apellido);

    char response[1024];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %lu\r\n\r\n%s",
        strlen(json), json);
    write(client_sock, response, strlen(response));
    
    printf("Acceso concedido a /app (IP: %s, Token válido)\n", client_ip);
    return 0;
}