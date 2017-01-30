#xchat frontend for the java server. I dont normally script for xchat (I dont normally
#script), but this seems to work. It's basically the irssi script with a few
#xchat specific modifications. I couldnt seem to get the /end_game command handler
#working tho. When the line is uncommented, it makes a connection then immediately
#disconnects. Maybe someone else can have a look at it?

use IO::Socket;

IRC::register("irc-chess", "0.1", "", "");
IRC::add_message_handler("PRIVMSG", sig_processPvt);
#IRC::add_command_handler("end_game", cmd_endGame);

#xchat colors:
#0=white
#1=black
#2=blue
#8-yellow
#foreground on foreground is %C<forground>,<background>
sub processColors
{
	$_=$_[0];

	#replace foreground/background colors
	my @fgbg;
	my $numFgBg=(@fgbg=/(<B\w+?><b\w+?>)/g);

	for (my $j=0; $j < $numFgBg; $j++)
	{
		my $t=$fgbg[$j];
		my ($background, $foreground)=($t=~/(<B\w+?>)(<b\w+?>)/);
		my $orig=$background.$foreground;

		$foreground=~s/<bBLACK>/%C14/g;
		$foreground=~s/<bWHITE>/%C0/g;

		$background=~s/<BBLUE>/,2/g;
		$background=~s/<BYELLOW>/,5/g;

		my $result=$foreground.$background;

		s/$orig/$result/;
	}

	#replace background-only colors
	s/<BBLUE>/%C0,2/g;
	s/<BYELLOW>/%C0,5/g;

	#replace rest of colors
	s/<NORMAL>/%C0/g;

	return $_;
}#processColors

#
#message formats:
#1. simple format:
#[username]Message
#
#2. complex format:
#[user1]msg1<:=:>[user2]msg2<:=:>commonMessage
#
sub processMsgFromServer 
{
	($msg, $nick)=@_;
	$delimiter="<:=:>";
	$_=$msg;

	#determine the type of message from the number of delimiters
	$numDelims=(@list=/$delimiter/g);

	if ($numDelims==0)
	{
		#simple message
		my ($username, $message)=/^\s*\[(.+?)\](.*?)$/;

		$message=processColors($message);

		#send message to player
		IRC::command("/msg $nick $message");
	}
	else
	{
		#complex message
		my ($user1, $msg1, $user2, $msg2, $commonMessage)=/^\s*\[(.+?)\](.*?)$delimiter\[(.+?)\](.*?)$delimiter(.*)$/s;

		#split message into seperate lines
		my @commonMessageList=split(/\n/, $commonMessage);

		#send common message to both users
		IRC::print("Sending common message to both users");
		my $numStrings;
		my @list;

		#print out blank lines since the string was split on newlines so
		#now they're lost. an extra space == blank line
		IRC::command("/msg $user1 %C0 "); 
		IRC::command("/msg $user2 %C0 "); 

		my $commonListSize=@commonMessageList;
		for (my $j=0; $j<$commonListSize; $j++)
		{
			$commonMessageList[$j]=processColors($commonMessageList[$j]);

			IRC::command("/msg $user1 $commonMessageList[$j]"); 
			IRC::command("/msg $user2 $commonMessageList[$j]"); 
		}
		IRC::command("/msg $user1 %C0 "); 
		IRC::command("/msg $user2 %C0 "); 

		#send messages for each user
		my @msg1List=split(/\n/, $msg1);
		my $msg1ListSize=@msg1List;

		for ($j=0; $j<$msg1ListSize; $j++)
		{
			IRC::command("/msg $user1 %B$msg1List[$j]%C0");
		}

		my @msg2List=split(/\n/, $msg2);
		my $msg2ListSize=@msg2List;

		for ($j=0; $j<$msg2ListSize; $j++)
		{
			IRC::command("/msg $user2 %B$msg2List[$j]%C0");
		}
	} #else
}#processOutput

#
#process a message received from the user.
#this will be something like "game start k"
#this will have to be changed to something like
#"game start k k1", where k1 is the user.
#if the format of the message is not correct,
#return INVALID. format checking is minimal.
#basically, if the first word is "game", then
#slap on the nickname and send it to the server.
#

sub processMsgFromClient
{
	($msg, $nick)=@_; 

	#IRC::print("msg from client:\n$msg\n");
	$msg=lc($msg);	

	if ($msg=~/^game\b/)
	{
		$msg = $msg." $nick";
		return $msg;
	}
	else
	{
		IRC::print("sending: msg $nick Error: Invalid Message");
		IRC::command("/msg $nick Error: Invalid Message");
		return "INVALID";	
	}
}#processMsgFromClient

#
#private messages received from other users eg. if they want to
#register a new game
sub sig_processPvt
{
	my ($address, $msgType, $myNick, $msg)=($_[0]=~/^:(.*?)\s(.*?)\s(.*?)\s:(.*?)$/);	
	my ($nick, $ident)=split(/!/, $address);
	
	$msgToSend=processMsgFromClient($msg, $nick);
	
	if ($msgToSend !~ /^INVALID$/)
	{
		IRC::print("Sending message now");
		send(SOCKET,$msgToSend,0);
		IRC::print("Waiting for message from server\n");
		recv(SOCKET,$buffer,32678,0); #read a max of 32k. 
		IRC::print("Received message");
		processMsgFromServer($buffer, $nick);
	}

	return;
}#sig_processPvt

#
#function to terminate game. it basically just closes 
#the connection to the server
#
sub cmd_endGame
{
	shutdown(SOCKET,2);
	close(SOCKET);
	IRC::print("Game ended. Socket shut down");
	$gameRunning=0;

	return 1;
}#cmd_endGame

BEGIN
{
	$PORT=1234;
	
	IRC::print("connecting to server\n");
	$tcpProtocolNumber = getprotobyname('tcp') || 6; 

	socket(SOCKET, PF_INET(), SOCK_STREAM(), $tcpProtocolNumber)
		or die("socket: $!");

	$internetPackedAddress = pack('S na4 x8', AF_INET(), $PORT, 127.0.0.1); 
	connect(SOCKET, $internetPackedAddress) or die("connect: $!");

	IRC::print("Game is now running");
	$gameRunning=1;
	
	IRC::print("IRC-Chess Script loaded\n");
}

