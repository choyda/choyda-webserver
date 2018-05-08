all: cd_server cd_client
.PHONY: clean

cd_server: cd_server.o
    gcc -o cd_server cd_server.o -Wall

cd_server.o: cd_server.c
    gcc -c -g cd_server.c

cd_client: cd_client.o
    gcc -o cd_client cd_client.o -Wall

cd_client.o: cd_client.c
    gcc -c -g cd_client.c

clean:
    rm -rf *.o
    rm -rf cd_server
    rm -rf cd_client
