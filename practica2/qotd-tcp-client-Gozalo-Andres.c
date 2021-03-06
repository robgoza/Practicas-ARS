//Practica tema 6, Gozalo Andres Roberto

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <string.h>
//capacidad del buffer
#define BUFFERSIZE 512 

int main (int argc, char *argv[]){
	char bfr[BUFFERSIZE];
	//Creacion de la estructura relacionada con el servidor
	struct sockaddr_in server;
	server.sin_family=AF_INET;

	//transformacion de la direccion IP recibida a network byte order	
        inet_aton(argv[1], &server.sin_addr);
	if (argc==2) {
		printf("No se ha especificado el puerto, se empleara el asociado a qotd\n");
		//obtencion del puerto por defecto para qotd
		struct servent *serv;
		serv=getservbyname("qotd","tcp");
		server.sin_port = serv-> s_port;
		if (serv == NULL){
			printf("Ha habido un error al obtener el puerto\n");
			perror("serv");
			exit(EXIT_FAILURE);
		}
	}
	if (argc==4 && (strcmp("-p",argv[2])==0)){
		int puerto;
		sscanf(argv[3],"%d", &puerto);
		server.sin_port = htons(puerto);
	}

	int s=socket(AF_INET, SOCK_STREAM, 0);

	if (s<0){
		printf("Fallo al crear el socket \n");
		perror("socket()");
		exit (EXIT_FAILURE);
	}

	//Creacion de la estructura relacionada con el cliente
	struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_port=0;
	client.sin_addr.s_addr= INADDR_ANY;
	int reci;

	//Comprobacion de los diferentes errores a la hora de la conexion
	int erro;
	erro = bind(s,(struct sockaddr*)&client,sizeof(client));
	if (erro<0){
		printf("Fallo en bind\n");
		perror("bind()");
		exit (EXIT_FAILURE);
	}
	int fallo;
	fallo = connect(s,(struct sockaddr*)&server,sizeof(server));
	if (fallo<0){
		printf("Fallo en conect\n");
		perror("conect()");
		exit (EXIT_FAILURE);
	}
	char *mensaje = "mensaje";
	if(send(s, mensaje,sizeof(mensaje),0)<0){
		printf("Fallo al enviar el mensaje\n");
		perror("send()");
		exit(EXIT_FAILURE);
	}
	//Conexion con el servidor
	reci =recv(s,bfr, BUFFERSIZE, 0); 
	if(reci<0){
		perror("recv");
		exit(EXIT_FAILURE);
	}

	//Imprimimos la cita
	printf("%s",bfr);
	shutdown(s,SHUT_RDWR);
	reci =recv(s,bfr, BUFFERSIZE, 0); 
	if(reci!=0){
		perror("recv");
		exit(EXIT_FAILURE);
	}
	exit (EXIT_SUCCESS);
	close(s);
	return 0;
}

 	
