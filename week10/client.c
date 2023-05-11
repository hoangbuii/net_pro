#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8000

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    // Check command line arguments
    if (argc < 2) {
        printf("Usage: %s <ip-address>\n", argv[0]);
        return -1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // Set server address and port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    } else {
        printf("Connected successfull\n");
    }


    while (1) {
        // Send download command to server
        strcpy(buffer, "DOWNLOAD\n");
        write(sock, buffer, strlen(buffer));

        // Read file name from user
        char filename[1024] = {0};
        
        printf("Enter file name(type 0 to exit): ");
        fgets(filename, sizeof(filename), stdin);
        filename[strlen(filename)-1] = '\0';
        write(sock, filename, strlen(filename));
        if (strcmp(filename, "0") == 0) {
            break;
        }
        
        //strcpy(filename, buffer);
        
        // Check if file exists
        long bytesReceived = 0;
        // bytesReceived = read(sock, buffer, sizeof(buffer));
        // buffer[bytesReceived] = '\0';
        
        // if (strcmp(buffer, "Found\n") != 0) {
        //     printf("%s", buffer);
        //     printf("File not found\n");
        //     continue;
        // }

        int exsit;
        read(sock, &exsit, sizeof(exsit));
        if (exsit == 1) {
            printf("File not found\n");
            continue;
        }
        
        // Read file size from server
        long fileSize;
        read(sock, &fileSize, sizeof(fileSize));
        if (fileSize < 0) {
            printf("File not found on server\n");
            return -1;
        }
        printf("File size: %ld bytes\n", fileSize);
        // Open local file for writing
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0) {
            printf("Failed to create local file\n");
            return -1;
        }

        // Receive file data from server
        bytesReceived = 0;
        while (bytesReceived < fileSize) {
            valread = read(sock, buffer, sizeof(buffer));
            if (valread < 0) {
                printf("Failed to receive file data\n");
                close(fd);
                return -1;
            }
            write(fd, buffer, valread);
            bytesReceived += valread;
        }
        printf("File downloaded: %s\n", filename);
        close(fd);
    }

    


    // Close local file and socket
    close(sock);

    return 0;
}