#include"../my_header.h"
int main(int argc,char** argv){
    FILE *f;
    int n;
    int total_send=0;
    if((f= fopen("test.txt","r"))==NULL)err_quit("FILE NOT FOUND");
    byte send_buffer[1025] = {0};
    byte rec_buffer[1025] = {0};
    int sockfd;
    struct sockaddr_in servAddr;
    if(argc<2){
        err_quit("Ip Not provided");
    }
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SERV_PORT);
    if(inet_pton(AF_INET,argv[1],(SA*)&servAddr.sin_addr)<0)err_quit("inet err"); 
    if(connect(sockfd,(SA*)&servAddr,sizeof(servAddr))<0)err_quit("connect err");
    printf("Setup Done\n");
    while((n = fread(send_buffer,1,1024,f))>0){
        send_buffer[n]=0;
        if((n = sendto(sockfd,send_buffer,n,0,(SA*)&servAddr,sizeof(servAddr)))<0)err_quit("sendto err");
        total_send+=n;
        if((n = recvfrom(sockfd,rec_buffer,sizeof(recvfrom),0,NULL,NULL)<0))err_quit("recvfrom err");
        rec_buffer[n] = 0;
        fputs(rec_buffer,stdout);
    }
    printf("Total Data of %d bytes transfered!\n",total_send);
    return 0;
}
