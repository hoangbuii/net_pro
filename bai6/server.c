#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8080
#define MAXSIZE 1024

int main(int argc, char *argv[])
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, clientaddr;
    socklen_t len = sizeof(clientaddr);
    char buffer[MAXSIZE];
    char filename[MAXSIZE];
    int n;

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // bind socket to address
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // listen for connections
    printf("Server is running in port 8080, waiting for connection...\n");
    if (listen(sockfd, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept a connection
    connfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
    printf("Client connected\n");

    // receive command from client
    n = recv(connfd, buffer, MAXSIZE, 0);
    buffer[n] = '\0';
    if (strcmp(buffer, "download") != 0) {
        printf("Invalid command from client\n");
        close(connfd);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // receive filename from client
    n = recv(connfd, filename, MAXSIZE, 0);
    filename[n] = '\0';
    printf("Filename received: %s\n", filename);

    // check if file exists and get file size
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("fopen");
        close(connfd);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0L, SEEK_END);
    long int filesize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    fclose(fp);

    // send file size to client
    char filesize_str[MAXSIZE];
    sprintf(filesize_str, "%ld", filesize);
    send(connfd, filesize_str, strlen(filesize_str), 0);
    //long int filesize = atol(filesize_str);
    printf("File size: %ld\n", filesize);

    // send file to client
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("fopen");
        close(connfd);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    while ((n = fread(buffer, 1, MAXSIZE, fp)) > 0) {
        send(connfd, buffer, n, 0);
    }
    printf("File has sent succesfully.\n");
    fclose(fp);

    // close connection
    close(connfd);
    close(sockfd);

    return 0;
}