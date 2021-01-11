#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <ctype.h>

#define PORT 4444
sqlite3* db;

static int callback(void *str,int argc, char **argv, char **azColName)
{
    int i;
    char *data=(char*)str;
    for(i=0;i<argc;i++)
    {
        if(argv[i])
            strcat(data,argv[i]);
        else
            strcat(data,"NULL");
        strcat(data," || ");
    }
    data[strlen(data)-3]='\0';
    strcat(data,"\n");
    return 0;
}

static int callback_insert(void *str, int argc, char ** argv, char **azColName)
{
    int i;
    char *data=(char*)str;
    for(i=0;i<argc;i++)
    {
        if(argv[i])
            strcat(data,argv[i]);
        else
            strcat(data,"NULL");
    }
    return 0;
}

int spaces(char s[256])
{
    for(int i=0; s[i]!='\0'; i++)
    {
        if(!isspace(s[i]))
        {
            return 1;
        }
    }
    return 0;
}

int main(){

    int rc=sqlite3_open("retele.db",&db);
    char sql[1000];
    char str[1000];
    char nombre[1000];
    char *zErrMsg;

    if(rc)
    {
       fprintf(stderr,"Err: %s\n",sqlite3_errmsg(db));
        return 0;
    }

	int sd; // descriptorul de socket
	struct sockaddr_in serverAddr; // structura folosita de server

    int client;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024]; 
  
	sd = socket(AF_INET, SOCK_STREAM, 0); // creare socket

	if(sd == -1)
    {
		printf("Error in connection\n");
		exit(1);
	}
	printf("Server socket is created\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET; // stabilirea familiei de socket
	serverAddr.sin_port = htons(PORT); // stabilirea portului
	serverAddr.sin_addr.s_addr =inet_addr("127.0.0.1"); //inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);//htonl(INADDR_ANY);//inet_addr("127.0.0.1"); //stabilirea adresei

	if(bind(sd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))==-1) // atasare socket
    {
		printf("Error in binding\n");
		exit(1);
	}
	printf("Bind to port %d\n", 4444);

	if(listen(sd, 5) == 0) // punem serverul sa asculte daca vin clienti sa se conecteze
    {
		printf("Listening..\n");
	}else
    {
		printf("Error in binding\n");
	}

    fflush(stdout);

	while(1)
    {
		client = accept(sd,(struct sockaddr*)&newAddr, &addr_size); // acceptam un client
		if(client < 0)
        {
            continue;
		}

		printf("Connection accepted from //%s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if(fork() == 0)
        {
            int closing = 0;

			while(1)
            {
                fflush(stdout); 
                memset(buffer,0, sizeof(buffer)); 
				if(recv(client, buffer, 1024, 0) < 0) // primire/citire mesaj
                { 
			        printf("[-]Error in receiving data.\n");
                    close(client);//
                    continue;//
                }

				if(strcmp(buffer, "exit!") == 0)
                {
                    send(client, buffer, strlen(buffer), 0); // transmitere/scriere mesaj
					bzero(buffer, sizeof(buffer));   
                    closing = 1;
				}
                else
                if(strcmp(buffer, "stergere") == 0)
                {
                    sprintf(buffer, "stergere");
                    send(client, buffer, strlen(buffer), 0); // transmitere/scriere mesaj
					bzero(buffer, sizeof(buffer));
                }
                else
                if(strncmp(buffer, "###", 3) == 0)
                {
                    strcpy(buffer,buffer+3);
                    strcpy(str,"");
        
                    sprintf(sql, "SELECT * FROM vcard WHERE TRIM(UPPER(name)) = TRIM(UPPER('%s'));",buffer) ;
                    rc=sqlite3_exec(db,sql,callback,str,&zErrMsg);
                    if(rc!=SQLITE_OK)
                    {
                        sprintf(buffer, "SQL error: %s\n",zErrMsg);
                        send(client, buffer, strlen(buffer), 0); 
					    bzero(buffer, sizeof(buffer)); 
                        sqlite3_free(zErrMsg);
                    }
                    else
                    {
                        if(strstr(buffer,str)==0)
                        {
                            sprintf(sql, "DELETE FROM vcard WHERE TRIM(UPPER(name)) = TRIM(UPPER('%s'));",buffer) ;
                            rc=sqlite3_exec(db,sql,callback,str,&zErrMsg);
                            if(rc!=SQLITE_OK)
                            {
                                sprintf(buffer, "SQL error: %s\n",zErrMsg); 
                                sqlite3_free(zErrMsg);
                            }
                            else
                            {
                                strcpy(buffer,"Stergerea s-a realizat cu succes!\n");
                            }
                        }
                        else
                        {
                            strcpy(buffer,"Numele nu exista!\n");
                        }
                        send(client, buffer, strlen(buffer), 0); 
					    bzero(buffer, sizeof(buffer)); 
                    }
                }
                else
                if(strcmp(buffer, "cautare") == 0)
                {
                    sprintf(buffer, "cautare");
                    send(client, buffer, strlen(buffer), 0); 
					bzero(buffer, sizeof(buffer));
                }
                else
                if(strncmp(buffer, "$$$", 3) == 0)
                {
                    strcpy(buffer,buffer+3);
                    strcpy(str,"");
                    sprintf(sql, "SELECT * FROM vcard WHERE TRIM(UPPER(name)) = TRIM(UPPER('%s'));",buffer) ;
                    rc=sqlite3_exec(db,sql,callback,str,&zErrMsg);
                    if(rc!=SQLITE_OK)
                    {
                        sprintf(buffer, "SQL error: %s\n",zErrMsg);
                        send(client, buffer, strlen(buffer), 0);
					    bzero(buffer, sizeof(buffer)); 
                        sqlite3_free(zErrMsg);
                    }
                    else
                    {
                        if(strstr(buffer,str)==0)
                        {
                            strcpy(buffer, str);
                        }
                        else
                        {
                            strcpy(buffer,"Numele nu exista!\n");
                        }
                        send(client, buffer, strlen(buffer), 0); 
					    bzero(buffer, sizeof(buffer)); 
                    }
                }
                else
                if(strcmp(buffer, "actualizare") == 0)
                {
                    sprintf(buffer, "actualizare");
                    send(client, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));
                }
                else
                if(strncmp(buffer, "***", 3) == 0)
                {
                    strcpy(buffer,buffer+3);
                    strcpy(nombre,buffer);
                    strcpy(str,"");
                    sprintf(sql, "SELECT * FROM vcard WHERE TRIM(UPPER(name)) = TRIM(UPPER('%s'));",buffer) ;
                    rc=sqlite3_exec(db,sql,callback,str,&zErrMsg);
                    if(rc!=SQLITE_OK)
                    {
                        sprintf(buffer, "SQL error: %s\n",zErrMsg);
                        send(client, buffer, strlen(buffer), 0);
					    bzero(buffer, sizeof(buffer)); 
                        sqlite3_free(zErrMsg);
                    }
                    else
                    {
                        if(strstr(buffer,str)==0)
                        {
                            strcpy(buffer,"");
                            strcat(buffer,"***");
                            strcat(buffer, str);
                        }
                        else
                        {
                            strcpy(buffer,"Numele nu exista!\n");
                        }
                        send(client, buffer, strlen(buffer), 0); 
					    bzero(buffer, sizeof(buffer)); 
                    }
                }
                else
                if(strncmp(buffer, "^^^", 3) == 0)
                {
                    strcpy(buffer,buffer+3);
                    char *p = strtok(buffer, ";");
                    char s[5][256];
                    int i = 0;
                    while(p!=NULL)
                    {
                        strcpy(s[i], p);
                        i++;
                        p = strtok(NULL, ";");
                    }
                    int okk = 0;
                    
                    if(spaces(s[0])==1)
                    {
                        sprintf(sql, "UPDATE vcard SET name='%s' WHERE TRIM(UPPER(name))=TRIM(UPPER('%s')) ;",s[0],nombre) ;
                        rc=sqlite3_exec(db,sql,callback,0,&zErrMsg);
                        if(rc!=SQLITE_OK)
                        {
                            sprintf(buffer, "SQL error: %s\n",zErrMsg);
                            send(client, buffer, strlen(buffer), 0); 
                            bzero(buffer, sizeof(buffer)); 
                            sqlite3_free(zErrMsg);
                            okk = 1;
                        }
                    }

                    if(spaces(s[1])==1)
                    {
                        sprintf(sql, "UPDATE vcard SET EMAIL='%s' WHERE TRIM(UPPER(name))=TRIM(UPPER('%s')) ;",s[1],nombre) ;
                        rc=sqlite3_exec(db,sql,callback,0,&zErrMsg);
                        if(rc!=SQLITE_OK)
                        {
                            sprintf(buffer, "SQL error: %s\n",zErrMsg);
                            send(client, buffer, strlen(buffer), 0); 
                            bzero(buffer, sizeof(buffer)); 
                            sqlite3_free(zErrMsg);
                            okk = 1;
                        }
                    }

                    if(spaces(s[2])==1)
                    {
                        sprintf(sql, "UPDATE vcard SET contact='%s' WHERE TRIM(UPPER(name))=TRIM(UPPER('%s')) ;",s[2],nombre) ;
                        rc=sqlite3_exec(db,sql,callback,0,&zErrMsg);
                        if(rc!=SQLITE_OK)
                        {
                            sprintf(buffer, "SQL error: %s\n",zErrMsg);
                            send(client, buffer, strlen(buffer), 0);
                            bzero(buffer, sizeof(buffer)); 
                            sqlite3_free(zErrMsg);
                            okk = 1;
                        }
                    }

                    if(spaces(s[3])==1)
                    {
                        sprintf(sql, "UPDATE vcard SET adress='%s' WHERE TRIM(UPPER(name))=TRIM(UPPER('%s')) ;",s[3],nombre) ;
                        rc=sqlite3_exec(db,sql,callback,0,&zErrMsg);
                        if(rc!=SQLITE_OK)
                        {
                            sprintf(buffer, "SQL error: %s\n",zErrMsg);
                            send(client, buffer, strlen(buffer), 0);
                            bzero(buffer, sizeof(buffer)); 
                            sqlite3_free(zErrMsg);
                            okk = 1;
                        }
                    }

                    if(spaces(s[4])==1)
                    {
                        sprintf(sql, "UPDATE vcard SET company='%s' WHERE TRIM(UPPER(name))=TRIM(UPPER('%s')) ;",s[4],nombre) ;
                        rc=sqlite3_exec(db,sql,callback,0,&zErrMsg);
                        if(rc!=SQLITE_OK)
                        {
                            sprintf(buffer, "SQL error: %s\n",zErrMsg);
                            send(client, buffer, strlen(buffer), 0); 
                            bzero(buffer, sizeof(buffer)); 
                            sqlite3_free(zErrMsg);
                            okk = 1;
                        }
                    }
                
                    //sprintf(sql, "UPDATE vcard SET name=IFNULL('%s',name), email=IFNULL('%s',email), contact=IFNULL('%s',contact), adress=IFNULL('%s',adress), company=IFNULL('%s',company) WHERE TRIM(UPPER(name))=TRIM(UPPER('%s')) ;",s[0],s[1],s[2],s[3],s[4],nombre) ;
                    strcpy(nombre,"");
                    /*
                    rc=sqlite3_exec(db,sql,callback,0,&zErrMsg);
                    if(rc!=SQLITE_OK)
                    {
                        sprintf(buffer, "SQL error: %s\n",zErrMsg);
                        send(client, buffer, strlen(buffer), 0); // transmitere/scriere mesaj
					    bzero(buffer, sizeof(buffer)); 
                        sqlite3_free(zErrMsg);
                    }
                    
                    else
                    */
                    if(okk == 0)
                    {
                        sprintf(buffer,"Modificarea s-a facut cu succes!\n");  
                        send(client, buffer, strlen(buffer), 0); 
					    bzero(buffer, sizeof(buffer)); 
                    }
                }
                else
                if(strcmp(buffer, "adaugare") == 0)
                {
                    sprintf(buffer, "adaugare");
                    send(client, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer)); 
                }
                else
                if(strncmp(buffer, "!!!", 3) == 0)
                {
                    strcpy(buffer,buffer+3);
                    char *p = strtok(buffer, ";");
                    char s[5][256];
                    int i = 0;
                    while(p!=NULL)
                    {
                        strcpy(s[i], p);
                        i++;
                        p = strtok(NULL, ";");
                    }
                    sprintf(sql, "INSERT INTO vcard VALUES('%s','%s','%s','%s','%s');",s[0],s[1],s[2],s[3],s[4]) ;
                    rc=sqlite3_exec(db,sql,callback,0,&zErrMsg);
                    if(rc!=SQLITE_OK)
                    {
                        sprintf(buffer, "SQL error: %s\n",zErrMsg);
                        send(client, buffer, strlen(buffer), 0); 
					    bzero(buffer, sizeof(buffer)); 
                        sqlite3_free(zErrMsg);
                    }
                    else
                    {
                        sprintf(buffer,"Adaugarea s-a facut cu succes!\n");  
                        send(client, buffer, strlen(buffer), 0); 
					    bzero(buffer, sizeof(buffer)); 
                    }
                }
                else
                if(strcmp(buffer, "stergere") == 0)
                {

                    sprintf(buffer,"Stergerea s-a facut cu succes!\n");  
                    send(client, buffer, strlen(buffer), 0); 
					bzero(buffer, sizeof(buffer));   
                }
                else
                {
					send(client, buffer, strlen(buffer), 0); 
					bzero(buffer, sizeof(buffer));
				} 

                if(closing == 1)
                {
                    printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    close(client);
                    break;
                }
			}
            exit(0);
		}
        else continue;

	}
	sqlite3_close(db);
    close(client);
	return 0;
}