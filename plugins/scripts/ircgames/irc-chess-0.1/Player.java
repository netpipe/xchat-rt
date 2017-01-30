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
 * Class to store all data for a player eg. color, name etc
 */
import java.util.*;

public class Player extends Util
{
	private String name;
	private String color;
	private int playerNum; //player1 or player2
	private int placement; //where the user starts out on the screen ie. top or bottom
	private String message=new String(""); //message to send to the client will be stored here
	Vector pieces=new Vector(); //pieces belonging to this user
	Vector capturedPieces=new Vector(); //list of captured pieces
	
	Player(String name, int playerNum, String color)
	{
		this.name=new String(name);
		this.color=new String(color);
		if (color.equals(BLACK))
			placement=TOP;
		else
			placement=BOTTOM;

		
		this.playerNum=playerNum;
		if (this.playerNum==1)
		{
			/* player 1 is white by default, and at the bottom of the screen */
			pieces.add(new Piece("P",bWHITE, 6, 0));
			pieces.add(new Piece("P",bWHITE, 6, 1));
			pieces.add(new Piece("P",bWHITE, 6, 2));
			pieces.add(new Piece("P",bWHITE, 6, 3));
			pieces.add(new Piece("P",bWHITE, 6, 4));
			pieces.add(new Piece("P",bWHITE, 6, 5));
			pieces.add(new Piece("P",bWHITE, 6, 6));
			pieces.add(new Piece("P",bWHITE, 6, 7));

			pieces.add(new Piece("R",bWHITE, 7, 0));
			pieces.add(new Piece("H",bWHITE, 7, 1));
			pieces.add(new Piece("B",bWHITE, 7, 2));
			pieces.add(new Piece("Q",bWHITE, 7, 3));
			pieces.add(new Piece("K",bWHITE, 7, 4));
			pieces.add(new Piece("B",bWHITE, 7, 5));
			pieces.add(new Piece("H",bWHITE, 7, 6));
			pieces.add(new Piece("R",bWHITE, 7, 7));
		}
		else
		{
			/* player 2 is black by default, and at the top of the screen */
			pieces.add(new Piece("P",bBLACK, 1, 0));
			pieces.add(new Piece("P",bBLACK, 1, 1));
			pieces.add(new Piece("P",bBLACK, 1, 2));
			pieces.add(new Piece("P",bBLACK, 1, 3));
			pieces.add(new Piece("P",bBLACK, 1, 4));
			pieces.add(new Piece("P",bBLACK, 1, 5));
			pieces.add(new Piece("P",bBLACK, 1, 6));
			pieces.add(new Piece("P",bBLACK, 1, 7));

			pieces.add(new Piece("R",bBLACK, 0, 0));
			pieces.add(new Piece("H",bBLACK, 0, 1));
			pieces.add(new Piece("B",bBLACK, 0, 2));
			pieces.add(new Piece("Q",bBLACK, 0, 3));
			pieces.add(new Piece("K",bBLACK, 0, 4));
			pieces.add(new Piece("B",bBLACK, 0, 5));
			pieces.add(new Piece("H",bBLACK, 0, 6));
			pieces.add(new Piece("R",bBLACK, 0, 7));
		}
	}

	public String getMessage()
	{
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

	public void clearMessage()
	{
		message=new String("");
	}
	
	public int getPlayerNum()
	{
		return playerNum;
	}

	/* returns a pointer to this player's king */
	public Piece getKing()
	{
		for (int i=0; i<pieces.size(); i++)
		{
			Piece p=(Piece)(pieces.get(i));

			if (p.getType()==Piece.KING)
			{
				return p; 
			}
		}

		System.err.println("WARNING!!!! RETURNING NULL FOR KING!!!!");
		return null;
	}
	
	/* remove a piece from this players list of pieces. this happens when
	 * a piece is captured by the other player */
	public int removePiece(int row, int col)
	{
		for (int i=0; i < pieces.size(); i++)
		{
			Piece p=(Piece)(pieces.get(i));

			if ((p.row==row) && (p.col==col))
			{
				pieces.remove(i);
				return 1;
			}
		}//for

		return -1;
	}

	/* shows the pieces belonging to this user */
	public void showPieces()
	{
		for (int i=0; i<pieces.size(); i++)
		{
			Piece p=(Piece)(pieces.get(i));
			System.out.println(p.getDisplayType()+" row: "+p.row+" col: "+p.col);
		}
		System.out.println("");
	}
	
	/* add the specified captured piece to list of captured pieces */
	public void addCapturedPiece(Piece piece)
	{
		capturedPieces.add(piece);	
	}
	
	/* get a piece belonging to this player from the specified row and column 
	 * returns null if piece is not found
	 */
	public Piece getPiece(int row, int col)
	{
		for (int i=0; i<pieces.size(); i++)
		{
			Piece p=(Piece)(pieces.get(i));
			if ((p.row==row) && (p.col==col))
				return p;
		}

		return null;
	}
	
	public String getColor()
	{
		return color;
	}

	public void setColor(String color)
	{
		this.color=color;
	}

	public String getName()
	{
		return name;
	}

	public void setName(String name)
	{
		this.name=new String(name);
	}

	public int getPlacement()
	{
		return placement;
	}
}//class player
