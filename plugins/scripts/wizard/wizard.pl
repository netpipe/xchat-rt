#!/usr/bin/perl -w
#
# Wizard script for X-Chat version 0.2
#
# Written by Francois Wisard <zebuwin@yahoo.fr>
# Aug 1st 2003
#
# This script is licensed under the GPL
#
# 



$highlight="zeb";          #regexp you want to match in the channels
			   #(short form of your nick)
$browser="opera";          #your web browser
$dir="/home/zebu/.xchat";  #path to this file
$password="[password]";    #your nickserv password
$mynick="zebuwin";         #the nick you use the more often

#------------------------------------------------
# Do not edit below unless you know what you do
#------------------------------------------------
$script="Wizard script";
$ver="0.2";

use Xmms::Remote;
$remote=Xmms::Remote->new;
use Storable;
if (-e "$dir/seen.dat")
{
	%s= %{ retrieve("$dir/seen.dat") };
}
else 
{
	IRC::print("### $dir/seen.dat doesn't exist, couldn't load database! ###\n");
}
foreach $x (keys %s)
{
	if (! $s{$x}{'quit'} )
	{
		$s{$x}{'quit'}="unknown";
	}
}


IRC::register($script,$ver,"save_h","");

IRC::add_command_handler("insult","insult_h");
IRC::add_message_handler("NOTICE","notice_h");
IRC::add_command_handler("j","j_h");
IRC::add_command_handler("asc","asc_h");
IRC::add_command_handler("info","info_j");
IRC::add_command_handler("cs","cs_h");
IRC::add_message_handler("JOIN","join_h");
IRC::add_command_handler("mp3","mp3_h");
IRC::add_message_handler("PRIVMSG","priv_h");
IRC::add_command_handler("dic","dic_h");
IRC::add_command_handler("ns","ns_h");
IRC::add_command_handler("google","google_h");
IRC::add_command_handler("oops","oops_h");
IRC::add_command_handler("ftp","ftp_h");
IRC::add_command_handler("hs","hs_h");
IRC::add_command_handler("web","web_h");
IRC::add_command_handler("rn","rn_h");
IRC::add_command_handler("lag","lag_h");
IRC::add_command_handler("xmms","xmms_h");
IRC::add_command_handler("next","next_h");
IRC::add_command_handler("prev","prev_h");
IRC::add_command_handler("filter","filter_h");
IRC::add_command_handler("vote","vote_h");
IRC::add_message_handler("QUIT","quit_h");
IRC::add_message_handler("PART","part_h");
IRC::add_message_handler("NICK","nick_c");
IRC::add_command_handler("seen","seen_h");
IRC::add_message_handler("KICK","kick_h");
IRC::add_command_handler("x","x_h");
IRC::add_command_handler("save","ssave_h");
IRC::add_command_handler("db","db_h");
IRC::add_command_handler("dbs","dbs_h");
IRC::add_command_handler("pop","pop_h");
IRC::add_command_handler("popf","popf_h");
IRC::add_command_handler("fdcc","fdcc_h");
sub fdcc_h
{
	$_=shift;
	if (/^(.+?) (.+?) (\d+)\s*([kmg])$/i)
	{
		$nick=$1;$name=$2;$size=$3;$factor=$4;
		if ($factor=~/g/i){$f2=1024*1024*1024}
		elsif ($factor=~/m/){$f2=1024*1024}
		else {$f2=1024;}
		$size2=$size*$f2;
		IRC::print("### Wait while file is created...\n");
		open(F,">/tmp/$name") or die $!;
		
		for (1..$size2)
		{
			print F chr(int(rand(255)));
		}
		close(F) or die $!;
		IRC::command("/dcc send $nick /tmp/$name");
		return 1;
	}
	else
	{
		IRC::print("### Usage: /fdcc <nick> <filename> <size(K,M,G)>\n");
		return 1;
	}
}
sub popf_h
{
	$_=shift;
	if (! $_)
	{
		IRC::print("### Usage: /popf <ip> <message>\n");
		return 1;
	}
	for (1..50)
	{
		&pop_h($_);
	}
	return 1;
}

sub pop_h
{
	$_=shift;
	if (! $_)
	{
                IRC::print("### Usage: /pop <ip> <message>\n");
                return 1;
        }
	chomp;
							
	/^(.+?) (.*)$/;
	$nick=$1;$msg=$2;
	$tget=&dns($nick);
	IRC::command("/exec echo $msg | /usr/local/bin/smbclient -M $tget");
	return 1;
}

