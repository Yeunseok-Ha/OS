#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
struct student{
	int id;
	char password[100];
	char targetc[4096];
	int result;

};
char *readFile(char *filename) {
    FILE *f = fopen(filename, "r");
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = (char *) malloc(length + 1);
    buffer[length] = '\0';
    fread(buffer, 1, length, f);
    fclose(f);
    return buffer;
}
int 
main(int argc, char const *argv[]) 
{
	extern char *optarg;
	struct sockaddr_in serv_addr; 
	int sock_fd ;
	int s, len ;
	char buffer[1024] = {0}; 
	char * data ;
	int listen_fd, new_socket ;
        struct sockaddr_in address;
        int addrlen = sizeof(address);
	int opt;
	char ipadd[30];
	char passandfile[30];
	char ip[30];
	char ports[30];
	char file[20];
	uint16_t portnum;
	struct student stud;
	while((opt = getopt(argc,argv,"n:u:k:")) != -1)
	{
	switch(opt)
	{

		case 'n':{//ip
			memcpy(ipadd,optarg,30);
			char *ptr = strtok(ipadd,":");
               		strcpy(ip,ptr);
               		ptr = strtok(NULL," ");
                	strcpy(ports,ptr);
                	portnum = atoi(ports);
			break;
			}
		case 'u'://id
			stud.id = atoi(optarg);
			break;
		case 'k':{//password
			memcpy(passandfile,optarg,30);
			char *ptr1 = strtok(passandfile," ");
			strcpy(stud.password,ptr1);
			break;}
		
		}		
	}
	strcpy(file, argv[7]);
	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	
	if (sock_fd <= 0) {
		perror("socket failed : ") ;
		exit(EXIT_FAILURE) ;
	} 

	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(portnum); 
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
		perror("inet_pton failed : ") ; 
		exit(EXIT_FAILURE) ;
	} 

	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}
	strcpy(stud.targetc,readFile(file));
	send(sock_fd,&stud, sizeof(struct student),0);
	shutdown(sock_fd,SHUT_WR);	
		
	while(recv(sock_fd,buffer,1024,0)>0){

		printf("%s\n",buffer);

	}	
	close(sock_fd);
//	create a new socket

	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;

        if (sock_fd <= 0) {
                perror("socket failed : ") ;
                exit(EXIT_FAILURE) ;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portnum+1);
        if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
                perror("inet_pton failed : ") ;
                exit(EXIT_FAILURE) ;
        }

        if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                perror("connect failed : ") ;
                exit(EXIT_FAILURE) ;
        }



	scanf("%s",buffer);
	
	send(sock_fd,buffer,1024,0);
	shutdown(sock_fd,SHUT_WR);
	printf("%s\n",buffer);
	printf("sent!\n");
	printf("buck\n");
	
	while(recv(sock_fd,(void*)&stud,sizeof(struct student),0)>0)
	{}
	
	printf("%d cases passed!\n",stud.result);
		
	
}
