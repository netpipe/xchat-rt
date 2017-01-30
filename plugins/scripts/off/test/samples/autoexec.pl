#!/usr/bin/perl -w

IRC::register("autoexec.pl","1.1","","");

  my $BB = "My-Bot";
  my $X = "X";

sub oninvite
{ 
  my $inv = shift(@_);
  $inv =~ /:(.+)!(.+) INVITE (.+) :(.+)/;
  my ($invitee,$channel) = ($1,$4);
  if ($invitee eq $X || $invitee eq $BB) {
      IRC::command("/join $channel");
      return 0;
  }
  return 0;
}

IRC::add_message_handler("INVITE", "oninvite");

sub onjoin
{
	my $oj = shift(@_);
	$oj =~ /:(.+)!(.+) JOIN :(.+)/;
	my ($nnick,$cchan) = ($1, $3); 
	#autoop example
	if ($nnick eq $BB || $nnick eq $X) {
	  IRC::command("/mode $cchan +o $nnick")
	}
	#autokick example
	if ($cchan eq "#lamers") {
	  IRC::command("/kick $cchan $nnick STFU!")
	}
	#autogreet example
	if ($cchan eq "#greeters" || $cchan eq "#spammers") {
	  IRC::send_raw("cprivmsg $nnick $cchan :Hello!!! Welcome to $cchan!!!");
	}
}

IRC::add_message_handler("JOIN", "onjoin");

