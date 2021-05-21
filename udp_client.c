#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>

int main(int argc,char* argv[]){

	struct sockaddr_in serverAddr;
	int socketfd;
	struct hostent *server;
	char *hostname;

	if(argc<3){
		return 0;
	}
	hostname=argv[1];
	int portno=atoi(argv[2]);
	socketfd=socket(AF_INET,SOCK_DGRAM,0);
	if(!socketfd){
		perror("Error in opening a socket");
		exit(1);
	}

	server=gethostbyname(hostname);
	if(server==NULL){
		fprintf(stderr,"Cant resolve hostname");
		exit(2);
	}

	bzero((char*)&serverAddr,sizeof(serverAddr));
	serverAddr.sin_family=AF_INET;
	bcopy((char*)server->h_addr,(char*)&serverAddr.sin_addr,server->h_length);
	serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverAddr.sin_port=htons(portno);

	char* buf="Hello there!!";
	int buflen=strlen(buf);

	int recvlen=0;

	if(sendto(socketfd,buf,buflen+1,0,(struct sockaddr*)&serverAddr,sizeof(serverAddr)) < 0){
		perror("Error in sending");
		exit(3);
	}

	char recvBuf[2048];
	buflen=sizeof(recvBuf);
	recvlen=0;
	int serLen=sizeof(serverAddr);

	if(recvlen=recvfrom(socketfd,recvBuf,buflen,0,(struct sockaddr*)&serverAddr,&serLen) < 0){
		perror("Error in recieving");
		exit(1);
	}

	recvBuf[recvlen]=0;
	printf("Recv from %s:%d: ",inet_ntoa(serverAddr.sin_addr),ntohs(serverAddr.sin_port));
	printf("%s\n",recvBuf);


	return 0;



}
