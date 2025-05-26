# Nombre del ejecutable
TARGET = suma

# Archivos fuente
SRC = main.c interfaz.c

# Flags de compilación con pkg-config para GTK+ 3
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`

# Directorio de interfaz
UI_DIR = ui

# Reglas
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)


requeriment:
	apt install gtk3-* libgtk-3-dev glade
