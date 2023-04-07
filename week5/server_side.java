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
// This is program in server side that allows communication by above protocol.

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {
    public static void main(String[] args) throws IOException {
        ServerSocket serverSocket = new ServerSocket(9696);
        System.out.println("Server is running on port 9696");
        
        while (true) {
            Socket clientSocket = serverSocket.accept();
            System.out.println("Client connected: " + clientSocket.getInetAddress().getHostAddress());
            
            BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
            
            String inputLine;
            while ((inputLine = in.readLine()) != null) {
                System.out.println("Received from client: " + inputLine);
                
                if (inputLine.equals("HELO Server")) {
                    out.println("200 Hello Client");
                } else if (inputLine.equals("USER INFO")) {
                    out.println("210 OK");
                    inputLine = in.readLine();
                    if (isValidUserInfo(inputLine)) {
                        out.println("211 User Info OK");
                    } else {
                        out.println("400 User Info Error");
                    }
                } else if (inputLine.equals("QUIT")) {
                    out.println("500 bye");
                    break;
                }
            }
            
            clientSocket.close();
            System.out.println("Client disconnected");
        }
    }
    
    private static boolean isValidUserInfo(String userInfo) {
        
        return true;
    }
}
