Instalar dependencia en FreeBSD:
```
sudo pkg install mysql80-client
```

Compilar:
```
make clean
make
./mysql "SELECT NOW()"
```
