import java.net.*;
import java.io.*;

public class Server {
    public static void main(String[] args) {
        try {
            int port = 8080;
            ServerSocket serverSocket = new ServerSocket(port);

            while (true) {
                System.out.println("Server listening on port " + port);

                Socket clientSocket = serverSocket.accept();
                System.out.println("Connected to client " + clientSocket.getInetAddress());
                
                BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
            
                while (true) {
                    
                    String command = in.readLine();
                    if (command.equals("download")) {
                        String filename = in.readLine();
                        System.out.println("Received file name: " + filename);
                    
                        File file = new File("SharedFolder/" + filename);
                        if (file.exists()) {
                            out.println("exist");
                            out.println(file.length());
                            System.out.println("File size: " + file.length());

                            byte[] buffer = new byte[1024];
                            FileInputStream fileInputStream = new FileInputStream(file);
                            BufferedInputStream bufferedInputStream = new BufferedInputStream(fileInputStream);
                            OutputStream outputStream = clientSocket.getOutputStream();

                            int bytesRead;
                            while ((bytesRead = bufferedInputStream.read(buffer)) != -1) {
                                outputStream.write(buffer, 0, bytesRead);
                            }
                            outputStream.flush();
                            System.out.println("File transfered successfully!");
                        } else {
                            out.println("not exist");
                            System.out.println(filename + " does not exist");
                        }
                    } else {
                        break;
                    }
                }
                System.out.println("Client disconnected");
                in.close();
                out.close();
                clientSocket.close();
            }
        } catch (IOException e) {
            System.out.println("Fatal error: " + e.getMessage());
        }
    }
}
