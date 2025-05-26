#include <stdbool.h>
#include <string.h>

// Estructura para almacenar IPs bloqueadas
typedef struct {
    const char *ip;
    bool blocked;
} DarklistEntry;

// Verifica si una IP está en la darklist (debe ser bloqueada)
bool is_ip_blocked(const char *ip);

// Lista de IPs bloqueadas
static DarklistEntry darklist[] = {
    {"192.168.1.100", true},  // Ejemplo de IP a bloquear
    {"10.0.0.99", true},      // Ejemplo de IP a bloquear
    // Añadir más IPs maliciosas según sea necesario
    {NULL, false}            // Fin de la lista
};

bool is_ip_blocked(const char *ip) {
    for (int i = 0; darklist[i].ip != NULL; i++) {
        if (darklist[i].blocked && strcmp(ip, darklist[i].ip) == 0) {
            return true;
        }
    }
    return false;
}
