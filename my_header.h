#ifndef __unp_h
#define __unp_h
#define MAXLINE 4096
#define LISTENQ 107
#define SERV_PORT 9877
typedef struct sockaddr SA;
typedef unsigned int ui;
typedef unsigned char byte;
// #include "../config.h"

/* if anything changes in this list of #include must change acsite.m4 also , for configuration tests.
 * */

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<fcntl.h>
#include<netdb.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/uio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/un.h>


#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef HAVE_POLL_H 
#include <poll.h>
#endif 

#ifdef HAVE_SRING_H
#include <string.h>
#endif 


/* three headers are normally needed for socket/file ioctl's:
 * <sys/ioctl.h> 
 * <sys.filio.h>
 * <sys/sockio.h>
 * */

#ifdef HAVE_SYS_IOCTL_H
#include<sys/ioctl.h>
#endif

#ifdef HAVE_SYS_FILIO_H
#include<sys/filio.h>
#endif

#ifdef HAVE_SYS_SOCKIO_H
#include<sys/sockio.h>
#endif

#ifdef HAVE_PTHREAD_H
#include<pthread.h>
#endif 
#endif 

void err_quit(char*);
void str_cli(FILE *fp, int sockfd);
void str_echo(int sockfd);
ssize_t writen(int fd,const void *vptr,size_t n);
ssize_t readline(int fd,void* vptr, size_t maxlen);

ssize_t readline(int fd,void* vptr, size_t maxlen){
    ssize_t n,rc;
    char c,*ptr;
    ptr = vptr;
    for(n=1;n<maxlen;n++){
    again:
        if((rc=read(fd,&c,1))==1){
            *ptr++=c;
            if(c=='\n')break;
        }else if(rc==0){
            *ptr=0;
            return n-1;
        }
        else{
            if(errno=EINTR)
                goto again;
            return -1;
        }
    }
    *ptr =0;
    return n;
}


ssize_t udp_writen(int fd,const void* vptr, size_t n){
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while(nleft > 0){
        // if((nwritten = sendto(fd,)<0){
            if(nwritten<0 && errno == EINTR)
                nwritten = 0; 
            else 
                return -1;
        // }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}



ssize_t writen(int fd,const void* vptr, size_t n){
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while(nleft > 0){
        if((nwritten = write(fd,ptr,nleft))<=0){
            if(nwritten<0 && errno == EINTR)
                nwritten = 0; 
            else 
                return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

void err_quit(char* str){
    perror(str);
    exit(0);
}
void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    while (fgets(sendline, MAXLINE, fp) != NULL) {
        if(writen(sockfd, sendline, strlen(sendline))==-1)err_quit("write err");
        if(readline(sockfd, recvline, MAXLINE) == 0)
            err_quit("str_cli: server terminated prematurely");
        fputs(recvline, stdout);
    }
}
void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];
again:
    while (( n = read(sockfd, buf, MAXLINE)) > 0)
        writen(sockfd, buf, n);
    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
    err_quit("str_echo: read error");
}


void str_echo_udp(int sockfd){
    ssize_t n;
    char buffer[MAXLINE];
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    bzero(&cli_addr,sizeof(cli_addr));
loop:
    while((n = recvfrom(sockfd,buffer,MAXLINE,0,NULL,NULL))>0)
        buffer[n]=0,send(sockfd,buffer,n,0);
    if(n<0 && errno == EINTR)
        goto loop;
    else if(n<0)err_quit("recvfrom error => ");
}
byte* xor(byte* a, byte* b,ui len){
    if(!a || !b)return NULL;
    byte* output = (byte*) malloc(sizeof(byte)*len);
    for(int i=1;i<len;i++)
    {
        output[i] = b[i] ^ a[i];
    }
    return output;
}



void recover3(byte** a, ui len){
    if(a[1]){
        if(a[2]){
            if(a[3])return; 
            else if(a[5])a[3] = xor(a[2],a[5],len);
            else if(a[6])a[3] = xor(a[1],a[6],len);  
            else a[3] = xor(xor(a[1],a[2],len),a[7],len);
        }
        else if(a[3]){
            if(a[4])a[2] = xor(a[1],a[4],len); 
            else if(a[5])a[2] = xor(a[3],a[5],len);
            else a[2] = xor(a[7],xor(a[1],a[3],len),len);
        }
        else if(a[4]){
            a[2]=xor(a[1],a[4],len);
            if(a[5])a[3]=xor(a[2],a[5],len);
            else if(a[6])a[3]=xor(a[1],a[6],len);
            else a[3]=xor(a[7],xor(a[1],a[2],len),len);
        }
        else{
            a[3] = xor(a[6],a[1],len);
            a[2] = xor(a[5],a[3],len);
        }
    }
    else if(a[2]){
        if(a[3]){
            if(a[4]) a[1] = xor(a[2],a[4],len);
            else if(a[6]) a[1] = xor(a[6],a[3],len);
            else a[1] = xor(a[7],xor(a[1],a[2],len),len);
        }
        else if(a[4]){
            a[1] = xor(a[4],a[2],len);
            if(a[5]) a[3] = xor(a[5],a[2],len); 
            else if(a[6]) a[3] = xor(a[6],a[1],len);
            else a[3] = xor(a[7],xor(a[1],a[2],len),len);
        }
        else{
            a[3] = xor(a[5],a[2],len);
            a[1] = xor(a[6],a[3],len);
        }
    }
    else if(a[3]){
        if(a[4]){
            if(a[5]) a[2] = xor(a[5],a[3],len),a[1] = xor(a[4],a[2],len),printf("here\n");
        }
        else{
            a[2] = xor(a[5],a[3],len);
            a[1] = xor(a[6],a[3],len);
        }
    }
    else{
        a[1] = xor(a[5],a[7],len);
        a[2] = xor(a[4],a[1],len);
        a[3] = xor(a[5],a[2],len);
    }
}

void create_xor(byte** packets,ui len){
    packets[4] = xor(packets[1],packets[2],len);
    packets[4][0]=4;
    packets[5] = xor(packets[2],packets[3],len);
    packets[5][0]=5;
    packets[6] = xor(packets[1],packets[3],len);
    packets[6][0]=6;
    packets[7] = xor(packets[4],packets[3],len);
    packets[7][0]=7;
}
void pstring(byte* s,ui len){
    for(int i=1;i<len;i++)printf("%c",s[i]);
    printf("\n");
}
/*
int main(){
    srand(time(NULL));
    byte **packets = (byte**) calloc(sizeof(byte*),8);
    for(int i=0;i<8;i++)packets[i]=(byte*) calloc(100, sizeof(byte));
    byte r;
    FILE* f = fopen("test.txt","r");
    for(int i=1;i<=3;i++){
        ui j=1;
        r = fgetc(f);
        packets[i][0]=i;
        while(r!=10){
            packets[i][j++]=r;
            r = fgetc(f);
        }
    }
    ui n = strlen(packets[1]);
    create_xor(packets,n);
    packets[2]=NULL;
    packets[3]=NULL;
    packets[4]=NULL;
    recover3(packets,n);
    for(int i=1;i<4;i++)pstring(packets[i],n);
    return 0;
}
*/

