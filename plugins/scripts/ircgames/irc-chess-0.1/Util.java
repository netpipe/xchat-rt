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

/* abstract class that contains several definitions, maybe useful functions etc */

public abstract class Util
{
	/* color definitions */
	final static String NONE		=	new String("");

	/* text colors */
	final static String BLACK   	=   new String("<BLACK>");
	final static String RED			=	new String("<RED>");
	final static String GREEN		=	new String("<GREEN>");
	final static String YELLOW		=	new String("<YELLOW>");
	final static String BLUE		=	new String("<BLUE>");
	final static String MAGENTA		=	new String("<MAGENTA>");
	final static String CYAN		=	new String("<CYAN>");
	final static String WHITE		=	new String("<WHITE>");

	/* bold text colors */
	final static String bBLACK   	=   new String("<bBLACK>");
	final static String bRED		=	new String("<bRED>");
	final static String bGREEN		=	new String("<bGREEN>");
	final static String bYELLOW		=	new String("<bYELLOW>");
	final static String bBLUE		=	new String("<bBLUE>");
	final static String bMAGENTA	=	new String("<bMAGENTA>");
	final static String bCYAN		=	new String("<bCYAN>");
	final static String bWHITE		=	new String("<bWHITE>");

	/* misc color manipulators */
	final static String NORMAL		=	new String("<NORMAL>");
	final static String BOLD		=	new String("<BOLD>");
	final static String UNDERLINE	=	new String("<UNDERLINE>");

	/* background colors */
	final static String BBLACK   	=   new String("<BBLACK>");
	final static String BRED		=	new String("<BRED>");
	final static String BGREEN		=	new String("<BGREEN>");
	final static String BYELLOW		=	new String("<BYELLOW>");
	final static String BBLUE		=	new String("<BBLUE>");
	final static String BMAGENTA	=	new String("<BMAGENTA>");
	final static String BCYAN		=	new String("<BCYAN>");
	final static String BWHITE		=	new String("<BWHITE>");


	/* position defs i.e. where the user starts off on the screen */
	final static int TOP=1;
	final static int BOTTOM=2;

	/*
	 * general return types 
	 * the return types above are mostly
	 * for specific functions. these are used
	 * throughout the code
	 *
	 * Ok, there's a whole lotta return types below. their names
	 * are the combination of several basic words:
	 *
	 * ERROR - duh
	 * FAILED - when a command fails, but no error occurs eg. when a game cannot
	 * 			be started for some reason. this can usually be interchanged with
	 * 			ERROR. It's here mostly for compatibility with old code
	 * NORMAL - same as return 1
	 * REPLACE/REPLACE_PAWN - when a pawn must be replaced with another piece
	 * MOVE - when a move was performed eg. a pawn was moved to another square
	 * EMPTY - a move to an empty square
	 * CAPTURE - a move to a square by capturing another piece
	 * CASTLE_LEFT/CASTLE_RIGHT - king is castled to the left/right
	 *
	 * the error codes are pretty self explanatory.
	 */
	final static int RETURN_ERROR=-1;
	final static int RETURN_FAILED=0;
	final static int RETURN_NORMAL=1;
	final static int RETURN_REPLACE_PAWN=2;
	final static int RETURN_ERROR_NO_START=3;
	final static int RETURN_ERROR_NOT_YOUR_MOVE=4;
	final static int RETURN_ERROR_MAX_NUM_REG=5; //max num players registered for game
	final static int RETURN_PLAYER1_REG=6; //player registered for game
	final static int RETURN_PLAYER2_REG=7;
	final static int RETURN_ERROR_ONLY_REG_ONCE=8;
	final static int RETURN_ERROR_GAME_ALREADY_STARTED=9;
	final static int RETURN_ERROR_INVALID_MOVE=10;
	final static int RETURN_ERROR_INVALID_REPLACEMENT_PIECE=11;
	final static int RETURN_SHOW_BOARD=12;
	final static int RETURN_MOVE_EMPTY=13;
	final static int RETURN_MOVE_CAPTURE=14;
	final static int RETURN_MOVE_EMPTY_CHECK=15;
	final static int RETURN_MOVE_CAPTURE_REPLACE=16;
	final static int RETURN_MOVE_EMPTY_REPLACE_PAWN=17;
	final static int RETURN_MOVE_CAPTURE_CHECK=18;
	final static int RETURN_MOVE_EMPTY_CHECK_REPLACE_PAWN=19;	
	final static int RETURN_MOVE_CHECK_CAPTURE_REPLACE_PAWN=20;
	final static int RETURN_MOVE_CASTLE_LEFT=21;
	final static int RETURN_MOVE_CASTLE_RIGHT=22;
	final static int RETURN_MOVE_CASTLE_LEFT_CHECK=23;
	final static int RETURN_MOVE_CASTLE_RIGHT_CHECK=24;
	final static int RETURN_PAWN_REPLACED=25;
}//class
