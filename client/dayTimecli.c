#include"unp.h"
void err_quit(char* str){
    perror(str);
    exit(0);
}

int main(int argc,char** argv){
    int sockfd,n;
    char recvlin[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if(argc != 2)err_quit("usage: a.out <IPadress>");
    if( (sockfd= socket(AF_INET, SOCK_STREAM,0))<0)err_quit("socket error");
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);
    if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr) <=0)err_quit("inet_pton error");
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        err_quit("connect error");
    }
    while(1){
        n=read(sockfd,recvlin,MAXLINE);
        if(n==0)break;
        recvlin[n]=0;
        if(fputs(recvlin,stdout)==EOF)err_quit("fputs error");
    }
    if(n<0)printf("here \n"),err_quit("read error");
    return 0;
}
