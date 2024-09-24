#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
typedef unsigned char byte;
void error(const char* a){
    perror(a);
    exit(1);
}

int main(int argc,char* argv[]){
    if(argc<2){
        fprintf(stderr,"Provid Port No \n");
        exit(1);
    }
    int sockfd;
    int newsockfd;
    int portno;
    int n;
    char * buffer = (char*) malloc(sizeof(char)*255);
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    
    sockfd = socket(AF_INET,SOCK_DGRAM,1);
    if(sockfd < 0)error("Error opening socket.");
    bzero((byte*) &serv_addr , sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port  = htons(portno);
    
    if(bind(sockfd,(struct sockaddr*) &serv_addr, sizeof(serv_addr))<0)error("binding failed");

    // file dicriptor and max connnection count
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd , (struct sockaddr*) &cli_addr,&clilen);
    if(newsockfd<0)error("newsockfd failed");

    while(1){
        bzero(buffer,255);
        n = read(newsockfd,buffer,255);
        if(n<0)error("read failed");
        printf("client : %s",buffer);
        bzero(buffer,255);
        fgets(buffer,255,stdin);
        n = write(newsockfd, buffer ,strlen(buffer));
        if(n<0)error("write failed");
        if(strcmp("Bye",buffer)==0){break;}
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}
