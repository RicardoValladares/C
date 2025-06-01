# Nombre del ejecutable
TARGET = mysql

# Archivos fuente
SRC = mysql.c

# Flags de compilación con pkg-config para MySQL
CFLAGS = `pkg-config --cflags mysqlclient`
LIBS = `pkg-config --libs mysqlclient`

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
	apt install libmysqlclient-dev
