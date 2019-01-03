//Practica tema 6 Gozalo Andres Roberto

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>

//capacidad maxima de buffer
#define BUFFERSIZE 512
#define MAXLENGTH 100

int s;
//manejador para las interrupciones
void signalHandler(int signal){
	if(signal==SIGINT){
		shutdown(s,2);
		exit(0);
	}
}


int main (int argc, char* argv[]){
	signal(SIGINT, signalHandler);
	struct sockaddr_in client;
	char resultado[BUFFERSIZE];
	int sent_bytes;
	int liste_bytes;
	int acep;
	socklen_t longitud;
	int pid;
	if (argc == 1){
		struct servent* serv;
		serv = getservbyname("qotd","tcp");
		if (serv == NULL){
			printf("Error encontrando el servicio\n");
			exit(EXIT_FAILURE);
		}
		client.sin_port = serv->s_port;
	}
	else if (argc == 3){
		int prt;
		sscanf(argv[2], "%d", &prt);
		client.sin_port = htons(prt);
	}
	//apertura del socket
	s = socket(AF_INET, SOCK_STREAM,0);
	if (s<0){
		perror("socket()");
		exit(EXIT_FAILURE);
	}
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(s,(struct sockaddr*) &client,sizeof(client))<0){
		perror("bind()");
		exit(EXIT_FAILURE);
	}
	char hostname [HOST_NAME_MAX];
	if(gethostname(hostname, HOST_NAME_MAX)<0){
		perror("gethostname()");
		exit(EXIT_FAILURE);
	}
	liste_bytes = listen(s,5);
	if(liste_bytes<0){
		perror("listen()");
		exit(EXIT_FAILURE);
	}
	longitud=(socklen_t)sizeof(client);

	//Bucle infinito para procesar las solicitudes
	while(1){
		memset(resultado, 0,sizeof(resultado));
		acep = accept(s,(struct sockaddr*) &client,
				&longitud);
		if(acep<0){
			perror("accept()");
			exit(EXIT_FAILURE);
		}
		//Creacion de un proceso para procesar la solicitud
		pid = fork();
		if(pid<0){
			perror("fork()");
			exit(EXIT_FAILURE);
		} else if(pid == 0){
			if(close(s)<0){
				perror("close()");
				exit(EXIT_FAILURE);
			}

	//impresion de la cita qotd
	char buffQuote [MAXLENGTH];
	memset(buffQuote, 0,sizeof(buffQuote));
	system("/usr/games/fortune -s > /tmp/tt.txt");
	FILE *fich = fopen("/tmp/tt.txt","r");
	int nc = 0;
	do{
		buffQuote[nc++] = fgetc(fich);
	}while(nc < MAXLENGTH-1);
	fclose(fich);
	char *frase="Quote of the day from vm2518:\n";
	strcat(resultado,frase);
	strcat(resultado,buffQuote);
	resultado[nc+1]='\0';
	int capacidad = strlen(resultado);

	//envio de la respuesta
	sent_bytes = send(acep,resultado,capacidad,0);
	if(sent_bytes<0){
		perror("sendto()");
		exit(EXIT_FAILURE);
	}
	int recv_bytes;
	recv_bytes = recv(acep,resultado,capacidad,0);
		if(recv_bytes<0){
			perror("recv()");
			close(acep);
			exit(EXIT_FAILURE);
		}
	if(shutdown(acep, SHUT_RDWR)<0){
		perror("shutdown()");
		close(acep);
		exit(EXIT_FAILURE);
	}
	if(close(acep)<0){
		perror("close()");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
	}
	else{
		if(close(acep)<0){
			perror("close()");
			exit(EXIT_FAILURE);
			}
		}
	}
	exit(EXIT_SUCCESS);
}