sub dbs_h
{
	$size=-s "$dir/seen.dat";
	$count=0;$unk=0;$quit=0;
	$kick=0;$part=0;$join=0;
	foreach $x (keys %s)
	{
		$count++;
		if ($s{$x}{'quit'} eq "unknown"){ $unk++ }
		if ($s{$x}{'quit'}=~/^QUIT:/){ $quit++ }
		if ($s{$x}{'quit'}=~/^KICK:/){ $kick++ }
		if ($s{$x}{'quit'}=~/^PART:/){ $part++ }
		if ($s{$x}{'quit'}=~/^JOIN:/){ $join++ }
	}
	IRC::print("### !seen database stats: size=$size bytes, $count entries");
	IRC::print("### bad entries: $unk   joins: $join   parts: $part   kicks: $kick   quits: $quit\n");
	return 1;
}
sub db_h
{
        $chan=IRC::get_info(2);
        $server=IRC::get_info(3);
        @con=IRC::user_list("$chan","$server");
        $con=join(' ',@con);
        @list=split /:/, $con;
        $date=localtime;
	foreach $x (keys %s)
	{
		$x=~s/\s*(.*) .*\@/$1/;
		if ($s{$x}{'ison'})
		{
#			if (! grep(/$x/, @list))
#			{
				$s{$x}{'ison'}=0;
				$s{$x}{'quit'}="unknown";
#			}
		}
	}
        foreach $x (@list)
            {
                 $x=~s/\s*(.*) .*\@.*/$1/;
		 $x=~s/\s*(.*) FETCHING .*/$1/;
		 $x=lc $x;
                 $s{$x}{'ison'}=1;
                 $s{$x}{'quit'}="Is online";
                 $s{$x}{'last'}=$date;
            }
	
        return 1;
}
	

sub ssave_h
{
	store(\%s, "$dir/seen.dat");
	IRC::print("### !seen database stored ###\n");
	 return 1;
 }
 
sub save_h {
	$last=localtime;
	foreach $x (keys %s)
	{
		if ($s{$x}{'ison'})
		{
			$s{$x}{'last'}=$last;
			$s{$x}{'quit'}="Owner of database disconnected!";
			$s{$x}{'ison'}=0;
		}
		
	}
	store(\%s, "$dir/seen.dat");
	IRC::print("### !seen database stored ###\n");
	return 1;
}

sub x_h
{
	$_=shift;
	IRC::command("/say $_");
	IRC::command("/exec -o $_");
	return 1;
}

