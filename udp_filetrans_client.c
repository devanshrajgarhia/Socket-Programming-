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

	char* filename="file.txt";

	bzero((char*)&serverAddr,sizeof(serverAddr));
	serverAddr.sin_family=AF_INET;
	bcopy((char*)server->h_addr,(char*)&serverAddr.sin_addr,server->h_length);
	serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverAddr.sin_port=htons(portno);

	

	int recvlen=0;

	if(sendto(socketfd,filename,strlen(filename),0,(struct sockaddr*)&serverAddr,sizeof(serverAddr)) < 0){
		perror("Error in sending");
		exit(3);
	}

	char recvBuf[2048];
	size_t buflen=sizeof(recvBuf);
	recvlen=0;
	int serLen=sizeof(serverAddr);
	recvlen=recvfrom(socketfd,recvBuf,buflen,0,(struct sockaddr*)&serverAddr,&serLen);
	if(recvlen < 0){
		perror("Error in recieving");
		exit(1);
	}

	recvBuf[recvlen]='\0';
	printf("mssg recvd from server is %s\n",recvBuf);

	char not_found[30]="NOT FOUND ";
	strcat(not_found,filename);
	if(strcmp(not_found,recvBuf)==0){
		printf("File %s not found\n",filename);
		exit(1);
	}
	printf("SERVER: %s",recvBuf);
	FILE* fptr;
	fptr = fopen("recv.txt","w");
	if(fptr==NULL){
		printf("cannot create the file\n");
		exit(5);
	}

	int i = 1;
    //client runs till END is received from server
    while(strcmp(recvBuf,"END")!=0)
    {
        char word[40] = "Word";
        char integer_string[32]; 
        sprintf(integer_string, "%d", i++);
        strcat(word, integer_string);
        printf("CLIENT : %s\n\n",word );
        //send Wordi to server 
        sendto(socketfd, (const char *)word, strlen(word), 0,(struct sockaddr *) &serverAddr, sizeof(serverAddr));
        //receive the ith word
        recvlen = recvfrom(socketfd,recvBuf,buflen,0,(struct sockaddr*)&serverAddr,&serLen);
        recvBuf[recvlen] = '\0';
        printf("SERVER : %s\n\n",recvBuf);
        //print wordi in new file
        fprintf(fptr, "%s\n", recvBuf);
         
    }
    close(socketfd);

	//printf("Recv from %s:%d: ",inet_ntoa(serverAddr.sin_addr),ntohs(serverAddr.sin_port));


	return 0;



}
