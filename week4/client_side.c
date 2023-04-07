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

Dưới đây là chương trình phía client
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDR "192.168.92.103"
#define SERVER_PORT 8888

int main() {
    // Tạo socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Kết nối đến server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Gửi thông điệp tới server
    char buffer[1024];
    int retcode;
    while (1) {
        // Đọc dòng lệnh từ người dùng
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        // Gửi dòng lệnh tới server
        if (send(sock, buffer, strlen(buffer), 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        // Nhận phản hồi từ server
        memset(buffer, 0, sizeof(buffer));
        if (recv(sock, buffer, sizeof(buffer), 0) == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        // Hiển thị phản hồi từ server
        printf("%s\n", buffer);

        // Kiểm tra mã trạng thái trả về từ server
        retcode = atoi(buffer);
        if (retcode >= 400 && retcode <= 499) {
            break;
        }
    }

    // Đóng kết nối và thoát chương trình
    close(sock);
    return 0;
}
