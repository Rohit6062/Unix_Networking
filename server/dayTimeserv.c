#include"unp.h"
#include<time.h>

int main(int argc,char** argv){
    int listenfd,connfd;
    FILE* urecv = fopen("urecv.c","r");
    if(urecv==NULL)err_quit("file error");
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    time_t ticks;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13);
    if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)err_quit("bind error");
    listen(listenfd,LISTENQ);
    for(;;){
        connfd = accept(listenfd,(struct sockaddr*)NULL,NULL);
        ticks = time(NULL);
        // snprintf(buff, sizeof(buff), "%.24s\r\n",ctime(&ticks));
        while(fread(buff,1,MAXLINE-1,urecv))write(connfd,buff,strlen(buff));
        // if(write(connfd,buff,strlen(buff)) < 0)err_quit("write Error");          
        close(connfd);
        fseek(urecv,0,SEEK_SET);
    }
    return 0;
}
