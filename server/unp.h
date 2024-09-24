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

void err_quit(char* str){
    perror(str);
    exit(0);
}


