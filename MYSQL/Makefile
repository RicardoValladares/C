# Nombre del ejecutable
TARGET = mysql
TARGETWIN = mysql.exe

# Archivos fuente
SRC = mysql.c

# Flags de compilación con pkg-config para MySQL
CFLAGS = `pkg-config --cflags mysqlclient`
LIBS = `pkg-config --libs mysqlclient`

# Directorio de interfaz
UI_DIR = ui

MING_COMPILER = x86_64-w64-mingw32-gcc
MING_CFLAGS = -I./mysql-windows/mysql-connector-c-6.1.11-winx64/include   -L./mysql-windows/mysql-connector-c-6.1.11-winx64/lib   -lmysql -lws2_32

# Reglas
all: $(TARGET) winbuild

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

winbuild:
	@if [ "$(shell uname)" = "Linux" ]; then \
		sed -i 's/#include <mysql\/mysql.h>/#include <mysql.h>/' $(SRC); \
		$(MING_COMPILER) -o $(TARGETWIN) $(SRC) $(MING_CFLAGS); \
		sed -i 's/#include <mysql.h>/#include <mysql\/mysql.h>/' $(SRC); \
	fi

run: 
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(TARGETWIN)


requeriment:
	apt install libmysqlclient-dev


