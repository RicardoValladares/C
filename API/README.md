Instalar dependencia en FreeBSD:
```
sudo pkg install -g 'GhostBSD*-dev'
```

Compilar:
```
make clean
make
./webservice
```

Obtener Token:
```
curl --location --request POST 'localhost:8080/login' \
--header 'Authorization: Basic YWRtaW46MTIzNA=='
```

Peticionar con Token:
```
curl --location --request GET 'localhost:8080/app' \
--header 'Authorization: Bearer 122d450c42dd6130e4c748a9795223e197b4a305e55806e128a4375f45142257' \
--form 'nombre="Ricardo"' \
--form 'apellido="Renderos"'
```
