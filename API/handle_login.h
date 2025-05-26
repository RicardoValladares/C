#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define USER "admin"
#define PASS "1234"


// Variables globales compartidas
extern char current_token[65];
extern time_t token_timestamp;

// Declaración de funciones
void generar_token_hex(char *buf, size_t length);
void base64_encode(const unsigned char *input, size_t len, char *output, size_t out_size);
int handle_login(int client_sock, const char *buffer, const char *client_ip);



// Definición de variables globales
char current_token[65] = "";
time_t token_timestamp = 0;

void generar_token_hex(char *buf, size_t length) {
    const char *hex = "0123456789abcdef";
    for (size_t i = 0; i < length; i++) {
        uint8_t byte = rand() % 256;
        buf[i * 2] = hex[(byte >> 4) & 0xF];
        buf[i * 2 + 1] = hex[byte & 0xF];
    }
    buf[length * 2] = '\0';
}

void base64_encode(const unsigned char *input, size_t len, char *output, size_t out_size) {
    const char *table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t i, j = 0;
    for (i = 0; i < len && j + 4 < out_size; i += 3) {
        int val = (input[i] << 16) + ((i + 1 < len ? input[i + 1] : 0) << 8) + (i + 2 < len ? input[i + 2] : 0);
        output[j++] = table[(val >> 18) & 0x3F];
        output[j++] = table[(val >> 12) & 0x3F];
        output[j++] = (i + 1 < len) ? table[(val >> 6) & 0x3F] : '=';
        output[j++] = (i + 2 < len) ? table[val & 0x3F] : '=';
    }
    output[j] = '\0';
}

int handle_login(int client_sock, const char *buffer, const char *client_ip) {
    printf("Procesando solicitud de login desde IP: %s\n", client_ip);
    
    const char *auth = strstr(buffer, "Authorization: Basic ");
    if (!auth) {
        const char *unauth =
            "HTTP/1.1 401 Unauthorized\r\n"
            "WWW-Authenticate: Basic realm=\"Login\"\r\n"
            "Content-Length: 0\r\n\r\n";
        write(client_sock, unauth, strlen(unauth));
        printf("Intento de login fallido (sin credenciales) desde IP: %s\n", client_ip);
        return -1;
    }

    char received[128] = {0};
    sscanf(auth, "Authorization: Basic %127s", received);
    char *nl = strpbrk(received, "\r\n");
    if (nl) *nl = '\0';

    char credentials[128];
    snprintf(credentials, sizeof(credentials), "%s:%s", USER, PASS);
    char expected[128];
    base64_encode((unsigned char *)credentials, strlen(credentials), expected, sizeof(expected));

    if (strcmp(received, expected) != 0) {
        const char *unauth =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n\r\n";
        write(client_sock, unauth, strlen(unauth));
        printf("Intento de login fallido (credenciales incorrectas) desde IP: %s\n", client_ip);
        return -1;
    }

    // Generar y guardar token
    generar_token_hex(current_token, 32);
    token_timestamp = time(NULL);

    char json[256];
    snprintf(json, sizeof(json), "{\"token\": \"%s\"}", current_token);

    char response[512];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %lu\r\n\r\n%s",
        strlen(json), json);
    write(client_sock, response, strlen(response));
    
    printf("Login exitoso desde IP: %s. Token generado: %s\n", client_ip, current_token);
    return 0;
}