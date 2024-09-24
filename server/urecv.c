#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFLEN 10240
#define LISTENPORT 1234

int main()
{
int sockfd, len, n;
char buffer[BUFLEN];
FILE* out;
struct sockaddr_in receiverAddr, senderAddr;
if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
    perror("socket system call failed");
    exit(EXIT_FAILURE);
}
memset(&receiverAddr, 0, sizeof(receiverAddr));
memset(&senderAddr, 0, sizeof(senderAddr));

receiverAddr.sin_family = AF_INET;
receiverAddr.sin_addr.s_addr = INADDR_ANY;
    printf("INADDR_ANY = %d\n", INADDR_ANY);
receiverAddr.sin_port = htons(LISTENPORT);

if(bind(sockfd, (const struct sockaddr *)&receiverAddr, sizeof(receiverAddr)) < 0)
{
    perror("bind syscall failed");
    exit(EXIT_FAILURE);
}
len = sizeof(senderAddr);
/* blocking subroutines or blocking syscalls
scanf("%d", &n); */
printf("waiting for data to be received: \n");
n = recvfrom(sockfd, (char *)buffer, BUFLEN, MSG_WAITALL, (struct sockaddr *) &senderAddr, &len);
buffer[n] = '\0';

// n = recvfrom(sockfd, (char *)buffer, BUFLEN, MSG_WAITALL, (struct sockaddr *) &senderAddr, &len);
out = fopen(buffer,"w");
while(strcmp("bye",buffer)){
    n = recvfrom(sockfd, (char *)buffer, BUFLEN, MSG_WAITALL, (struct sockaddr *) &senderAddr, &len);
    printf("%d\n",n);
    buffer[n] = '\0';
    fwrite(buffer,strlen(buffer),1,out);
    printf("data received: %s\n", buffer);
}
fclose(out);
return 0;
}

/*

these are just source codes
urecv.c
usend.c
systeminternal.c

these are the programs [executables]
urecv
usend
systeminternal

$./urecv

*/

