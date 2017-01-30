/* 
 * test program to pass data to the server 
 * arg1 must me the port of the server on the local machine
 */
import java.io.*;
import java.net.*;


public class testClient 
{
	Socket socket=null;
	OutputStream sockOut=null;
	BufferedWriter sockOutput=null; //where we write output to the socket
	BufferedReader input;
	String buffer;

	testClient(int port) throws IOException
	{
		input=new BufferedReader(new InputStreamReader(System.in));

		socket=new Socket("localhost",port);
		System.out.println("Connected to server on port: "+socket.getPort());
		sockOut=socket.getOutputStream();
		sockOutput=new BufferedWriter(new OutputStreamWriter(sockOut));
	}

	public static void main(String args[]) throws NumberFormatException 
	{
		if (args.length < 1)
		{
			System.out.println("Usage: testClient <port>");
			System.exit(1);
		}

		try
		{
			testClient client = new testClient(Integer.parseInt(args[0]));
			while (true)
			{
				System.out.println("Waiting for input:");
				client.buffer=client.input.readLine();
				System.out.println("Writing \""+client.buffer+"\" to socket");
				client.sockOutput.write(client.buffer);	
				client.sockOutput.flush();
			}

		}//try
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
