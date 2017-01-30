#----------------------------------------------------------------------
# $Id: irc-crypt.pl,v 1.6 2004/11/01 17:20:46 jk Exp $
#
# IDEA crypt for x-chat 2.0.8+
# 2004-10-31 Fook <jaska@kivela.net>
#
# Needs IRC::Crypt and the irc-crypt library.
#----------------------------------------------------------------------

use strict;
use IRC::Crypt;
use Unicode::String;

$::irc_crypt::name = 'irc-crypt';
($::irc_crypt::version) =  q{$Revision: 1.6 $} =~ m/Revision: ([\d\.]+) /;
$::irc_crypt::description = 'IDEA Crypt for x-chat 2.0.8+';
$::irc_crypt::keyfilename = Xchat::get_info( 'xchatdir' ) . '/irc-crypt-keys.conf';

#----------------------------------------------------------------------
# helper subs
#----------------------------------------------------------------------

# do_load_keys( filename )
#   - load encryption keys from file <filename>
#
sub do_load_keys( $ )
{
	my ($filename) = @_;
	$filename ||= $::irc_crypt::keyfilename;

	my $n = 0;
	unless(open F, '<'.$filename)
	{
		Xchat::print( "Cannot open '$filename': $!");
		return 0;
	}
	while(<F>)
	{
		chomp;
		next if /^\s*;/;
		next if /^\s*$/;
		my ($target, $key, $idea) = /^\s*([#\w]+)\s+"([^"]+)"\s+(\d+)/;
		IRC::Crypt::add_default_key($target, $key);
		$::irc_crypt::idea{$target} = $idea;
		++$n;
	}
	close F;
	return $n;
}

sub hook_say()
{
	$::irc_crypt::hook1 = Xchat::hook_command( 'say', 'say' );
	$::irc_crypt::hook2 = Xchat::hook_command( '', 'say' );
}

#----------------------------------------------------------------------
# server hooks
#----------------------------------------------------------------------
sub privmsg
{
	my ($a, $b) = @_;
	my ($line) = @$b;
	$line =~ m/\:(.*?)\!(.*?)\sPRIVMSG\s(.*?)\s\:(.*)?/;
	my $msgnick = $1;
	my $hmsender = $2; 
	my $msgchan = $3;
	my $msgline = $4;
	
	if(IRC::Crypt::is_encrypted_message_p($msgline))
	{
		my($msg, $nick, $tdiff) = IRC::Crypt::decrypt_message($msgline);
		if(defined($nick))
		{
			my $info = Xchat::user_info($nick);
			# %C2<%O$3%O$1%C2>%O$t$2%O
			Xchat::emit_print( "Channel Message", $nick, $msg, "¿".$info->{prefix});
			return Xchat::EAT_XCHAT;
		}
		else
		{
			Xchat::print("$msg");
			return Xchat::EAT_NONE;
		}
	}
	return Xchat::EAT_NONE;
}

#----------------------------------------------------------------------
# command hooks
#----------------------------------------------------------------------
sub load_keys
{
	my ($a, $b) = @_;
	my ($command, $arg1) = @$a;
	do_load_keys($arg1);
	return Xchat::EAT_NONE;
}

sub idea
{
	$::irc_crypt::idea{Xchat::get_info('channel')} = 1;
	return Xchat::EAT_ALL;
}

sub noidea
{
	$::irc_crypt::idea{Xchat::get_info('channel')} = 0;
	return Xchat::EAT_ALL;
}

sub key
{
	my ($a,$b) = @_;
	my ($command, $arg1) = @$a;
	my $chan = Xchat::get_info( 'channel' );
	if(IRC::Crypt::add_default_key( $chan, $arg1 ))
	{
		Xchat::print("Set key new for channel $chan\n");
	}
	return Xchat::EAT_ALL;
}

sub say
{
	my ($a, $b) = @_;
	my ($command) = @$a;
	my $text = "";
	if($command eq 'say')
	{
		$text = $b->[1];
	}
	else
	{
		$text = $b->[0];
	}
	
	my $chan = Xchat::get_info( 'channel' );

	if($::irc_crypt::idea{$chan})
	{
		my $nick = Xchat::get_info( 'nick' );
		my $u = Unicode::String::utf8($text);
		$text = $u->latin1;
		my $crypted = IRC::Crypt::encrypt_message_to_address($chan, $nick, $text);
		if($crypted)
		{
			IRC::send_raw("PRIVMSG $chan :$crypted\r\n");
			my $info = Xchat::user_info($nick);
			Xchat::emit_print( "Channel Message", $nick, $text, "¿".$info->{prefix});
			return Xchat::EAT_ALL;
		}
		else
		{
			Xchat::print "Cannot encrypt!";
			return Xchat::EAT_ALL;
		}
	}
	else
	{
		return Xchat::EAT_NONE;
	}
}

#----------------------------------------------------------------------
# initialization
#----------------------------------------------------------------------
Xchat::register( $::irc_crypt::name, $::irc_crypt::version,
		         $::irc_crypt::description );

Xchat::print( $::irc_crypt::name . " " . 
              $::irc_crypt::version . " loaded, with IRC::Crypt " .
			  IRC::Crypt->VERSION);

do_load_keys( $::irc_crypt::keyfilename );

Xchat::hook_command( 'loadkeys', 'load_keys',
                     { help_text => 'LOADKEYS [<file>] -- Load irc_crypt keys from file.' });
Xchat::hook_command( 'idea', 'idea',
					{ help_text => "IDEA -- Set encryption on for current channel."});
Xchat::hook_command( 'noidea', 'noidea',
					{ help_text => "NOIDEA -- Set encryption off for current channel."});

Xchat::hook_command( 'key', 'key',
                    { help_text => "KEY <key> -- Set encryption key for current channel." });

hook_say();
Xchat::hook_server( 'PRIVMSG', 'privmsg' );

#----------------------------------------------------------------------
# $Log: irc-crypt.pl,v $
# Revision 1.6  2004/11/01 17:20:46  jk
# Added 'KEY' command.
# Eat my commands, preventing them from going to the server.
#
# Revision 1.5  2004/10/31 22:09:49  jk
# Use emit_print to output the messages. This way we get
# working nick coloring and stuff.
#
# Revision 1.4  2004/10/31 20:14:52  jk
# Force latin1 charset.
#
# Revision 1.3  2004/10/31 19:24:00  jk
# Fix indentation.
#
# Revision 1.2  2004/10/31 18:53:59  jk
# First functional version.
#
# Revision 1.1  2004/10/31 08:41:21  jk
# Initial revision
#
