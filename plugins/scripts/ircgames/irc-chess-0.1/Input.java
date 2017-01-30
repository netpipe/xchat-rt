/* input thread. receives input and passes to parse function. also sets up output stream */

import java.net.*;
import java.io.*;
import java.util.*;

public class Input extends Thread 
{
	ServerSocket serverSocket=null;
	Socket socket=null;
	BufferedInputStream in=null;
	private static PrintStream out=null;
	String buffer=null;
	Chess chess=null; //pointer to main class (object)
	String stArray[]=null; //array of tokenized strings

	/* misc vars */
	static int failCount=0;

	/* stream types */
	final static int STDOUT = 1;
	final static int STDERR = 2;
	final static int STDOUTNOSOCK=3; //dont write to socket
	final static int STDERRNOSOCK=4;
	final static int SOCKONLY=5; //output to sock but not stdout/err

	/* 
	 * create a socket and connect it to the specified port on localhost 
	 * The program accepts only ONE connection, that's it
	 */ 
	Input(int port, Chess chess) throws IOException,SecurityException
	{
		this.chess=chess;

		serverSocket=new ServerSocket(port);
		serverSocket.setSoTimeout(0);

		/* wait for connection */
		System.out.println("Waiting for connection");
		socket=serverSocket.accept();
		System.out.println("Connection accepted from client's port "+socket.getPort());

		/* I/O streams for socket */
		in=new BufferedInputStream(socket.getInputStream());
		out=new PrintStream(socket.getOutputStream());
	}


	/************functions to create messages used by print()********************/
	/* delimiter used in messages to client */
	private final static String delimiter=new String("<:=:>");

	/* creates a properly formatted message to send to the client */
	private static String makeClientMessage(GameInfo game)
	{
		Player player1=game.player1;
		Player player2=game.player2;

		String player1Message=game.player1.getMessage();
		String player2Message=game.player2.getMessage();
		String commonMessage=game.getMessage();
		
		String message=new String("[");
		message=message.concat(game.player1.getName());
		message=message.concat("]");
		message=message.concat(player1Message);
		message=message.concat(delimiter);
		message=message.concat("[");
		message=message.concat(game.player2.getName());
		message=message.concat("]");
		message=message.concat(player2Message);
		message=message.concat(delimiter);
		message=message.concat(commonMessage);

		/* clear out the old messages */
		game.player1.clearMessage();
		game.player2.clearMessage();
		game.clearMessage();
		game.clearBoardMessage();
		
		return message;
	}

	/* creates a message for the specified user */
	private static String makeClientMessage(String username, String messageToSend)
	{
		String message=new String("[");
		message=message.concat(username);
		message=message.concat("]");
		message=message.concat(messageToSend);

		return message;
	}

	/* 
	 * print data to socket and stdout/stderr depending on "stream" 
	 * "stream" also determines if data should be output to the socket
	 * "data" must contain the relevant newline chars since this function
	 * only uses System.out.print(). 
	 *
	 * Data to the socket will be printed as given. Data to stdout/err
	 * will be newline terminated. the (overloaded) print functions
	 * create the final messages by calling makeClientMessage()
	 */
	public static void print(String username, String messageToSend, 
			int stream)
	{
		String data=makeClientMessage(username, messageToSend);

		switch (stream)
		{
			case STDOUT:
				System.out.print(data+"\n");
				out.print(data);
				break;

			case STDERR:
				System.err.print(data+"\n");
				out.print(data);
				break;

			case STDOUTNOSOCK:
				System.out.print(data+"\n");
				break;

			case STDERRNOSOCK:
				System.err.print(data+"\n");
				break;

			case SOCKONLY:
				out.print(data);
				break;
		}//switch
	}//print

	public static void print(GameInfo game, int stream)
	{
		Player player1=game.player1;
		Player player2=game.player2;

		String data=makeClientMessage(game);

		switch (stream)
		{
			case STDOUT:
				System.out.print(data+"\n");
				out.print(data);
				break;

			case STDERR:
				System.err.print(data+"\n");
				out.print(data);
				break;

			case STDOUTNOSOCK:
				System.out.print(data+"\n");
				break;

			case STDERRNOSOCK:
				System.err.print(data+"\n");
				break;

			case SOCKONLY:
				out.print(data);
				break;
		}//switch
	}//print

	/**************end of fns print() related fns********************/

	/* this function closes the old socket and related i/o streams
	 * and attemps to establish the connection again, and reopen the
	 * streams */
	void reestablishConnection() throws IOException
	{

		try
		{
			in.close();
			out.close();
			socket.close();

			/* all the internal data is still kept between connections.
			 * that's so ppl's games dont get interrupted
			 * write a cleanup function and get rid of everything in 
			 * gameList to override this behaviour
			 */

			/* we only get here if the socket is closed, so wait for another connection*/
			System.out.println("Waiting for new connection");
			socket=serverSocket.accept();
			System.out.println("Connection accepted from client's port "+socket.getPort());

			in=new BufferedInputStream(socket.getInputStream());
			out=new PrintStream(socket.getOutputStream());
		}
		catch(IOException e)
		{
			System.err.println("Error: Failed to reestablish connection.");
			failCount++;
			if (failCount==10) //to avoid looping forever
			{
				System.err.println("Error. Exiting after 10 failed reestablish attempts");
				System.exit(-1);
			}
		}
	}//reestablishConnection

	/* continuously get input */
	void getInput() throws IOException
	{
		/* get input forever */
		while (true)
		{
			/* declare this outside and you'll have to keep re-initializing it.
			 * that's a pain. which is why things are being done in here
			 */
			byte b[]=new byte[1500]; //for packet load
			int num_bytes_read=in.read(b,0,b.length);
			if (num_bytes_read==-1) //eof
			{
				System.out.println("Lost connection");
				reestablishConnection();
			}
			else
			{
				buffer=new String(b,0,num_bytes_read);

				/* tokenize string, putting values into a string array */
				StringTokenizer st=new StringTokenizer(buffer);
				int numTokens=st.countTokens();
				stArray=new String[numTokens];
				for (int j=0; j<numTokens; j++)
				{
					stArray[j]=new String(st.nextToken());
				}//j

				chess.parseParam(stArray);
			}
		} //while
	}//getInput

	public void run() 
	{
		while (true)
		{
			try
			{
				getInput();
			}
			catch (IOException e)
			{
				System.out.println("Error: unknown I/O exception");
				e.printStackTrace();

				System.out.println("Attempting to re-establish connection");
			}
			catch (SecurityException e)
			{
				System.out.println("Error: unknown socket security error");
				e.printStackTrace();
			}
		}
	}
}//class Input

