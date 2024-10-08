#include"../unp.h"

int main(int argc, char **argv)
{
    FILE* f = fopen("test.txt","r");
    int sockfd;
    struct sockaddr_in servaddr;
    if (argc != 2)
    err_quit("usage: tcpcli <IPaddress>");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0)err_quit("inet eror:");
    if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr))<0)err_quit("connect_Error");
    str_cli(f, sockfd);
    exit(0);
}
/* do it all */
