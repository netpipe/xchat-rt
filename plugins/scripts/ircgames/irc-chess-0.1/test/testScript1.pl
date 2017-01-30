#script to quickly make several chess moves

use Irssi;
use Irssi::Irc;

sub sig_setup1
{
	my ($server, $message, $nick, $address, $target) = @_;

	if ($message=="0")
	{
		$server->command("msg $target game register k k1");
		$server->command("msg $target game register k k2");
		$server->command("msg $target game start k");
		$server->command("msg $target game move k d2-d4 k1");
		$server->command("msg $target game move k e7-e5 k2");
	}

	if ($message=="1")
	{
		$server->command("msg $target game register k k1");
		$server->command("msg $target game register k k2");
		$server->command("msg $target game start k");
		$server->command("msg $target game move k d2-d4 k1");
		$server->command("msg $target game move k e7-e5 k2");
		$server->command("msg $target game move k d1-d3 k1");
		$server->command("msg $target game move k g8-f6 k2");
		$server->command("msg $target game move k b1-c3 k1");
	}

	#almost gets to a check position
	if ($message=="2")
	{
		$server->command("msg $target game register k k1");
		$server->command("msg $target game register k k2");
		$server->command("msg $target game start k");
		$server->command("msg $target game move k d2-d4 k1");
		$server->command("msg $target game move k e7-e5 k2");
		$server->command("msg $target game move k d1-d3 k1");
		$server->command("msg $target game move k g8-f6 k2");
		$server->command("msg $target game move k b1-c3 k1");
		$server->command("msg $target game move k f6-e4 k2");
		$server->command("msg $target game move k d3-b5 k1");
		$server->command("msg $target game move k d8-h4 k2");
		#	$server->command("msg $target game move k b5-e5 k1");

	}

	#gets to a check position
	if ($message=="3")
	{
		$server->command("msg $target game register k k1");
		$server->command("msg $target game register k k2");
		$server->command("msg $target game start k");
		$server->command("msg $target game move k d2-d4 k1");
		$server->command("msg $target game move k e7-e5 k2");
		$server->command("msg $target game move k d1-d3 k1");
		$server->command("msg $target game move k g8-f6 k2");
		$server->command("msg $target game move k b1-c3 k1");
		$server->command("msg $target game move k f6-e4 k2");
		$server->command("msg $target game move k d3-b5 k1");
		$server->command("msg $target game move k d8-h4 k2");
		$server->command("msg $target game move k b5-e5 k1");
	}

	#point before where knight was causing a null pointer exception
	if ($message=="4")
	{
		$server->command("msg $target game register k k1");
		$server->command("msg $target game register k k2");
		$server->command("msg $target game start k");
		$server->command("msg $target game move k d2-d4 k1");
		$server->command("msg $target game move k e7-e5 k2");
		$server->command("msg $target game move k d1-d3 k1");
		$server->command("msg $target game move k g8-f6 k2");
		$server->command("msg $target game move k b1-c3 k1");
		$server->command("msg $target game move k f6-e4 k2");
		$server->command("msg $target game move k d3-b5 k1");
		$server->command("msg $target game move k d8-h4 k2");
		$server->command("msg $target game move k b5-e5 k1");
		$server->command("msg $target game move k e8-d8 k2");
		$server->command("msg $target game move k e5-c7 k1");
		$server->command("msg $target game move k d8-c7 k2");
	}

	#setup to test castle1
	if ($message=="5")
	{
		$server->command("msg $target game register k k1");
		$server->command("msg $target game register k k2");
		$server->command("msg $target game start k");
		$server->command("msg $target game move k g2-g3 k1");	
		$server->command("msg $target game move k b7-b6 k2");	
		$server->command("msg $target game move k g1-h3 k1");	
		$server->command("msg $target game move k c8-a6 k2");	
		$server->command("msg $target game move k f1-g2 k1");	
		$server->command("msg $target game move k e7-e5 k2");	
		$server->command("msg $target game move k d2-d4 k1");	
		$server->command("msg $target game move k g7-g5 k2");	
		$server->command("msg $target game move k c1-g5 k1");	
		$server->command("msg $target game move k d8-f6 k2");	
		$server->command("msg $target game move k f2-f4 k1");	
		$server->command("msg $target game move k b8-c6 k2");	

		#both kings can now castle
		#$server->command("msg $target game move k f2-f4 k1");	
	}
	
	#gets to a point where a pawn can be replaces for another piece
	if ($message=="6")
	{
		$server->command("msg $target game register k k1");
		$server->command("msg $target game register k k2");
		$server->command("msg $target game start k");
		$server->command("msg $target game move k g2-g3 k1");	
		$server->command("msg $target game move k b7-b6 k2");	
		$server->command("msg $target game move k g1-h3 k1");	
		$server->command("msg $target game move k c8-a6 k2");	
		$server->command("msg $target game move k f1-g2 k1");	
		$server->command("msg $target game move k e7-e5 k2");	
		$server->command("msg $target game move k d2-d4 k1");	
		$server->command("msg $target game move k g7-g5 k2");	
		$server->command("msg $target game move k c1-g5 k1");	
		$server->command("msg $target game move k d8-f6 k2");	
		$server->command("msg $target game move k f2-f4 k1");	
		$server->command("msg $target game move k b8-c6 k2");	
		$server->command("msg $target game move k b2-b4 k1");
		$server->command("msg $target game move k e5-e4 k2");
		$server->command("msg $target game move k d4-d5 k1");
		$server->command("msg $target game move k e4-e3 k2");
		$server->command("msg $target game move k d5-d6 k1");
		$server->command("msg $target game move k h7-h5 k2");
		$server->command("msg $target game move k d6-c7 k1");
		$server->command("msg $target game move k h5-h4 k2");
		#$server->command("msg $target game move k h5-h4 k2");
	}
}

Irssi::signal_add('message public', 'sig_setup1');
