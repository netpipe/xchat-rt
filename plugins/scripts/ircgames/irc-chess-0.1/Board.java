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

/* stores everything related to the board */
import java.io.*;

public class Board extends Util
{
	private class Square
	{
		String color; //string eg. <BLACK> that determines the color for this square
		Piece piece;

		Square(Piece piece, String color)
		{
			this.piece=piece;
			this.color=new String(color);
		}

		/* returns true if the square is empty. i.e. is occupied by an empty piece */
		boolean isEmpty()
		{
			return (piece.isEmpty()) ? true : false;
		}

		String getColor()
		{
			return color;
		}
	} //class Square

	/***************************end class Square***************************/

	/* 
	 * stores messages that are to be displayed 
	 * eg showBoard()
	 */
	private String message=new String("");
	
	/* board colors (alternating colors for neighbouring squares) */
	private final static String boardColors[]=new String[]{BYELLOW, BBLUE};

	/* 
	 *  move variables ie. when a piece is to be moved. Positions start
	 * at 0
	 */
	private final static int MOVE_NONE                     	=-1; //initialization value. a placeholder 
	private final static int MOVE_INVALID					=0;
	private final static int MOVE_EMPTY						=1; //move to an empty square
	private final static int MOVE_CAPTURE					=2; //move and capture a piece
	private final static int MOVE_CHECK_EMPTY				=3; //if the move put the other player into check
	private final static int MOVE_CHECK_CAPTURE				=4;
	private final static int MOVE_EMPTY_REPLACE				=5;//when a pawn needs to 	
	private final static int MOVE_CAPTURE_REPLACE			=6;//be replaced with another piece
	private final static int MOVE_CHECK_EMPTY_REPLACE		=7; 
	private final static int MOVE_CHECK_CAPTURE_REPLACE		=8;
	private final static int MOVE_CASTLE_LEFT				=9; //when the king is to be castled
	private final static int MOVE_CASTLE_RIGHT				=10;

	/* misc defs */
	private final static int IS_CHECK=0; //if the king is in check or not
	private final static int NOT_CHECK=1;
	private final static int ACTION_SWITCH=2; //for temporarily modifying the state of the board
	private final static int ACTION_RESTORE=3;
	private final static int TEST_KING=4; //test king to c if it's in check - isThreatened()
	private final static int TEST_NORMAL=5; //test any piece

	/* the start and end row/cols of a move 
	 * multiple values need to be shared between 2 functions. this is the
	 * easiest way of doing that without causing problems. the values
	 * passed between functions are int startRow, int endRow etc,
	 * so having these global values of type Integer adds a bit of confusion
	 * but prevents silly errors (eg. if a function tries using the global
	 * var instead of the parameter)
	 */
	private Integer startRow, startCol, endRow, endCol;

	/* variables for function testMovePiece() */
	private Piece tempPiece=null;
	private Piece tempEmptyPiece=new Piece("E", NONE, -2, -2);
	private int tempMoveType=-1;

	/* vars to temporarily store position of pawn to replace.
	 * when not in use, must be set to -1
	 */
	private int replacePawnRow=-1;
	private int replacePawnCol=-1;

	/* board variables */
	Square board[][];
	String colLetters[]=new String[]{"a","b","c","d","e","f","g","h"};

	Board(Player player1, Player player2)
	{
		board=new Square[][]
		{
			{
				new Square(player2.getPiece(0,0), boardColors[0]),
				new Square(player2.getPiece(0,1), boardColors[1]),
				new Square(player2.getPiece(0,2), boardColors[0]),
				new Square(player2.getPiece(0,3), boardColors[1]),
				new Square(player2.getPiece(0,4), boardColors[0]),
				new Square(player2.getPiece(0,5), boardColors[1]),
				new Square(player2.getPiece(0,6), boardColors[0]),
				new Square(player2.getPiece(0,7), boardColors[1]),
			},

			{
				new Square(player2.getPiece(1,0), boardColors[1]),
				new Square(player2.getPiece(1,1), boardColors[0]),
				new Square(player2.getPiece(1,2), boardColors[1]),
				new Square(player2.getPiece(1,3), boardColors[0]),
				new Square(player2.getPiece(1,4), boardColors[1]),
				new Square(player2.getPiece(1,5), boardColors[0]),
				new Square(player2.getPiece(1,6), boardColors[1]),
				new Square(player2.getPiece(1,7), boardColors[0]),
			},

			{
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1])
			},

