#!/usr/bin/perl -w
# cryptolite script by sick_boy@irc.euirc.net based on 
# crypt.pl xchat script written by rodney@irc.xchat.org
# irssi port by signal; unified script by sick_boy
# Licensed under GPL v2.0 and following - no warranty of any kind


my $VERSION = '1.00';
my %IRSSI = (
    authors     => 'signal',
    contact     => 'signal@wurstpower.org',
    name        => 'cryptolite-irssi.pl',
    description => 'irssi port of sicks cryptolite.pl,' ,
    license     => 'GPL',
);

use strict;

BEGIN{
    use vars '$irssi','$xchat';
    eval q{
	use Irssi;
	Irssi::version();
    };
    $irssi = !$@;
    eval q{
	IRC::get_info(1);
    };
    $xchat = !$@;
}


my $keycode = "wmndislajvaeraegazrtbxcfgh6743344312" ;
#              abcdefghijklmnopqrstuvwxyz0123456789

my $key_file ="$ENV{HOME}/.cryptolite.key" ;

if ( -e "$key_file" ) {
	open (KEYF, "<$key_file");
	$keycode = <KEYF>;
	close (KEYF);
}

sub new_key
{
  $keycode = shift ; 
  open (KEYF, ">$key_file");
  print KEYF "$keycode\n";
  close (KEYF);
  &get_key;
  return 1 ;
}

sub get_key {
  if ($irssi) {
	Irssi::print("key is : $keycode") ;
  } elsif ($xchat) {
	IRC::print("key is : $keycode") ;
  }
	return 1 ;
}

sub crypt
{
  my ($mess, $server, $witem);
  if ($irssi) {
	( $mess, $server, $witem) = @_;
	Irssi::active_win->command("echo \002Crypt\002 $server->{nick}> $mess") ;
  } elsif ($xchat) {
	$mess = shift ;
	IRC::print("\00310$mess\n") ;
  }
  $mess =~ y/A-Z/a-z/;
  eval "\$mess =~ y/abcdefghijklmnopqrstuvwxyz0123456789/$keycode/;";
  my $anzahl = length($mess);
  my $halb = int($anzahl/2) ;
  my $mes1 = substr( "$mess",0,$halb) ;
  my $mes2 = substr("$mess",$halb,$anzahl-$halb) ;
  $mess = $mes1 . "µ" . $mes2 ;
  if ($irssi)		{ Irssi::active_win->command("say $mess"); }
  elsif ($xchat)	{ IRC::command("$mess") ; }
  return 1 ;
}

sub event_privmsg
{
	my ($server, $data, $nick, $address);
	my ($target, $text);
	my $actLine;

	if ($irssi) {
		($server, $data, $nick, $address) = @_;
		($target, $text) = split(/ :/, $data, 2);
	} elsif ($xchat) {
		$actLine = shift(@_);
		#parse act. msg
		$actLine =~ m/\:(.*?)\!(.*?)\sPRIVMSG\s(.*?)\s\:(.*)?/;
		$text = $4;
	}
	if ($text=~/µ/) {
		$text=~ s/µ//g;
		$text=~ y/A-Z/a-z/;
		eval "\$text =~ y/$keycode/abcdefghijklmnopqrstuvwxyz0123456789/;";
		if ($irssi)	{ $server->command("echo -window $target \002Crypt\002 $nick> $text"); }
		elsif ($xchat)	{ IRC::print("\00311$text\n") ; }
	}
  return 0 ;

}

sub help
{
  if ($irssi) {
  Irssi::print("CRYPTOLITE Script:\n") ;
  Irssi::print("/cryptohelp brings up this menu\n") ;
  Irssi::print("/getkey ---will display the current key\n") ;
  Irssi::print("/setkey newkey  --will change the key (must contain all letters a-z no caps and 0-9 for it to werk ;), change it if no good)\n") ;
  Irssi::print("/cr text to send  --sends this after it encrypts it of course\n") ;
  } elsif ($xchat) {
  IRC::print("CRYPTOLITE Script:\n") ;
  IRC::print("/cryptohelp brings up this menu\n") ;
  IRC::print("/getkey ---will display the current key\n") ;
  IRC::print("/setkey newkey  --will change the key (must contain all letters a-z no caps and 0-9 for it to werk ;), change it if no good)\n") ;
  IRC::print("/cr text to send  --sends this after it encrypts it of course\n") ;
  }
  return 1 ;
}

if ($irssi) {
  Irssi::signal_add("event privmsg", "event_privmsg");
  Irssi::command_bind('cryptohelp','help');
  Irssi::command_bind('cr','crypt');
  Irssi::command_bind('getkey','get_key');
  Irssi::command_bind('setkey','set_key');
  Irssi::print("CRYPTOLITE script by sick_boy") ;
  Irssi::print("please do a /cryptohelp for the options") ;
} elsif ($xchat) {
  IRC::register("cryptolite script","1.0","","") ;
  IRC::print("CRYPTOLITE script by sick_boy\n");
  IRC::print("please do a /crypthelp for the options\n") ;
  IRC::add_message_handler("PRIVMSG","event_privmsg") ;
  IRC::add_command_handler("cryptohelp","help") ;
  IRC::add_command_handler("cr","crypt") ;
  IRC::add_command_handler("setkey","set_key") ;
  IRC::add_command_handler("getkey","get_key") ;
}
