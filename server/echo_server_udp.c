#include"../my_header.h"
int main(){
    int listen_fd;
    struct sockaddr_in serv_addr,cli_addr;
    listen_fd = socket(AF_INET,SOCK_DGRAM,0);
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(SERV_PORT);
    if(bind(listen_fd,(SA*)&serv_addr,sizeof(serv_addr))<0)err_quit("bind:");
    str_echo_udp(listen_fd,(SA*)&cli_addr,sizeof(cli_addr)); 
    return 0;
}
