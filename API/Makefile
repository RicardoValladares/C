
CC = gcc
OUT = webservice
SRC = webservice.c

all: $(OUT)

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC)
	x86_64-w64-mingw32-gcc -o webservice.exe webservice.c -lws2_32


clean:
	rm -f $(OUT)
