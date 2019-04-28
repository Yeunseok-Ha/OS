// Partly taken from https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
struct student{
	int id;
	char password[100];
	char targetc[4096];
	int result;
	struct student *p;
};

struct ioutc{
	char targetc[4096];

	struct {
	
	char input[100];


	}in[11];	

	struct {
	char output[1024];
	}out[11];
	int state;
}i;
char dir[50];
char ip[30];

char *readFile(char *filename) {
    FILE *f = fopen(filename, "r");
 //   assert(f);
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = (char *) malloc(length + 1);
    buffer[length] = '\0';
    fread(buffer, 1, length, f);
    fclose(f);
    return buffer;
}

void
child_proc(int conn,uint16_t work,uint16_t port)
{
	struct student new;
	int len = 0 ;
	int s ;
	int result = 0;
	struct student stu;
	int listen_fd, new_socket ;
        struct sockaddr_in address;
        int addrlen = sizeof(address);
	int filenum ; 
	char tc_filenum[2];
	char tc_file[50];

	while((s= recv(conn,(void*)&stu,sizeof(struct student),0))>0){
	new.id = stu.id;
	strcpy(new.password,stu.password);
	strcpy(new.targetc,stu.targetc);

	}	
	strcpy(i.targetc,stu.targetc);
	
	for (filenum = 1; filenum < 11; filenum++){
		sprintf(tc_filenum, "%d",filenum);
		strcpy(tc_file, dir);
		strcat(tc_file, "/");
		strcat(tc_file, tc_filenum);
		strcat(tc_file, ".in");
		strcpy(i.in[filenum-1].input,readFile(tc_file));
	}
//=====================Worker Socket========================//
 	struct sockaddr_in serv_addr;
        int sock_fd ;
        char buffer[1024] = {0};
        char * data ;
        sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
        if (sock_fd <= 0) {
                perror("socket failed : ") ;
                exit(EXIT_FAILURE) ;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(work);
        if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
               perror("inet_pton failed : ") ;
                exit(EXIT_FAILURE) ;
       }
	printf("%d\n",work);
        if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                perror("connect failed : ") ;
                exit(EXIT_FAILURE) ;
        }



	send(sock_fd,&i, sizeof(struct ioutc),0);

	shutdown(sock_fd,SHUT_WR);


	recv(sock_fd,(void*)&i,sizeof(struct ioutc),0);

	//state =0 complete, state = 1 error, state =2 time exceed
	int count =0;
	if(i.state == 0){
	
	for (filenum = 1; filenum < 11; filenum++){
                sprintf(tc_filenum, "%d",filenum);
                strcpy(tc_file, dir);
                strcat(tc_file, "/");
                strcat(tc_file, tc_filenum);
                strcat(tc_file, ".out");
                if(strcmp(i.out[filenum-1].output, readFile(tc_file)) ==0)
	                count++;

        }
	stu.result = count;

	printf("Number of test cases passed is : %d\n",stu.result);
	data = "It's complete, please type in password";
	len=strlen(data);
	send(conn,data,len,0);
	shutdown(conn,SHUT_WR);
	close(conn);
//	create a new socket connection
//==================================back to submitter====================================
//	socket created new_socket

	 listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
        if (listen_fd == 0)  {
                perror("socket failed : ");
                exit(EXIT_FAILURE);
        }

        memset(&address, '0', sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY /* the localhost*/ ;
        address.sin_port = htons(port+1);
        if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
                perror("bind failed : ");
                exit(EXIT_FAILURE);
        }

        if (listen(listen_fd, 16 /* the size of waiting queue*/) < 0) {
                        perror("listen failed : ");
                        exit(EXIT_FAILURE);
                }

	 new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;

	while(recv(new_socket,buffer,1024,0)>0){}	
	
	if(strcmp(buffer,stu.password)==0){
		send(new_socket,&stu,sizeof(struct student),0);
		shutdown(new_socket,SHUT_WR);
	}
	else{
	printf("This is the wrong Password!\n");
		
	}

	}
	else if(i.state ==1){
		printf("Compilation error\n");
	}
	else if(i.state ==2)
	{
		printf("Time takes too long\n");
	}
}

int 
main(int argc, char const *argv[]) 
{ 
	int listen_fd, new_socket ; 
	struct sockaddr_in address; 
	int opt; 
	int addrlen = sizeof(address); 
	uint16_t portnum;
	char buffer[1024] = {0}; 
	char ipadd[30];
	char wport[30];
//	char dir[50];
	uint16_t portnumworker;
	while((opt = getopt(argc,argv,"p:w:"))!=-1){
	switch(opt)
	{
		case 'p':
		portnum = atoi(optarg);
		break;
		case 'w':
		memcpy(ipadd,optarg,30);
		char *ptr = strtok(ipadd,":");
		strcpy(ip,ptr);
		ptr = strtok(NULL,"\n");
		strcpy(wport,ptr);
		portnumworker = atoi(wport);
		break;
	
			
	}
		
	
	}
	strcpy(dir,argv[5]);
	listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (listen_fd == 0)  { 
		perror("socket failed : "); 
		exit(EXIT_FAILURE); 
	}
	
	memset(&address, '0', sizeof(address)); 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY /* the localhost*/ ; 
	address.sin_port = htons(portnum); 
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed : "); 
		exit(EXIT_FAILURE); 
	} 

	while (1) {
		if (listen(listen_fd, 16 /* the size of waiting queue*/) < 0) { 
			perror("listen failed : "); 
			exit(EXIT_FAILURE); 
		} 

		new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
		if (new_socket < 0) {
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 

		if (fork() > 0) {
			child_proc(new_socket,portnumworker,portnum) ;
		}
		else {
			close(new_socket) ;
		}
	}
} 

