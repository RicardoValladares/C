#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>  // Para access()
#include <mysql/mysql.h>

// gcc select_now.c -o select_now -lmysqlclient

void imprimir_resultado(MYSQL_RES *res) {
    int num_fields = mysql_num_fields(res);
    MYSQL_ROW row;
    MYSQL_FIELD *fields = mysql_fetch_fields(res);

    for (int i = 0; i < num_fields; i++) {
        printf("%-20s", fields[i].name);
    }
    printf("\n");

    for (int i = 0; i < num_fields; i++) {
        printf("--------------------");
    }
    printf("\n");

    while ((row = mysql_fetch_row(res))) {
        for (int i = 0; i < num_fields; i++) {
            printf("%-20s", row[i] ? row[i] : "NULL");
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s \"consulta SQL\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    char query[4096] = {0};
    for (int i = 1; i < argc; ++i) {
        strcat(query, argv[i]);
        if (i < argc - 1) strcat(query, " ");
    }

    // Credenciales de fallback codificadas (editar según tu entorno)
    const char *fallback_user = "root";
    const char *fallback_pass = "123456";
    const char *fallback_db   = "mysql";
    const char *fallback_host = "127.0.0.1";

    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "mysql_init() falló\n");
        return EXIT_FAILURE;
    }

    int conectado = 0;

    // Intento 1: configuración por archivo .cnf
    if (access("db_config.cnf", R_OK) == 0) {
        mysql_options(conn, MYSQL_READ_DEFAULT_FILE, "db_config.cnf");
        mysql_options(conn, MYSQL_READ_DEFAULT_GROUP, "client");

        if (mysql_real_connect(conn, NULL, NULL, NULL, NULL, 0, NULL, 0)) {
            conectado = 1;
        } else {
            fprintf(stderr, "mysql_real_connect() falló con .cnf: %s\n", mysql_error(conn));
        }
    }

    // Intento 2: conexión codificada en el código
    if (!conectado) {
        fprintf(stderr, "Intentando conexión directa codificada...\n");

        conn = mysql_init(NULL); // Re-inicializar
        if (!conn) {
            fprintf(stderr, "mysql_init() falló\n");
            return EXIT_FAILURE;
        }

        if (mysql_real_connect(conn, fallback_host, fallback_user, fallback_pass, fallback_db, 0, NULL, 0)) {
            conectado = 1;
        } else {
            fprintf(stderr, "Conexión directa falló: %s\n", mysql_error(conn));
            mysql_close(conn);
            return EXIT_FAILURE;
        }
    }

    // Ejecutar consulta
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Error en consulta: %s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res) {
        imprimir_resultado(res);
        mysql_free_result(res);
    } else {
        if (mysql_field_count(conn) == 0) {
            printf("Consulta ejecutada correctamente. Filas afectadas: %" PRIu64 "\n",
                   (uint64_t) mysql_affected_rows(conn));
        } else {
            fprintf(stderr, "Error al obtener resultado: %s\n", mysql_error(conn));
        }
    }

    mysql_close(conn);
    return EXIT_SUCCESS;
}
