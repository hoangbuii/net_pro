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
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAXSIZE];
    char filename[MAXSIZE];
    int n;

    // get IP address from keyboard
    char server_ip[MAXSIZE];
    printf("Enter server IP address: ");
    fgets(server_ip, MAXSIZE, stdin);
    server_ip[strcspn(server_ip, "\n")] = 0;

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(server_ip);
    servaddr.sin_port = htons(PORT);

    // connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    } else {
        printf("Connected to server\n");
    }

    // send command to server
    strcpy(buffer, "download");
    send(sockfd, buffer, strlen(buffer), 0);

    // get filename from keyboard and send to server
    printf("Enter filename to download: ");
    fgets(filename, MAXSIZE, stdin);
    filename[strcspn(filename, "\n")] = 0;
    send(sockfd, filename, strlen(filename), 0);

    // receive file size from server
    n = recv(sockfd, buffer, MAXSIZE, 0);
    buffer[n] = '\0';
    long int filesize = atol(buffer);
    printf("File size: %ld\n", filesize);

    // receive file data from server and save to file
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("fopen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    int bytes_received = 0;
    while (bytes_received < filesize) {
        n = recv(sockfd, buffer, MAXSIZE, 0);
        fwrite(buffer, 1, n, fp);
        bytes_received += n;
    }
    printf("File has received succesfully.\n");
    fclose(fp);

    // close socket
    close(sockfd);

    return 0;
}