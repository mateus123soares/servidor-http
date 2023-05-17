ALL:
	gcc -c server.c -g 
	gcc -c client.c -g
	gcc server.o client.o -o  exec -lpthread
	./exec

clean: 
	rm exec

server:
	gcc -c server.c -g 
	gcc server.o -o  exec-server -lpthread
	./exec-server

client:
	gcc -c client.c -g 
	gcc client.o -o  exec-client -lpthread
	./exec-client