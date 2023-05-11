#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGE_SIZE 1024

// Structure to hold client information
typedef struct {
    int socket;
    char userID[50];
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];
int clientCount = 0;
pthread_mutex_t clientsMutex;

// Function to send a message to a specific client
void sendMessageToClient(int clientSocket, const char* message) {
    write(clientSocket, message, strlen(message));
}

// Function to broadcast a message to all connected clients
void broadcastMessage(const char* message) {
    pthread_mutex_lock(&clientsMutex);

    for (int i = 0; i < clientCount; i++) {
        sendMessageToClient(clients[i].socket, message);
    }

    pthread_mutex_unlock(&clientsMutex);
}

void sendMessageToUser(int clientSocket, const char* receiver, const char* message) {
    int receiverIndex = -1;
    pthread_mutex_lock(&clientsMutex);
    for (int i = 0; i < clientCount; i++) {
    if (strcmp(clients[i].userID, receiver) == 0) {
        //printf("|%s|\n", clients[i].userID);
        receiverIndex = i;
        break;
        }
    }
    
    if (receiverIndex != -1) {
        //sprintf(message, "%s: %s", sender, messageContent);
        sendMessageToClient(clients[receiverIndex].socket, message);
    } else {
        char unknowMessage[MAX_MESSAGE_SIZE];
        sprintf(unknowMessage, "%s offline", receiver);
        sendMessageToClient(clientSocket, unknowMessage);
    }
    pthread_mutex_unlock(&clientsMutex);
}

// Function to handle a client's connection
void* handleClient(void* arg) {
    int clientSocket = *(int*)arg;
    char buffer[MAX_MESSAGE_SIZE];

    // Receive userID from the client
    read(clientSocket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
    printf("Client connected: %s\n", buffer);

    // Store the client's information
    pthread_mutex_lock(&clientsMutex);
    clients[clientCount].socket = clientSocket;
    strcpy(clients[clientCount].userID, buffer);
    clientCount++;
    pthread_mutex_unlock(&clientsMutex);

    // Send online userIDs to the all client
    char onlineUsers[MAX_MESSAGE_SIZE];
    strcpy(onlineUsers, "Online users: \n");
    pthread_mutex_lock(&clientsMutex);
    for (int i = 0; i < clientCount; i++) {
        strcat(onlineUsers, clients[i].userID);
        strcat(onlineUsers, "\n");
    }
    pthread_mutex_unlock(&clientsMutex);
    broadcastMessage(onlineUsers);

    while (1) {
        // Receive chat message from the client
        strcpy(buffer, "");
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        //printf("0: %s\n", buffer);
        if (bytesRead <= 0) {
            // Client disconnected
            printf("a client disconnected");

            // Remove the client's information
            pthread_mutex_lock(&clientsMutex);
            for (int i = 0; i < clientCount; i++) {
                if (clients[i].socket == clientSocket) {
                    for (int j = i; j < clientCount - 1; j++) {
                        clients[j] = clients[j + 1];
                    }
                    clientCount--;
                    break;
                }
            }
            pthread_mutex_unlock(&clientsMutex);
            
            // Send updated online userIDs to all clients
            char updatedOnlineUsers[MAX_MESSAGE_SIZE];
            strcpy(updatedOnlineUsers, "Online users: \n");
            pthread_mutex_lock(&clientsMutex);
            for (int i = 0; i < clientCount; i++) {
                strcat(updatedOnlineUsers, clients[i].userID);
                strcat(updatedOnlineUsers, "\n");
            }
            pthread_mutex_unlock(&clientsMutex);
            broadcastMessage(updatedOnlineUsers);

            break;
        } else {
            // Broadcast the chat message to all clients
            buffer[bytesRead] = '\0';
            //printf("1: %s\n", buffer);
            char sender[100], receiver[100], messageContent[1024];
            sscanf(buffer, "%s %s %s", receiver, messageContent, sender);
            char message[MAX_MESSAGE_SIZE * 2];
            sprintf(message, "%s: %s", sender, messageContent);
            //printf("1: |%s|%s|%s|\n", sender, receiver, messageContent);
            //broadcastMessage(message);
            sendMessageToUser(clientSocket, receiver, message);
        }
   
    }
    return NULL;
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    pthread_t thread;

    // Initialize mutex
    pthread_mutex_init(&clientsMutex, NULL);

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080); // Change the port number if needed

    // Bind the server socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Failed to bind");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for connections...\nPress Ctrl + C to terminate server.\n");

    while (1) {
        // Accept a client connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen)) < 0) {
            perror("Failed to accept");
            continue;
        }

        // Create a new thread to handle the client connection
        if (pthread_create(&thread, NULL, handleClient, &clientSocket) != 0) {
            perror("Failed to create thread");
            close(clientSocket);
        }
    }

    // Close server socket
    close(serverSocket);

    // Destroy mutex
    pthread_mutex_destroy(&clientsMutex);

    return 0;
}
