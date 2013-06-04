#include<stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define SERVPORT 3333
#define MAXDATASIZE 100 
int main(int argc, char *argv[])
{
    int sockfd; int recvbytes;
    char buf[MAXDATASIZE];
    struct hostent *host;
	
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket err \n");
        exit(1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVPORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.2");//*((struct in_addr *)host->h_addr);
    printf("serv_addr.sin_addr1: %s\n", inet_ntoa(serv_addr.sin_addr));
    bzero(&(serv_addr.sin_zero), 8);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("connect  err \n");
        exit(1);
    }
    if ((recvbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
    {
        printf("recv   err!\n");
        exit(1);
    }
    buf[recvbytes] = '\0';
    printf("\nReceived: [%s]\n", buf);
    close(sockfd);
    return 1;
}