			{
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0])
			},

			{
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1])
			},

			{
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[1]),
				new Square(new Piece("E", NONE, -1, -1), boardColors[0])
			},

			{
				new Square(player1.getPiece(6,0), boardColors[0]),
				new Square(player1.getPiece(6,1), boardColors[1]),
				new Square(player1.getPiece(6,2), boardColors[0]),
				new Square(player1.getPiece(6,3), boardColors[1]),
				new Square(player1.getPiece(6,4), boardColors[0]),
				new Square(player1.getPiece(6,5), boardColors[1]),
				new Square(player1.getPiece(6,6), boardColors[0]),
				new Square(player1.getPiece(6,7), boardColors[1]),
			},

			{
				new Square(player1.getPiece(7,0), boardColors[1]),
				new Square(player1.getPiece(7,1), boardColors[0]),
				new Square(player1.getPiece(7,2), boardColors[1]),
				new Square(player1.getPiece(7,3), boardColors[0]),
				new Square(player1.getPiece(7,4), boardColors[1]),
				new Square(player1.getPiece(7,5), boardColors[0]),
				new Square(player1.getPiece(7,6), boardColors[1]),
				new Square(player1.getPiece(7,7), boardColors[0]),
			}
		}; //board
	} //Board Constructor

	public String getMessage()
	{
		return message;
	}

	public void clearMessage()
	{
		message=new String("");
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

	/* 
	 * mark the specified square on the board as empty 
	 * this function changes the BOARD, not the piece
	 * WARNING: this function changes the piece on the square.
	 * if something else is pointing to that piece, then that still
	 * points to the object. that was causing a stopped error before
	 */
	private void setEmpty(int row, int col)
	{
		String squareColor=board[row][col].getColor();
		board[row][col]=new Square(new Piece("E", NONE, -1, -1), squareColor);
	}

	/* gets start row/col and end row/col for a given move. Once we have those values,
	   it will be possible to determine whether a move is valid or not
	   */
	private int processRange(String move)
	{
		int dashPos=move.indexOf("-");

		if ((dashPos==-1) || (dashPos != 2) || (move.length() != 5))
		{
			return RETURN_ERROR_INVALID_MOVE;
		}

		String first=move.substring(0,dashPos);
		String second=move.substring(dashPos+1, move.length());

		/* get start column */
		if ((first.startsWith("A") || first.startsWith("a")))
			startCol=new Integer(0);
		else if ((first.startsWith("B") || first.startsWith("b")))
			startCol=new Integer(1);
		else if ((first.startsWith("C") || first.startsWith("c")))
			startCol=new Integer(2);
		else if ((first.startsWith("D") || first.startsWith("d")))
			startCol=new Integer(3);
		else if ((first.startsWith("E") || first.startsWith("e")))
			startCol=new Integer(4);
		else if ((first.startsWith("F") || first.startsWith("f")))
			startCol=new Integer(5);
		else if ((first.startsWith("G") || first.startsWith("g")))
			startCol=new Integer(6);
		else if ((first.startsWith("H") || first.startsWith("h")))
			startCol=new Integer(7);
		else
		{
			return RETURN_ERROR_INVALID_MOVE;
		}

		/* get end column */
		if ((second.startsWith("A") || second.startsWith("a")))
			endCol=new Integer(0);
		else if ((second.startsWith("B") || second.startsWith("b")))
			endCol=new Integer(1);
		else if ((second.startsWith("C") || second.startsWith("c")))
			endCol=new Integer(2);
		else if ((second.startsWith("D") || second.startsWith("d")))
			endCol=new Integer(3);
		else if ((second.startsWith("E") || second.startsWith("e")))
			endCol=new Integer(4);
		else if ((second.startsWith("F") || second.startsWith("f")))
			endCol=new Integer(5);
		else if ((second.startsWith("G") || second.startsWith("g")))
			endCol=new Integer(6);
		else if ((second.startsWith("H") || second.startsWith("h")))
			endCol=new Integer(7);
		else 
		{
			return RETURN_ERROR_INVALID_MOVE;
		}

		/* 
		   internally, the layout of the rows on the board are 0-7, but the board the user
		   sees has row numbers reversed, ie 7-0. Therefore the user specified values must
		   be reversed
		   */
		/* get start row */
		if (first.endsWith("1"))
			startRow=new Integer(7);
		else if (first.endsWith("2")) 
			startRow=new Integer(6);
		else if (first.endsWith("3"))
			startRow=new Integer(5);
		else if (first.endsWith("4"))
			startRow=new Integer(4);
		else if (first.endsWith("5"))
			startRow=new Integer(3);
		else if (first.endsWith("6"))
			startRow=new Integer(2);
		else if (first.endsWith("7"))
			startRow=new Integer(1);
		else if (first.endsWith("8"))
			startRow=new Integer(0);
		else
		{
			return RETURN_ERROR_INVALID_MOVE;
		}

		/* get end row */
		if (second.endsWith("1"))
			endRow=new Integer(7);
		else if (second.endsWith("2")) 
			endRow=new Integer(6);
		else if (second.endsWith("3"))
			endRow=new Integer(5);
		else if (second.endsWith("4"))
			endRow=new Integer(4);
		else if (second.endsWith("5"))
			endRow=new Integer(3);
		else if (second.endsWith("6"))
			endRow=new Integer(2);
		else if (second.endsWith("7"))
			endRow=new Integer(1);
		else if (second.endsWith("8"))
			endRow=new Integer(0);
		else 
		{
			return RETURN_ERROR_INVALID_MOVE;
		}

		return RETURN_NORMAL;
	}//processRange


	/* 
	 * checks if the player's piece at the specified position is under attack. 
	 * this function works from the player's piece outwards instead of checking
	 * all the other pieces and seeing if they're attacking the specified piece
	 *
	 * note, the code in this function can be reduced significantly by
	 * smartly messing with the conditional statements. but i'm not smart.
	 *
	 * if type==TEST_KING then we check the king of that player, otherwise
	 * we test the piece at board[row][col]
	 */
	private boolean isThreatened(Player player, int type, int row, int col) 
	{
		boolean check=false;
		boolean checkMate=false;
		int checkRow, checkCol;


		Piece piece=null;

		switch (type)
		{
			case TEST_NORMAL:
				piece=player.getPiece(row, col);
				break;

			case TEST_KING:
				piece=player.getKing();
				break;
		}

		/* check if being attacked from the north */
		checkRow=piece.row-1;
		while (checkRow >= 0)
		{
			boolean jumpOut=false;
			Piece p=board[checkRow][piece.col].piece;

			/* if the piece on the square belongs to the other player, check if
			 * it poses a threat to us */
			if ((!p.isEmpty()))
			{
				if (!p.getColor().equals(piece.getColor()))
				{
					switch (p.getType())
					{
						case Piece.ROOK:
						case Piece.QUEEN:
							check=true;
							break;
					}

					jumpOut=true;
				}
				else
				{
					jumpOut=true;
				}
			}

			if (jumpOut)
				break;

			checkRow--;
		}//while

		if (!check) //to speed things up
		{
			/* check for a threat from the south */
			checkRow=piece.row+1;
			while (checkRow < 8)
			{
				boolean jumpOut=false;
				Piece p=board[checkRow][piece.col].piece;


				if (!p.isEmpty())
				{
					if (!p.getColor().equals(piece.getColor()))
					{
						switch (p.getType())
						{
							case Piece.ROOK:
							case Piece.QUEEN:
								check=true;
								break;
						}//switch

						jumpOut=true;
					}
					else
					{
						jumpOut=true;
					}
				}//if

				if (jumpOut)
					break;

				checkRow++;
			}//while
		}

		/* check for attack from the west */
		if (!check)
		{
			checkCol = piece.col-1;
			while (checkCol >= 0)
			{
				boolean jumpOut=false;
				Piece p=board[piece.row][checkCol].piece;

				if (!p.isEmpty())
				{
					if (!p.getColor().equals(piece.getColor()))
					{
						switch (p.getType())
						{
							case Piece.ROOK:
							case Piece.QUEEN:
								check=true;
								break;
						}
						jumpOut=true;
					}
					else
					{
						jumpOut=true;
					}
				}
				if (jumpOut)
					break;

				checkCol--;
			}//while
		}

		/* check for attack from the east */
		if (!check)
		{
			checkCol = piece.col+1;
			while (checkCol < 8)
			{
				boolean jumpOut=false;
				Piece p=board[piece.row][checkCol].piece;

				if (!p.isEmpty()) 
				{
					if (!p.getColor().equals(piece.getColor()))
					{
						switch (p.getType())
						{
							case Piece.ROOK:
							case Piece.QUEEN:
								check=true;
								break;
						}
						jumpOut=true;
					}
					else
					{
						jumpOut=true;
					}
				}

				if (jumpOut)
					break;

				checkCol++;
			}//while
		}

		/* check for an attack from the north-east */
		if (!check)
		{
			checkRow=piece.row-1;
			checkCol=piece.col+1;
			while ((checkRow >= 0) && (checkCol < 8))
			{
				boolean jumpOut=false;
				Piece p=board[checkRow][checkCol].piece;

				if ((!p.isEmpty())) 
				{
					if ((!p.getColor().equals(piece.getColor())))
					{
						switch (p.getType())
						{
							case Piece.BISHOP:
							case Piece.QUEEN:
								check=true;
								break;

							case Piece.PAWN:
								if ((checkRow==piece.row-1) &&
										(checkCol==piece.col+1) &&
										(player.getPlacement()==TOP))
								{
									check=true;	
								}
								break;
						}//switch
						jumpOut=true;
					}
					else  // a bead belonging to this user encountered, so just jump out 
					{
						jumpOut=true;
					}
				} 

				if (jumpOut)
					break;

				checkRow--;
				checkCol++;
			}//while
		}


		/* check for attack from north-west */
		if (!check)
		{
			checkRow=piece.row-1;
			checkCol=piece.col-1;
			while ((checkRow >= 0) && (checkCol > 0))
			{
				boolean jumpOut=false;
				Piece p=board[checkRow][checkCol].piece;

				if ((!p.isEmpty())) 
				{
					if ((!p.getColor().equals(piece.getColor())))
					{
						switch (p.getType())
						{
							case Piece.BISHOP:
							case Piece.QUEEN:
								check=true;
								break;

							case Piece.PAWN:
								if ((checkRow==piece.row-1) &&
										(checkCol==piece.col-1) &&
										(player.getPlacement()==TOP))
								{
									check=true;	
								}
								break;
						}//switch
						jumpOut=true;
					}
					else // a bead belonging to this user encountered, so just jump out
					{
						jumpOut=true;
					}
				} 

				if (jumpOut)
					break;

				checkRow--;
				checkCol--;
			}//while
		}

		/* check for attack from south east */
		if (!check)
		{
			checkRow=piece.row+1;
			checkCol=piece.col+1;
			while ((checkRow < 8) && (checkCol < 8))
			{
				boolean jumpOut=false;
				Piece p=board[checkRow][checkCol].piece;

				if ((!p.isEmpty())) 
				{
					if ((!p.getColor().equals(piece.getColor())))
					{
						switch (p.getType())
						{
							case Piece.BISHOP:
							case Piece.QUEEN:
								check=true;
								break;

							case Piece.PAWN:
								if ((checkRow==piece.row+1) &&
										(checkCol==piece.col+1) &&
										(player.getPlacement()==BOTTOM))
								{
									check=true;	
								}
								break;
						}//switch
						jumpOut=true;
					}
					else // a bead belonging to this user encountered, so just jump out 
					{
						jumpOut=true;
					}
				} 

				if (jumpOut)
					break;

				checkRow++;
				checkCol++;
			}//while
		}

		/* check for attack from the south west */
		if (!check)
		{
			checkRow=piece.row+1;
			checkCol=piece.col-1;
			while ((checkRow < 8) && (checkCol >= 0))
			{
				boolean jumpOut=false;
				Piece p=board[checkRow][checkCol].piece;

				if ((!p.isEmpty())) 
				{
					if ((!p.getColor().equals(piece.getColor())))
					{
						switch (p.getType())
						{
							case Piece.BISHOP:
							case Piece.QUEEN:
								check=true;
								break;

							case Piece.PAWN:
								if ((checkRow==piece.row+1) &&
										(checkCol==piece.col+1) &&
										(player.getPlacement()==BOTTOM))
								{
									check=true;	
								}
								break;
						}//switch
						jumpOut=true;
					}
					else // a bead belonging to this user encountered, so just jump out 
					{
						jumpOut=true;
					}
				} 

				if (jumpOut)
					break;

				checkRow++;
				checkCol--;
			}//while
		}


		/* now check for attacks by knights. There's a maximum of 8 possible places
		 * a knight might attack from, around the piece. check these positions, and
		 * avoid index out of bounds errors
		 */

		/* check top-left */
		if (!check)
		{
			checkRow=piece.row-2;
			checkCol=piece.col-1;
			if ((checkRow >= 0) && (checkCol >= 0))
			{
				Piece p=board[checkRow][checkCol].piece;
				if ((!p.getColor().equals(piece.getColor())) &&
						(p.getType()==Piece.KNIGHT))
				{
					check=true;
				}
			}
		}

		/* check top-right */
		if (!check)
		{
			checkRow=piece.row-2;
			checkCol=piece.col+1;
			if ((checkRow >= 0) && (checkCol < 8))
			{
				Piece p=board[checkRow][checkCol].piece;
				if ((!p.getColor().equals(piece.getColor())) &&
						(p.getType()==Piece.KNIGHT))
				{
					check=true;
				}
			}
		}

		/* check right-up */
		if (!check)
		{
			checkRow=piece.row-1;
			checkCol=piece.col+2;
			if ((checkRow >= 0) && (checkCol < 8))
			{
				Piece p=board[checkRow][checkCol].piece;
				if ((!p.getColor().equals(piece.getColor())) &&
						(p.getType()==Piece.KNIGHT))
				{
					check=true;
				}
			}
		}	

		/* check right-down */
		if (!check)
		{
			checkRow=piece.row+1;
			checkCol=piece.col+2;
			if ((checkRow < 8) && (checkCol < 8))
			{
				Piece p=board[checkRow][checkCol].piece;
				if ((!p.getColor().equals(piece.getColor())) &&
						(p.getType()==Piece.KNIGHT))
				{
					check=true;
				}
			}
		}

		/* check bottom-left */
		if (!check)
		{
			checkRow=piece.row+2;
			checkCol=piece.col-1;
			if ((checkRow < 8) && (checkCol >= 0))
			{
				Piece p=board[checkRow][checkCol].piece;
				if ((!p.getColor().equals(piece.getColor())) &&
						(p.getType()==Piece.KNIGHT))
				{
					check=true;
				}
			}
		}

		/* check bottom-right */
		if (!check)
		{
			checkRow=piece.row+2;
			checkCol=piece.col+1;
			if ((checkRow < 8) && (checkCol < 8))
			{
				Piece p=board[checkRow][checkCol].piece;
				if ((!p.getColor().equals(piece.getColor())) &&
						(p.getType()==Piece.KNIGHT))
				{
					check=true;
				}
			}
		}

		/* check left-up */
		if (!check)
		{
			checkRow=piece.row-1;
			checkCol=piece.col-2;
			if ((checkRow >= 0) && (checkCol >= 0))
			{
				Piece p=board[checkRow][checkCol].piece;
				if ((!p.getColor().equals(piece.getColor())) &&
						(p.getType()==Piece.KNIGHT))
				{
					check=true;
				}
			}
		}

		/* check left-down */
		if (!check)
		{
			checkRow=piece.row+1;
			checkCol=piece.col-2;
			if ((checkRow < 8) && (checkCol >= 0))
			{
				Piece p=board[checkRow][checkCol].piece;
				if ((!p.getColor().equals(piece.getColor())) &&
						(p.getType()==Piece.KNIGHT))
				{
					check=true;
				}
			}
		}

		if (check)
		{
			return true;
		}
		else
		{
			return false;
		}
	} //isCheck


	/*
	 * checks if making the specified move (with values from start/end row/col)
	 * will result in player's king being put in check. retuns IS_CHECK and
	 * NO_CHECK. the state of the board remains unchanged. in the function however,
	 * the move is performed, a check test is performed and the move is undone
	 * again.
	 */
	private int testCheck(Player player, int startRow, int startCol, 
			int endRow, int endCol)
	{

		/* temporarily perform the specified move */
		if (board[endRow][endCol].isEmpty())
			testMovePiece(MOVE_EMPTY, ACTION_SWITCH, startRow, startCol,
					endRow, endCol);
		else
			testMovePiece(MOVE_CAPTURE, ACTION_SWITCH, startRow, startCol,
					endRow, endCol);


		/* pass end row/col cos the pieces have been switched!!! */
		boolean result=isThreatened(player, TEST_KING, endRow, endCol);

		/* restore the original state of the board. we dont need to specify a move type */
		testMovePiece(-1, ACTION_RESTORE, startRow, startCol,
				endRow, endCol);

		return (result) ? IS_CHECK : NOT_CHECK;
	}//player

	/* 
	 * checks if the specified rook move (from startCol/row, endCol/row is valid.
	 * this is moved to a seperate function since it can be used for the queen as 
	 * well. It's really just part of function validMove()
	 */
	private int validMoveRook(Player player, Player otherPlayer, int startRow,
			int startCol, int endRow, int endCol)
	{
		/* with the rook, either the rows are the same and the cols are different,
		   or the cols are the same and the rows are different */

		Square start=board[startRow][startCol];
		Square end=board[endRow][endCol];

		int i;

		/* same row different col */
		if ((endRow==startRow) && (endCol != startCol))
		{
			/* 	make sure there's no pieces between the start and end
				positions */
			if (startCol < endCol)
			{
				for (i=startCol+1; i < endCol; i++)
				{
					Square t=board[startRow][i];
					if (!t.isEmpty())
						return MOVE_INVALID;
				}

				if (board[startRow][i]==end) 
				{
					if (end.isEmpty())
					{
						switch (testCheck(player, startRow, startCol, endRow, endCol))
						{
							case NOT_CHECK:
								{
									if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
										return MOVE_CHECK_EMPTY;
									else
										return MOVE_EMPTY;
								}

							case IS_CHECK:
								return MOVE_INVALID;
						}
					}
					else
						if (!end.piece.getColor().equals(start.piece.getColor()))
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_CAPTURE;
										else
											return MOVE_CAPTURE;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}

						}
						else
							return MOVE_INVALID;
				}
			}
			else // start > end 
			{ 
				for (i=startCol-1; i > endCol; i--)
				{
					Square t=board[startRow][i];
					if (!t.isEmpty())
						return MOVE_INVALID;
				}

				if (board[startRow][i]==end) 
				{
					if (end.isEmpty())
					{
						switch (testCheck(player, startRow, startCol, endRow, endCol))
						{
							case NOT_CHECK:
								{
									if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
										return MOVE_CHECK_EMPTY;
									else
										return MOVE_EMPTY;
								}

							case IS_CHECK:
								return MOVE_INVALID;
						}
					}
					else
						if (!end.piece.getColor().equals(start.piece.getColor()))
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_CAPTURE;
										else
											return MOVE_CAPTURE;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}

						}
						else
							return MOVE_INVALID;
				}
			}
		} //if

		/* same col different row */
		if ((endRow != startRow) && (endCol==startCol))
		{
			/* make sure there's no pieces between the start and end
			   positions */

			if (startRow < endRow)
			{
				for (i=startRow+1; i < endRow; i++)
				{
					Square t=board[i][startCol];
					if (!t.isEmpty())
						return MOVE_INVALID;
				}

				if (board[i][startCol]==end) 
				{
					if (end.isEmpty())
					{
						switch (testCheck(player, startRow, startCol, endRow, endCol))
						{
							case NOT_CHECK:
								{
									if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
										return MOVE_CHECK_EMPTY;
									else
										return MOVE_EMPTY;
								}

							case IS_CHECK:
								return MOVE_INVALID;
						}
					}
					else
						if (!end.piece.getColor().equals(start.piece.getColor()))
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_CAPTURE;
										else
											return MOVE_CAPTURE;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}

						}
						else
							return MOVE_INVALID;
				}
			}
			else
			{
				for (i=startRow-1; i > endRow; i--)
				{
					Square t=board[i][startCol];
					if (!t.isEmpty())
						return MOVE_INVALID;
				}

				if (board[i][startCol]==end) 
				{
					if (end.isEmpty())
					{
						switch (testCheck(player, startRow, startCol, endRow, endCol))
						{
							case NOT_CHECK:
								{
									if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
										return MOVE_CHECK_EMPTY;
									else
										return MOVE_EMPTY;
								}

							case IS_CHECK:
								return MOVE_INVALID;
						}
					}
					else
						if (!end.piece.getColor().equals(start.piece.getColor()))
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_CAPTURE;
										else
											return MOVE_CAPTURE;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}

						}
						else
							return MOVE_INVALID;
				}
			}
		}//if

		return MOVE_INVALID;
	} //validMoveRook


	/* checks if the move made with the bishop is valid or not.
	 * this was moved to a seperate function since it can also be used
	 * for the queen (a combination of rook and bishop). this function
	 * is really just an extention of function validMove()
	 */
	private int validMoveBishop(Player player, Player otherPlayer, int startRow,
			int startCol, int endRow, int endCol)
	{
		/* the trick with this is either incrementing or decrementing both rows
		 * and columns. That way a white bishop always stays on white squares, and
		 * black always stays on black. the same rules as above apply eg. make sure
		 * squares in between are empty etc etc 
		 *
		 *  there are 4 cases that need to be considered:
		 *  1. endRow > startRow and endCol > startCol
		 *  2. endRow > startRow and endCol < startCol
		 *  3. endRow < startRow and endCol > startCol
		 *  4. endRow < startRow and endCol < startCol
		 */

		Square start=board[startRow][startCol];
		Square end=board[endRow][endCol];

		int i;
		/* check that all the squares between start and end are empty */
		/* case 1: moving south-east */
		if ((endRow > startRow) && (endCol > startCol))
		{
			for (i=1; i < (endRow-startRow); i++)
			{
				Square t=board[startRow+i][startCol+i];
				if (!t.isEmpty())
					return MOVE_INVALID;
			}

			if (board[startRow+i][startCol+i]==end) 
			{
				if (end.isEmpty())
				{
					switch (testCheck(player, startRow, startCol, endRow, endCol))
					{
						case NOT_CHECK:
							{
								if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
									return MOVE_CHECK_EMPTY;
								else
									return MOVE_EMPTY;
							}

						case IS_CHECK:
							return MOVE_INVALID;
					}
				}
				else
					if (!end.piece.getColor().equals(start.piece.getColor()))
					{
						switch (testCheck(player, startRow, startCol, endRow, endCol))
						{
							case NOT_CHECK:
								{
									if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
										return MOVE_CHECK_CAPTURE;
									else
										return MOVE_CAPTURE;
								}

							case IS_CHECK:
								return MOVE_INVALID;
						}

					}
					else
						return MOVE_INVALID;
			}
		}

		/* case 2 - moving south-west*/
		if ((endRow > startRow) && (endCol < startCol))
		{
			for (i=1; i < (endRow-startRow); i++)
			{
				Square t=board[startRow+i][startCol-i];
				if (!t.isEmpty())
					return MOVE_INVALID;
			}

			if (board[startRow+i][startCol-i]==end) 
			{
				if (end.isEmpty())
				{
					switch (testCheck(player, startRow, startCol, endRow, endCol))
					{
						case NOT_CHECK:
							{
								if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
									return MOVE_CHECK_EMPTY;
								else
									return MOVE_EMPTY;
							}

						case IS_CHECK:
							return MOVE_INVALID;
					}
				}
				else
					if (!end.piece.getColor().equals(start.piece.getColor()))
					{
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_CAPTURE;
										else
											return MOVE_CAPTURE;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}

						}
					}
					else
						return MOVE_INVALID;
			}
		}

		/* case 3 - moving north-east */
		if ((endRow < startRow) && (endCol > startCol))
		{
			for (i=1; i < (startRow-endRow); i++)
			{
				Square t=board[startRow-i][startCol+i];
				if (!t.isEmpty())
					return MOVE_INVALID;
			}

			if (board[startRow-i][startCol+i]==end)
			{
				if (end.isEmpty())					
				{
					switch (testCheck(player, startRow, startCol, endRow, endCol))
					{
						case NOT_CHECK:
							{
								if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
									return MOVE_CHECK_EMPTY;
								else
									return MOVE_EMPTY;
							}

						case IS_CHECK:
							return MOVE_INVALID;
					}
				}
				else
					if (!end.piece.getColor().equals(start.piece.getColor()))
					{
						switch (testCheck(player, startRow, startCol, endRow, endCol))
						{
							case NOT_CHECK:
								{
									if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
										return MOVE_CHECK_CAPTURE;
									else
										return MOVE_CAPTURE;
								}

							case IS_CHECK:
								return MOVE_INVALID;
						}

					}
					else
						return MOVE_INVALID;
			}
		}

		/* case 4 - north-west */
		if ((endRow < startRow) && (endCol < startCol))
		{
			for (i=1; i < (startRow-endRow); i++)
			{
				Square t=board[startRow-i][startCol-i];
				if (!t.isEmpty())
					return MOVE_INVALID;
			}

			if (board[startRow-i][startCol-i]==end) 
			{
				if (end.isEmpty())
				{
					switch (testCheck(player, startRow, startCol, endRow, endCol))
					{
						case NOT_CHECK:
							{
								if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
									return MOVE_CHECK_EMPTY;
								else
									return MOVE_EMPTY;
							}

						case IS_CHECK:
							return MOVE_INVALID;
					}
				}
				else
					if (!end.piece.getColor().equals(start.piece.getColor()))
					{
						switch (testCheck(player, startRow, startCol, endRow, endCol))
						{
							case NOT_CHECK:
								{
									if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
										return MOVE_CHECK_CAPTURE;
									else
										return MOVE_CAPTURE;
								}

							case IS_CHECK:
								return MOVE_INVALID;
						}

					}
					else
						return MOVE_INVALID;
			}
		}

		/* make sure we're not trying to move to the same row/col */
		if ((startRow==endRow) || (startCol==endCol))
			return MOVE_INVALID;

		return MOVE_INVALID;
	}//validMoveBishop

	/* determines whether or not a move is valid based on the start/end row/col values
	   calculated. 

	   placement refers to whether we're at the top/bottom of board. if we're at the top,
	   we have to move downwards. if we're at the bottom, we move up.

	   When we get to this function, we already know that a move is being made on the board:
	   i.e. the start and end positions are definately on the board. So these checks can
	   safely be ignored here since they were preformed previously

returns: 
1 if move is valid and simply moving bead to an empty square
2 if move is valid and we're capturing a piece
0 if not valid
-1 on error

before returning, checks must be made to ensure that make the move will not leave the king
in check
*/
	private int validMove(Player player, Player otherPlayer, 
			int startRow, int startCol, int endRow, int endCol)
	{
		Square start=board[startRow][startCol];
		Square end=board[endRow][endCol];

		/* check if the move is valid for the type of chess piece we're dealing
		   with. Take into account that depending on whether the user is black or white,
		   direction will vary with certain beads, eg pawns
		   */
		switch (start.piece.getType())
		{
			case Piece.PAWN:
				{
					if (player.getPlacement()==TOP) //top moving down
					{
						if (endRow <= startRow)
							return MOVE_INVALID;

						if (start.piece.isFirstMove())
						{
							/* allowed to move 1 or 2 spaces */	
							Square up=board[startRow+1][startCol];
							if ((endRow==(startRow+2)) && (end.isEmpty()) && (up.isEmpty()) &&
									(endCol==startCol))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
												return MOVE_CHECK_EMPTY;
											else
												return MOVE_EMPTY;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}//switch
							}
						}//first move

						/* can only move one space forward */
						if ((endRow==(startRow+1)) && (end.isEmpty()) && (endCol==startCol))
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
										{
											if (endRow==7)
												return MOVE_CHECK_EMPTY;
											else
												return MOVE_CHECK_EMPTY_REPLACE; //replace pawn
										}
										else
										{
											if (endRow==7)
												return MOVE_EMPTY_REPLACE;
											else
												return MOVE_EMPTY;
										}
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}
						}

						/* check if we're capturing a piece */
						/* check down left */
						if (startCol-1 >= 0)
						{
							if ((endRow==(startRow+1)) && (endCol==(startCol-1)) &&
									(!end.piece.getColor().equals(start.piece.getColor())) &&
									(!end.isEmpty()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, 
														endRow, endCol)==IS_CHECK) 
											{
												if (endRow==7)
													return MOVE_CHECK_CAPTURE_REPLACE;
												else
													return MOVE_CHECK_CAPTURE;
											}
											else
											{
												if (endRow==7)
													return MOVE_CAPTURE_REPLACE;
												else
													return MOVE_CAPTURE;
											}
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}//switch
							}
						}

						/* check down right */
						if (startCol+1 < 8)
						{
							if ((endRow==(startRow+1)) && (endCol==(startCol+1)) &&
									(!end.piece.getColor().equals(start.piece.getColor())) &&
									(!end.isEmpty()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, 
														startRow, startCol, endRow, endCol)==IS_CHECK) 
											{
												if (endRow==7)
													return MOVE_CHECK_CAPTURE_REPLACE;
												else
													return MOVE_CHECK_CAPTURE;
											}
											else
											{
												if (endRow==7)
													return MOVE_CAPTURE_REPLACE;
												else													
													return MOVE_CAPTURE;
											}
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}//switch
							}
						}
					}//if
					else //bottom moving up
					{ 
						/* moving upwards so we want endRow < start Row */
						if (endRow >= startRow)
							return MOVE_INVALID;

						if (start.piece.isFirstMove())
						{
							/* allowed to move 1 or 2 spaces */	
							Square up=board[startRow-1][startCol];
							if ((endRow==(startRow-2)) && (end.isEmpty()) && (up.isEmpty()) &&
									(endCol==startCol))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, 
														endRow, endCol)==IS_CHECK)
												return MOVE_CHECK_EMPTY;
											else
												return MOVE_EMPTY;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}//switch
							}
						}

						/* can only move one space forward */
						if ((endRow==(startRow-1)) && (end.isEmpty()) && (endCol==startCol))
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, 
													endRow, endCol)==IS_CHECK)
										{
											if (endRow==0)
												return MOVE_CHECK_EMPTY_REPLACE;
											else
												return MOVE_CHECK_EMPTY;
										}
										else
										{
											if (endRow==0)
												return MOVE_EMPTY_REPLACE;
											else
												return MOVE_EMPTY;
										}
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}
						}

						/* check if we're capturing a piece */
						/* check up left */
						if (startCol-1 >= 0)
						{
							if ((endRow==(startRow-1)) && (endCol==(startCol-1)) &&
									(!end.piece.getColor().equals(start.piece.getColor())) &&
									(!end.isEmpty()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, 
														startCol, endRow, endCol)==IS_CHECK)
											{
												if (endRow==0)
													return MOVE_CHECK_CAPTURE_REPLACE;
												else
													return MOVE_CHECK_CAPTURE;
											}
											else
											{
												if (endRow==0)
													return MOVE_CAPTURE_REPLACE;
												else
													return MOVE_CAPTURE;
											}
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}

							}
						}

						/* check up right */
						if (startCol+1 < 8)
						{
							if ((endRow==(startRow-1)) && (endCol==(startCol+1)) &&
									(!end.piece.getColor().equals(start.piece.getColor())) &&
									(!end.isEmpty()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, 
														endRow, endCol)==IS_CHECK)
											{
												if (endRow==0)
													return MOVE_CHECK_CAPTURE_REPLACE;
												else
													return MOVE_CHECK_CAPTURE;
											}
											else
											{
												if (endRow==0)
													return MOVE_CAPTURE_REPLACE;
												else
													return MOVE_CAPTURE;
											}
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}
							}
						}
					}
				} //pawn
				break;

			case Piece.ROOK:
				return (validMoveRook(player, otherPlayer, startRow, startCol,
							endRow, endCol));

			case Piece.KNIGHT:
				{
					/* we have 8 possible moves here. we'll go north, east, south, west,
					   and for each one work out the wot the case will be if the knight
					   moved left/right/up/down
					   */

					/* north/left */
					if ((endRow==(startRow-2)) && (endCol==(startCol-1)))
					{
						if (end.isEmpty())
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_EMPTY;
										else
											return MOVE_EMPTY;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}

						}
						else
						{
							if (!end.piece.getColor().equals(start.piece.getColor()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
												return MOVE_CHECK_CAPTURE;
											else
												return MOVE_CAPTURE;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}
							}
						}
					}

					/* north/right */
					if ((endRow==(startRow-2)) && (endCol==(startCol+1)))
					{
						if (end.isEmpty())
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_EMPTY;
										else
											return MOVE_EMPTY;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}
						}
						else
						{
							if (!end.piece.getColor().equals(start.piece.getColor()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
												return MOVE_CHECK_CAPTURE;
											else
												return MOVE_CAPTURE;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}
							}
						}

					}

					/* east/up */
					if ((endRow==(startRow-1)) && (endCol==(startCol+2)))
					{
						if (end.isEmpty())
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_EMPTY;
										else
											return MOVE_EMPTY;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}
						}
						else
						{
							if (!end.piece.getColor().equals(start.piece.getColor()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
												return MOVE_CHECK_CAPTURE;
											else
												return MOVE_CAPTURE;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}
							}
						}

					}

					/* east/down */
					if ((endRow==(startRow+1)) && (endCol==(startCol+2)))
					{
						if (end.isEmpty())
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_EMPTY;
										else
											return MOVE_EMPTY;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}
						}
						else
						{
							if (!end.piece.getColor().equals(start.piece.getColor()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
												return MOVE_CHECK_CAPTURE;
											else
												return MOVE_CAPTURE;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}
							}
						}

					}

					/* south/left */
					if ((endRow==(startRow+2)) && (endCol==(startCol-1)))
					{
						if (end.isEmpty())
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_EMPTY;
										else
											return MOVE_EMPTY;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}
						}
						else
						{
							if (!end.piece.getColor().equals(start.piece.getColor()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
												return MOVE_CHECK_CAPTURE;
											else
												return MOVE_CAPTURE;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}
							}
						}

					}

					/* south/right */
					if ((endRow==(startRow+2)) && (endCol==(startCol+1)))
					{
						if (end.isEmpty())
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_EMPTY;
										else
											return MOVE_EMPTY;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}
						}
						else
						{
							if (!end.piece.getColor().equals(start.piece.getColor()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
												return MOVE_CHECK_CAPTURE;
											else
												return MOVE_CAPTURE;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}
							}
						}

					}

					/* west/up */
					if ((endRow==(startRow-1)) && (endCol==(startCol-2)))
					{
						if (end.isEmpty())
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_EMPTY;
										else
											return MOVE_EMPTY;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}
						}
						else
						{
							if (!end.piece.getColor().equals(start.piece.getColor()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
												return MOVE_CHECK_CAPTURE;
											else
												return MOVE_CAPTURE;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}
							}
						}

					}

					/* west/down */
					if ((endRow==(startRow+1)) && (endCol==(startCol-2)))
					{
						if (end.isEmpty())
						{
							switch (testCheck(player, startRow, startCol, endRow, endCol))
							{
								case NOT_CHECK:
									{
										if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
											return MOVE_CHECK_EMPTY;
										else
											return MOVE_EMPTY;
									}

								case IS_CHECK:
									return MOVE_INVALID;
							}
						}
						else
						{
							if (!end.piece.getColor().equals(start.piece.getColor()))
							{
								switch (testCheck(player, startRow, startCol, endRow, endCol))
								{
									case NOT_CHECK:
										{
											if (testCheck(otherPlayer, startRow, startCol, endRow, endCol)==IS_CHECK) //player 2 is check!!
												return MOVE_CHECK_CAPTURE;
											else
												return MOVE_CAPTURE;
										}

									case IS_CHECK:
										return MOVE_INVALID;
								}
							}
						}

					}
				}//knight
				break;

			case Piece.BISHOP:				
				{
					return validMoveBishop(player, otherPlayer, startRow, startCol,
							endRow,endCol);
				} //bishop

			case Piece.QUEEN:
				{
					if ((startCol==endCol) || (startRow==endRow))
						return (validMoveRook(player, otherPlayer, startRow, startCol,
									endRow, endCol));
					else
						return (validMoveBishop(player, otherPlayer, startRow, startCol,
									endRow, endCol));
				}//queen

			case Piece.KING:
				{
					/* like queen, with the restriction that it can only move
					 * one square at a time
					 */

					/* check if we should call rook function */
					if (startRow==endRow) 
					{
						if ((startCol != endCol) && (Math.abs(startCol-endCol)==1))
						{
							return (validMoveRook(player, otherPlayer, startRow, startCol,
										endRow, endCol));
						}
						else
						{
							/*  trying to castle the king 
							 *  make sure the king and rook havent moved before, that
							 *  there are no pieces between the king and rook, 
							 *  and that none of the spaces between the king's current
							 *  position and where it will be after the castle poses
							 *  any threat to the king i.e. the king will not be
							 *  in check in any of these positions
							 */
							if ((startCol != endCol) && (Math.abs(startCol-endCol)==2) && 
									(start.piece.isFirstMove())) 
							{
								if (startCol < endCol)
								{										
									/* player moving right */
									if ((board[startRow][7].piece.getType()==Piece.ROOK) &&
											(board[startRow][7].piece.isFirstMove()))
									{
										/* make sure all the spaces between the king and 
										 * castle are empty */
										if ((board[startRow][startCol+1].isEmpty()) &&
												(board[startRow][startCol+1].isEmpty()))
										{
											if (testCheck(player, startRow, startCol, startRow, 
														startCol+1)==NOT_CHECK)
											{
												if (testCheck(player, startRow, startCol, startRow,
															startCol+2)==NOT_CHECK)
												{
													return MOVE_CASTLE_RIGHT;
												}
												else // check
													return MOVE_INVALID;
											}
											else // check 
												return MOVE_INVALID;
										}
										else
										{
											return MOVE_INVALID;
										}
									}
								}
								else
								{
									/* player moving left */
									if ((board[startRow][0].piece.getType()==Piece.ROOK) &&
											(board[startRow][0].piece.isFirstMove()))
									{
										/* make sure the 3 spaces between the king and rook
										 * are empty */
										if ((board[startRow][startCol-1].isEmpty()) &&
												(board[startRow][startCol-2].isEmpty()) &&
												(board[startRow][startCol-2].isEmpty()))
										{
											if (testCheck(player, startRow, startCol, startRow,
														startCol-1)==NOT_CHECK)
											{
												if (testCheck(player, startRow, startCol, startRow,
															startCol-2)==NOT_CHECK)
												{
													return MOVE_CASTLE_LEFT;
												}
												else
													return MOVE_INVALID;
											}
											else // check 
												return MOVE_INVALID;
										}
										else
										{
											return MOVE_INVALID;
										}
									}
								}
							}//if castle
						}
					}
					else // startRow != endRow 
					{
						if ((startCol != endCol) && (Math.abs(startCol-endCol)==1) &&
								(Math.abs(startRow-endRow)==1))
						{
							return (validMoveBishop(player, otherPlayer, startRow, startCol,
										endRow, endCol));
						}
					}

					if (startCol==endCol)
					{
						if ((startRow != endRow) && (Math.abs(startRow-endRow)==1))
						{
							return (validMoveRook(player, otherPlayer, startRow, startCol,
										endRow, endCol));
						}
					}
					else // startCol != endCol 
					{
						if ((startRow != endRow) && (Math.abs(startRow-endRow)==1) &&
								(Math.abs(startCol-endCol)==1))
						{
							return (validMoveBishop(player, otherPlayer, startRow, startCol,
										endRow, endCol));
						}
					}
				}
				break;

			case Piece.EMPTY:
				return MOVE_INVALID;
		}//switch

		/* if we havent successfully exited already, then we have an invalid move */
		return MOVE_INVALID;
	}//validMove

	/*
	 * this is like the movePiece function below, but instead is used ONLY when testing
	 * for things like check/check-mate, when the state of the board must be 
	 * TEMPORARILY modified. when a capture is performed, this function doesn't
	 * assign the captured piece to the other player's list of captured pieces, it
	 * stores the value so that when a restore operation is performed, things
	 * can go back to normal. do NOT use this function to make permanent changes to
	 * the board. use movePiece() instead. THIS IS FOR TEMPORARY CHANGES ONLY!!!!
	 *
	 * action can be ACTION_SWITCH or ACTION_RESTORE
	 *
	 * when performing a switch, both moveType and action are necessary. when performing
	 * a restore, only action is necessary. moveType is ignored. it's value is determined from
	 * tempMoveType
	 */
	private void testMovePiece(int moveType, int action, int startRow, int startCol,
			int endRow, int endCol)
	{
		Square start=board[startRow][startCol];
		Square end=board[endRow][endCol];

		int type=(tempMoveType != MOVE_NONE) ? tempMoveType : moveType;

		switch (action)
		{
			case ACTION_SWITCH:
				{
					switch (type)
					{
						case MOVE_EMPTY:
							{
								tempPiece=start.piece;
								start.piece=end.piece;
								end.piece=tempPiece;

								/* update position */
								start.piece.setPosition(startRow, startCol);
								end.piece.setPosition(endRow, endCol);

								/* set the action type */
								tempMoveType=MOVE_EMPTY;

								/* reset temp vars */
								tempPiece=null;
							}
							break;

						case MOVE_CAPTURE:
							{
								tempPiece=end.piece;
								end.piece=start.piece;
								start.piece=tempEmptyPiece; //global empty piece

								start.piece.setPosition(startRow, startCol);
								end.piece.setPosition(endRow, endCol);

								/* set the action type */
								tempMoveType=MOVE_CAPTURE;
							}
							break;
					} //switch type
				} //case action_switch
				break;

			case ACTION_RESTORE: // moveType ignored. this restores the last action 
				{
					switch (type)
					{	
						case MOVE_EMPTY:
							{
								tempPiece=start.piece;
								start.piece=end.piece;
								end.piece=tempPiece;

								start.piece.setPosition(startRow, startCol);
								end.piece.setPosition(endRow, endCol);

								/* reset temp values */
								tempPiece=null;
								tempMoveType=MOVE_NONE;
							}
							break;

						case MOVE_CAPTURE: // tempPiece stores the piece to restore 
							{
								start.piece=end.piece;
								end.piece=tempPiece;

								/* no need to set the position for end piece since it
								 * wasnt changed to begin with*/
								start.piece.setPosition(startRow, startCol);
								tempEmptyPiece.setPosition(-2, -2);

								/* reset temp values */
								tempPiece=null;
								tempMoveType=MOVE_NONE;
							}
							break;
					}//switch for restore->type 
				} //case action_restore
				break;
		}//switch action
	}//testMovePiece

	/*
	   actually move a piece.
	   to move to an empty square, we just swap the contents of the squares.
	   to capture a piece, we replace the piece and assign the captured piece to 
	   the other player's list of captured pieces
	   */
	private void movePiece(int moveType, Player player, Player otherPlayer,
			int startRow, int startCol, int endRow, int endCol)
	{
		Square start=board[startRow][startCol];
		Square end=board[endRow][endCol];

		switch(moveType)
		{
			case MOVE_EMPTY:
				{
					Piece t=start.piece;
					start.piece=end.piece;
					end.piece=t;

					/* update position of the piece */
					end.piece.setPosition(endRow, endCol);
					start.piece.setPosition(startRow, startCol); //not necessary for empty piece
				}
				break;

			case MOVE_CAPTURE:
				{
					/* assign captured piece to other player's list of captured pieces 
					 * and remove it from owner's list
					 */
					player.addCapturedPiece(end.piece);
					otherPlayer.removePiece(endRow, endCol); // the board still points 
					//  to the piece so this is 
					//  fine

					end.piece=start.piece;

					/* WARNING. START WILL STILL POINT TO THE OLD OBJECT EVEN AFTER
					 * WE CALL THIS FUNCTION UNLESS WE EXPLICITLY CHANGE IT. REMEMBER,
					 * START IS JUST A POINTER TO AN OBJECT AND THIS FUNCTION DOES
					 * NOT DELETE THE OBJECT START WAS POINTING TO!!!!!!!!
					 */
					setEmpty(startRow, startCol);
					start=board[startRow][startCol]; // this line is NECESSARY since we updated
					//	board[startRow][startCol] 


					/* update position of piece */
					end.piece.setPosition(endRow, endCol);
					start.piece.setPosition(-2,-2); //indicate it's not on the board
				}
				break;
		}//switch
	}//movePiece

	/*
	 * move is a range eg. a1-a5
	 * this function tests if the move is valid, and if so makes the move
	 * tracking captured pieces etc etc
	 *
	 * return:
	 * 1=success
	 * 0=move not successful
	 * -1=error
	 */
	int makeMove(String move, Player player, Player otherPlayer)
	{
		if (processRange(move)==-1)
		{
			return RETURN_ERROR_INVALID_MOVE;
		}

		/* we do things this way since every function should use
		 * their parameters for start/end row/col...NOT the
		 * vars global to this function
		 */
		int startRow=this.startRow.intValue();
		int endRow=this.endRow.intValue();
		int startCol=this.startCol.intValue();
		int endCol=this.endCol.intValue();

		/* move the piece and indicate that the first move has already been made */
		//System.out.print("Pieces belonging to "+player.getName()+": ");
		//player.showPieces();
		switch (validMove(player, otherPlayer, startRow, startCol,
					endRow, endCol))
		{
			case MOVE_EMPTY:
				{
					movePiece(MOVE_EMPTY, player, otherPlayer, startRow, startCol,
							endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove(); 
					prependMessage(getBoard()+"\n");
					return RETURN_MOVE_EMPTY;
				}

			case MOVE_CAPTURE:
				{
					movePiece(MOVE_CAPTURE, player, otherPlayer, startRow,
							startCol, endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove();
					prependMessage(getBoard()+"\n");
					return RETURN_MOVE_CAPTURE;
				}

			case MOVE_INVALID:
					System.out.println("Got here a");
					return RETURN_ERROR_INVALID_MOVE;

			case MOVE_CHECK_EMPTY:
				{
					movePiece(MOVE_EMPTY, player, otherPlayer, startRow,
							startCol, endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove();

					/* warn the other player that he's in check */
					otherPlayer.appendMessage("Information: Check!!!!\n");

					/* add board to the message to display to both users */
					prependMessage(getBoard()+"\n");

					return RETURN_MOVE_EMPTY_CHECK;
				}

			case MOVE_CHECK_CAPTURE:
				{
					movePiece(MOVE_CAPTURE, player, otherPlayer, startRow,
							startCol, endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove();
					
					otherPlayer.appendMessage("Information: Check!!!!\n");
					prependMessage(getBoard()+"\n");
					return RETURN_MOVE_CAPTURE_CHECK;
				}

				/* the next 4 cases are for when pawns need to be replaced with another piece */
			case MOVE_EMPTY_REPLACE:
				{
					movePiece(MOVE_EMPTY, player, otherPlayer, startRow, startCol,
							endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove(); 

					/* store position of pawn to replace */
					replacePawnRow=endRow;
					replacePawnCol=endCol;

					prependMessage(getBoard()+"\n");
					player.appendMessage("Information: Enter a new piece to replace your pawn.\n");
					player.appendMessage("Use the \"game replace <ID> <NewPiece>\" command\n");

					return RETURN_MOVE_EMPTY_REPLACE_PAWN;
				}

			case MOVE_CHECK_EMPTY_REPLACE:
				{
					movePiece(MOVE_EMPTY, player, otherPlayer, startRow,
							startCol, endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove();

					/* store position of pawn to replace */
					replacePawnRow=endRow;
					replacePawnCol=endCol;
					
					prependMessage(getBoard()+"\n");
					otherPlayer.appendMessage("Information: Check!!!!\n");
					player.appendMessage("Information: Enter a new piece to replace your pawn.\n");
					player.appendMessage("Use the \"game replace <ID> <NewPiece>\" command\n");				

					return RETURN_MOVE_EMPTY_CHECK_REPLACE_PAWN;
				}

			case MOVE_CAPTURE_REPLACE:
				{
					movePiece(MOVE_CAPTURE, player, otherPlayer, startRow,
							startCol, endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove();

					/* store position of pawn to replace */
					replacePawnRow=endRow;
					replacePawnCol=endCol;
					
					prependMessage(getBoard()+"\n");
					player.appendMessage("Information: Enter a new piece to replace your pawn.\n");
					player.appendMessage("Use the \"game replace <ID> <NewPiece>\" command\n");				

					return RETURN_MOVE_CAPTURE_REPLACE;
				}

			case MOVE_CHECK_CAPTURE_REPLACE:
				{
					movePiece(MOVE_CAPTURE, player, otherPlayer, startRow,
							startCol, endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove();

					/* store position of pawn to replace */
					replacePawnRow=endRow;
					replacePawnCol=endCol;

					prependMessage(getBoard()+"\n");
					otherPlayer.appendMessage("Information: Check!!!!\n");
					player.appendMessage("Information: Enter a new piece to replace your pawn.\n");
					player.appendMessage("Use the \"game replace <ID> <NewPiece>\" command\n");				
	
					return RETURN_MOVE_CHECK_CAPTURE_REPLACE_PAWN;
				}

			case MOVE_CASTLE_LEFT: //ie the king is begin castled to the left
				{
					/* move the king */
					movePiece(MOVE_EMPTY, player, otherPlayer, startRow,
							startCol, endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove();

					/* move the castle 3 spaces to the right*/
					movePiece(MOVE_EMPTY, player, otherPlayer, startRow,
							0, startRow, 3);
					
					prependMessage(getBoard()+"\n");

					/* check if other player is check */
					if (isThreatened(otherPlayer, TEST_KING, -1, -1))
					{
						otherPlayer.appendMessage("Information: Check!!!\n");
						return RETURN_MOVE_CASTLE_LEFT_CHECK;
					}
					else
						return RETURN_MOVE_CASTLE_LEFT;
				}
			case MOVE_CASTLE_RIGHT: //king is being castled to the right
				{
					/* move the king */
					movePiece(MOVE_EMPTY, player, otherPlayer, startRow,
							startCol, endRow, endCol);
					board[endRow][endCol].piece.setNotFirstMove();

					/* move the castle */
					movePiece(MOVE_EMPTY, player, otherPlayer, startRow,
							7, startRow, 5);

					prependMessage(getBoard()+"\n");

					/* check if the other player is in check after this move */
					if (isThreatened(otherPlayer, TEST_KING, -1, -1))
					{
						otherPlayer.appendMessage("Information: Check!!!\n");
						return RETURN_MOVE_CASTLE_RIGHT_CHECK;
					}
					else
						return RETURN_MOVE_CASTLE_RIGHT;
				}

			case RETURN_ERROR:
				{
					return RETURN_ERROR;
				}
		}//switch

		return RETURN_ERROR_INVALID_MOVE;
	}//makeMove

	/* 
	 * replace a pawn at replacePawnRow and replacePawnCol
	 * with the specified piece. The piece cannot be a king
	 * or another pawn. it can be R,H,B or Q
	 */
	int replacePawn(Player player, Player otherPlayer, String piece)
	{
		Piece p=board[replacePawnRow][replacePawnCol].piece;

		piece=piece.toUpperCase();

		if (piece.equals("R"))
		{
			p.setDisplayType(piece);
		}
		else if (piece.equals("H"))
		{
			p.setDisplayType(piece);
		}
		else if (piece.equals("B"))
		{
			p.setDisplayType(piece);
		}
		else if (piece.equals("Q"))
		{
			p.setDisplayType(piece);
		}
		else
		{
			return RETURN_ERROR_INVALID_REPLACEMENT_PIECE;
		}

		/* reset the values of replacePawnRow/Col */
		replacePawnRow=-1;
		replacePawnCol=-1;

		/* check if this replacement has put the other player into check */
		if (isThreatened(otherPlayer, TEST_KING, -1, -1))
			otherPlayer.appendMessage("Information: Check!!!\n");

		prependMessage(getBoard()+"\n");
		return RETURN_PAWN_REPLACED;
	}//replacePawn


	/* returns the board as a string, ready for display */
	String getBoard()
	{
		String outputString=new String("\n");

		for (int row=0; row<8; row++)
		{
			outputString=outputString.concat(" "+(8-row)+" ");
			for (int col=0; col<8; col++)
			{
				Square t=board[row][col];

				if (t.piece.isEmpty())
				{
					outputString=outputString.concat(t.color+"   ");
				}
				else
				{
					outputString=outputString.concat(t.color+t.piece.color+" "+t.piece.getDisplayType()+" ");
				}//for col
			}
			outputString=outputString.concat(NORMAL+"\n");
		}//for row

		/* print out letters */
		outputString=outputString.concat("   ");
		for (int col=0; col<8; col++)
		{
			outputString=outputString.concat(" "+colLetters[col]+" ");
		}
		outputString=outputString.concat("\n\n");

		return outputString;
	}//showBoard

	/* 
	 * retrieves the board with getBoard() and stores it in the message
	 * buffer. it will be displayed in Chess.java. That's where everything
	 * is displayed
	 */
	int showBoard()
	{
		String board=getBoard();
		prependMessage(board);
		return RETURN_SHOW_BOARD;
	}
}//java
