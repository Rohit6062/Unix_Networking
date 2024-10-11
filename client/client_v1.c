#include"../my_header.h"
#define buff_size 10240
void send_file(FILE* file_to_send,int sockfd,const SA* servaddr,socklen_t servlen){
    fseek(file_to_send,0,SEEK_END);
    ui file_size = ftell(file_to_send);
    fseek(file_to_send,0,SEEK_SET);
    int n;
    byte inc=0;
    byte** packet_buffer = (byte**) malloc(sizeof(byte*)*7);
    for(int i=0;i<7;i++)
        packet_buffer[i]=(byte*)calloc(sizeof(byte),buff_size+1);
    ui curr_size = file_size;
    ui i=0;
    ui ds=0;
    while((n=fread(packet_buffer[i]+1,1,buff_size,file_to_send))){
        printf("n = %d %d %d %d\n", n,i,inc,i+inc);
        curr_size-=n;
        packet_buffer[i][0] = i+inc;
        i++;
        if(i==3){
            i=0;
            create_xor(packet_buffer,buff_size,inc);
            inc +=8;
            for(int k=0;k<7;k++){
                if(sendto(sockfd,packet_buffer[k],buff_size+1,0,servaddr,servlen)==-1)
                    err_quit("write err");
            }
            printf("Data Sent %d\n",curr_size);
            for(int k=0;k<1000000;k++){};
        }
    }
    if(i>0){
        if(i==1)
            bzero(packet_buffer[2]+1,buff_size),
            bzero(packet_buffer[1]+1,buff_size),
            create_xor(packet_buffer,buff_size,inc);
        else if(i==2) 
            bzero(packet_buffer[2]+1,buff_size),
            create_xor(packet_buffer,buff_size,inc);
        packet_buffer[0][0]=255;
        for(int k=0;k<7;k++)
            if(sendto(sockfd,packet_buffer[k],buff_size,0,servaddr,servlen)==-1)
                err_quit("write err");
            printf("last one\n");
    }
}

int main(int argc,char** argv){
    FILE* f = fopen("vid","r");
    int sockfd;
    struct sockaddr_in servaddr;
    if(argc != 2)
        err_quit("user: udpcli <IPaddress>");
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    send_file(f,sockfd,(SA*)&servaddr,sizeof(servaddr));
    return 0;
}
