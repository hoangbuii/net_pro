// Bui Huy Hoang - 20021359
// Description:
// For this protocol
// Client:  Client:  HELO Server
// Server:  200 Hello Client
// Client:  USER INFO
// Server:  210 OK
// Client: { “User name”: abc123, “User age”: abc123}
// Server: 400 User Info Error
// Client:  USER INFO
// Server:  210 OK
// Client:  { “User name”: abc123, “User age”: abc123}
// Server: 211 User Info OK
// Client: QUIT 
// Server: 500 bye
// This is program in client side that allows communication by above protocol.


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) throws IOException {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter the server IP address: ");
        String serverIP = scanner.nextLine();
        
        Socket socket = new Socket(serverIP, 9696);
        BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
        
        String inputLine;
        while ((inputLine = in.readLine()) != null) {
            System.out.println("Received from server: " + inputLine);
            
            if (inputLine.equals("200 Hello Client")) {
                out.println("USER INFO");
            } else if (inputLine.equals("210 OK")) {
                String userInfo = getUserInfo();
                out.println(userInfo);
            } else if (inputLine.equals("211 User Info OK")) {
                out.println("QUIT");
                break;
            } else if (inputLine.equals("400 User Info Error")) {
                System.out.println("Invalid user info, please try again.");
                out.println("USER INFO");
            } else if (inputLine.equals("500 bye")) {
                break;
            }
        }
        
        socket.close();
        System.out.println("Connection closed");
    }
    
    private static String getUserInfo() {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter user info in JSON format: ");
        String userInfo = scanner.nextLine();
        return userInfo;
    }
}
