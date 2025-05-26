
CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lmysqlclient
OUT = mysql
SRC = mysql.c

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LIBS)

clean:
	rm -f $(OUT)
