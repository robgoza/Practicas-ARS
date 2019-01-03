//Practica tema 6 Gozalo Andres Roberto

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

//capacidad maxima de buffer
#define BUFFERSIZE 512
#define MAXLENGTH 100
//capacidad maxima mensaje
#define CAPACIDAD 130

int main (int argc, char* argv[]){
	struct sockaddr_in server;
	struct sockaddr_in client;
	char bfr[BUFFERSIZE];
	char resultado[CAPACIDAD];
	int sent_bytes;
	int recv_bytes;
	if (argc == 1){
		struct servent* serv;
		serv = getservbyname("qotd","udp");
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
	int s = socket(AF_INET, SOCK_DGRAM,0);
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
	socklen_t len = sizeof(server);
	//Bucle infinito para procesar las solicitudes
	while(1){
		recv_bytes = recvfrom(s, bfr, BUFFERSIZE,0,
				(struct sockaddr*) &server,&len);
		if(recv_bytes<0){
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}
	static char buffQuote [MAXLENGTH];
	system("/usr/games/fortune -s > /tmp/tt.txt");
	FILE *fich = fopen("/tmp/tt.txt","r");
	int nc = 0;
	do{
		if(fgetc(fich)==EOF){
			break;
		}
		buffQuote[nc++] = fgetc(fich);
	}while(nc < MAXLENGTH-1);
	fclose(fich);
	char frase[30]="Quote of the day from vm2518:\n";
	strcat(resultado,frase);
	strcat(resultado,buffQuote);
	strcat(resultado,'\0');


	//envio de la respuesta
	sent_bytes = sendto(s,resultado,CAPACIDAD,0,
			(struct sockaddr*)&server,sizeof(server));
	if(sent_bytes<0){
		perror("sendto()");
		exit(EXIT_FAILURE);
	}
	printf("Enviado: %s \n",bfr);
	exit(EXIT_SUCCESS);
	}
}




