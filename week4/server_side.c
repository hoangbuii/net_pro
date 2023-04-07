// Bui Huy Hoang - 20021359
/*  Cho giao thức như dưới đây
Client:  HELO Server
Server:  200 Hello Client
Client:  USER NAME
Server: 210  User name OK
Client: abc123
Server: 410 User name error      // Nếu tên có ký tự số hoặc ký tự đặc biệt thì trả lại mã 400 báo lỗi
Client:  USER NAME
Server: 210  User name OK
Client: Hoang
Server: 211 User name Hoang OK 
Client:  USER AGE 
Server: 220 User age OK
Client:  abc123                           
Server: 420 User age error         // Nếu tuổi k phải là số thì trả lại mã 400 báo lỗi
Client:  USER AGE 
Server: 220 User age OK
Client: 20
Server: 221 User age 20 OK
Client: QUIT 
Server: 500 bye
Viết chương trình phía client và phía server cho phép giao tiếp với nhau bằng giao thức như trên. 

Dưới đây là chương trình phía server
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 8888

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char* hello = "200 Hello Client\n210 User name OK\n";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    send(new_socket, hello, strlen(hello), 0);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        read(new_socket, buffer, 1024);

        if (strncmp(buffer, "USER NAME", 9) == 0) {
            if (strlen(buffer) > 10 && buffer[10] != ' ' && !isdigit(buffer[10])) {
                send(new_socket, "211 User name ", 14, 0);
                send(new_socket, buffer + 10, strlen(buffer) - 10, 0);
                send(new_socket, " OK\n", 4, 0);
            } else {
                send(new_socket, "410 User name error\n", 20, 0);
            }
        }

        if (strncmp(buffer, "USER AGE", 8) == 0) {
            int i = 9;
            while (buffer[i] != '\n') {
                if (!isdigit(buffer[i])) {
                    send(new_socket, "420 User age error\n", 19, 0);
                    break;
                }
                i++;
            }
            if (buffer[i] == '\n') {
                send(new_socket, "221 User age ", 13, 0);
                send(new_socket, buffer + 9, strlen(buffer) - 9, 0);
                send(new_socket, " OK\n", 4, 0);
            }
        }

        if (strncmp(buffer, "QUIT", 4) == 0) {
            send(new_socket, "500 bye\n", 8, 0);
            break;
        }
    }

    close(new_socket);
    close(server_fd);

    return 0;
}
