#ifndef __unp_h
#define __unp_h
#define MAXLINE 4096
#define LISTENQ 107
#define SERV_PORT 9877
typedef struct sockaddr SA;
typedef unsigned int ui;
typedef unsigned char byte;
typedef unsigned short si;
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
#include"stdbool.h"
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

typedef struct{
    byte* hello;
    byte* name;
    long file_size;
    si buffer_len;
    FILE* fp;
}file_info;


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


void str_echo_udp(int sockfd, SA* cli_addr,socklen_t clilen){
    socklen_t len;
    int n;
    char buffer[MAXLINE];
    while(1){
        len = clilen;
        n = recvfrom(sockfd,buffer,MAXLINE,0,cli_addr,&len);
        buffer[n]=0;
        printf("buffer = %s\n", buffer);
        printf("n = %d\n", n);
        if((n = sendto(sockfd,buffer,n,0,cli_addr,clilen))<0)err_quit("sendto in str_echo_udp: ");
        printf("n = %d\n", n);
    }
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

void pstring(byte* s,ui len){
    for(int i=0;i<len;i++)printf("%c",s[i]);
}

byte* mystrcpy(byte* a,byte* b,ui len)
{
    for(ui i=0;i<len;i++){
        a[i] = b[i];
    }
    return a;
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



bool recover3(byte** a, ui len,bool* avail_packets){
    byte cnt=0;
    for(int i=0;i<7;i++)if(!avail_packets[i])cnt++;
    if(cnt>3)return false;
    if(avail_packets[0]){
        if(avail_packets[1]){
            if(avail_packets[2])cnt = 1; 
            else if(avail_packets[4])a[2] = xor(a[1],a[4],len);
            else if(avail_packets[5])a[2] = xor(a[0],a[5],len);  
            else a[2] = xor(xor(a[0],a[1],len),a[6],len);
        }
        else if(avail_packets[2]){
            if(avail_packets[3])a[1] = xor(a[0],a[3],len); 
            else if(avail_packets[4])a[1] = xor(a[2],a[4],len);
            else a[1] = xor(a[6],xor(a[0],a[2],len),len);
        }
        else if(avail_packets[3]){
            a[1]=xor(a[0],a[3],len);
            if(avail_packets[4])a[2]=xor(a[1],a[4],len);
            else if(avail_packets[5])a[2]=xor(a[0],a[5],len);
            else a[2]=xor(a[6],xor(a[0],a[1],len),len);
        }
        else{
            a[2] = xor(a[5],a[0],len);
            a[1] = xor(a[4],a[2],len);
        }
    }
    else if(avail_packets[1]){
        if(avail_packets[2]){
            if(avail_packets[3]) a[0] = xor(a[1],a[3],len);
            else if(avail_packets[5]) a[0] = xor(a[5],a[2],len);
            else a[0] = xor(a[6],xor(a[0],a[1],len),len);
        }
        else if(avail_packets[3]){
            a[0] = xor(a[3],a[1],len);
            if(avail_packets[4]) a[2] = xor(a[4],a[1],len); 
            else if(avail_packets[5]) a[2] = xor(a[5],a[0],len);
            else a[2] = xor(a[6],xor(a[0],a[1],len),len);
        }
        else{
            a[2] = xor(a[4],a[1],len);
            a[0] = xor(a[5],a[2],len);
        }
    }
    else if(avail_packets[2]){
        if(avail_packets[3]){
            if(a[4]) a[1] = xor(a[4],a[2],len),a[0] = xor(a[3],a[1],len);
        }
        else{
            a[1] = xor(a[4],a[2],len);
            a[0] = xor(a[5],a[2],len);
        }
    }
    else{
        a[0] = xor(a[4],a[6],len);
        a[1] = xor(a[3],a[0],len);
        a[2] = xor(a[4],a[1],len);
    }
    return true;
}

void create_xor(byte** packets,ui len,int inc){
    packets[3] = xor(packets[0],packets[1],len);
    packets[3][0]=3+inc;
    packets[4] = xor(packets[1],packets[2],len);
    packets[4][0]=4+inc;
    packets[5] = xor(packets[0],packets[2],len);
    packets[5][0]=5+inc;
    packets[6] = xor(packets[3],packets[2],len);
    packets[6][0]=6+inc;
}

file_info* init_file_info(){
    file_info* info = (file_info*)calloc(sizeof(file_info),1); 
    info->name = (byte*) malloc(sizeof(byte)*50);
    info->hello = malloc(sizeof(byte)*10);
    info->hello = "hello!";
    return info;
}

//
// int main(){
//     srand(time(NULL));
//     byte **packets = (byte**) calloc(sizeof(byte*),8);
//     for(int i=0;i<8;i++)packets[i]=(byte*) calloc(100, sizeof(byte));
//     byte r;
//     FILE* f = fopen("test.txt","r");
//     for(int i=1;i<=3;i++){
//         ui j=1;
//         r = fgetc(f);
//         packets[i][0]=i;
//         while(r!=10){
//             packets[i][j++]=r;
//             r = fgetc(f);
//         }
//     }
//     ui n = strlen(packets[1]);
//     create_xor(packets,n);
//     packets[1]=NULL;
//     packets[3]=NULL;
//     packets[4]=NULL;
//     recover3(packets,n);
//     for(int i=1;i<4;i++)pstring(packets[i],n);
//     return 0;
// }
//
