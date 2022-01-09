all: servidor cliente

servidor: servidor.o
	gcc servidor.o -o servidor

servidor.o: servidor.c
	gcc -c servidor.c 


cliente: cliente.o
	gcc cliente.o -o cliente 

cliente.o: cliente.c
	gcc -c cliente.c 

clean: 
	rm -f programa *.o servidor cliente