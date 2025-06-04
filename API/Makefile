
CC = gcc
OUT = webservice
SRC = webservice.c

WC = x86_64-w64-mingw32-gcc
WOUT = webservice.exe
LIBS = -lws2_32

all: $(OUT)

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC)
	$(WC) -o $(WOUT) $(SRC) $(LIBS)


clean:
	rm -f $(OUT)
	rm -f $(WOUT)
