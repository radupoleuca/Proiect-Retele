#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RED "\x1B[31m"
#define CYN "\x1B[36m"
#define RESET "\x1B[0m"

int port;
unsigned long adress;

int main(int argc, char *argv[])
{

	int sd;
    int  ret;
	struct sockaddr_in serverAddr;
	char buffer[1024]=" ";
	int verif = 0;

    if(argc!=3)
    {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }
    port = atoi(argv[2]); // stabilim portul;
	adress = inet_addr(argv[1]);
	sd = socket(AF_INET, SOCK_STREAM, 0); // descriptorul de socket

	if(sd == -1)
    {
		printf("Error in connection\n");
		exit(1);
	}

	printf("Client Socket is created\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = adress;//inet_addr("127.0.0.1");

	
	if(connect(sd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1){
		printf("Error in connection\n");
		exit(1);
	}
	printf("Connected to Server\n");

	int ok = 0;

	while(1)
    {
		if(ok == 0)
		{
        	printf(RED "Pentru cautarea unei carti de vizita tastati comanda 'cautare'.\n");
			printf("Pentru adaugarea unei noi carti de vizita tastati comanda 'adaugare'.\n");
			printf("Pentru actualizarea unei carti de vizita tastati comanda 'actualizare'.\n");
			printf("Pentru stergerea unei carti de vizita tastati comanda 'stergere'.\n");
			printf("Pentru a va deconecta tastati comanda 'exit!'.\n" RESET);
			ok = 1;
		}

		if(verif == 0)
		{
			printf(CYN "Introduceti comanda:  " RESET);
        	fflush(stdout);
        	memset(buffer,0, sizeof(buffer));
			read(0,buffer,sizeof(buffer));
			send(sd, buffer, strlen(buffer), 0);
		}
		else
		{
			verif = 0;
			send(sd, buffer, strlen(buffer), 0);
		}
		memset(buffer,0, sizeof(buffer));

		if(recv(sd, buffer, 1024, 0) < 0){
			printf("Error in receiving data\n");
		}
        else
        {
			buffer[strlen(buffer)-1]='\0';
			printf("%s\n", buffer);

			if(strcmp(buffer, "exit!") == 0){
				printf("Disconnected from server!\n");
				break;
			}
			else
			if(strcmp(buffer, "cautare") == 0)
			{
				verif = 1;
				char name[50];
				printf("Introduceti numele persoanei:\n");
				printf("nume: ");
				fflush(stdout);
        		memset(name,0, sizeof(name));
				read(0,name,sizeof(name));
				name[strlen(name)-1]='\0';
				fflush(stdout);
        		memset(buffer,0, sizeof(buffer));
				strcat(buffer,"$$$");
				strcat(buffer,name);
			}
			else
			if(strcmp(buffer, "stergere") == 0)
			{
				verif = 1;
				char name[50];
				printf("Introduceti numele persoanei:\n");
				printf("nume: ");
				fflush(stdout);
        		memset(name,0, sizeof(name));
				read(0,name,sizeof(name));
				name[strlen(name)-1]='\0';
				fflush(stdout);
        		memset(buffer,0, sizeof(buffer));
				strcat(buffer,"###");
				strcat(buffer,name);
			}
			else
			if(strcmp(buffer, "actualizare") == 0)
			{
				verif = 1;
				char name[50];
				printf("Introduceti numele persoanei:\n");
				printf("nume: ");
				fflush(stdout);
        		memset(name,0, sizeof(name));
				read(0,name,sizeof(name));
				name[strlen(name)-1]='\0';
				fflush(stdout);
        		memset(buffer,0, sizeof(buffer));
				strcat(buffer,"***");
				strcat(buffer,name);
			}
			else
			if(strcmp(buffer, "adaugare") == 0)
			{
				verif = 1;
				char name[50],email[50],contact[50],adress[50],company[50];
				printf("Introduceti datele:\n");

				printf("nume: ");
				fflush(stdout);
        		memset(name,0, sizeof(name));
				read(0,name,sizeof(name));
				name[strlen(name)-1]='\0';

				printf("email: ");
				fflush(stdout);
        		memset(email,0, sizeof(email));
				read(0,email,sizeof(email));
				email[strlen(email)-1]='\0';

				printf("contact: ");
				fflush(stdout);
        		memset(contact,0, sizeof(contact));
				read(0,contact,sizeof(contact));
				contact[strlen(contact)-1]='\0';

				printf("adresa: ");
				fflush(stdout);
        		memset(adress,0, sizeof(adress));
				read(0,adress,sizeof(adress));
				adress[strlen(adress)-1]='\0';

				printf("company: ");
				fflush(stdout);
        		memset(company,0, sizeof(company));
				read(0,company,sizeof(company));
				company[strlen(company)-1]='\0'; 

				char str[256]="";
				strcat(str,"!!!");
				strcat(str,name);
				strcat(str," ;");
				strcat(str,email);
				strcat(str," ;");
				strcat(str,contact);
				strcat(str," ;");
				strcat(str,adress);
				strcat(str," ;");
				strcat(str,company);
				
				fflush(stdout);//
        		memset(buffer,0, sizeof(buffer));
				strcpy(buffer,str);
			}
			else
			if(strncmp(buffer, "***",3) == 0)
			{
				strcpy(buffer,buffer+3);

				verif = 1;
				char name[50],email[50],contact[50],adress[50],company[50];
				printf("Introduceti noile date (sau lasati necompletat campul daca nu doriti):\n");

				printf("nume: ");
				fflush(stdout);
        		memset(name,0, sizeof(name));
				read(0,name,sizeof(name));
				name[strlen(name)-1]='\0';

				printf("email: ");
				fflush(stdout);
        		memset(email,0, sizeof(email));
				read(0,email,sizeof(email));
				email[strlen(email)-1]='\0';

				printf("contact: ");
				fflush(stdout);
        		memset(contact,0, sizeof(contact));
				read(0,contact,sizeof(contact));
				contact[strlen(contact)-1]='\0';

				printf("adresa: ");
				fflush(stdout);
        		memset(adress,0, sizeof(adress));
				read(0,adress,sizeof(adress));
				adress[strlen(adress)-1]='\0';

				printf("company: ");
				fflush(stdout);
        		memset(company,0, sizeof(company));
				read(0,company,sizeof(company));
				company[strlen(company)-1]='\0'; 

				char str[256]="";
				strcat(str,"^^^");
				strcat(str,name);
				strcat(str," ;");
				strcat(str,email);
				strcat(str," ;");
				strcat(str,contact);
				strcat(str," ;");
				strcat(str,adress);
				strcat(str," ;");
				strcat(str,company);
				
				fflush(stdout);
        		memset(buffer,0, sizeof(buffer));
				strcpy(buffer,str);
			}
		}
	}
	close(sd);
	return 0;
}