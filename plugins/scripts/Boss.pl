#!/usr/bin/perl
###############
# Boss.pl - Auto-op script for Xchat with basic ban/say/kick/unban features
# Controlled from "private" channel, effects "public" channel
# By Flash_ - 27/11/06
###############
# Features: 
# !csay Blah
# !ckick Nick
# !cban Nick
# !emote Nick
# !autovoice (Toggle)
# !topic Blah-blah
#
###############
use strict;
###############
# Config Values
###############
#
my $public_chan = '#privchan'; # Public channel (Where bot acts)
my $private_chan = '#pubchan'; # Private chan (Where bot is controlled from)
my $private_chankey = 'notsaying'; # Key for private chan, if needed.
#
my $auto_voice = 1; # Auto voice everyone who joins.
my $auto_greeting = 0; # How to greet new joines. 0 = None. 1 = PM. 2 = Public message. (Spammy)
my $auto_greet_msg = "Welcome to my special channel, %n"; # Message to greet new joiner with.
my $auto_rejoin = 1; # Automatically rejoin on kick, AND force a rejoin every 15 minutes in case of dropouts.
#
my $csay = '!csay'; # What triggers words are
my $ckick = '!ckick';
my $cban = '!cban';
my $cemote = '!emote';
my $ctopic = '!topic';
my $cmodes = '!mode';
my $cunban = '!unban';
my $chelp = '!chelp';
my $autogreet_toggle = '!autogreet'; 
my $autovoice_toggle = '!autovoice';
# End config.
###############
# Don't edit stuff below here
#
my $boss_version = "001";
Xchat::register( "Flashy's Boss Script", "$boss_version", "Boss", "" );
Xchat::hook_print('Channel Message', "boss_watch");
Xchat::print("Loaded Flashy's Boss Script v.$boss_version");
Xchat::hook_print( "Join", "chat_joins");     # Hook Joins

if ($auto_rejoin == 1) {
	Xchat::command("set -quiet irc_auto_rejoin ON"); # Tell xcaht to rejoin on kick
	Xchat::hook_timer(900000,"rejoin_channels");  # Set timer to rejoin every 15 minutes	
	rejoin_channels();
	}

sub rejoin_channels { # Join channels.
	Xchat::command("join $public_chan");
	Xchat::command("join $private_chan $private_chankey"); 
	}

sub chat_joins {
	my $joiner=$_[0][0];
	my $channel = $_[0][1];	
	if ($channel eq $public_chan) {
		if ($auto_voice == 1) { # Let's voice them
			Xchat::command("mode +v $joiner");
			}
		my $temp_greet_msg = $auto_greet_msg; # Copy so we don't overwrite it globally
		$temp_greet_msg =~ s/%n/$joiner/g;
		if ($auto_greeting == 1) { Xchat::command("msg $joiner $temp_greet_msg"); } # PM message to them
		if ($auto_greeting == 2) { Xchat::command("say $temp_greet_msg"); } # PM message to them			
		} # End public chan join funcs
}

