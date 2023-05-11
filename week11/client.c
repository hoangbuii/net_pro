#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_MESSAGE_SIZE 1024

int clientSocket;
char userID[50];

// Function to receive and display messages from the server
void* receiveMessages(void* arg) {
    char buffer[MAX_MESSAGE_SIZE];

    while (1) {
        // Receive messages from the server
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            // Server disconnected
            printf("Server disconnected.\n");
            exit(1);
        } else {
            buffer[bytesRead] = '\0';
            printf("%s\n", buffer);
        }
    }

    // Close client socket
    close(clientSocket);

    // Exit the thread
    pthread_exit(NULL);
}

int main() {
    struct sockaddr_in serverAddr;
    char buffer[MAX_MESSAGE_SIZE];

    // Create client socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Change the port number if needed

    // Get server IP address from user
    char serverIP[16];
    printf("Enter server IP address: ");
    fgets(serverIP, sizeof(serverIP), stdin);
    serverIP[strcspn(serverIP, "\n")] = 0;
    if (inet_pton(AF_INET, serverIP, &(serverAddr.sin_addr)) <= 0) {
        perror("Invalid server IP address");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Failed to connect");
        exit(EXIT_FAILURE);
    }

    // Get userID from user
    printf("Enter your userID: ");
    fgets(userID, sizeof(userID), stdin);
    userID[strcspn(userID, "\n")] = 0;

    // Send userID to the server
    write(clientSocket, userID, strlen(userID));

    // Create a new thread to receive messages from the server
    pthread_t thread;
    if (pthread_create(&thread, NULL, receiveMessages, NULL) != 0) {
        perror("Failed to create thread");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    printf("Enter your message (e.g. alice hello)\nPress Ctrl + C to exit\n");
    while (1) {
        // Send chat message to the server
        
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        // Check if the user wants to exit
        if (strcmp(buffer, "exit") == 0) {
            write(clientSocket, "", 0);
            break;
        }
        strcat(buffer, " ");
        strcat(buffer, userID);
        write(clientSocket, buffer, strlen(buffer));
    }

    // Wait for the receiving thread to finish
    pthread_join(thread, NULL);

    return 0;
}

