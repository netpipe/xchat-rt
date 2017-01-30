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

/* information about a single piece on the board */

public class Piece
{	
	private int type; //king, queen, rook etc. values to be used internally
	private boolean firstMove=true; //becomes false when this piece is moved
	String displayType; //king=K, queen=Q etc. this is what appears on screen.
	String color;
	int row, col; //position of the piece on the board. -1 means it doesnt belong to a player
	
	/* 
	 * type==EMPTY color==NONE denotes no piece. every square has a 
	 * piece, so squares on the board that are empty have empty pieces
	 */

	/* bead types */
	final static int EMPTY=0;
	final static int ROOK=1;
	final static int KNIGHT=2;
	final static int BISHOP=3;
	final static int QUEEN=4;
	final static int KING=5;
	final static int PAWN=6;

	Piece(Piece piece)
	{
		type=piece.type;
		firstMove=piece.firstMove;
		displayType=new String(piece.displayType);
		color=new String(piece.color);
	}

	Piece(String displayType, String color, int row, int col)
	{
		this.row=row;
		this.col=col;
		
		if (displayType.equals("K"))
		{
			this.displayType=new String(displayType);
			this.color=new String(color);
			type=KING;
		}

		if (displayType.equals("Q"))
		{
			this.displayType=new String(displayType);
			this.color=new String(color);
			type=QUEEN;
		}

		if (displayType.equals("B"))
		{
			this.displayType=new String(displayType);
			this.color=new String(color);
			type=BISHOP;
		}

		if (displayType.equals("H"))
		{
			this.displayType=new String(displayType);
			this.color=new String(color);
			type=KNIGHT;
		}

		if (displayType.equals("R"))
		{
			this.displayType=new String(displayType);
			this.color=new String(color);
			type=ROOK;
		}

		if (displayType.equals("P"))
		{
			this.displayType=new String(displayType);
			this.color=new String(color);
			type=PAWN;
		}

		if (displayType.equals("E")) //empty
		{
			this.displayType=new String(displayType);
			this.color=new String(color);
			type=EMPTY;
		}
	}

	/* sets the position of this piece on the board 
	 * -1 means it's an empty piece on the board
	 * -2 means it's in a player's list of captured pieces, not on the board
	 */
	void setPosition(int row, int col)
	{
		this.row=row;
		this.col=col;
	}
	
	boolean isFirstMove()
	{
		return firstMove;
	}

	void setNotFirstMove()
	{
		firstMove=false;
	}

	String getColor()
	{
		return color;
	}

	boolean isEmpty()
	{
		if (this.getType()==EMPTY)
			return true;
		else
			return false;
	}

	int getType()
	{
		return this.type;
	}

	/* this function assumes a valid type
	 * will be passed as a parameter */
	void setType(int type)
	{
		this.type=type;
	}
	
	String getDisplayType()
	{
		return this.displayType;
	}

	void setDisplayType(String displayType)
	{
		this.displayType=new String(displayType);
		if (displayType.equals("R"))
			this.type=ROOK;
		else if (displayType.equals("H"))
			this.type=KNIGHT;
		else if (displayType.equals("B"))
			this.type=BISHOP;
		else if (displayType.equals("Q"))
			this.type=QUEEN;
		else if (displayType.equals("K"))
			this.type=KING;
		else if (displayType.equals("P"))
			this.type=PAWN;
		else if (displayType.equals("E"))
			this.type=EMPTY;
	}
	
	/* change the type and displayType values to empty */
	void setEmpty()
	{
		type=EMPTY;
		displayType=new String("E");
	}
}//class Piece