sub boss_watch { # Watch for triggers
	my $channel = lc(Xchat::get_info( 'channel' ));
	##################################
	# Check admin channel for commands
	if ($channel eq $private_chan) { # We're in the private control channel
		my $r_nick = $_[0][0];             
		my $r_line = $_[0][1];				
		my @rowr = split(/ /,$r_line);
		my $r_first=$rowr[0];
		my $r_second;

		if (lc($r_first) eq $csay) { # !say stuff
			my $msg = join ' ', @rowr[1 ..  @rowr];
			Xchat::command("msg $public_chan $msg");		
			return Xchat::EAT_NONE;
			} # End !say

		if (lc($r_first) eq $ckick) { # !Kick
			my $msg = join ' ', @rowr[2 ..  @rowr];
			my $kickee = $rowr[1];
			my $context = Xchat::find_context($public_chan);
			Xchat::set_context($context); # Set context to public chan
			my @users = map { $_->{nick} } Xchat::get_list('users');

			if (grep { $kickee eq $_ } @users) { # Found him, let's kick him
				my $msg = join ' ', @rowr[2 ..  @rowr];
				chop($msg);
				if ($msg eq undef) { $msg = "Courtesy of MPUK"; }
				Xchat::command("kick $kickee $msg");
				Xchat::command("msg $private_chan $kickee has been kicked from $public_chan ($msg)");
				} else { # No user of that nick
				Xchat::command("msg $private_chan Sorry, can't find any user of nick '$kickee' in $public_chan");
				}			
			return Xchat::EAT_NONE;
			} # end !kick

		if (lc($r_first) eq $cban) { # Ban (And kick)
			my $msg = $rowr[1];
			if ($msg eq undef) { Xchat::command("say Usage: $cban Nick/Hostmask"); return Xchat::EAT_NONE; } # Default ban message
			my $context = Xchat::find_context($public_chan);
			Xchat::set_context($context); # Set context to public chan
			my @users = map { $_->{nick} } Xchat::get_list('users');
			
			if ($msg =~ /\@/) { # It#s a hostmask - just apply the ban, no need to be clever
				Xchat::command("mode +b $msg");
				Xchat::command("msg $private_chan Hostmask ban of ' $msg ' applied");
				} else { # Nick. Instead of me trying to guess a hostmask, rely on Xchat's internal plotting.
				Xchat::print("($msg) - [@users]");
					if (grep { lc($msg) eq lc($_) } @users) { # Found him, let's kick him
						Xchat::command("ban $msg");
						Xchat::command("msg $private_chan Banned user ' $msg '");			
						Xchat::command("kick $msg"); # Clear him out.
						} else { # Can't find user.
						Xchat::command("msg $private_chan Sorry, can't find any user of nick '$msg' in $public_chan");
						}
				}
			}

# Unban
		if (lc($r_first) eq $cunban) { # !unban
			my $msg = $rowr[1];
			if ($msg eq undef) { Xchat::command("say Usage: $cunban Hostmask"); return Xchat::EAT_NONE; } # Default ban message
			my $context = Xchat::find_context($public_chan);
			Xchat::set_context($context); # Set context to public chan
			Xchat::command("mode -b $msg");
			Xchat::command("msg $private_chan $msg ban removed");
			return Xchat::EAT_NONE;
			}

		if (lc($r_first) eq $autovoice_toggle) { # Autovoice toggle
			if ($auto_voice == 0) { Xchat::command("say Autovoice now ON"); $auto_voice=1; }
				else { Xchat::command("say Autovoice now OFF"); $auto_voice=0; }
			return Xchat::EAT_NONE;
			}

		if (lc($r_first) eq $autogreet_toggle) { # Autogreet toggle
			if ($auto_greeting == 0) { Xchat::command("say Autogreet now PRIVATE"); $auto_greeting=1; }
				elsif ($auto_greeting == 1) { Xchat::command("say Autogreet now PUBLIC"); $auto_greeting=2; }
				else { Xchat::command("say Autogreet now OFF"); $auto_greeting=0; }
			return Xchat::EAT_NONE;
			}

		if (lc($r_first) eq $ctopic) { # Topic
			my $topic = join ' ', @rowr[1 ..  @rowr];
			chop($topic);
			if ($topic eq undef) {
				Xchat::command("say Usage: $ctopic blah blah blah");
				} else {
				my $context = Xchat::find_context($public_chan);
				Xchat::set_context($context); # Set context to public chan
				Xchat::command("topic $topic");
				}
			return Xchat::EAT_NONE;
			}

		if (lc($r_first) eq $cmodes) { # Topic
			my $modes = $rowr[1];
			if ($modes eq undef) {
				Xchat::command("say Usage: $cmodes MODES");
				} else {
				my $context = Xchat::find_context($public_chan);
				Xchat::set_context($context); # Set context to public chan
				Xchat::command("mode $modes");
				}	
			return Xchat::EAT_NONE;
			}			

		### More fun stuff
		if (lc($r_first) eq $cemote) { # Emote			
			my $msg = join ' ', @rowr[1 ..  @rowr];
			my $context = Xchat::find_context($public_chan);
			Xchat::set_context($context); # Set context to public chan
			Xchat::command("me $msg");
			return Xchat::EAT_NONE;
			}

		if (lc($r_first) eq $chelp) { # help
			Xchat::command("\00311say Boss script V.$boss_version by Flash_");
			Xchat::command("say Commands issued in here are acted out in $public_chan");
			Xchat::command("say $ckick {Nick} (Reason) = Kick {Nick} with optional (Reason)");
			Xchat::command("say $cban {Nick/Hostmask} = Ban (and if nick, kick) Nick or Hostname");
			Xchat::command("say $cemote {Action} = Emote {Action}");
			Xchat::command("say $ctopic {Topic} = Set {Topic}");
			Xchat::command("say $cmodes {Modes} = Apply Channel Modes (Careful)");
			Xchat::command("say $csay {Text} = Say {Text}");
			Xchat::command("say $chelp = This help");
			Xchat::command("say =====================");
			Xchat::command("say $autogreet_toggle = Toggle AutoGreet (Off / PM / Public)");
			Xchat::command("say $autovoice_toggle = Toggle AutoVoice (Off / On)");
			return Xchat::EAT_NONE;
			}
		} # End priv channel watch.
	}
