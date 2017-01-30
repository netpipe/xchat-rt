/*
 * Copyright (C) 2002 Faeem Ali <kodgehopper@netscape.net>
 * 
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 */

/* irc chess 
 *
 * accepts 1 parameter: port to listen on 
 * this class receives input and sends output. other objects simply
 * returns appropriate values to this object. ALL output to the
 * socket is done from here. the program is meant to be syncronous.
 * keeping all I/O in here keeps management of synchronous I/O simple.
 */
import java.util.*;
import java.io.*;

public class Chess extends Util
{
	/* list of all games */
	private Vector gameList = new Vector();

	/* when set to true, the only allowed move is the replace command */
	private boolean replacePawn=false;

	/*
	 * out is for writing data to the socket ie. communicating with
	 * the (irc) client
	 */
	synchronized int parseParam(String param[]) 
	{
		/* the client script will sent at LEAST the username */
		String username=param[param.length-1];

		if (param.length < 3)
		{
			Input.print(username, "Error: not enough parameters specified", 
					Input.STDERR);
			return -1;
		}


		if (param[0].equals("game"))
		{
			/* since replace pawn is a special case (ie. the same user get's to replace the
			 * pawn so the same user has 2 consecutive moves), handle it seperately */
			if (replacePawn)
			{
				if (param[1].equals("replace"))
				{
					if (param.length < 5)
					{
						Input.print(username, "Error: Insufficient parameters for replace command",
								Input.STDERR);
						return -1;
					}

					boolean gameFound=false;

					for (int i=0; i<gameList.size(); i++)
					{
						GameInfo t=(GameInfo)(gameList.get(i));
						if (t.gameToken.equals(param[2]))
						{
							if ((t.player1.getName().equals(param[4])) ||
									(t.player2.getName().equals(param[4])))
							{	
								Player player;

								if (param[4].equals(t.player1.getName()))
									player=t.player1;
								else
									player=t.player2;

								switch (t.replacePawn(player, param[3]))
								{
									case RETURN_REPLACE_PAWN:
										{
											replacePawn=false;
											Input.print(t, Input.SOCKONLY);
											return RETURN_NORMAL;
										}	

									case RETURN_FAILED:
										{
											Input.print(username, 
													"Error starting game. Both players need to register and the \"game start\" command must be executed", 
													Input.STDERR);
										}
										break;

									case RETURN_ERROR_NOT_YOUR_MOVE:
										{
											Input.print(username, "Error: not your move",
													Input.STDERR);
										}
										break;

									case RETURN_ERROR_INVALID_REPLACEMENT_PIECE:
										Input.print(username,
												"Error: Invalid piece entered. Your choices are R,H,B and Q",
												Input.STDERR);
										break;
								}
							}
							else
							{
								Input.print(username, "Error: Invalid player", Input.STDERR);
								return RETURN_ERROR;
							}
							gameFound=true;
							break;
						}
					}//for

					if (!gameFound)
					{
						Input.print(username, "Error: Invalid game ID entered", Input.STDERR);
						return RETURN_ERROR;
					}

					return RETURN_ERROR;	
				}
				else
				{
					Input.print(username, "Error: Only the replace command is allowed!!!",
							Input.STDERR);
				}

				return RETURN_NORMAL;
			} //replacePawn
			else
			{
				if (param[1].equals("register"))
				{
					boolean registered=false;

					if (param.length < 4)
					{
						Input.print(username, "Error: Not enough parameters for register command", 
								Input.STDERR);
						return RETURN_ERROR;
					}

					/* check if we're modifiying an existing board or creating a new one */
					for (int j=0; j<gameList.size(); j++)
					{
						GameInfo t=(GameInfo)(gameList.get(j));
						if (t.gameToken.equals(param[2]))
						{							
							switch(t.registerPlayer(param[3]))
							{
								case RETURN_ERROR_MAX_NUM_REG:
									Input.print(username, 
											"Error: Maximum number of players registered", 
											Input.STDERR);
									break;

								case RETURN_PLAYER1_REG:
									Input.print(username,
											t.player1.getName()+" registered as player 1 for game "
											+t.gameToken+". You are White", 
											Input.STDOUT);
									break;

								case RETURN_PLAYER2_REG:
									Input.print(username,
											"registered as player 2 for game "+param[2]+". You are Black", 
											Input.STDOUT);
									break;

								case RETURN_ERROR_ONLY_REG_ONCE:
									Input.print(username, 
											"Error: can only register once", Input.STDERR);
									break;
							}
							registered=true;
						}
					}//for

					if (!registered)
					{
						GameInfo gameInfo=new GameInfo(param[2], param[3]);	
						gameList.add(gameInfo);
						Input.print(username, "registered as player 1 for game "+param[2]+". You are White",
								Input.STDOUT);
					}

					/* debugging code */
					//showGameInfo();

					return RETURN_NORMAL;
				} //token2=register

				if (param[1].equals("start"))
				{
					boolean gameFound=false;

					if (param.length < 4)
					{
						Input.print(username, "Error: not enough parameters for the \"start\" command",
								Input.STDERR);
						return RETURN_ERROR;
					}
					
					/* draw the board for the specified game */
					for (int i=0; i<gameList.size(); i++)
					{
						GameInfo t=(GameInfo)(gameList.get(i));

						if (t.gameToken.equals(param[2]))
						{
							try
							{
								if ((param[3].equals(t.player1.getName())) || 
										(param[3].equals(t.player2.getName())))
								{

									if (t.startGame()==RETURN_ERROR_GAME_ALREADY_STARTED)
									{
										Input.print(username, 
												"Error: Game already started", Input.STDOUT);
									}

									switch (t.showBoard())
									{
										case RETURN_SHOW_BOARD:
											{
												Input.print(t, Input.SOCKONLY);
											}
											break;
										case RETURN_ERROR_NO_START:
											{
												Input.print(username,
														"Error: Unable to start game. Perhaps both players haven't registered yet, or you didn't execute the \"start\" command?", 
														Input.STDERR);
											}
											break;
									}

									gameFound=true;
									break;
								}
							}//try
							catch (NullPointerException e)
							{
								/* in case both players havent registered yet */
								Input.print(username, 
										"Error: Unable to start game. Perhaps both players haven't registered yet?",
										Input.STDERR);
								return RETURN_ERROR;
							}
						}
					}//for

					/* game not found */
					if (!gameFound)
					{
						Input.print(username, 
								"Error: Invalid game ID or you are not authorized to start this game", 
								Input.STDERR);
						return RETURN_NORMAL;
					}
					return RETURN_NORMAL;
				}//token2==start

				if (param[1].equals("stop"))
				{
					boolean gameFound=false;

					for (int i=0; i<gameList.size(); i++)
					{
						GameInfo t=(GameInfo)(gameList.get(i));

						if (t.gameToken.equals(param[2]))
						{
							gameFound=true;

							if (param.length < 4)
							{
								Input.print(username, 
										"Error: Not enough parameters for \"stop\" command", 
										Input.STDERR);
								return -1;
							}
							else
							{
								if ((param[3].equals(t.player1.getName())) || 
										(param[3].equals(t.player2.getName())))
								{
									Input.print(username,
											"Game "+t.gameToken+" stopped by "+param[3], 
											Input.STDOUT);
									gameList.remove(i);
								}		
								else
								{	
									Input.print(username, 
											"Error: You are not aruthorized to stop this game",
											Input.STDERR);
									return -1;
								}
							}
						}//if

						if (!gameFound)
						{
							Input.print(username,
									"Error: Invalid game ID entered", 
									Input.STDERR);
							return -1;
						}
					}//for

					return RETURN_NORMAL;
				}//token2==stop

				/* 
				 * format of move command:
				 * game move <gameToken> <range> <player>
				 */
				if (param[1].equals("move"))
				{
					boolean gameFound=false;

					if (param.length < 5)
					{
						Input.print(username,
								"Error: Not enough parameters specified for move command", 
								Input.STDERR);					
						return -1;
					}

					/* find the proper game */
					for (int j=0; j<gameList.size(); j++)
					{
						GameInfo t=(GameInfo)(gameList.get(j));

						if (t.gameToken.equals(param[2]))
						{
							/* 
							 * make sure that that a valid user is making the
							 * move
							 */
							try
							{
								Player player=null;
								boolean playerFound=false;
								
								if (param[4].equals(t.player1.getName()))
									{
										player=t.player1;
										playerFound=true;
									}
								else
									if (param[4].equals(t.player2.getName()))
									{
										player=t.player2;
										playerFound=true;
									}

								if (playerFound)
								{
									switch (t.makeMove(param[3], player))
									{
										case RETURN_MOVE_EMPTY:
										case RETURN_MOVE_CAPTURE:
											Input.print(t, Input.SOCKONLY);
											break;


										case RETURN_MOVE_EMPTY_CHECK:
										case RETURN_MOVE_CAPTURE_CHECK:
											Input.print(t, Input.SOCKONLY);
											break;

										case RETURN_MOVE_EMPTY_CHECK_REPLACE_PAWN:
										case RETURN_MOVE_EMPTY_REPLACE_PAWN:
										case RETURN_MOVE_CHECK_CAPTURE_REPLACE_PAWN:
										case RETURN_MOVE_CAPTURE_REPLACE:
											Input.print(t, Input.SOCKONLY);
											replacePawn=true;
											break;
											
										case RETURN_ERROR_NOT_YOUR_MOVE:
											Input.print(username, 
													"Error: Not your move", Input.STDERR);
											break;

										case RETURN_MOVE_CASTLE_LEFT:
										case RETURN_MOVE_CASTLE_RIGHT:
										case RETURN_MOVE_CASTLE_LEFT_CHECK:
										case RETURN_MOVE_CASTLE_RIGHT_CHECK:
											Input.print(t, Input.SOCKONLY);
											break;

										case RETURN_ERROR_NO_START:
											Input.print(username,
													"Error: Unable to start game. Perhaps both players haven't registered yet, or you didn't execute the \"start\" command?", 
													Input.STDERR);
											break;

										case RETURN_ERROR:
											Input.print(username, "Error: Some unknown error occurred. Life's a Bitch that way sometimes", 
													Input.STDERR);
											break;

										case RETURN_ERROR_INVALID_MOVE:
											Input.print(username,
													"Error: Invalid Move: "+param[3], 
													Input.STDERR);
											break;	
									}//switch
									gameFound=true;
								} //if playerFound
								else
								{
									Input.print(username,
											"Error: Invalid User: "+username, Input.STDOUT);
								}
							}//try
							catch (NullPointerException e)
							{
								Input.print(username,
										"Error: Unable to make move. Have both players registered for the game and has the game been started?", 
										Input.STDERR);
								return RETURN_ERROR;
							}
						}//main if
					}//for

					if (!gameFound)
					{
						Input.print(username, "Error: Invalid game ID entered", Input.STDERR);
						return RETURN_ERROR;
					}

					return RETURN_NORMAL;
				}//token2==move

				/* 
				 * at this point, we've checked for all valid commands (start, stop, register, move etc),
				 * so if we do have a command here, it has to be an invalid command. 
				 * so just display the relevant error message
				 */
				Input.print(username, "Error: Invalid command entered", Input.STDERR);
				return RETURN_ERROR;
			} //main else	
		}//token1==game
		else
		{
			Input.print(username, "Error: Invalid Command: Bad format", Input.STDERR);
			return -1;
		}
	} //parseParam


	public static void main(String args[])
	{
		if (args.length < 1)
		{
			System.out.println("Usage: Chess <port>");
			System.exit(1);
		}

		Chess chess=new Chess();

		int port=Integer.parseInt(args[0]);
		try
		{
			Thread input = new Thread(new Input(port, chess));
			input.start();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}


}
