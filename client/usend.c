#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define REMOTEPORT 1234
#define BUFLEN 10240
int main()
{
    char* name = (char*) malloc(sizeof(char)*50);
    printf("Enter name of File to send: \n");
    scanf("%s",name);
    FILE* fp  = fopen(name,"r");
    if(!fp){printf("Failed to open file\n");exit(0);}
    int sockfd;
    char buffer[BUFLEN];
    char *messageStr = (char*) malloc(sizeof(char)*BUFLEN);
    struct sockaddr_in   receiverAddr;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    bzero(&receiverAddr, sizeof(receiverAddr));
    receiverAddr.sin_family = AF_INET;
    printf("AF_INET = %d\n", AF_INET);
    receiverAddr.sin_port = htons(REMOTEPORT);
    printf("receiverAddr.sin_addr = %d\n", receiverAddr.sin_port);
    receiverAddr.sin_addr.s_addr = INADDR_ANY;
    printf("receiverAddr.sin_addr.s_addr = %d\n", receiverAddr.sin_addr.s_addr);

    sendto(sockfd, (const char *)name, strlen(name), 0, (const struct sockaddr *) &receiverAddr, sizeof(receiverAddr));
    sleep(0.1);
    while(fread(buffer,BUFLEN,1,fp)){
        printf("%d\n",strlen(buffer));
        sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *) &receiverAddr, sizeof(receiverAddr));
        sleep(0.1);
    }
    strcpy(buffer,"bye");
    sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *) &receiverAddr, sizeof(receiverAddr));
    close(sockfd);
    return 0;
}
