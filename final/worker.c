// Partly taken from https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

char cmd[100];
char buf[10];
int filenum=1;
pid_t child_pid;
int st;

struct inoutc{
	char targetc[4096];
	struct {
		char input[100];
	}in[11];

	struct {
		char output[1024];
	}out[11];
	int state;	//0 for complete, 1 for compile error, 2 for time exceed
};

char *readFile(char *filename){
	FILE *f = fopen(filename, "r");
	
	fseek(f,0, SEEK_END);
	long length = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buffer = (char *) malloc(length +1);
	buffer[length] = '\0';
	fread(buffer, 1, length, f);
	fclose(f);
	return buffer;
}

int compilation(){
	if(system("gcc -o test test.c")!=0)
                return -1;
	else
		return 0;
}

void alrm_handler(int sig)
{
//	printf("time exceed for testcase no.%d\n",filenum);	
	exit(0);	
}

int execution(char *in){
	pid_t pid;
	struct itimerval t ;

	pid = fork();
	if(pid == 0){
		child_pid = getpid();
		sprintf(buf,"%d",filenum);
		strcat(buf, ".out");

		int fd = open(buf, O_WRONLY | O_CREAT, 0644);
		dup2(fd,1);
		close(fd);
		
		sprintf(buf,"%d",filenum);
                strcat(buf, ".in");
		fd = open(buf, O_RDONLY);

		if(dup2(fd, 0)<0)
			printf("failed to redirect %s to standard input\n", buf);
		close(fd);
		

	        signal(SIGALRM, alrm_handler) ;

	        t.it_value.tv_sec = 3 ;
	        t.it_value.tv_usec = 0 ; // 3.0sec
	        t.it_interval = t.it_value ;

		setitimer(ITIMER_REAL, &t, 0x0);

		execl("./test","test", (char*) 0x0);
//		st++;
	}

	wait(0x0);
	return 0;
}


void
child_proc(int conn)
{
	struct inoutc new;
	FILE *cp;
	int i, s ;
	char filename[15];
	
	while ( (s = recv(conn, (void*)&new, sizeof(struct inoutc), 0)) > 0 ) { }

	cp = fopen("./test.c","w");
	
	for(i = 0; (i<sizeof(new.targetc)) && (fputc(new.targetc[i], cp)!=EOF) ; i++);

	fclose(cp);

	new.state = 0;
	if(compilation() == -1){
		new.state = -1;
		send(conn, (void*)&new, sizeof(struct inoutc), 0);
		printf("compilation error\n");
		return;
	} // passing the state of compilation error
	//executing testcase in .in files
	for(filenum = 1; filenum < 11; filenum++){
		sprintf(filename,"%d",filenum);
		strcat(filename, ".in");
		cp = fopen(filename, "w");
		for(i = 0; (i<sizeof(new.in[filenum-1].input)) && (fputc(new.in[filenum-1].input[i], cp)!=EOF) ; i++);

		fclose(cp);

		execution(new.in[filenum-1].input);

		strcpy(filename, "./");
		sprintf(buf,"%d",filenum);
		strcat(filename, buf);
		strcat(filename, ".out");
		strcpy(new.out[filenum-1].output,readFile(filename));
	}

	system("rm *.in");
	system("rm *.out");
	system("rm test");
	system("rm test.c");

	s = send(conn, (void*)&new, sizeof(struct inoutc), 0);
	printf("test result sent\n");
	shutdown(conn, SHUT_WR) ;
	
	return;
}

int 
main(int argc, char const *argv[]) 
{ 
	extern char *optarg;
	int listen_fd, new_socket ; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[1024] = {0};
	FILE *fp;
	int c;
	uint16_t portnum;

	c=getopt(argc,argv,"p:");
	switch(c){
		case 'p':
			portnum = atoi(optarg);
			break;
	}
//	pthread_t thread1, thread2;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (listen_fd == 0)  { 
		perror("socket failed : "); 
		exit(EXIT_FAILURE); 
	}
	
	memset(&address, '0', sizeof(address)); 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY ; 
	address.sin_port = htons(portnum); 
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed : "); 
		exit(EXIT_FAILURE); 
	} 

	while (1) {
		if (listen(listen_fd, 16 ) < 0) { 
			perror("listen failed : "); 
			exit(EXIT_FAILURE); 
		} 

		new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
		if (new_socket < 0) {
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 

		if (fork() > 0) {
			child_proc(new_socket) ;
			
		}
		else {
			close(new_socket) ;
		}
	}	
	return 0;
} 

