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
/* 
 * this class stores all the info for a single game 
 */

public class GameInfo extends Util
{

	/* general game info */
	public String gameToken=null; //unique for every game
	public Player player1=null;
	public Player player2=null;
	private String message = new String(""); 	//messages for both users
	//we set this to a new string so
	//we can always use concat()
	//without any hassles
	/* misc */
	private boolean canPlay=false; //game is ready to be started
	private boolean gameStarted=false; //signals if game has started or not
	private String whoseMove=WHITE; //track whose turn it is to move

	/* white beads */
	private final int WPAWN=101;
	private final int WCASTLE=102;
	private final int WKNIGHT=103;
	private final int WBISHOP=104;
	private final int WQUEEN=105;
	private final int WKING=106;

	/* black beads */
	private final int BPAWN=201;
	private final int BCASTLE=202;
	private final int BKNIGHT=203;
	private final int BBISHOP=204;
	private final int BQUEEN=205;
	private final int BKING=206;

	/* board to store beads */
	private Board board=null;

	GameInfo(String token, String player)
	{
		gameToken=token;
		registerPlayer(player);
	} //GameInfo (String, String)

	String getMessage()
	{
		message=message.concat("\n");
		message=message.concat(getBoardMessage());
		return message;
	}

	public void appendMessage(String msg)
	{
		message=message.concat(msg);
	}
	
	public void prependMessage(String msg)
	{
		msg=msg.concat(message);
		message=msg;
	}

	/* this clears the board message as well,
	 * since the board is always accessed through
	 * this class */
	void clearMessage()
	{
		message=new String("");
	}

	void clearBoardMessage()
	{
		board.clearMessage();
	}
	
	String getBoardMessage()
	{
		return board.getMessage();
	}
	
	/* display the board */
	int showBoard()
	{
		if (gameStarted)
		{
			/* board is stored in the Board's message buffer */
			board.showBoard();
			return RETURN_SHOW_BOARD;
		}
		else
			return RETURN_ERROR_NO_START;

	}//showBoard


	/* replaces a pawn on the board with the specified piece */
	int replacePawn(Player player, String piece)
	{
		Player otherPlayer;

		if (gameStarted)
		{
			/* make sure the move is being made by the right player */
			if (player.getColor().equals(whoseMove))
			{
				if (player.getPlayerNum()==1)
					otherPlayer=player2;
				else
					otherPlayer=player1;

				switch (board.replacePawn(player, otherPlayer, piece))
				{
					case RETURN_PAWN_REPLACED:
						switchPlayer(player, otherPlayer);
						return RETURN_REPLACE_PAWN;

					case RETURN_ERROR:
						return RETURN_ERROR;

					case RETURN_ERROR_INVALID_REPLACEMENT_PIECE:
						return RETURN_ERROR_INVALID_REPLACEMENT_PIECE;
				}
			}//if color=whoseMove
			else
				return RETURN_ERROR_NOT_YOUR_MOVE;
		}
		else
			return RETURN_FAILED;

		return RETURN_FAILED;
	} //replacePawn

	/* modify whoseMove to show it's the other player's turn, and
	 * add a message to both player's message list informing them
	 * whose turn it is */
	void switchPlayer(Player player, Player otherPlayer)
	{
		whoseMove=(whoseMove.equals(BLACK)) ? WHITE : BLACK;
		
		player.appendMessage("Information: It is "+otherPlayer.getName()+"'s move\n");
		otherPlayer.appendMessage("Information: It is your move\n");
	}//switchPlayer
	 
	/* to make a move. Accepts a range as a parameter. This will be
	 * passed to the board for processing
	 */
	int makeMove(String move, Player player)
	{
		if (gameStarted)
		{
			/* make sure the right player is making the move */
			if (player.getColor().equals(whoseMove))
			{
				Player otherPlayer;

				if (player.getPlayerNum()==1)
					otherPlayer=player2;
				else
					otherPlayer=player1;

				switch (board.makeMove(move, player, otherPlayer))
				{
					case RETURN_MOVE_EMPTY:
						switchPlayer(player, otherPlayer);
						return RETURN_MOVE_EMPTY;

					case RETURN_MOVE_CAPTURE:
						switchPlayer(player, otherPlayer);
						return RETURN_MOVE_CAPTURE;

					case RETURN_MOVE_EMPTY_CHECK:
						switchPlayer(player, otherPlayer);
						return RETURN_MOVE_EMPTY_CHECK;

					case RETURN_MOVE_CAPTURE_CHECK:
						switchPlayer(player, otherPlayer);
						return RETURN_MOVE_CAPTURE_CHECK;

					case RETURN_MOVE_EMPTY_REPLACE_PAWN:
						return RETURN_MOVE_EMPTY_REPLACE_PAWN;

					case RETURN_MOVE_EMPTY_CHECK_REPLACE_PAWN:
						return RETURN_MOVE_EMPTY_CHECK_REPLACE_PAWN;

					case RETURN_MOVE_CAPTURE_REPLACE:
						return RETURN_MOVE_CAPTURE_REPLACE;

					case RETURN_MOVE_CHECK_CAPTURE_REPLACE_PAWN:
						return RETURN_MOVE_CHECK_CAPTURE_REPLACE_PAWN;

					case RETURN_MOVE_CASTLE_LEFT:
						switchPlayer(player, otherPlayer);
						return RETURN_MOVE_CASTLE_LEFT;

					case RETURN_MOVE_CASTLE_LEFT_CHECK:
						switchPlayer(player, otherPlayer);
						return RETURN_MOVE_CASTLE_LEFT_CHECK;
						
					case RETURN_MOVE_CASTLE_RIGHT:
						switchPlayer(player, otherPlayer);
						return RETURN_MOVE_CASTLE_RIGHT;

					case RETURN_MOVE_CASTLE_RIGHT_CHECK:
						switchPlayer(player, otherPlayer);
						return RETURN_MOVE_CASTLE_RIGHT_CHECK;

					case RETURN_ERROR_INVALID_MOVE:
						return RETURN_ERROR_INVALID_MOVE;
					
					case RETURN_ERROR:
						return RETURN_ERROR;
				}//switch
			}
			else
				return RETURN_ERROR_NOT_YOUR_MOVE;
		}
		else
			return RETURN_ERROR_NO_START;

		return RETURN_NORMAL;
	} //makeMove

	/* register a player */
	int registerPlayer(String player)
	{
		if (player1==null)
		{
			/* player1 is white by default */
			player1=new Player(player, 1, WHITE); 
			return RETURN_PLAYER1_REG;
		}
		else
		{
			if (player2==null)
			{
				if (player.equals(player1.getName()))
				{	
					return RETURN_ERROR_ONLY_REG_ONCE;
				}
				else
				{
					player2=new Player(player, 2, BLACK);
					canPlay=true;
					return RETURN_PLAYER2_REG;
				}
			}
			else
			{
				return RETURN_ERROR_MAX_NUM_REG;
			}
		}//main else
	}//registerPlayer

	int startGame()
	{
		if (canPlay) //game can start since both players are registered
		{
			if (gameStarted)
			{
				return RETURN_ERROR_GAME_ALREADY_STARTED;
			}
			else
			{
				appendMessage("White moves first\n\n");
				player1.appendMessage("It is your move\n");
				player2.appendMessage("It is "+player1.getName()+"'s move\n");
				gameStarted=true;
				board=new Board(player1, player2);
			}
		}

		return RETURN_NORMAL;
	}//startGame
}