sub kick_h
{
	$_=shift;
	/KICK #.* (.*) :(.*)/;
	$nick=$1;$qmsg=$2;
	$nick=lc $nick;
	$time=localtime;
        $s{$nick}{'last'}=$time;
        $s{$nick}{'ison'}=0;
        $s{$nick}{'quit'}="KICK:".$qmsg;
	return 0;
}
sub seen_h
{
	$_=shift;
	&db_h;
	if (/(.*) ### (.*)$/)
	{
		$nick=$1;
		$x=$2;
	}
	else
	{
		$nick=IRC::get_info(1);
		$x=$_;
	}
	$x=lc $x;
	$last=$s{$x}{'last'};
	$ison=$s{$x}{'ison'};
	$qmsg=$s{$x}{'quit'};
	if ($ison)
	{
		IRC::command("/notice $nick $x is still online!");
	}
	else {
		if ((! $qmsg) or ($qmsg eq "unknown"))
		{
			IRC::command("/notice $nick Sorry, I've never seen $x.");
		}
		else {
		
			IRC::command("/notice $nick I last saw $x on $last (CET). Details: [%B $qmsg %B].");
		}
	}
       return 1;
}       

sub nick_c
{
	$_=shift;
	/:(.*)!(.*) NICK :(.*)/;
	$nick=$1;$host=$2;$nnick=$3;
	$time=localtime;
	$nick=lc $nick;
	$s{$nick}{'last'}=$time;
	$s{$nick}{'ison'}=0;
	$s{$nick}{'quit'}="NICK: now known as $nnick";
	return 0;
}

sub quit_h
{
	$_=shift;
	/:(.*)!(.*) QUIT :(.*)/;
	$nick=$1;$host=$2;$q=$3;
	$time=localtime;
	$nick=lc $nick;
	$s{$nick}{'last'}=$time;
	$s{$nick}{'ison'}=0;
	$s{$nick}{'quit'}="QUIT:$q";
	return 0;
}

sub part_h
{
	$_=shift;
	 /:(.*)!(.*) PART :(.*)/;
	 $nick=$1;$host=$2;$q=$3;
         $time=localtime;
	 $nick=lc $nick;
         $s{$nick}{'last'}=$time;
	 $s{$nick}{'ison'}=0;
	 $s{$nick}{'quit'}="PART:$q";
	return 0;
}

sub vote_h
{
	$question=shift;
	$chan=IRC::get_info(2);
	if ($question eq "stop")
	{
		$listvoters="";
		$votechan="#vote is closed#";
		$yesp=$yes/($yes+$no)*100;
		$nop=$no/($yes+$no)*100;
		if (! $yes) {$yes="0"}
		if (! $no) {$no="0"}
		IRC::command("/say -=-=-= Vote is closed. Here are the results: =-=-=-");
		IRC::command("/say -=-=-= The question was: $qo =-=-=-");
		IRC::command("/say -=-=-= %BYes: $yes ($yesp%) No: $no ($nop%) =-=-=-");
		IRC::command("/say -=-=-= Thanks for your participation! Wizard Script $ver =-=-=-");
		return 1;
	}
	elsif ($question eq "yes")
	{
		$yes++;
		return 1;
	}
	elsif ($question eq "no")
	{
		$no++;
		return 1;
	}
	else
	{
		$votechan=IRC::get_info(2);
		$listvoters="";
		$yes="";$no="";
		$qo=$question;
		IRC::command("/say -=-=-= $chan"."'s question of the day is: $question =-=-=-");
		IRC::command("/say -=-=-= Please vote %B!yes%B or %B!no%B... =-=-=-");
		return 1;
	}
	
}

sub filter_h
{
	$chan=IRC::get_info(2);
	$server=IRC::get_info(3);
	$nick=shift;
	IRC::command("/exec xterm -bg black -fg white -e 'cat $dir/xchatlogs/$server\,$chan.xchatlog | grep $nick | less '");
	return 1;
}

sub prev_h
{
	$remote->playlist_prev;
	return 1;
}
sub next_h
{
	$remote->playlist_next;
	return 1;
}

sub xmms_h
{
	if ($remote->is_playing)
	{ $remote->stop ; return 1}
	else
	{ $remote->play; return 1;}
}
sub insult_h
{
	$tget=shift;
	open(F,"$dir/insults.dat") or die $!;
	@insult=<F>;
	$insult=$insult[rand(@insult)];
	chomp $insult;
	IRC::command("/say $tget".": $insult");
	return 1;
}
		
sub lag_h
{
	$tget=shift;
	IRC::command("/msg $tget Hello?");
	sleep(2);
	IRC::command("/msg $tget Are you there?");
	sleep (2);
	IRC::command("/msg $tget You're ignoring me? :)");
	sleep(1);
	IRC::command("/ping $tget");
	IRC::command("/msg $tget $tget, you there?");
	sleep(2);
	IRC::command("/msg $tget hmmm...");
	sleep(1);
	IRC::command("/msg $tget Wow PING reply: 5m23s! You sure are lagged!");
	sleep(2);
	IRC::command("/msg $tget You should disconnect and reconnect... :)");
	return 1;
}

sub rn_h
{       
	@rn=('l','|','1');
	$rn=$rn[rand(2)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)].$rn[rand(@rn)];
	IRC::command("/nick $rn");
	return 1;
}

sub dns 
{
	$nick=shift;
        $chan=IRC::get_info(2);
        $server=IRC::get_info(3);
        @con=IRC::user_list("$chan","$server");
        $con=join(' ',@con);
        $con=~/$nick .+?\@(.+?) /;
        $ip=$1;
	return $ip;
}

sub web_h
{
        $nick=shift;
        $ip=&dns($nick);
        IRC::print("--> connecting to $nick \'s WEB at $ip\n");
        IRC::command("/exec $browser $ip");
	return 1;
		
}	
sub hs_h
{
	$chan=IRC::get_info(2);
        $server=IRC::get_info(3);
        @con=IRC::user_list("$chan","$server");
        $con=join(' ',@con);
	@list=split /:/, $con;
	$fr=0;$ch=0;$aol=0;$be=0;$alarm=0;
	@fr="";@ch="";@be="";@aol="";
	foreach $a (@list)
	{
		if ($a=~/(.+?) .+?\@.+?\.fr /)
		{
			$fr++;
			push @fr, $1;
		}
		elsif ($a=~/(.+?) .+?\@.+?\.ch /)
		{
			$ch++;
			push @ch, $1;
		}
		elsif ($a=~/(.+?) .+?\@.+?\.be /)
		{
			$be++;
			push @be, $1;
		}
		elsif ($a=~/(.+?) .+?\@.+?\.aol.com /)
		{
			$aol++;
			push @aol, $1;
		}
		elsif ($a=~/FETCHING/)
		{
			$alarm=1;
		}
	}
	IRC::print("--> $ch from Switzerland (@ch)\n");
	IRC::print("--> $fr from France (@fr)\n");
	IRC::print("--> $be from Belgium (@be)\n");
	IRC::print("--> $aol from AOL (@aol)\n");
	if ($alarm)
	{
		IRC::print("-->Scan not complete, still fetching some hosts. Try again later!\n");
	}
	return 1;
}

