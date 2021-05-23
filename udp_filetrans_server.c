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
	size_t buflen=sizeof(recvBuf);
	int recvlen=0;
	int clilen=sizeof(clientAddr);
	recvlen=recvfrom(socketfd,recvBuf,buflen,0,(struct sockaddr*)&clientAddr,&clilen);
	if(recvlen< 0){
		perror("Error in recieving");
		exit(1);
	}

	recvBuf[recvlen]='\0';
	printf("\nClient: %s\n",recvBuf);
	FILE* fptr=fopen("file.txt","r");
	if(fptr==NULL){
		printf("\nServer: File not found\n");
		char notfound[30] = "NOTFOUND ";
		strcat(notfound, recvBuf);
		sendto(socketfd,notfound,strlen(notfound),0,(struct sockaddr*)&clientAddr,sizeof(clientAddr));
		exit(5);
	}

	char* temp;
	size_t l = 0;
	getline(&temp, &l, fptr);
	// print the word on server display
	printf("\nSERVER: %s\n", temp);

	//send this word to client
	sendto(socketfd,temp,strlen(temp),0,(struct sockaddr*)&clientAddr,sizeof(clientAddr));
	
	// now check if this word is END, if yes stop else scan next word and send that
	while(strcmp(temp, "END"))
	{
		printf("\nSERVER: Sent the message to the client, waiting for reply..\n");	
		
		char clientMessage[500];
		int n = recvfrom(socketfd, (char *)clientMessage, sizeof(clientMessage), 0, (struct sockaddr *) &clientAddr, &clilen);
		clientMessage[n] = '\0';
		printf("\nCLIENT: %s\n", clientMessage);
	
		// read the next word from the file
		fscanf(fptr, "%s", temp);
		
		// print the word on server display
		printf("\nSERVER: %s\n", temp);
		
		//send this word to client
		sendto(socketfd,temp,strlen(temp),0,(struct sockaddr*)&clientAddr,sizeof(clientAddr));

	}

	while(1)
	{
		clilen=sizeof(clientAddr);
		if(recvlen=recvfrom(socketfd,recvBuf,buflen,0,(struct sockaddr*)&clientAddr,&clilen) < 0){
			perror("Error in recieving");
			exit(1);
		}

		recvBuf[recvlen]=0;
		printf("Recv from %s:%d: ",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
		printf("%s\n",recvBuf);

		if(sendto(socketfd,recvBuf,recvlen,0,(struct sockaddr*)&clientAddr,sizeof(clientAddr))<0){
			perror("Error in sending");
			exit(2);
		}
	}

	return 0;



}
