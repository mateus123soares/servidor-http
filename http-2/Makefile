ALL:
	gcc -c server.c -g 
	gcc -c client.c -g
	gcc -c init-server.c -g
	gcc server.o client.o init-server.o -o  exec -lpthread
	./exec

clean: 
	rm exec

server:
	gcc -c init-server.c -g
	gcc -c server.c -g 
	gcc server.o init-server.o -o exec -lpthread
	./exec

client:
	gcc -c client.c -g 
	gcc client.o -o  exec-client -lpthread
	./exec-client