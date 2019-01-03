// Practica tema 8, Gozalo Andres Roberto
#include "ip-icmp-ping.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	int exten = 0;
	
	//Comprobacion de argumentos
	if(argc!=2 && argc!=3){
		printf("Sintaxis incorrecta.\n");
		exit(EXIT_FAILURE);
	}
	if(argc==3 && strcmp(argv[2],"-v")==0){
		exten=1;
	}
	//Conversion de IP
	struct in_addr dir;
	inet_aton(argv[1], &dir);

	//Direccion de destino
	struct sockaddr_in dest;
	dest.sin_family=AF_INET;
	dest.sin_port=0;
	dest.sin_addr=dir;
	socklen_t lon = sizeof(dest);

	//Creacion del socket
	int s = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	if(s<0){
		perror("socket()");
		exit(EXIT_FAILURE);
	}
	ECHORequest sol;

	//Creacion del datagrama ICMP
	sol.icmpHeader.Type=8;
	sol.icmpHeader.Code=0;
	sol.icmpHeader.Checksum=0;
	sol.ID=getpid();
	sol.SeqNumber=0;
	strcpy(sol.payload,"Este es el payload.");

	//Calculo del checksum
	sol.icmpHeader.Checksum=0;
	int numTrozos=sizeof(sol)/2;
	unsigned short int *p;
	unsigned int cont=0;
	p=(unsigned short int*)&sol;
	int i;
	for(i=0;i<numTrozos;i++){
		cont=cont+(unsigned int)*p;
		p++;
	}
	cont=(cont >> 16)+(cont & 0x0000ffff);
	cont=(cont >> 16)+(cont & 0x0000ffff);
	cont=~cont;
	sol.icmpHeader.Checksum=(unsigned short int)cont;

	//Envio de la solicitud
	int sen=sendto(s,&sol,72,0,(struct sockaddr*)&dest,sizeof(dest));
	if(sen<0){
		perror("sendto()");
		exit(EXIT_FAILURE);
	}
	
	//Recepcion de la respuesta
	ECHOResponse resp;
	int rec=recvfrom(s,&resp,516,0,(struct sockaddr*)&dest,&lon);
		if(rec<0){
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}
	//Impresion detalles de la solicitud
	printf("Solicitud ICMP enviada a %s.\n",argv[1]);
	if(exten==1){
		printf("->Type: %u\n",sol.icmpHeader.Type);
		printf("->Code: %u\n",sol.icmpHeader.Code);
		printf("->Identifier (pid): %u\n",sol.ID);
		printf("->Seq. number: %u\n",sol.SeqNumber);
		printf("->Cadena a enviar: %s\n",sol.payload);
		printf("->Checksum: 0x%x\n",sol.icmpHeader.Checksum);
		printf("->Tamaño total del paquete ICMP: %lu\n",sizeof(sol));
	}
	
	//Impresion detalles de la respuesta
	
 switch(resp.icmpHeader.Type) {
        case 0 :
            printf("Recibida respuesta ICMP correcta desde %s.\n", argv[1]);
            if (exten){
                printf("->Tamaño de la respuesta: %d\n", rec);
                printf("->Cadena recibida: %s\n", resp.payload);
                printf("->ID: %u\n", resp.ID);
                printf("->TTL: %u\n", resp.ipHeader.TTL);
            }
            break;

        case 3:
            printf("Destination unreachable: ");
            switch(resp.icmpHeader.Code){
                case 0:
                    printf("Destination network unreachable.\n");
                    break;
                case 1:
                    printf("Destination host unreachable.\n");
                    break;
                case 2:
                    printf("Destination protocol unreachable.\n");
                    break;
                case 3:
                    printf("Destination port unreachable.\n");
                    break;
                case 4:
                    printf("Fragmentation required, and DF flag set.\n");
                    break;
                case 5:
                    printf("Source route failed.\n");
                    break;
                case 6:
                    printf("Destination network unknown.\n");
                    break;
                case 7:
                    printf("Destination host unknown.\n");
                    break;
                case 8:
                    printf("Source host isolated.\n");
                    break;
                case 9:
                    printf("Network administratively prohibited.\n");
                    break;
                case 10:
                    printf("Host administratively prohibited.\n");
                    break;
                case 11:
                    printf("Network unreachable for ToS.\n");
                    break;
                case 12:
                    printf("Host unreachable for ToS.\n");
                    break;
                case 13:
                    printf("Communication administratively prohibited.\n");
                    break;
                case 14:
                    printf("Host precedence violation.\n");
                    break;
                case 15:
                    printf("Precedence cutoff in effect.\n");
                    break;
            }
            break;

	case 5:
	    printf("Redirect Message: ");
	    switch(resp.icmpHeader.Code){
		    case 1:
			    printf("Redirect Message: Redirect Datagram for the 		Host.\n%d%d",resp.icmpHeader.Code,resp.icmpHeader.Type );
			    break;
		    case 2:
			    printf("Redirect Message: Redirect Datagram for the 	ToS & network.\n%d%d",resp.icmpHeader.Code,resp.icmpHeader.Type );
			    break;
		    case 3:
			    printf("Redirect Message: Redirect Datagram for the 	ToS & host. \n%d%d",resp.icmpHeader.Code,resp.icmpHeader.Type );
			    break;
	    }
	    break;
	case 9:
	    printf("Router Advertisement.\n%d%d",
				   resp.icmpHeader.Code,resp.icmpHeader.Type );
	    break;
	case 10:
	    printf("Router Solicitation.\n%d%d",
				   resp.icmpHeader.Code,resp.icmpHeader.Type );
	    break;
				
        case 11:
            printf("Time exceeded: ");
            switch(resp.icmpHeader.Code){
                case 0:
                    printf("Time Exceeded: TTL expired in transit.\n%d%d",
				   resp.icmpHeader.Code,resp.icmpHeader.Type );
                    break;
                case 1:
                    printf("Time Exceeded: Fragment reassembly.\n%d%d",
				   resp.icmpHeader.Code,resp.icmpHeader.Type );
                    break;
            }
            break;

        case 12:
            printf("Parameter problem. Bad IP header: ");
            switch(resp.icmpHeader.Code){
                case 0:
                    printf("Parameter problem: Pointer indicates the error.\n%d%d",resp.icmpHeader.Code,resp.icmpHeader.Type );
                    break;
                case 1:
                    printf("Parameter problem: Missing a required option.\n%d%d"
				  ,resp.icmpHeader.Code,resp.icmpHeader.Type );

                    break;
                case 2:
                    printf("Parameter problem:Bad length.\n%d%d",
				   resp.icmpHeader.Code,resp.icmpHeader.Type );
                    break;
            }
            break;

	case 13:
	    printf("Timestamp.\n%d%d",
				   resp.icmpHeader.Code,resp.icmpHeader.Type );
	    break;
	case 14:
	    printf("Timestamp Reply.\n%d%d",
				   resp.icmpHeader.Code,resp.icmpHeader.Type );
	    break;

        default:
            printf("Error.\n");
            break;

    }

    return 0;
}
