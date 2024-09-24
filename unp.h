#ifndef __unp_h
#define __unp_h
#define MAXLINE 4096
#define LISTENQ 107
#define SERV_PORT 9877
typedef struct sockaddr SA;
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


