#include <stdbool.h>
#include <string.h>

// Estructura para almacenar IPs autorizadas
typedef struct {
    const char *ip;
    bool enabled;
} WhitelistEntry;

// Verifica si una IP está en la whitelist
bool is_ip_whitelisted(const char *ip);


// Lista de IPs autorizadas
static WhitelistEntry whitelist[] = {
    {"127.0.0.1", true},    // Localhost
    {"192.168.1.1", true},  // Ejemplo de IP local
    {"10.0.0.5", true},     // Ejemplo de IP privada
    // Añadir más IPs según sea necesario
    {NULL, false}           // Fin de la lista
};

bool is_ip_whitelisted(const char *ip) {
    for (int i = 0; whitelist[i].ip != NULL; i++) {
        if (whitelist[i].enabled && strcmp(ip, whitelist[i].ip) == 0) {
            return true;
        }
    }
    return false;
}