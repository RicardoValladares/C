# Nombre del ejecutable
TARGET = suma
TARGETWIN = suma.exe

# Archivos fuente
SRC = main.c interfaz.c

# Flags de compilación con pkg-config para GTK+ 3
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`

# Interfaz grafica
UI_DIR = ui

MXE_COMPILER = ./mxe/usr/bin/x86_64-w64-mingw32.static-gcc
MXE_CFLAGS = `./mxe/usr/bin/x86_64-w64-mingw32.static-pkg-config --cflags --libs gtk+-3.0`

# Reglas
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)
	@if [ ! -f "$(MXE_COMPILER)" ]; then \
        cd mxe && make gtk3 -j 8 MXE_TARGETS='x86_64-w64-mingw32.static' && cd ..; \
        exit 1; \
    fi
	$(MXE_COMPILER) -o $(TARGETWIN) $(SRC) $(MXE_CFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(TARGETWIN)


requeriment:
	apt install gtk3-* libgtk-3-dev glade
