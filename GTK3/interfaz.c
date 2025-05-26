#include <gtk/gtk.h>
#include "interfaz.h"
#include <ctype.h>


static GtkWidget *entrada1;
static GtkWidget *entrada2;
static GtkWidget *resultado;

static void on_boton_suma_clicked(GtkButton *button, gpointer user_data) {
    const gchar *texto1 = gtk_entry_get_text(GTK_ENTRY(entrada1));
    const gchar *texto2 = gtk_entry_get_text(GTK_ENTRY(entrada2));

    if (g_strcmp0(texto1, "") == 0 || g_strcmp0(texto2, "") == 0) {
        gtk_label_set_text(GTK_LABEL(resultado), "Por favor, complete ambos campos.");
        return;
    }
    
    if (!es_numero_valido(texto1) || !es_numero_valido(texto2)) {
        gtk_label_set_text(GTK_LABEL(resultado), "Error: Ingrese solo números válidos.");
        return;
    }

    double num1 = atof(texto1);  // Cambiado de atoi a atof para decimales
    double num2 = atof(texto2);  // Cambiado de atoi a atof para decimales
    double suma = num1 + num2;

    gchar buffer[64];
    
    // Mostrar como entero si no tiene parte decimal
    if (suma == (int)suma) {
        snprintf(buffer, sizeof(buffer), "Resultado: %d", (int)suma);
    } else {
        // Mostrar con 2 decimales si es un número decimal
        snprintf(buffer, sizeof(buffer), "Resultado: %.2f", suma);
    }
    
    gtk_label_set_text(GTK_LABEL(resultado), buffer);
}

void crear_interfaz() {
    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;

    if (!gtk_builder_add_from_file(builder, "interfaz.ui", &error)) {
        g_printerr("Error cargando interfaz.ui: %s\n", error->message);
        g_clear_error(&error);
        return;
    }

    GtkWidget *ventana = GTK_WIDGET(gtk_builder_get_object(builder, "ventana_principal"));
    entrada1 = GTK_WIDGET(gtk_builder_get_object(builder, "entrada1"));
    entrada2 = GTK_WIDGET(gtk_builder_get_object(builder, "entrada2"));
    resultado = GTK_WIDGET(gtk_builder_get_object(builder, "resultado"));
    GtkWidget *boton_suma = GTK_WIDGET(gtk_builder_get_object(builder, "boton_suma"));

    if (!ventana || !entrada1 || !entrada2 || !resultado || !boton_suma) {
        g_printerr("Error: No se pudieron cargar todos los widgets desde el archivo .ui\n");
        return;
    }

    gtk_window_set_position(GTK_WINDOW(ventana), GTK_WIN_POS_CENTER);

    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(boton_suma, "clicked", G_CALLBACK(on_boton_suma_clicked), NULL);


    // Cargar estilo CSS
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    if (gtk_css_provider_load_from_path(css_provider, "estilo.css", NULL)) {
        gtk_style_context_add_provider_for_screen(
            screen,
            GTK_STYLE_PROVIDER(css_provider),
            GTK_STYLE_PROVIDER_PRIORITY_USER
        );
    } else {
        g_printerr("No se pudo cargar estilo.css\n");
    }

    gtk_widget_show_all(ventana);
}

gboolean es_numero_valido(const gchar *texto) {
    if (texto == NULL || *texto == '\0') {
        return FALSE;
    }

    int punto_decimal = 0;
    int digitos = 0;

    for (int i = 0; texto[i] != '\0'; i++) {
        if (i == 0 && (texto[i] == '-' || texto[i] == '+')) {
            continue; // Permitir signo al inicio
        }
        if (texto[i] == '.') {
            punto_decimal++;
            if (punto_decimal > 1) {
                return FALSE; // Más de un punto decimal
            }
        } else if (!isdigit(texto[i])) {
            return FALSE; // Carácter no válido
        } else {
            digitos++;
        }
    }

    return (digitos > 0); // Debe tener al menos un dígito
}
