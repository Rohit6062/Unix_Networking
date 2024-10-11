#include"../my_header.h"

void recieve_file(int listen_fd,SA* clie_addr,socklen_t* clilen,FILE* fp){
    if(!fp)return ;
    fseek(fp,0,SEEK_SET);
    int l=1;
    int buff_len = 10240;
    byte** packets = (byte**) calloc(sizeof(byte*),7);
    for(int i=0;i<7;i++)packets[i] = (byte*) calloc(buff_len+1, sizeof(byte));
    byte* buffer = (byte*) calloc(sizeof(byte),buff_len+1);
    bool* avail_packets = (bool*) calloc(sizeof(bool),7);
    byte inc = 0;
    int file_size = 3756532;
    int i=0;
    int tmp,som;
    printf("started recieving: \n");
    while(file_size>0){
        if( (som = recvfrom(listen_fd,buffer,buff_len+1,0,clie_addr,clilen)) < 0)err_quit("recvfrom error");
        tmp = buffer[0];
        if(tmp >= (inc+6)){
            if(recover3(packets,buff_len+1,avail_packets)==false)err_quit("recover3 err");
            inc+=8;
            fwrite(packets[0]+1,1,buff_len,fp);
            fwrite(packets[1]+1,1,buff_len,fp);
            fwrite(packets[2]+1,1,buff_len,fp);
            file_size -= ((buff_len)*3);
            bzero(avail_packets,7);
        }
        else{
            if( inc > tmp ) continue;
            mystrcpy(packets[tmp - inc],buffer,buff_len+1);
            avail_packets[tmp - inc] = true;
        }
        printf("file_size = %d\n", file_size);
    }
}

int main(){
    int listen_fd;
    socklen_t clilen;
    struct sockaddr_in serv_addr,clie_addr;
    listen_fd = socket(AF_INET,SOCK_DGRAM,0);
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);  
    serv_addr.sin_addr.s_addr  = htonl(INADDR_ANY);
    if(bind(listen_fd,(SA*)&serv_addr,sizeof(serv_addr))<0)err_quit("bind err");
    FILE* fp = fopen("new","w");
    recieve_file(listen_fd,(SA*)&clie_addr,&clilen,fp); 
    printf("File succefully recieved\n");
    fclose(fp);
    return 0;
}
