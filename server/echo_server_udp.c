#include"../my_header.h"
int main(){
    int listen_fd,conn_fd;
    pid_t child_pid;  
    socklen_t clilen;  
    struct sockaddr_in serv_addr,cli_addr;
    listen_fd = socket(AF_INET,SOCK_DGRAM,0);
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(SERV_PORT);
    bind(listen_fd,(SA*)&serv_addr,sizeof(serv_addr));
    if((listen(listen_fd,5)>0))err_quit("listen: ");
    while(1){
        clilen = sizeof(cli_addr);
        if((conn_fd = accept(listen_fd,(SA*)&cli_addr,&clilen)<0))err_quit("accept error");
        if((child_pid = fork())==0){
            close(listen_fd);
            str_echo_udp(conn_fd); 
            printf("Done\n");
            exit(0);
        } 
    }
    return 0;
}
