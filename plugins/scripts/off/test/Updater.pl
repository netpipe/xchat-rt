#!/usr/bin/perl
#
# Updater
# To update scripts and perform limited file commands remotely using passworded query. Written originally to update scripts on a Windows machine that I didn't have 
# GUI access to. This allowed me to change nicks, change Xchat settings, DCC scripts, move them, rename them, reloadall - pretty much everything you can do if you 
# were sitting at the GUI via Semi-Secure PM, with the exception of start the remote Xchat if it's not running.
#
# Obviously this script is installed on the remote PC you wish to control. You do not need to run it on the client at all.
#
# WARNING: DO change the password, but DO NOT use this script unless you have a requirement for it. (Potentially will give control of your PC to somebody else)
#
# Seperator = ^  (Spaces no good due to long paths)
# Commands (via query to the bot):
#	!info			password									# Get some info about host and check the script is alive.
#	!xcommand		password		[command]					# Issue direct command to xchat
#	!xexec			password		[command-line]				# Use /exec to execute a command and pipe output. Note cmd /c blah for system commands on windows
#	!reloadall		password									# Force /reloadall (Can also be done with xcommand)
# Usage:
# Windows, to get a directory listing (note quotes around long paths with spaces - it's an OS thing): !xexec^password^cmd /c dir "c:\Documents and Settings\My Documents"
# To set the remote client to accept DCC's without prompts (note lack of leading /): !xcommand^password^set dcc_auto_send 1
# To say something to a specific channel: !xcommand^password^msg #channel Message
# To change the remote's nick: !xcommand^password^nick NewNick
# To reload all the scripts in the autoload dir: !reloadall^password   (Can also do this via !xcommand)
##################################################################
use strict;
my $ud_version = "0.2";
my $password = "password"; # <---- Change this!
my $cfgdir = Xchat::get_info( 'xchatdir' );
my $xchatver = Xchat::get_info('version');
Xchat::register( "Flashy's Updater Script", "$ud_version", "Updater", "" );
Xchat::print("Loaded Flashy's Updater Script v.$ud_version");
Xchat::hook_print('Private Message', "updater_pms");   
Xchat::hook_print('Private Message to Dialog', "updater_pms"); 

sub updater_pms { 
	my $nick = $_[0][0];
	my $line = $_[0][1];
	my @bum = split /\^/,$line;

	if ($bum[1] eq $password) {
			my $happy=0;			
			if (lc($bum[0]) eq '!info') {
				Xchat::command("say Welcome $nick. This is Flash's updater v.$ud_version.");
				Xchat::command("say Xchat version $xchatver, Config path: $cfgdir");
				$happy=1;
				}
			if (lc($bum[0]) eq '!reloadall') { 
				Xchat::command("say Forcing a /reloadall in 1 second");
				Xchat::command("timer 1 reloadall");
				$happy=1;
				}
			if (lc($bum[0]) eq '!xcommand') {
				my $cmd = $bum[2];
				if (not defined $cmd) {
					Xchat::command("say Error in submission. No command supplied");
					return Xchat::EAT_NONE;
					}
				Xchat::command("say Executing /$cmd in 1 second");
				Xchat::command("timer 1 $cmd");
				$happy=1;
				}
			if (lc($bum[0]) eq '!xexec') { 
				my $cmd = $bum[2];
				if (not defined $cmd) {
					Xchat::command("say Error in submission. No command supplied");
					return Xchat::EAT_NONE;
					}
				Xchat::command("say Executing system $cmd in 1 second");
				Xchat::command("timer 1 exec -o $cmd");
				$happy=1;
			}
	if ($happy == 0) { Xchat::command("say Sorry, I don't understand the command ' $bum[0] '"); }
	} 
	return Xchat::EAT_NONE;
}