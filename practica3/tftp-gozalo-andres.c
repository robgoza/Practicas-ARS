// Practica tema 7, Gozalo Andres Roberto


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main (int argc, char *argv[]){
	int s;
	int exten;
	int bloque;
	struct sockaddr_in server;
	char bfr[516];
	char ack[4];
	if(argc == 4 || argc == 5){
		if(inet_aton(argv[1], &server.sin_addr) == 0){
			perror("inet_aton()");
			exit(EXIT_FAILURE);
		}
		struct in_addr dir;
		inet_aton(argv[1],&dir);
		struct servent* ser;
		ser = getservbyname("tftp","udp");
		if(ser == NULL){
			perror("getservbyname()");
			exit(EXIT_FAILURE);

		}
		//Activacion del sistema detallado
		if(argc==5 && strcmp(argv[4], "-v")==0){
			exten=1;
		}
		server.sin_port = ser->s_port;

		server.sin_family=AF_INET;
		server.sin_addr =dir;

		//Creacion del socket
		s=socket(AF_INET, SOCK_DGRAM,0);
		if(s<0){
			perror("socket()");
			exit(EXIT_FAILURE);
		}
	}
	//Modo lectura
	if(strcmp(argv[2],"-r")==0){
		//Envio de la peticion de lectura
		char RRQ[128]="RG";
		strcat(RRQ,argv[3]);
		RRQ[2+strlen(argv[3])]=100;
		strcat(RRQ,"octet");
		RRQ[2+strlen(argv[3])]=0;
		RRQ[0]=0;
		RRQ[1]=1;
		if(sendto(s, RRQ, 128, 0, (struct sockaddr*)&server,
				       	sizeof(server))<0){	
			perror("RRQ");
			exit(EXIT_FAILURE);
		}
		if(exten==1){
			printf("Enviada solicitud de lectura de %s a servidor TFTP en %s.\n", argv[3], argv[1]);
		}
		FILE *fich = fopen(argv[3],"w");
		int recb;
		int esp=0;
		socklen_t sizeServ=sizeof(server);

		do{
			//Recepcion del datagrama
			recb=recvfrom(s, bfr, 516, 0, (struct sockaddr*)&server,&sizeServ);
			bloque = (unsigned char)bfr[2]*256+(unsigned char)bfr[3];	
			esp++;
			if(esp!=bloque){
				printf("Orden incorrecto.\n");
				exit(EXIT_FAILURE);
			}
			if(bfr[1]==5){
				//printf("Fallo %i%i.\n", bfr[2], bfr[3]);
				switch(bfr[3]){
					case 0:
						printf("No definido. Comprobar errstring\n");
						break;
					case 1:
						printf("Fichero no encontrado\n");
						break;
					case 2:
						printf("violacion de acceso\n");
						break;
					case 3:
						printf("Espacio de almacenamiento lleno\n");
						break;
					case 4:
						printf("Operacion TFTP ilegal\n");
						break;
					case 5:
						printf("Identificador de transferencia desconocido\n");
						break;
					case 6:
						printf("El fichero ya existe \n");
						break;
					case 7:
						printf("Usuario desconocido\n");
						break;
					}
				exit(EXIT_FAILURE);
			}
			if (exten==1){
				printf("Recibido bloque %i.del servidor tftp\n", bloque);
			}
			
			//Almacenamiento de los datos en el fichero
			fwrite(bfr+4, 1, recb-4,fich);

			//Envio del ACK
			ack[0]=0;
			ack[1]=4;
			ack[2]=bfr[2];
			ack[3]=bfr[3];
			if(sendto(s, ack, 4, 0, (struct sockaddr*)&server, 
						sizeof(server))<0){
				perror("ACK");
				exit(EXIT_FAILURE);
			}
			if(exten==1){
				printf("Enviamos el ACK del bloque %i.\n",
					       	bloque);
			}
		}while(recb==516);
		
		//Cierre del fichero
		if(exten==1){
			printf("El bloque %i era el ultimo se cerrara el fichero.\n",bloque);
		}
		fclose(fich);
	}
	
	//Modo escritura
	
	if(strcmp(argv[2],"-w")==0){
		//Envio de la peticion de escritura
		char WRQ[128]="RG";
		strcat(WRQ,argv[3]);
		WRQ[2+strlen(argv[3])]=100;
		strcat(WRQ,"octet");
		WRQ[2+strlen(argv[3])]=0;
		WRQ[0]=0;
		WRQ[1]=2;
		if(sendto(s, WRQ, 128, 0, (struct sockaddr*)&server,
					sizeof(server))<0){
			perror("sendto()");
			exit(EXIT_FAILURE);
		}
		if(exten==1){
			printf("Enviada solicitud de escritura de %s a servidor tftp en %s. \n", argv[3], argv[1]);
		}
		
		FILE *fich = fopen(argv[3],"r");
		int pos=0;
		while(!feof(fich)){
			//Lectura del fichero
			bfr[pos%512+4]=fgetc(fich);
			pos++;

			if(pos%512==0 || feof(fich)){
				//Recepcion de ACK

		socklen_t sizeServ = sizeof(server);
				recvfrom(s, ack, 4, 0, (struct sockaddr*)&server,&sizeServ);
			bloque=(unsigned char)ack[2]*256+(unsigned char)ack[3]
				+1;
			if(ack[1]==5){
				//printf("Fallo %i%i. \n", ack[2], ack[3]);
				switch(ack[3]){
					case 0:
						printf("No definido. Comprobar errstring\n");
						break;
					case 1:
						printf("Fichero no encontrado\n");
						break;
					case 2:
						printf("violacion de acceso\n");
						break;
					case 3:
						printf("Espacio de almacenamiento lleno\n");
						break;
					case 4:
						printf("Operacion TFTP ilegal\n");
						break;
					case 5:
						printf("Identificador de transferencia desconocido\n");
						break;
					case 6:
						printf("El fichero ya existe \n");
						break;
					case 7:
						printf("Usuario desconocido\n");
						break;
				}
				exit(EXIT_FAILURE);
			}
			if(exten==1){
				printf("ACK recibido del bloque %i. \n"
						,bloque-1);
			}

			int size;
			if(pos%512==0){
				size=516;
			}else{
				size=pos%512+3;
			}

			//Envio de un paquete de datos
			bfr[0]=0;
			bfr[1]=3;
			bfr[2]=bloque/256;
			bfr[3]=bloque%256;
			if(sendto(s, bfr, size, 0, (struct sockaddr*)&server,sizeof(server))<0){
				perror("sendto()");
				exit(EXIT_FAILURE);
			}
		}
	}
	//Cierre del fichero
	if(exten==1){
		printf("El bloque %i era el ultimo: cerramos el fichero.\n",
				bloque);
		fclose(fich);
	}
}
return 0;
}

	
