#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define REMOTEPORT 1234
#define BUFLEN 1024
int main()
{
  int sockfd;
  char buffer[BUFLEN];
  char *messageStr = "a very important Good Morning message with 100s of emojis!!!";
  struct sockaddr_in   receiverAddr;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  memset(&receiverAddr, 0, sizeof(receiverAddr));
  receiverAddr.sin_family = AF_INET;
  receiverAddr.sin_port = htons(REMOTEPORT);
  receiverAddr.sin_addr.s_addr = INADDR_ANY;
  sendto(sockfd, (const char *)messageStr, strlen(messageStr), 0, (const struct sockaddr *) &receiverAddr, sizeof(receiverAddr));
  printf("the very important good morning message sent.....\n");
  close(sockfd);
  return 0;
}
