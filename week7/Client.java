import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) throws IOException {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter the server IP address: ");
        String serverIP = scanner.nextLine();
        int port = 8080;

        Socket socket = new Socket(serverIP, port);
        BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
        System.out.println("Connected to server");
	
        while (true) {
            System.out.print("Enter the filename(type 0 to exit): ");
            String filename = scanner.nextLine();
            if (filename.equals("0")) {
                out.println("exit");
                break;
            }
            out.println("download");
            out.println(filename);

            String response = in.readLine();
            if (response.equals("exist")) {
                long fileSize = Long.parseLong(in.readLine());
                System.out.println("File size: " + fileSize);

                byte[] buffer = new byte[1024];
                InputStream inputStream = socket.getInputStream();
                FileOutputStream fileOutputStream = new FileOutputStream(filename);
                BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(fileOutputStream);

                int bytesRead;
                while ((bytesRead = inputStream.read(buffer)) != -1) {
                    bufferedOutputStream.write(buffer, 0, bytesRead);
                    fileSize -= bytesRead;
                    if (fileSize == 0) {
                        break;
                    }
                }
                bufferedOutputStream.flush();
                System.out.println("File downloaded successfully!");
            } else {
                System.out.println("File does not exist on the server!");
            }
        }
	System.out.println("Disconnected to server");
        in.close();
        out.close();
        socket.close();
        scanner.close();
    }
}
