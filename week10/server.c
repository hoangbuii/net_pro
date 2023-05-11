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
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind server socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("File Transfer Server started.\n");

    while (1) {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("New client connected: %s\n", inet_ntoa(address.sin_addr));

        // Start new thread to handle client request
        if (!fork()) {
            char buffer[1024];
            int bytesReceived;
            long fileSize;
            int fd;

            while (1) {
                // Wait for client to send download command
                bytesReceived = read(new_socket, buffer, sizeof(buffer));
                buffer[bytesReceived] = '\0';
                if (strcmp(buffer, "DOWNLOAD\n") != 0) {
                    write(new_socket, "Invalid command\n", strlen("Invalid command\n"));
                    close(new_socket);
                    exit(EXIT_FAILURE);
                }

                // Read file name from client
                char filename[1024] = {0};
                bytesReceived = read(new_socket, filename, sizeof(filename));
                filename[bytesReceived] = '\0';
                if (strcmp(filename, "0") == 0) {
                    break;
                }
                char directory[] = "SharedFolder/";
                strcat(directory, filename);
                printf("Received file name: \"%s\", from: %s\n", filename, inet_ntoa(address.sin_addr));

                // Check if file exists
                int exsit;
                if ((fd = open(directory, O_RDONLY)) < 0) {
                    printf("File not found\n");
                    exsit = 1;
                    write(new_socket, &exsit, sizeof(exsit));
                    //write(new_socket, "File not found\n", strlen("File not found\n"));
                    continue;
                } else {
                    exsit = 0;
                    write(new_socket, &exsit, sizeof(exsit));
                    // write(new_socket, "Found\n", strlen("Found\n"));
                }
                
                
                // Send file size to client
                fileSize = lseek(fd, 0, SEEK_END);
                write(new_socket, &fileSize, sizeof(fileSize));
                printf("File size: %ld bytes\n", fileSize);

                // Send file to client
                lseek(fd, 0, SEEK_SET);
                while ((bytesReceived = read(fd, buffer, sizeof(buffer))) > 0) {
                    write(new_socket, buffer, bytesReceived);
                }
                close(fd);
                printf("File sent to client: %s\n", filename);

            }






            

            // Close file descriptor and socket
            printf("Client disconnected: %s\n", inet_ntoa(address.sin_addr));
            close(new_socket);
            exit(EXIT_SUCCESS);
        }

        // Close socket in parent process
        close(new_socket);
    }

    return 0;
}
