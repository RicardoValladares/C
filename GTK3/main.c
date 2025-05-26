#include <gtk/gtk.h>
#include "interfaz.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    crear_interfaz();
    gtk_main();
    return 0;
}
