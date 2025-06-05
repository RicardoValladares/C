
CC = gcc
OUT = webservice
SRC = webservice.c

WC = x86_64-w64-mingw32-gcc
WOUT = webservice.exe
LIBS = -lws2_32

all: $(OUT) winbuild

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC)

winbuild:
	@if [ "$(shell uname)" = "Linux" ]; then \
		$(WC) -o $(WOUT) $(SRC) $(LIBS); \
	fi

run: 
	./$(OUT)

clean:
	rm -f $(OUT) $(WOUT)
