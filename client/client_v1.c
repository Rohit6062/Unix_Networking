#include"../my_header.h"
#define buff_size 1024

void send_file(FILE* file_to_send,int sockfd,const SA* servaddr,socklen_t servlen){
    fseek(file_to_send,0,SEEK_END);
    ui file_size = ftell(file_to_send);
    ui i=0;
    byte** packet_buffer = (byte**) malloc(sizeof(byte*)*8);
    for(int i=0;i<8;i++)packet_buffer[i]=(byte*)malloc(sizeof(byte)*buff_size+1);
    ui curr_size = file_size;
    while(curr_size){
        for(i=0;i<3;i++){
            fgets(packet_buffer[i+1]+1,buff_size,file_to_send);
            printf("data=> %s\n",packet_buffer[i+1]+1);
            packet_buffer[i+1][0] = i+1;
        }
        if(i<3){printf("need to handle this\n");exit(0);}
        else 
            create_xor(packet_buffer,buff_size);
        for(int i=1;i<8;i++){
            if(sendto(sockfd,packet_buffer[i],buff_size,0,servaddr,servlen)==-1)err_quit("write err");
        }
    }
}

int main(int argc,char** argv){
    FILE* f = fopen("test.txt","r");
    int sockfd;
    struct sockaddr_in servaddr;
    if(argc != 2)
        err_quit("user: udpcli <IPaddress>");
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if(inet_pton(AF_INET,argv[1],(SA*)&servaddr.sin_addr)<=0)err_quit("inet error: ");
    if(connect(sockfd,(SA*)&servaddr,sizeof(servaddr))<0)err_quit("connect Error:");
    send_file(f,sockfd,(SA*)&servaddr,sizeof(servaddr));
    return 0;
}