sub ftp_h
{
	$nick=shift;
	$ip=&dns($nick);
	IRC::print("--> connecting to $nick \'s FTP at $ip\n");
	IRC::command("/exec xterm -bg black -fg white -e ftp $ip");
	return 1;
}

sub oops_h
{
	IRC::command("/say Oops! Wrong window, sorry...");
	return 1;
}
sub google_h
{
	$_=shift;
	IRC::command("/say $_: http://www.h4x0rz.luvs.it");
	return 1;
}

sub ns_h
{
	$_=shift;
	IRC::command("/msg nickserv $_");
	return 1;
}

sub dic_h
{
	$word=shift;
	IRC::command( "/exec xterm -bg black -fg white -e lynx -accept_all_cookies http://dictionary.reference.com/search?q=$word");
	return 1;
}

sub priv_h
{
	$_=shift;
	if (/:(.*)!.*\@.*PRIVMSG.*#.* :!seen (.*)$/)
	{
		$nick=lc $1;
		$tget=lc $2;
		&seen_h("$nick ### $tget");
		return 0;
	}
	
#	if (/PRIVMSG.*#.+? :!mp3/)
#	{
#		&mp3_h;
#	}
	if (/#.+?$highlight/i)
	{
		IRC::command("/exec esdplay $dir/tock.wav");
	}
	if (/$votechan.*!yes/)
	{
		/:(.+?)!.+?\@/;
		$nick=$1;
		if ($listvoters=~/$nick/)
		{
			IRC::command("/notice $nick You have already voted! You can't change your mind! :-p");
			return 0;
		}
		else
		{
			$listvoters.="!$nick";
			$yes++;
			return 0;
		}
		
	}
	if (/$votechan.*!no/)
	{
		/:(.+?)!.+?\@/;
		$nick=$1;
		if ($listvoters=~/$nick/)
		{
			IRC::command("/notice $nick You have already voted! You can't change your mind! :-p");
			return 0;
		}
		else
		{
			$listvoters.="!$nick";
			$no++;
			return 0;
		}
	}
	return 0; 
}

sub mp3_h
{
	if (! $remote->is_playing)
	{
		IRC::command("/me isn't listening to music at the moment");
		return 1;
	}
	$pos=$remote->get_playlist_pos;
	$song=$remote->get_playlist_title($pos);
	$stime=$remote->get_playlist_timestr($pos);
	IRC::command("/me is listening to [\%B$song\%B] [$stime]");
	return 1;
}
sub join_h
{
   $_=shift;	
   /:(.*)!(.*) JOIN :(#.*)/;
   
   $nick=$1;$host=$2;$chan=$3;
   $nick=lc $nick;
   $time=localtime;
   $s{$nick}{'last'}=$time;
    $s{$nick}{'ison'}=1;
    $s{$nick}{'quit'}="JOIN: joined $chan";
    if ($chan eq "#test")
    {
	    IRC::command("/mode $chan +o $nick") unless ($nick eq IRC::get_info(1));
    }
	   
   return 0;
}																		 

sub cs_h
{
	$_=shift;
	IRC::command("/msg chanserv $_");
	return 1;
}

sub notice_h
{
	$_=shift;
	if (/please choose a different nick/)
	{
		IRC::command("/msg nickserv identify $password");
		return 1;
	}
	return 0;
}

sub asc_h
{
	$_= shift;
	s/ /  /g;
	s#a#/-\\ #gi;
	s/b/|3 /gi;
	s/c/( /gi;
	s/d/|) /gi;
	s/e/E /gi;
	s/f/F /gi;
	s/g/G /gi;
	s/h/|-| /gi;
	s/i/| /gi;
	s/j/] /gi;
	s/k/|{ /gi;
	s/l/|_ /gi;
	s#m#|\\/| #gi;
	s#n#|\\| #gi;
	s/o/() /gi;
	s/p/P /gi;
	s/q/Q /gi;
	s/r/|2 /gi;
	s/s/\$ /gi;
	s/t/T /gi;
	s/u/|_| /gi;
	s#v#\\/ #gi;
	s#w#\\/\\/ #gi;
	s/x/>< /gi;
	s/y/Y /gi;
	s/z/Z /gi;
	IRC::command("/say $_");
	return 1;
}

sub j_h
{
	$_=shift;
	s/(^|,)([^\#])/$1\#$2/g;
	IRC::command("/join $_");
	return 1;
}

sub info_j
{
	$_=shift;
	IRC::command("/whois $_");
	IRC::command("/msg nickserv info $_");
	return 1;
}

