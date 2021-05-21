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

	if(argc<2){
		return 0;
	}
	int portno=atoi(argv[1]);
	socketfd=socket(AF_INET,SOCK_DGRAM,0);
	if(!socketfd){
		perror("Error in opening a socket");
		exit(1);
	}
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverAddr.sin_port=htons(portno);

	if(bind(socketfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr))<0){
		perror("failed to bind");
		return 0;
	}

	int optval=1;
	setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int));

	struct sockaddr_in clientAddr;
	char recvBuf[2048];
	int buflen=sizeof(recvBuf);
	int recvlen=0;
	int clilen=sizeof(clientAddr);

	while(1)
	{
		clilen=sizeof(clientAddr);
		if(recvlen=recvfrom(socketfd,recvBuf,buflen,0,(struct sockaddr*)&clientAddr,&clilen) < 0){
			perror("Error in recieving");
			exit(1);
		}

		//recvBuf[recvlen]=0;
		printf("Recv from %s:%d: ",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
		printf("%s\n",recvBuf);

		if(sendto(socketfd,recvBuf,recvlen,0,(struct sockaddr*)&clientAddr,sizeof(clientAddr))<0){
			perror("Error in sending");
			exit(2);
		}
	}

	return 0;



}
