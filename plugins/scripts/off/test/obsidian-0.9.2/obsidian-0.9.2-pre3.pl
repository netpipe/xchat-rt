#!/usr/bin/perl -w
#############################################################################
#
#	!!!! READ THE IMPORTANT NOTE DOWN BELOW !!!!
#
#	Obsidian FServe 0.9.2 for X-Chat
#
#       This software is copyleft and distributed under the terms of the
#       GPL version 2. You should have recieved a copy of the GPL with this
#       program. This copy may be attached to this file so if You havn't
#       already read this important document jump to the end and read it.
#
#       Copyright (c) 2001-2003 Alexander Werth (alexander.werth@gmx.de)
#       Copyright (c) 2001 Martin Persson (mep@passagen.se) (inactive)
#
#       If You have any problems with this software please try to gather as
#       much information as possible before sending a typical "don't work,
#       help now." email. If You don't know the exact nature and cause of
#       the problem send as much info as possible. This should at least
#       contain the version of xchat, perl, obsidian fserve, operation system,
#       a copy or exception of the file .xchat/obsidian.conf and any
#       messages that appear when loading this script.
#
#############################################################################
#
#	Note: -  Since release 0.9.0 a new scheme for saving variables has
#               been introduced. So You are requiered to reenter all
#               variables hopefully for the last time.
#             - Big changes so we have a major version release.
#               Next major version will be 0.10.0 and not 1.0.0.
#             - Thanks for all the testing, hints and patches.
#             - Check the new website at:
#               http://homepages.tu-darmstadt.de/~awerth/obsidian/
#             - Thanks to Moritz Bunkus (mb) for a lot of usefull features.
#             - Thanks to Patrick Klein (pk) for the big multiserver patch.
#
#	CHANGELOG
#	=====================================================================
#
#	2004/08/20:		released 0.9.2-pre3
#                    -  Really recognise done sends with spaces.
#                    -  Should work better on MacOS now.
#
#	2004/01/30:		released 0.9.2-pre2
#                    -  Big Patch: Multiserver capability (pk).
#                    -  Recognize nick changes in xchat2.
#                    -  New way to parse handler parameters.
#                    -  Better separation of serving and non serving channels.
#                    -  /fs set <variable> [data] instead of ... <data>.
#
#	2003/07/26:		released 0.9.2-pre1
#                    -  Basic xchat 2 support
#
#      2003/07/26:          released 0.9.1
#                    -  Changed about message to include correct version
#
#	2003/01/27:		released 0.9.1-rc2
#                    -  Fixed bug in send_dead (Jeshua).
#                    -  Changed internals send_files.
#                    -  Fixed segfault in dcc_chat_failed_handler. Finally.
#                    -  Use Perl package for Obsidian.
#
#	2002/10/24:		released 0.9.1-rc1
#                    -  short_notice does what it's supposed to do (thansen).
#                    -  some changes on close_tabs (thansen).
#                    -  accept !list nickname and !list.
#                    -  the german sz in the logo gave some problems. disposed.
#                    -  special send_file sub
#                    -  rewrite of admin_send_queue
#
#	2002/08/27:		released 0.9.1-pre4
#                    -  Fixed bug. In admin operations.
#                    -  Fixed bug. /fs move working with failqueues.
#                    -  Fixed bug. Wrong message on admin aborted send.
#                    -  Change. Move accepts #n or n.
#                    -  Fixed Bug in sends stats.
#
#	2002/07/14:		released 0.9.1-pre3
#                    -  Fixed bug. Inconsistent send lists.
#
#	2002/06/06:		released 0.9.1-pre2
#                    -  Added a short_notice toggle (Jens Woch)
#                    -  Added support for /fs notify [channel].
#                    -  Warning for raw_welcome_msg (deprecated).
#                    -  Fixed bug. Channels are case insensitive now.
#
#	2002/04/16:		released 0.9.1-pre1
#                    -  find able to use wildcards (Adam Young & mb).
#                    -  Respect xchats ignore list a little (Adam Young).
#                    -  Shutdown fserve if the server connection is lost (mb).
#                    -  Rework timers based on system time (mb).
#                    -  Use xchat's conf path for obsidian files (mb).
#                    -  Respect user set dcc_timeout in xchat (mb).
#                    -  Added "Closing idle connection in 30 seconds" msg (mb).
#                    -  Added /fs who command to list (mb).
#                    -  New /fs show_files command (mb).
#                    -  New /fs sendf <nick> <file> command (mb).
#                    -  New /fs queuef <nick> <file> command (mb).
#                    -  New /fs smsg, /fs qmsg and /fs amsg command (mb).
#                    -  New /fs send_queue <nr> command (mb).
#                    -  Fixed bug with lower and uppercase nicknames (mb).
#                    -  Various spelling bugs (mb).
#                    -  User defineable obsidian.welcome file supported.
#                    -  find does respond with ctcp trigger.
#                    -  serve_hidden option for secret fserveing.
#                    -  Added fail queues.
#                    -  Workaround for faulty sends stats.
#                    -  priority_op     (priority for oped users)
#                    -  priority_voice  (priority for voiced users)
#
#	2002/04/11:		released 0.9.0
#                    -  Update files changed.
#
#	2002/03/20:		released 0.9.0-rc2
#                    -  Perl warning in move command fixed.
#                    -  Endless loop in update files fixed.
#                    -  File::stat workaround applied.
#                    -  Added help for /fs on startup.
#
#	2002/02/03:		released 0.9.0-rc1
#                    -  Bug in smart sends found. Small files didn't send.
#                    -  Removing of people not in channel was to sensitive.
#                    -  Changed welcome text.
#                    -  Fixed uninitialized values in /vars & /set.
#                    -  Fixed bug in about.
#                    -  Fixed bug in clr_slot_a.
#                    -  Added warning for experimental close_tab_auto.
#
#
#	2002/01/13:		released 0.9.0-pre3
#                    -  option to count sends as well.
#                    -  auto online after 5 minutes of inactivity.
#                    -  smart sends. Small files could be send immediatly.
#                    -  gpl and about command added.
#
#	2001/11/12:		released 0.9.0-pre2
#                    -  early opening of tabs.
#                    -  automatic closing of some tabs.
#                    -  sperarated toggles for notice and welcome message.
#                    -  log debug to file.
#                    -  some bugs with notices fixed.
#
#	2001/10/25:		released 0.9.0-pre1
#                    -  new author. Mail to alexander.werth@gmx.de.
#		     -  support for filesize sensitive slots per user.
#		     - 	files in queue can be dequeued and moved by operator.
#		     -	files in queue can be dequeued by user.
#                    -  case insensitive commands supported.
#                    -  case insensitive file and dir matching supported.
#                    -  You can notify channels per command /fs notify.
#                    -  You can have silent channels that are not notified.
#                    -  CTCP trigger support
#                    -  raw (hidden) notice and welcome message.
#                    -  split variables into variables and toggles
#                    -  dead sends are removed from sendlist
#                    -  introduced online help /fs help <keyword>
#                    -  Fixed a few minor bugs.
#                    -  released under GPL with permission of previous author.
#
#
#	2001/06/28:		released 0.8.0
#			show_notice now uses /notice when user types !list
#			and some other minor issues...
#
#			this is probably the last version of obsidian fserve
#                       for xchat, i have switched irc client to
#                       irssi (irssi.org)
#			so there is really no reason for me to continue
#                       writing a fserve for xchat
#
#	2001/06/22:     released 0.7.6
#			added basic @find support and filename caching
#			implemented support for nick changes
#			fixed send_next_file not to cause a stall of xchat
#			replaced save/load_queue with save/load_backup
#			and implemented auto_backup, auto_backup saves the
#			sends and the queue to file at regular intervals
#
#	2001/06/16:	released 0.7.5
#			included perl memleak patch, read important note below
#
#			WHOPS!!! just fixed a bug that caused send_next_file
#			to always send the last file in queue first, sorry
#			about that, i recently switched the order...
#
#			queue slot now starts from 1 not 0
#                       (users got confused...)
#
#			fixed display_who to show correct number of users,
#			corrected user cmd "stats" to actually display
#			stats and not the same as "who", fixed display_sends
#			to show the correct slotnumber, fixed max_queue bug
#			added $note to variables, fixed a few color bugs
#			add option to /msg instead of /notice in channels,
#			Min CPS (if not 0) is now shown in notify_channel,
#			added support for loading and saving queues
#			fixed bunch of other bugs i didn't write down
#
#	2001/06/12:	released 0.7.4
#			no longer uses strict mode
#			online time & send fail counters implemented
#			new server commands: stats sends queues
#			new user commands: stats dequeue
#			code cleanups
#
#	2001/06/11: 	first release (version 0.7.3) after three days of 
#		 	active developement
#
#	TODO		
#	=====================================================================
#		
#                    -  fix the totally screwed failqueue handling.
#                    -  get multiple instances of obsidian working.
#                    -  voiced/op only.
#                    -  resume filename.
#                    -  extensive find response.
#                    -  @nick command to get filelist file.
#                    -  create the above from the shared files.
#                    -  accept , as channel seperator.
#                    -  getall / get * / get regexp option
#                    -  implement multiple triggers ( use multiple xchat's    )
#                    -  multiple server support     ( with -d confdir for now.)
#                    -  fserve banlist nick based
#                    -  fserve banlist ip/identd based
#                    -  respect xchats ignore list (partly done)
#                    -  priority_nick   (priority for certain nicks)
#                    -  notify_to_voice (notify at join)
#                    -  better command handling and general source cleanups
#                    -  better color removal
#                    -  add "reconnect to fast" timer
#                    -  really get raw dcc msgs working.
#                    -  support kdcc (kib dcc)
#                    -  object based queues (v2)
#                    -  tree based config (v2)
#                    -  graphical frontend (v2)
#                    -  gui help system (v2)
#                    -  and probably much more...
#			
#############################################################################

#help me to keep this package together
{
package Obsidian;
#use strict;

#############################################################################
#
#	Public variables, the values are overwritten if config file exists
#	(these are the default values, feel free to change them)
#
#############################################################################

# toggles (0 or 1 only)
$var{'debug'}            = 0;
$var{'ctcptrigger'}      = 1;
$var{'restorequeues'}    = 1;
$var{'autoon'}           = 0;
$var{'notice_as_msg'}    = 0;
$var{'short_notice'}     = 0;
$var{'raw_chnl_msg'}     = 1;
$var{'raw_wlcm_msg'}     = 0;
$var{'open_tab_early'}   = 1;
$var{'close_tab_auto'}   = 0;
$var{'case_cmd'}         = 0;
$var{'case_file'}        = 0;
$var{'fs_sensitive'}     = 0;
$var{'priority_op'}      = 0;
$var{'priority_voice'}   = 0;
$var{'count_sends'}      = 1;
$var{'send_small_now'}   = 1;
$var{'ads_when_full'}    = 1;

#variables
$var{'max_users'}        = 10;
$var{'max_sends'}        = 2;
$var{'max_queues'}       = 20;
$var{'slots_big'} 	 = 1;
$var{'slots_med'} 	 = 4;
$var{'slots_small'}      = 10;
$var{'size_big'}         = 30000000;
$var{'size_med'}         = 1000000;
$var{'max_fails'} 	 = 5;
$var{'min_speed'} 	 = 0;	# 9.5 kB/s 9728
$var{'idle_time'} 	 = 180;
$var{'auto_backup'} 	 = 0;
$var{'trigger'} 	 = "!trigger";
$var{'serve_hidden'}     = "#testchnl3";
$var{'serve_no_notify'}  = "#testchnl2";
$var{'notify_to_voice'}  = "#testchnl2";
$var{'notify_channels'}  = "#testchnl";
$var{'netaliases'}	 = "";
$var{'notify_timeout'}   = 900;	# in seconds
$var{'root_dir'} 	 = "/home/alex/var/irc/scripts";
$var{'priority_nicks'}   = "nick";
$var{'note'}             = "write something =)";

#shortcut variables
$c1 = "\00313";	# highlight text color
$c2 = "\00314";	# regular text color
$c3 = "\00313";	# directory color
$c4 = "\00315";	# file color

#$logo = "\00314[\00313Oßsidian FServe\00314]\003";
$logo = "\00314[\00313Obsidian FServe\00314]\003";

# directory to store all Obsidian related files in
#$conf_dir = $ENV{'HOME'} . "/.xchat";
$conf_dir = IRC::get_info(4);

#############################################################################
#
#	private global variables
#
#############################################################################
$file_server = 0;		# 1 = enabled, 0 = disabled

# data variables
%user_online_time = ();		# contains the time the user connected
%user_status = ();		# contains connected/connecting users
				# value: -1 = connecting
				#        >0 = time of user's last message
%user_info = ();                # oped or voiced ?
%user_idle_warning = ();        # has the idle warning been sent?
%user_dir = ();			# contains current dir for user
%user_server = ();              # contains the server the user was on

@user_commands = ();
%file_cache = ();
%file_cache_uc = ();

@queue_norm = ();
@queue_fail = ();
@queue_send = ();

@welcome_msg =();

# statistical variables
$offline_time = 0;		# should not be saved
$autoon_time = 0;		# should not be saved
$online_time = 0;		# should not be saved
$record_cps = 0;
$record_cps_user = "";
$file_count = 0;
$send_fails = 0;
$byte_count = 0;
$access_count = 0;
$timer_count = 0;
$backup_timer = 0;

# other global variables and data
$notify_timer = 0;

#############################################################################
#
#	X-Chat message, print and command handlers declarations
#
#############################################################################

IRC::print("$logo Initializing...\n");
IRC::register("Obsidian", "0.9.2", "Obsidian::shutdown", "");

IRC::add_message_handler("PRIVMSG", "Obsidian::privmsg_handler");
IRC::add_message_handler("DCC", "Obsidian::dcc_msg_handler"); # xchat 1.8

#IRC::add_message_handler("DPRIVMSG", "Obsidian::privmsg_handler");
#IRC::add_message_handler("CTCP", "Obsidian::dcc_msg_handler");
#IRC::add_message_handler("DCC", "Obsidian::dcc_msg_handler");
#IRC::add_message_handler("DCC CHAT Text", "Obsidian::dcc_msg_handler");
#IRC::add_message_handler("DCC CHAT", "Obsidian::dcc_msg_handler");
#IRC::add_message_handler("CHAT", "Obsidian::dcc_msg_handler");

#xchat 2.0
IRC::add_print_handler("DCC CHAT Text", "Obsidian::dcc_msg_handler"); # xchat 2.0

IRC::add_print_handler("DCC Connected", "Obsidian::dcc_connect_handler"); # xchat 1.8
#IRC::add_print_handler("DCC CHAT Connected", "Obsidian::dcc_connect_handler");
#IRC::add_print_handler("DCC CHAT connection established", "Obsidian::dcc_connect_handler");
#IRC::add_print_handler("DCC CHAT chat", "Obsidian::dcc_connect_handler");
IRC::add_print_handler("DCC CHAT Connect", "Obsidian::dcc_chat_connect_handler"); # xchat 2.0
#IRC::add_print_handler("DCC CHAT Active", "Obsidian::dcc_connect_handler");

IRC::add_print_handler("DCC CHAT Failed", "Obsidian::dcc_chat_failed_handler");
IRC::add_print_handler("DCC CHAT Reoffer", "Obsidian::dcc_chat_reoffer_handler");

IRC::add_print_handler("DCC Conection Failed","Obsidian::dcc_timeout_handler");

IRC::add_print_handler("DCC Abort", "Obsidian::dcc_abort_handler");
IRC::add_print_handler("DCC SEND Abort", "Obsidian::dcc_abort_handler");

IRC::add_print_handler("DCC Stall", "Obsidian::dcc_timeout_handler");
IRC::add_print_handler("DCC SEND Stall", "Obsidian::dcc_timeout_handler");

IRC::add_print_handler("DCC Timeout", "Obsidian::dcc_timeout_handler");

IRC::add_print_handler("DCC Timeout", "Obsidian::dcc_timeout_handler");
IRC::add_print_handler("DCC SEND Timeout", "Obsidian::dcc_timeout_handler");
IRC::add_print_handler("DCC SEND timed out - aborting", "Obsidian::dcc_timeout_handler");

IRC::add_print_handler("DCC SEND Failed", "Obsidian::dcc_send_failed_handler");
IRC::add_print_handler("DCC SEND failed", "Obsidian::dcc_send_failed_handler");

IRC::add_print_handler("DCC SEND Complete", "Obsidian::dcc_send_complete_handler");
#IRC::add_print_handler("DCC SEND complete", "Obsidian::dcc_send_complete_handler"); # xchat 2.0

IRC::add_print_handler("Change Nick", "Obsidian::change_nick_handler");
IRC::add_print_handler("Disconnected", "Obsidian::disconnect_handler");

IRC::add_command_handler("fs", "Obsidian::server_command_handler");
IRC::add_timeout_handler(200, "Obsidian::obstimer");

# load config file if exists
if (!(-e ("$conf_dir/obsidian.conf"))) {
	IRC::print("$logo Config file not found...");
	IRC::print("$logo type \'/fs\' for information on how to setup the server");
	IRC::print("$logo then do a \'/fs save\' to save the config file");
	IRC::print("\n");
} else {
	load_settings();
	if ($var{'restorequeues'}) {
                loadq("obsidian.queue");
        }
}

# if debugging is on, open a separate tab for debug messages
if ($var{'debug'}) {
	IRC::command("/query obsidian_dbg");
}

IRC::print("$logo Type /fs to get help on the available commands.\n");

#############################################################################
#
#	Handlers called by X-Chat (must return appropriate values)
#
#############################################################################

#
#	obstimer(): handles timer events
#
sub obstimer
{
#       update the DCC sends
#       update_dcc_send_list();

	while (@user_commands >= 4) {
		parse_user_command();
	}

	# only do this every 4 secs
	if ($timer_count++ >= 20) {
                my $now = time();
		$timer_count = 0;

		# add 5 seconds to all users idletime
		foreach my $user (keys %user_status) {
                        my $user_idle = $now - $user_status{$user};
                        my $idle_remaining = $var{'idle_time'} - $user_idle;
                        my $server = $user_server{$user};
			if ($user_status{$user} >= 0) {
                                if (($var{'idle_time'} > 30)
                                    && ($idle_remaining <= 30)
                                    && !$user_idle_warning{$user}) {
                                        send_user_msg_with_server($user, $server, "Closing idle connection in 30 seconds.");
                                        $user_idle_warning{$user} = 1;
                                }
                                if ($user_idle > $var{'idle_time'}) {
					send_user_msg_with_server($user, $server, "Idletime (".$c1."$var{'idle_time'}".$c2." seconds) reached... come back when you are not asleep ;)");
                                        debug_msg("close user $user because of Idletime reached.");
#					IRC::command("/dcc close chat $user");
#					free_user_vars($user);
					close_user($user, $server);
				}
			} elsif ($user_status{$user} == -1) {
				if (($now - $user_online_time{$user}) > IRC::get_prefs("dcc_timeout")) {
					send_user_msg_with_server($user, $server, "DCC offer timeout!\n");
                                        debug_msg("close user $user because of DCC offer timeout.");
#					IRC::command("/dcc close chat $user");
#					free_user_vars($user);
					close_user($user, $server);
				}
			}
		}

                my $sends = 0;
                $sends = @queue_send - 1;
                for (my $i = $sends; $i >= 0; $i--) {
                        $queue_send[$i]->{checks} += 5;

                        if ($queue_send[$i]->{checks} >= 120) {
                                $queue_send[$i]->{checks} = 0;

                                if (send_dead($i)) {
                                        debug_msg("remove dead send $i of: ".$queue_send[$i]->{nick}."  send_files[$i]: ".$queue_send[$i]->{file});
                                        remove_from_send_slot($queue_send[$i]->{nick}, $queue_send[$i]->{file}, 0);
                                } elsif (($var{'min_speed'} > 0) && (check_send_speed($i) == 0)) {
                                        # speed is lower than $var{'min_speed'}
                                        debug_msg("remove slow send $i of: ".$queue_send[$i]->{nick}."  send_files[$i]: ".$queue_send[$i]->{file});
                                        remove_from_send_slot($queue_send[$i]->{nick}, $queue_send[$i]->{file}, 0);
                                }
			}
		}

		# only do this if file server is enabled
		if ($file_server) {
			$online_time += 5;
	
			# backup system...
			if ($var{'auto_backup'} > 0) {
				$backup_timer += 5;
				if ($backup_timer > $var{'auto_backup'}) {
					$backup_timer = 0;

					save_settings();
					saveq();
				}
			}

			# check if notifies should be sent out again
			if (length($var{'notify_channels'}) > 0
                            && $var{'notify_timeout'} > 0
                            && $var{'ads_when_full'} > 0 &&
                            (($now - $notify_timer)>=$var{'notify_timeout'})) {
				notify_channels($var{'notify_channels'});
				$notify_timer = $now;
			}

			# check if there are files to send
                        if (send_next_file(1)) {
                                send_next_file(0);
                        }
		} else {
			$offline_time += 5;
			$autoon_time += 5;
                        if($var{'autoon'} && ($autoon_time >= 300)) {
                                IRC::print("$logo Fileserver Auto Online");
                                $file_server= 1;
                                $notify_time = $now;
                                cache_welcome();
                                cache_files();
                        }
                }
	}

	IRC::add_timeout_handler(200, "Obsidian::obstimer");
	return 0;
}


#
# 	privmsg_handler(): handles PRIVMSG, CTCP and channel messages
#
sub privmsg_handler
{
	if (!$file_server) {
		return 0;
	}

	my $line = shift(@_);
	my $server = IRC::get_info(3);
	my $net = which_net($server);
	debug_msg($line);

	$line =~ /:(.*)!(.*@.*) PRIVMSG (.*) :(.*)/;
	
	my $nickname = $1;
	my @aliases = which_aliases($net);
	my $nick = $nickname . "@" . $aliases[0];
	my $hostname = $2;
	my $dest = $3;
	my $message = $4;
        my $ucmessage= uc($message);
        $ucmessage=~ s/\x1//g; #extract ctcp messages

	debug_msg("PRIVMSG from $nick ($hostname) to $dest on $server");
        debug_msg("message: $message");
        check_ignore("$nickname!$hostname");

        #check if the message is interesting at all.
        if (!(($ucmessage eq uc($var{'trigger'})) ||
              ($ucmessage eq "!LIST" || $ucmessage eq "!LIST ".uc(IRC::get_info(1))) ||
              ($ucmessage =~ /^\@FIND/))) {
                return 0;
        }
        #prepare some data
        my $listen_channel= " ".$var{'serve_no_notify'}." ".$var{'notify_to_voice'}." ".$var{'notify_channels'}." ";
        my $flag_listen= ($listen_channel=~ / $dest /im) || ($listen_channel=~ / $dest\@$net /im);
        my $hidden_channel= " ".$var{'serve_hidden'}." ";
        my $flag_hidden= ($hidden_channel=~ / $dest /im) || ($hidden_channel=~ / $dest\@$net /im);
        my $flag_direct= uc($dest) eq uc(IRC::get_info(1));
        if ($flag_listen) {debug_msg("flag_listen set"); };
        if ($flag_hidden) {debug_msg("flag_hidden set"); };
        if ($flag_direct) {debug_msg("flag_direct set"); };
        #check if the message is a trigger.
        if ($ucmessage eq uc($var{'trigger'})) {
                # Check if the server is switched on for the channel
                # at all. If it was goint to another channel ignore it.
                if (!($flag_listen || $flag_hidden || $flag_direct)) { return 0; }
                my $channelall= user_is_in_channels($nick, $server, $var{'serve_hidden'}." ".$listen_channel);
                if (!($channelall eq "")) {
                        debug_msg("user found in channels: ".$channellist." message: ".$message." length: ".length($message));
                        my $userinfo= user_info($nick, $channelall);
                        open_dcc_chat($nick, $server, $userinfo);
                }
        }
        #check if the message is !list or !list nick
        if ($ucmessage eq "!LIST" || $ucmessage eq "!LIST ".uc(IRC::get_info(1))) {
                if (!($flag_listen || $flag_direct)) { return 0; }
                my $channellist= user_is_in_channels($nick, $server, $listen_channel);
                if (!($channellist eq ""))  {
                        show_notice($nick, $server);
                }
        }
        #check if the message is @find
	if ($ucmessage =~ /^\@FIND/) {
                if (!($flag_listen || $flag_direct)) { return 0; }
                my $channellist= user_is_in_channels($nick, $server, $listen_channel);
                if (!($channellist eq ""))  {
                        $message =~ s/\s+/ /g;
                        my @fields = split(' ', $message);
                        shift(@fields);
                        find_pattern($nick, $server, "@fields");
                }
        }
        return 0;
}


#
#	dcc_connect_handler(): handles newly established DCC connections of xchat 1.8.x
#
sub dcc_connect_handler
{
	my ($chat,$nick) = split_params("nn",@_);
	
	my $server = IRC::get_info(3);
	my $net = which_net($server);
	my @aliases = which_aliases($net);
	$nick = $nick . "@" . $aliases[0];
	
	debug_msg("dcc_connect_handler(): chat: $chat, nick: $nick");

	if (defined $user_status{$nick} && $chat eq "CHAT") {
		$access_count += 1;
		initialize_user_vars($nick, $server);
	}

	return 0;
}

#
#	dcc_chat_connect_handler(): handles newly established DCC chat connections of xchat 2
#
sub dcc_chat_connect_handler
{
        #FIXME TEST
	debug_msg("dcc_connect_handler(): ".@_);
#	my ($t1,$t2,$t3) = split_params(@_,"nnn");
#      debug_msg("dcc_connect_handler(): t1: $t1  t2: $t2  t3: $t3");
	my ($chat,$nick) = @_;
	debug_msg("dcc_connect_handler(): nick: $nick  chat: $chat");
	my ($nick,$ctcp,$reserved) = split_params("nnn",@_);
	debug_msg("dcc_connect_handler(): nick: $nick  ctcp: $ctcp  reserved: $reserved");
	
	my $server = IRC::get_info(3);
	my $net = which_net($server);
	my @aliases = which_aliases($net);
	$nick = $nick . "@" . $aliases[0];
	debug_msg("dcc_connect_handler(): nick: $nick  server: $server  net: $net");

	if (defined $user_status{$nick}) {
		$access_count += 1;
		initialize_user_vars($nick, $server);
	}

	return 0;
}

#
#	dcc_chat_reoffer_handler(): handles DCC chat reoffer events
#
sub dcc_chat_reoffer_handler
{
	my ($nick) = split_params("n",@_);

	my $server = IRC::get_info(3);
	my $net = which_net($server);
	my @aliases = which_aliases($net);
	my $nick = $nick . "@" . $aliases[0];

	if (defined $user_status{$nick}) {
		debug_msg("reestablishing connection...");
                $access_count += 1;
		initialize_user_vars($nick, $server);
		return 1;	# important!
	}

	return 0;
}

#
# 	dcc_msg_handler(): handles DCC messages of xchat 1.8 and
#		           DCC CHAT text print handlers of xchat 2.0
#
sub dcc_msg_handler
{
	if (!$file_server) {
		return 0;
	}

	my $line = shift(@_);

	debug_msg("dcc_msg_handler(): $line");

	my $server = IRC::get_info(3);
	my $net = which_net($server);
	my @aliases = which_aliases($net);

	my @args = split(' ', $line);
#	$line =~ /(.*) (.*) (.*): (.*)/;
	shift(@args); shift(@args);
	my $nick = shift(@args);
	my $cmd = shift(@args);
	$nick = $nick . "@" . $aliases[0];

	# ignore messages from users not connected to server
	unless (defined $user_status{$nick}) {
		debug_msg("ignoring unconnected user");
		return 0;
	}
	
	# clear idletime for user
        $user_status{$nick} = time();
        $user_idle_warning{$nick} = 0;
        debug_msg("push user command: ".$nick." cmd: ".$cmd." args: "."@args");
	# put the command on buffer
	push(@user_commands, $nick);
	push(@user_commands, $server);
	push(@user_commands, $cmd);
	push(@user_commands, "@args");

	return 0;
}


#
#	parse_user_commands(): executes command in @user_commands
#
sub parse_user_command
{
	# fetch the command from buffer
	my $nick = shift(@user_commands);
	my $server = shift(@user_commands);
	my $cmd = shift(@user_commands);
	my $args = shift(@user_commands);

        debug_msg("parse_user_command: ".$nick." @".$server." cmd: ".$cmd." args: "."@args");

	$cmd =~ s/\003\d\d?//g;		# remove color information
	$cmd =~ s/%C\d\d?//g;

        unless ($var{'case_cmd'}) {
                $cmd = lc($cmd);
        }

	unless ($cmd eq "set") { 
		$args =~ s/\003\d\d?//g;
		$args =~ s/%C\d\d?//g;
	}

	if ($cmd eq "dir" || $cmd eq "ls") {
		list_dir($nick);
	} elsif ($cmd eq "cd") {
		if (length($args) == 0) {
			send_user_msg_with_server($nick, $server, "Command \'cd\' needs an argument");
		} else {
			change_dir($nick, $server, $args);
		}
	} elsif ($cmd eq "cd..") {
		change_dir($nick, $server, "..");
	} elsif ($cmd eq "pwd") {
		display_path($nick, $server);
	} elsif ($cmd eq "queues" || $cmd eq "queue") {
		if (@queue_fail >= 1) {
                        display_fail_queues($nick, $server);
                }
		display_queues($nick, $server);
	} elsif ($cmd eq "dequeue") {
		if (length($args) == 0) {
			send_user_msg_with_server($nick, $server, "Command \'dequeue\' needs an argument");
		} else {
			dequeue_file($nick, $server, $args);
		}
	} elsif ($cmd eq "get") {
		if (length($args) == 0) {
			send_user_msg_with_server($nick, $server, "Command \'get\' needs an argument");
		} else {
			queue_file($nick, $server, $args);
		}
	} elsif ($cmd eq "clr_queues" || $cmd eq "clr_queue") {
		clr_queue($nick, $server);
	} elsif ($cmd eq "sends") {
		display_sends($nick, $server);
	} elsif ($cmd eq "exit" || $cmd eq "quit" || $cmd eq "bye") {
		send_user_msg_with_server($nick, $server, "Closing connection...");
                debug_msg("close user $nick because of exit command.");
                close_user($nick, $server);
	} elsif ($cmd eq "who") {
		display_who($nick, $server);
	} elsif ($cmd eq "stats") {
		display_stats($nick, $server);
	} elsif ($cmd eq "help") {
                display_help($nick, $server, $args);
	}
}


#
#	dcc_send_complete_handler(): handles completed DCC sends for xchat 1.8
#
sub dcc_send_complete_handler
{
	my ($file,$nick,$speed) = split_params("fnn",@_);
	my $server = IRC::get_info(3);
	my $net = which_net(IRC::get_info(3));
	my @aliases = which_aliases($net);
	$nick = $nick . "@" . $aliases[0];
	debug_msg("dcc_send_complete_handler(): file: $file, nick: $nick, speed: $speed");

	my $size = remove_from_send_slot($nick, $file, 1);
	if ($size >= 0) {
		# increase file counter and byte counter
		$file_count += 1;
		$byte_count += $size;

		if ($speed > $record_cps) {
			$record_cps_user = $nick;
			$record_cps = $speed;
		}
	}

	return 0;
}

#
#	dcc_timeout_handler(): handles timedout DCC events
#
sub dcc_timeout_handler
{
	my ($type,$file,$nick) = split_params("nfn",@_);
	my $server = IRC::get_info(3);
	my $net = which_net($server);
	
	my @aliases = which_aliases($net);
	my $nick = $nick . "@" . $aliases[0];

	debug_msg("dcc_timeout_handler(): type: $type, file: $file, nick: $nick");

	if ($type eq "SEND" && remove_from_send_slot($nick, $file, 0) >= 0) {
	}

	return 0;
}

#
# 	dcc_abort_handler(): handles aborted DCC events
#
sub dcc_abort_handler
{
	my ($type,$file,$nick) = split_params("nfn",@_);

	my $server = IRC::get_info(3);
	my $net = which_net($server);
	my @aliases = which_aliases($net);
	my $nick = $nick . "@" . $aliases[0];

	# ignore users not connected to ServU
	unless (defined $user_status{$nick}) {
		return 0;
	}

	debug_msg("dcc_abort_handler(): <$type> <$file> <$nick>");

	if ($type eq "CHAT") {
                close_user ($nick, $server);
#		free_user_vars($nick);
#                if ($var{'close_tab_auto'}) {
#                        debug_msg("close user $nick because of DCC abort handler.");
#                       IRC::command("/close");
#                        IRC::command("/close $nick");
#                }
	} elsif ($type eq "SEND" && remove_from_send_slot($nick, $file, 0) >= 0) {
	}

	return 0;
}


#
#	dcc_chat_failed_handler(): handles failed DCC chats
#
sub dcc_chat_failed_handler
{
	my ($nick) = split_params("n",@_);
	my $server = IRC::get_info(3);
	my $net = which_net($server);
	my @aliases = which_aliases($net);
	my $nick = $nick . "@" . $aliases[0];
	debug_msg("dcc_chat_failed_handler(): $nick");

	# ignore users not connected to ServU
	unless (defined $user_status{$nick}) {
		return 0;
	}

	debug_msg("dcc_chat_failed_handler(): nick: $nick");
        if ($var{'close_tab_auto'}) {
		debug_msg ("close_user debug:  /close $user\n");
		IRC::command_with_channel("/close", $user);
	}
	free_user_vars ($nick);
#      close_user ($nick);
	debug_msg ("dcc_chat_failed_handler():  exit");

#	return 1; Crashes ?
	return 0;
#/home/alex/bin/xchats: line 2:  2512 Segmentation fault      (core dumped) /usr/bin/xchat -d ~/.xchat.serv
#	free_user_vars($nick);
#        if ($var{'close_tab_auto'}) {
#                debug_msg("close user $nick because of DCC chat failed handler.");
#               IRC::command("/close $nick");
#               IRC::command("/close");
#        }
#	return 0;
}


#
#	dcc_send_failed_handler(): handles failed DCC sends
#
sub dcc_send_failed_handler
{
	my ($file,$nick) = split_params("fn",@_);
	my $server = IRC::get_info(3);
	my $net = which_net($server);
	my @aliases = which_aliases($net);

	my $nick = $nick . "@" . $aliases[0];
	debug_msg("dcc_abort_handler(): $file to $nick");

	if (remove_from_send_slot($nick, $file, 0) >= 0) {
		$send_fails++;
	}

	return 0;
}


#
#	change_nick_handler(): updates variables when users nick changes
#
sub change_nick_handler
{
	my ($old_nick,$new_nick) = split_params("nn",@_);
	my $server = IRC::get_info(3);
	my $net = which_net($server);
	my @aliases = which_aliases($net);

	my $old_nick = $old_nick . "@" . $aliases[0];
	my $new_nick = $new_nick . "@" . $aliases[0];
	debug_msg("change_nick_handler(): $old_nick -> $new_nick");

	if (defined $user_status{$old_nick}) {
		# update user specific variables		
		$user_status{$new_nick} = $user_status{$old_nick};
		$user_info{$new_nick} = $user_info{$old_nick};
		$user_dir{$new_nick} = $user_dir{$old_nick};
		$user_online_time{$new_nick} = $user_online_time{$old_nick};

		free_user_vars($old_nick);
	}

	# go through the sends
	for (my $i = 0; $i < @queue_send; $i++) {
		if (compare_nick($queue_send[$i]->{nick},$old_nick)) {
			$queue_send[$i]->{nick} = $new_nick;
		}
	}

	# go through the queue
	for ($i = 0; $i < @queue_norm; $i++) {
		if (compare_nick($queue_norm[$i]->{nick},$old_nick)) {
			$queue_norm[$i]->{nick} = $new_nick;
		}
	}

	return 0;
}


#
#       disconnect_handler: called when disconnected from server. Stops
#                           the FServe.
#
sub disconnect_handler
{
        IRC::print("$logo Disconnected from server, stopping the FServe...");
        $file_server = 0;
        $autoon_time = 0;		
#	save_settings();
	if ($var{'restorequeues'} == 1) {
                saveq();
        }
}


#
#      splitt_params(): seperates all parameters of a handler.
#        Allowing both xchat 2 and xchat 1.8 syntax.
#        A parameter describing the expected syntax of the line is rquired.
#        The problem are mainly filenames with spaces.
#        split_params("nfn",@_): Three parameters, the second is a filename.
#        Obviously only one filename can be in the parameter set.
#
sub split_params
{
        my $format = shift(@_);
        my $line = shift(@_);
        debug_msg(IRC::get_info(0));
        my @dst;
        my @src = split(' ', $line);
        debug_msg("line: <@src> $line format: $format");
        # remove (null) values from xchat 2 parameter sets
#        if ( not (IRC::get_info(0) =~ /^1\./)) {
#                debug_msg("xchat 2+ adjust");
#                for ($i=length $format;$i < 4; $i++) { pop @src }
#        }
        # read nick type parameter in front
        while (not ($format =~ /^f/) and length $format) {
                push @dst, shift @src;
                $format = substr $format,2;
        }
        # get the filename if any
        if ($format =~ /^f/) {
                my @buf;
                @buf = splice @src, 0, @src + 1 - length $format;
                debug_msg("filename: @buf");
                push @dst, join ' ',@buf;
        }
        # read the rest if any
        while (@src) {
                push @dst, shift @src;
        }
        debug_msg("@dst");
        return @dst;
}


#############################################################################
#
#	              Fserve commands
#
#############################################################################



#
# Shows the welcome message that is send when someone types the trigger.
#
sub show_welcome
{
	my ($nick) = @_;
        my ($to_nick, $net) = split('@', $nick);
        my $server = which_server($net);
        if ($var{'open_tab_early'}) {
                IRC::command_with_server("/query ".$to_nick, which_server($net));
        }

        my $line1="############################################################";
#        my $line2="#       ~= [\00313Oßsidian\00314] fileserver for X-Chat 1.8.x =~       #";
#        my $line2="#       ~= [\00313Obsidian\00314] fileserver for X-Chat 1.8.x =~       #";
        my $line2="#   ~= [\00313Obsidian 0.9.2-pre2\00314], a fileserver for X-Chat =~   #";
        my $line3="#                                                          #";
        my @wlcm1= ("#  Basic Commands: help              dir                   #",
                    "#                  cd <directory>    get <filename>        #");
        my @wlcm2= ("#  If you are not in the channel where You typed the       #",
                    "#  trigger when your send is available it will be skipped! #");
        my @wlcm3= ("#  I'm using a case sensitive file system, so please       #",
                    "#  use upper letters also.                                 #");
        my @wlcm4= ("#  The amount of files You can queue depends on the file-  #",
                    "#  size. You can queue more small files than big files.    #");
        my @wlcm5= ("#  Small files will be sent immediatly.                    #");

        my @wlcm= ($line1, $line2, $line1, $line3);
	foreach (@wlcm1) { push(@wlcm, $_) };
        push(@wlcm, $line3);
	foreach (@wlcm2) { push(@wlcm, $_) };
        push(@wlcm, $line3);
	if ($var{'case_file'}) {
                foreach (@wlcm3) { push(@wlcm, $_) };
        }
	if ($var{'fs_sensitive'}) {
                foreach (@wlcm4) { push(@wlcm, $_) };
        }
	if ($var{'send_small_now'}) {
                foreach (@wlcm5) { push(@wlcm, $_) };
        }
        push(@wlcm, $line3);
        push(@wlcm, $line1);

 	foreach (@welcome_msg) { push(@wlcm, $_) };

        push(@wlcm, $c2."Current path: [".$c1.((length($user_dir{$nick}) == 0)?"/":"$user_dir{$nick}").$c2."]");
	foreach (@wlcm) {
                my $buf = $_;
                if ($var{'raw_wlcm_msg'}) {
                        send_user_raw_msg($nick, $server, $buf."\n");
                } else {
                        send_user_msg($nick, $buf);
                }
        }
}


#
# Shows the basic functions of this script when someone types: /fs
#
sub show_basic_help
{
        IRC::print("$logo Fileserver for X-Chat 1.8.x (" . (($file_server)?"Online":"Offline") .  ")");
        IRC::print("$logo");
        IRC::print("$logo   Command  Parameters    Description");
        IRC::print("$logo   =======  ==========    ===========");
        IRC::print("$logo   on                     enable server");
        IRC::print("$logo   off                    disable server");
        IRC::print("$logo   load                   load config file");
        IRC::print("$logo   save                   save config file");
        IRC::print("$logo   saveq                  saves sends/queue");
        IRC::print("$logo   loadq                  loads sends/queue");
        IRC::print("$logo   load_backup            loads sends/queue backup");
        IRC::print("$logo   update_files           recache files");
        IRC::print("$logo   show_files             show files in the cache");
        IRC::print("$logo   queuef <nick> <file>   queues a file for a user");
        IRC::print("$logo   sendf  <nick> <file>   sends a file to a user");
        IRC::print("$logo   send_queue #a          sends a queued file now");
        IRC::print("$logo   reset_sends            resets sends");
        IRC::print("$logo   clr_queues             clears entire queue");
        IRC::print("$logo   clr_fqueues            clears entire failqueue");
        IRC::print("$logo   dequeue   #a           clear slot #a");
        IRC::print("$logo   move  #a  #b           moves #a to #b");
        IRC::print("$logo   stats                  show statistics");
        IRC::print("$logo   notify [channel]       notify channel(s) now");
        IRC::print("$logo   smsg   <message>       message to users receiving files");
        IRC::print("$logo   amsg   <message>       message to all users");
        IRC::print("$logo   qmsg   <message>       message to users in queue");
        IRC::print("$logo   sends                  show active sends");
        IRC::print("$logo   queues                 list queued files");
        IRC::print("$logo   vars                   list variables");
        IRC::print("$logo   toggle                 list switchs");
        IRC::print("$logo   toggle <switch> <0|1>  set switch");
        IRC::print("$logo   set    <var>   <data>  set variable");
        IRC::print("$logo   help   <keyword>       help on keyword");
        IRC::print("$logo   who                    show connected users");
        IRC::print("$logo   about                  info about this fserve");
        IRC::print("$logo   gpl                    view license of this fserve");
        IRC::print("$logo");
        IRC::print("$logo usage: /fs <command> [<parameters>]");
        IRC::print("\n");
}


#
# Shows all variable of this script when someone types: /fs vars or /fs set
#
sub show_vars
{
        my $cl1 = $c1; $cl1 =~ s/\003/%C/; # restore
        my $cl2 = $c2; $cl2 =~ s/\003/%C/; # colors to
        my $cl3 = $c3; $cl3 =~ s/\003/%C/; # printable
        my $cl4 = $c4; $cl4 =~ s/\003/%C/; # form
        IRC::print("$logo Variable         Current Value");
        IRC::print("$logo ========         =============");
        IRC::print("$logo max_users        $var{'max_users'}");
        IRC::print("$logo max_sends        $var{'max_sends'}");
        IRC::print("$logo max_queues       $var{'max_queues'}");
        IRC::print("$logo slots_big        $var{'slots_big'}");
        IRC::print("$logo slots_med        $var{'slots_med'}");
        IRC::print("$logo slots_small      $var{'slots_small'}");
        IRC::print("$logo size_big         $var{'size_big'}");
        IRC::print("$logo size_med         $var{'size_med'}");
        IRC::print("$logo idle_time        $var{'idle_time'}");
        IRC::print("$logo max_fails        $var{'max_fails'}");
        IRC::print("$logo min_speed        $var{'min_speed'}");
        IRC::print("$logo auto_backup      $var{'auto_backup'}");
        IRC::print("$logo trigger          $var{'trigger'}");
        IRC::print("$logo serve_no_notify  $var{'serve_no_notify'}");
        IRC::print("$logo serve_hidden     $var{'serve_hidden'}");
        IRC::print("$logo notify_to_voice  $var{'notify_to_voice'}");
        IRC::print("$logo notify_channels  $var{'notify_channels'}");
        IRC::print("$logo netaliases       $var{'netaliases'}");
        IRC::print("$logo notify_timeout   $var{'notify_timeout'}");
        IRC::print("$logo root_dir         $var{'root_dir'}");
        IRC::print("$logo hilite_color     $cl1");
        IRC::print("$logo text_color       $cl2");
        IRC::print("$logo dir_color        $cl3");
        IRC::print("$logo file_color       $cl4");
        IRC::print("$logo logo             $logo");
        IRC::print("$logo note             $var{'note'}");
        IRC::print("\n$logo All time values are in seconds, if notify_timeout,");
        IRC::print("$logo min_speed or auto_backup is set to zero, the");
        IRC::print("$logo the function is disabled...");
        IRC::print("\n");
}


#
# Shows all toggles of this script when someone types: /fs toggle
#
sub show_toggle
{
        IRC::print("$logo Switch           Current Value");
        IRC::print("$logo ========         =============");
        IRC::print("$logo debug            $var{'debug'}");
        IRC::print("$logo autoon           $var{'autoon'}");
        IRC::print("$logo restorequeues    $var{'restorequeues'}");
        IRC::print("$logo notice_as_msg    $var{'notice_as_msg'}");
        IRC::print("$logo short_notice     $var{'short_notice'}");
        IRC::print("$logo ctcptrigger      $var{'ctcptrigger'}");
        IRC::print("$logo raw_chnl_msg     $var{'raw_chnl_msg'}");
        IRC::print("$logo raw_wlcm_msg     $var{'raw_wlcm_msg'}");
        IRC::print("$logo open_tab_early   $var{'open_tab_early'}");
        IRC::print("$logo close_tab_auto   $var{'close_tab_auto'}");
        IRC::print("$logo case_cmd         $var{'case_cmd'}");
        IRC::print("$logo case_file        $var{'case_file'}");
        IRC::print("$logo fs_sensitive     $var{'fs_sensitive'}");
        IRC::print("$logo priority_op      $var{'priority_op'}");
        IRC::print("$logo priority_voice   $var{'priority_voice'}");
        IRC::print("$logo count_sends      $var{'count_sends'}");
        IRC::print("$logo send_small_now   $var{'send_small_now'}");
        IRC::print("$logo ads_when_full    $var{'ads_when_full'}");
        IRC::print("\n");
}


#
# 	server_command_handler(): handles /fs commands
#
sub server_command_handler
{
	my $line = shift(@_);
	my $server = IRC::get_info(3);
	$line =~ s/\s+/ /g;	# remove excessive whitespace chars

        $autoon_time= 0;        # reset inactivity timer

	my @args = split(' ', $line);
	if (@args == 0 || ($args[0] eq "help" && @args == 1)) {
                show_basic_help();
	} elsif ($args[0] eq "help") {
                show_help($args[1]);
        } else {
		my $cmd = shift(@args);

		if ($cmd eq "set") {
			if (@args == 0) {
                                show_vars();
                        } elsif (@args < 1) {
				IRC::print("$logo Usage: /fs set <variable> [data]");
				IRC::print("$logo use \'/fs vars\' or \'/fs set\'for list of variables");
			} else {
				my $cmd = shift(@args);
				my $data = "@args";
				my $col = "\00301";

				# convert from %Cx? to \003xx form
				if ($cmd =~ m/color/) {
					$col = $data;
					$col =~ s/%C//;
					$col = sprintf("\003%02d", $col);
				}

				if ($cmd eq "max_users") {
					$var{'max_users'} = $data;
				} elsif ($cmd eq "max_sends") {
					$var{'max_sends'} = $data;
				} elsif($cmd eq "max_queues") {
					$var{'max_queues'} = $data;
				} elsif($cmd eq "slots_big") {
					$var{'slots_big'} = $data;
				} elsif($cmd eq "slots_med") {
					$var{'slots_med'} = $data;
				} elsif($cmd eq "slots_small") {
					$var{'slots_small'} = $data;
				} elsif($cmd eq "size_big") {
					$var{'size_big'} = $data;
				} elsif($cmd eq "size_med") {
					$var{'size_med'} = $data;
				} elsif($cmd eq "idle_time") {
					$var{'idle_time'} = $data;
				} elsif($cmd eq "max_fails") {
					$var{'max_fails'} = $data;
				} elsif($cmd eq "min_speed") {
					$var{'min_speed'} = $data;
				} elsif($cmd eq "trigger") {
					$var{'trigger'} = $data;
				} elsif($cmd eq "notify_channels") {
					$var{'notify_channels'} = $data;
				} elsif($cmd eq "serve_no_notify") {
					$var{'serve_no_notify'} = $data;
				} elsif($cmd eq "serve_hidden") {
					$var{'serve_hidden'} = $data;
				} elsif($cmd eq "notify_to_voice") {
					$var{'notify_to_voice'} = $data;
				} elsif($cmd eq "netaliases") {
					$var{'netaliases'} = $data;
				} elsif($cmd eq "notify_timeout") {
					$var{'notify_timeout'} = $data;
				} elsif($cmd eq "root_dir") {
					$var{'root_dir'} = $data;
					cache_welcome();
					cache_files();
				} elsif($cmd eq "hilite_color") {
					$c1 = $col;
				} elsif($cmd eq "text_color") {
					$c2 = $col;
				} elsif($cmd eq "dir_color") {
					$c3 = $col;
				} elsif($cmd eq "file_color") {
					$c4 = $col;
				} elsif($cmd eq "logo") {
					$logo = $data;
					$logo =~ s/%C/\003/g; # fix the colors
				} elsif($cmd eq "note") {
					$var{'note'} = $data;
					$var{'note'} =~ s/%C/\003/g; # fix the colors
				} elsif($cmd eq "auto_backup") {
					$var{'auto_backup'} = $data;
				} else {
					IRC::print("$logo Unknown variable \'$cmd\'");
					return 1;
				}

				IRC::print("$logo Setting \'$cmd\' to \'$data\'");
			}
		} elsif ($cmd eq "vars") {
                        show_vars();

		} elsif (($cmd eq "toggle") && (@args == 0)) {
                        show_toggle();

		} elsif (($cmd eq "toggle") && (@args == 2)) {
                        my $cmd = shift(@args);
                        my $data = "@args";
                        my $col = "\00301";

                        # convert from %Cx? to \003xx form
                        if ($cmd =~ m/color/) {
                                $col = $data;
                                $col =~ s/%C//;
                                $col = sprintf("\003%02d", $col);
                        }

                        if ($cmd eq "debug") {
                                $var{'debug'} = $data;
                        } elsif($cmd eq "notice_as_msg") {
                                $var{'notice_as_msg'} = $data;
                        } elsif($cmd eq "short_notice") {
                                $var{'short_notice'} = $data;
                        } elsif($cmd eq "autoon") {
                                $var{'autoon'} = $data;
                        } elsif($cmd eq "restorequeues") {
                                $var{'restorequeues'} = $data;
                        } elsif($cmd eq "ctcptrigger") {
                                $var{'ctcptrigger'} = $data;
                        } elsif($cmd eq "raw_chnl_msg") {
                                $var{'raw_chnl_msg'} = $data;
                        } elsif($cmd eq "raw_wlcm_msg") {
                                $var{'raw_wlcm_msg'} = $data;
                                if ($var{'close_tab_auto'}) {
                                        IRC::print("$logo Warning: this feature is deprecated. Use open_tab_early instead.");
                                }
                        } elsif($cmd eq "open_tab_early") {
                                $var{'open_tab_early'} = $data;
                        } elsif($cmd eq "close_tab_auto") {
                                $var{'close_tab_auto'} = $data;
                                if ($var{'close_tab_auto'}) {
                                        IRC::print("$logo Warning: experimental feature. This might crash xchat.");
                                }
                        } elsif($cmd eq "case_cmd") {
                                $var{'case_cmd'} = $data;
                        } elsif($cmd eq "case_file") {
                                $var{'case_file'} = $data;
                        } elsif($cmd eq "fs_sensitive") {
                                $var{'fs_sensitive'} = $data;
                        } elsif($cmd eq "count_sends") {
                                $var{'count_sends'} = $data;
                        } elsif($cmd eq "priority_op") {
                                $var{'priority_op'} = $data;
                        } elsif($cmd eq "priority_voice") {
                                $var{'priority_voice'} = $data;
                        } elsif($cmd eq "send_small_now") {
                                $var{'send_small_now'} = $data;
                        } elsif($cmd eq "ads_when_full") {
                                $var{'ads_when_full'} = $data;
                        } else {
                                IRC::print("$logo Unknown switch \'$cmd\'");
                                return 1;
                        }
                        IRC::print("$logo Setting \'$cmd\' to \'$data\'");

		} elsif ($cmd eq "save") {
			save_settings();
		} elsif ($cmd eq "load") {
			load_settings();
		} elsif ($cmd eq "who") {
			display_who("obsidian server", $server);
			IRC::print("\n");
		} elsif ($cmd eq "on") {
#			IRC::add_timeout_handler(100, "obstimer");
                        $timer_count--;
                        IRC::print("$logo Fileserver Online");
			IRC::print("\n");
			$file_server = 1;
                        $notify_timer = time();
			cache_welcome();
			cache_files();
		} elsif ($cmd eq "off") {
			IRC::print("$logo Fileserver Offline");
			IRC::print("\n");
			$file_server = 0;

			#FIXME, kick out users...
		} elsif ($cmd eq "stats") {
			display_stats("obsidian server", $server);
			IRC::print("\n");
		} elsif ($cmd eq "notify") {
                        if (@args >= 1) {
                                notify_channels(join(' ', @args));
                        } else {
                                notify_channels($var{'notify_channels'});
                        }
		} elsif ($cmd eq "queues") {
                        if (@queue_fail >= 1) {
                                display_fail_queues("obsidian server", $server);
                        }
			display_queues("obsidian server", $server);
			IRC::print("\n");
		} elsif ($cmd eq "sends") {
			display_sends("obsidian server", $server);
			IRC::print("\n");
		} elsif ($cmd eq "reset_sends") {
                        @queue_send = ();
		} elsif ($cmd eq "saveq") {
			saveq();
		} elsif ($cmd eq "loadq") {
			loadq("obsidian.queue");
		} elsif ($cmd eq "load_backup") {
			loadq("obsidian.bqueue");
		} elsif ($cmd eq "clr_queues" || $cmd eq "clr_queue") {
			clear_queue();
		} elsif ($cmd eq "clr_fqueues" || $cmd eq "clr_fqueue") {
			clear_fqueue();
		} elsif ($cmd eq "dequeue") {
			clr_slot_a(shift(@args));
		} elsif ($cmd eq "move") {
			move_a_to_b(shift(@args),shift(@args));
		} elsif ($cmd eq "update_files") {
			cache_welcome();
			cache_files();
                } elsif ($cmd eq "smsg") {
                        message_to_receivers(join(' ', @args));
                } elsif ($cmd eq "qmsg") {
                        message_to_queues(join(' ', @args));
                } elsif ($cmd eq "amsg") {
                        message_to_all(join(' ', @args));
                } elsif ($cmd eq "show_files") {
                        show_files();
                } elsif ($cmd eq "queuef") {
                        admin_queue_file(shift(@args), join(' ', @args));
                } elsif ($cmd eq "sendf") {
                        admin_send_file(shift(@args), join(' ', @args));
                } elsif ($cmd eq "send_queue") {
                        admin_send_queue(shift(@args));
		} elsif ($cmd eq "about") {
                        show_help("about");
		} elsif ($cmd eq "gpl") {
                        show_help("gpl");
		} else {
			IRC::print("$logo Huh!?\n");
		}
	}

	# no further handling required
	return 1;
}


#
#	shutdown(): called when script is unloaded
#
sub shutdown
{
	debug_msg("shutdown() of obsidian initiated.");
	IRC::print("$logo Cleaning up...");
	save_settings();
	if ($var{'restorequeues'} == 1) {
                saveq();
        }
        return 0;
}


#############################################################################
#
#	Functions called by the script
#
#############################################################################

#
#       message_to_receivers($message): sends a privmsg to all users
#                                       that have an active DCC send
#

sub message_to_receivers
{
        my $message = shift;
        my %msg_to = ();
        my $user;

        foreach $item (@queue_send) {
                $msg_to{$item->{user}} = 1;
        }
        foreach $user (keys %msg_to) {
		my ($to_nick, $net) = split('@', $user);
		my $server = which_server($net);
                IRC::command_with_server("/msg $to_nick $message", $server);
        }
}


#
#       message_to_queues($message): sends a privmsg to all users
#                                    that have a queued file
#
sub message_to_queues
{
        my $message = shift;
        my %msg_to = ();
        my $user;

        foreach $item (@queue_norm) {
                $msg_to{$item->{user}} = 1;
        }
        foreach $user (keys %msg_to) {
		my ($to_nick, $net) = split('@', $user);
		my $server = which_server($net);
                IRC::command_with_server("/msg $to_nick $message", $server);
        }
}


#
#       message_to_all($message): sends a privmsg to all users
#                                 that have either an active DCC send
#                                 or a queued file
#
sub message_to_all
{
        my $message = shift;
        my %msg_to = ();
        my $user;

        foreach $item (@queue_send,@queue_fail,@queue_norm) {
                $msg_to{$item->{user}} = 1;
        }
        foreach $user (keys %msg_to) {
		my ($to_nick, $net) = split('@', $user);
		my $server = which_server($net);
                IRC::command_with_server("/msg $to_nick $message", $server);
        }
}


#
#       build_file_list: builds a flat list of cached files, sizes, paths
#
sub build_file_list
{
        my @list;
        my $dir;

        foreach $dir (keys %file_cache) {
                # sub-arrays: [0] sub dirs, [1] file names, [2] sizes
                my @data = @{$file_cache{$dir}};
                my @files = @{$data[1]};
                my @sizes = @{$data[2]};
                my $i;
                for ($i = 0; $i < scalar(@files); $i++) {
                        push(@list, $files[$i]);
                        push(@list, $sizes[$i]);
                        push(@list, $dir);
                }
        }
        return @list;
}

#
#       show_files: displays the list of cached files
#
sub show_files
{
        my @list = build_file_list();
        my $i;

        for ($i = 0; $i < scalar(@list) / 3; $i++) {
                IRC::print("$logo ".$c1.($i + 1).$c2.") name (".$c1.
                        $list[$i * 3].$c2.") size (".$c1.
                        size_to_string($list[$i * 3 + 1]).$c2.")");
        }
}


#
#       admin_queue_file($nick,$name): queues a file for the user $nick.
#                                      $name can either be the number as
#                                      shown by "/fs show_files" or the
#                                      complete name.
#
sub admin_queue_file
{
	my ($nick, $file) = @_;

        if (! $nick || ! $file) {
                IRC::print("$logo ".$c2."You have to provide the nick and ".
                        "file name (or \#number).");
                return;
        }

        my $net = which_net(IRC::get_info(3));

	my @list = build_file_list();

        if (scalar(@list) == 0) {
                IRC::print("$logo ".$c2."There are no cached files ".
                        "that could be sent.");
                return;
        }

        my $number;

        $file =~ s/^\s+//;
        # number requested?
        if ($file =~ /^\#([0-9]+)/) {
                $number = $1;
                if (($number <= 0) || ($number > (scalar(@list) / 3))) {
                        IRC::print("$logo ".$c2."That number (".$c1.$number.
                                $c2.") is out of range (".$c1."1".$c2."..".$c1.
                                (scalar(@list) / 3).$c2.").");
                        return;
                }
                # Array indices are 0-based, user interaction is 1-based :-)
                $number--;
        } else {
                my $i;
                my $found = 0;
                my $lcfile = lc($file);
                # Find file in the list
                for ($i = 0; $i < scalar(@list) / 3; $i++) {
                        if (lc($list[$i * 3]) eq $lcfile) {
                                $found = 1;
                                $number = $i;
                                last;
                        }
                }
                if (! $found) {
                        IRC::print("$logo ".$c2."The file ".$c1.$file.$c2.
                                " is not in the cache.");
                        return;
                }
        }

        my %item;
        $item{nick}= $nick . "@" . $net;
        $item{path}= $list[$number * 3 + 2] . "/";
        $item{file}= $list[$number * 3];
        $item{size}= $list[$number * 3 + 1];
        $item{fails}= 0;
	$item{checks} = 0;
	push(@queue_norm, \%item);

	send_user_msg_with_server($nick."@".$net, $server, "The admin queued ".$c1.$list[$number * 3].$c2.
                " for you in slot ".$c1."#".@queue_norm);
}


#
#       admin_send_file($nick,$name): sends a file to the user $nick.
#                                     $name can either be the number as
#                                     shown by "/fs show_files" or the
#                                     complete name.
#
sub admin_send_file
{
	my ($nick, $file) = @_;
	my $server = IRC::get_info(3);
	my $net = which_net($server);

        if (! $nick || ! $file) {
                IRC::print("$logo ".$c2."You have to provide the nick and ".
                        "file name (or \#number).");
                return;
        }

        my @list = build_file_list();

        if (scalar(@list) == 0) {
                IRC::print("$logo ".$c2."There are no cached files ".
                        "that could be sent.");
                return;
        }

        my $number;
        $file =~ s/^\s+//;
        # number requested?
        if ($file =~ /^\#([0-9]+)/) {
                $number = $1;
                if (($number <= 0) || ($number > (scalar(@list) / 3))) {
                        IRC::print("$logo ".$c2."That number (".$c1.$number.
                                $c2.") is out of range (".$c1."1".$c2."..".$c1.
                                (scalar(@list) / 3).$c2.").");
                        return;
                }
                # Array indices are 0-based, user interaction is 1-based :-)
                $number--;
        }
        else {
                my $i;
                my $found = 0;
                my $lcfile = lc($file);
                # Find file in the list
                for ($i = 0; $i < scalar(@list) / 3; $i++) {
                        if (lc($list[$i * 3]) eq $lcfile) {
                                $found = 1;
                                $number = $i;
                                last;
                        }
                }
                if (! $found) {
                        IRC::print("$logo ".$c2."The file ".$c1.$file.$c2.
                               " is not in the cache.");
                        return;
                }
        }

        my $full_path = $list[$number * 3 + 2] . "/" . $list[$number * 3];
	IRC::command_with_server("/notice $nick ".$c2."The admin requested to send you this file (".$c1.size_to_string($list[$number * 3 + 1]).$c2.")", $server);
	IRC::command_with_server("/dcc send $nick \"$full_path\"", $server);

	# add data to current sends
        my %item;
        $item{nick}= $nick . "@" . $net;
        $item{path}= $list[$number * 3 + 2] . "/";
        $item{file}= $list[$number * 3];
        $item{size}= $list[$number * 3 + 1];
	$item{stime} = time();
	$item{checks} = 0;
#	$item{speed} = 0;
	push(@queue_send, \%item);
}


#
#       admin_send_queue($number): Immediately sends the queued file number
#                                  to the appropriate user. Disregards send
#                                  slots limits.
#
sub admin_send_queue
{
        my $is = shift(@_);
        $is =~ s/\#//g;
	my $i = $is;

        if (($i == 0) || ($i > scalar(@queue_norm) || -$i > scalar(@queue_fail))) {
                IRC::print("$logo That number is not a valid queue slot.");
                return;
        }

        my $item;
        if ($i > 0) {
                $item= @queue_norm[$i-1];
                splice(@queue_norm, $i-1, 1);
        } else {
                $item= @queue_fail[-$i-1];
                splice(@queue_fail, -$i-1, 1);
        }

        my $nick = $item->{nick};
	
	my ($to_nick, $net) = split('@', $nick);
	my $server = which_server($net);
	
	if($server == -1) {
		debug_msg("admin_send_queue(): Server for $net not found");
		return 1;
	}

	if (send_file($item->{nick},
		      $item->{file},
		      $item->{size},
		      $item->{path})) {
	  # add data to current sends
	  $item->{checks} = 0;
	  $item->{stime} = time();
	  $item->{speed} = 0;
	  push(@queue_send, $item);
	}
        return 0;


}


#
# 	notify_channels($channels): notifies channels about the server
#				    (called every $notice_timeout seconds)
sub notify_channels
{
	my ($channels) = @_;
	my $i = 0;
        my @channels= ();
        @channels= split (/[\t ]+/, $channels);
        #IRC::print("$logo start notifying channels. ".@channels);
	while ($i < @channels) {
                IRC::print("$logo notify channel: ".$channels[$i]);
		notify_channel($channels[$i]);
                $i++;
        }
}

#
# 	notify_channel($channel): notifies channels about the server
#				  (called every $notice_timeout seconds)
sub notify_channel
{
	my ($channel) = @_;
	my @channel_list = IRC::channel_list();
	my ($to_chan, $net) = split('@', $channel);
	my $server = which_server($net);
	my $i = 0;

	# check to see that channel is joined first
	while ($i < @channel_list) {
		if (($channel_list[$i] eq $to_chan) && (lc($channel_list[$i+1]) eq lc($server))) {
			# channel available, show notice
			show_notice($channel, $server, 0);
			return;
		}
		$i += 3;
	}
}

#
#	show_notice($dest): sends server notice to channel or user
#
sub show_notice
{
	my ($destination, $server) = @_;
        my ($dest, $net) = split('@', $destination);
        my $msg = "";

        if (!$var{'short_notice'}) {
                $msg = "(FServe Online) ";
        }

        if ($var{'ctcptrigger'}) {
              $msg .= "Trigger:(/ctcp ".IRC::get_info(1)." $var{'trigger'}) ";
        } else {
              $msg .= "Trigger:($var{'trigger'}) ";
        }

        if (!$var{'short_notice'}) {
                $msg .= "Accessed:(".$access_count." times) ";

                if ($var{'min_speed'} > 0) {
                        $msg .= "Min CPS:(".size_to_string($var{'min_speed'})."/s) ";
                }
                if ($record_cps > 0) {
                        $msg .= "Record CPS:(" . size_to_string($record_cps) . "/s by $record_cps_user) ";
                }
                if ($file_count > 0) {
                        $msg .= "Snagged:(" . size_to_string($byte_count) . " in $file_count file(s)) ";
                }
        }

	$msg .= "Online:(" . scalar values(%user_status) . "/$var{'max_users'}) ";
	$msg .= "Sends:(" . @queue_send . "/$var{'max_sends'}) ";
#	$msg .= "Queues:(" . @queue_norm . "/$var{'max_queues'}) ";
	$msg .= "Queues:(" . (@queue_norm + @queue_fail) . "/$var{'max_queues'}) ";
	
        if (!$var{'short_notice'}) {
                if (length($var{'note'}) > 0) {
                        $msg .= "Note:("."$var{'note'}".") ";
                }

                $msg .= "$logo";
        }

	# add some color
#	$msg =~ s/\(/$c2\[$c1/g;
#	$msg =~ s/\)/$c2\]/g;
	$msg =~ s/\(/$c2\($c1/g;
	$msg =~ s/\)/$c2\)/g;

        my $chnldest = ($dest =~ /^\#/);
        if ($var{'raw_chnl_msg'} && $var{'notice_as_msg'} && $chnldest) {
                debug_msg("notice as raw msg to ".$dest);
                my $cmd = "PRIVMSG ".$dest." :".$c2.$msg."\r\n";
#             IRC::send_raw($cmd);
                IRC::command_with_server("/raw ".$cmd, $server);
        } elsif ($var{'raw_chnl_msg'}) {
                debug_msg("notice as raw notice to ".$dest);
                my $cmd = "NOTICE ".$dest." :".$c2.$msg."\r\n";
#             IRC::send_raw($cmd);
                IRC::command_with_server("/raw ".$cmd, $server);
        } elsif ($var{'notice_as_msg'} && $chnldest) {
                debug_msg("notice as msg to ".$dest);
                IRC::command_with_server("/msg $dest $msg", $server);
        } else {
                debug_msg("notice as notice to ".$dest);
                IRC::command_with_server("/notice $dest $msg", $server);
        }
}

#
# 	open_dcc_chat($nick): offer connection to $nick
#
sub open_dcc_chat
{
	my ($nick, $server, $ui) = @_;
	my ($to_nick, $net) = split('@', $nick);

	debug_msg("open_dcc_chat(): nick: $nick  server: $server  ui: $ui  to_nick: $to_nick  net: $net");

	if (defined $user_status{$nick}) {
		send_user_msg_with_server($nick, $server, "You are already connected");
	} elsif (scalar values(%user_status) >= $var{'max_users'}) {
		send_user_msg_with_server($nick, $server, "Too many users connected ($var{'max_users'})");
	} else {
                $user_info{$nick} = $ui;
		$user_status{$nick} = -1;	# user is connecting
		$user_online_time{$nick} = time();
		$user_server{$nick} = $server;
		IRC::command_with_server("/dcc chat $to_nick", $server);
	}
}	

#
#	change_dir($nick, $server): sets directory to $root_dir/$directory for $nick
#			   (users can never get below $root_dir by ../../)
#
sub change_dir
{
	my ($nick, $server, $directory) = @_;

	my @dir_fields = split('/', $directory); #"@args");
	my @udir_fields = ();

	# fix cd /newdir from other dir stuff
	unless (substr($directory, 0, 1) eq "/") {
		@udir_fields = split('/', $user_dir{$nick});
	}

	foreach my $dir (@dir_fields) {
		if (length($dir) <= 0 || $dir eq ".") {
			# do nothing
		} elsif ($dir eq "..") {
			# go to parent directory
			if (scalar @udir_fields > 0) {
				pop(@udir_fields);
			}
		} else {
			# append dir
			push(@udir_fields, $dir);
		}
	}
	
	# join produces an extra slash for empty fields...
	# we don't want that
	my $dir = "";
	foreach my $udir (@udir_fields) {
		if (length($udir) > 0) {
			$dir .= '/' . $udir;
		}
	}

	if (defined $file_cache{$var{'root_dir'}.$dir}) {
		$user_dir{$nick} = $dir;
		if (length($dir) == 0) {
			$dir = "/";
		}
		send_user_msg_with_server($nick, $server, "[".$c1."$dir".$c2."]");
	} elsif (!$var{'case_file'} &&
                 defined $file_cache_uc{uc($var{'root_dir'}.$dir)}) {
                $dir= find_nc_dir("",uc($dir));
                debug_msg("dir: $dir");
		$user_dir{$nick} = $dir;
		if (length($dir) == 0) {
			$dir = "/";
		}
		send_user_msg_with_server($nick, $server, "[".$c1."$dir".$c2."]");
        } else {
                if ($var{'case_file'}) {
                        send_user_msg_with_server($nick, $server, "[".$c1."$dir".$c2."] does not exist (".$c1."check case".$c2.")!");
                } else {
                        send_user_msg_with_server($nick, $server, "[".$c1."$dir".$c2."] does not exist!");
                }
	}
}

#
#	find_nc_dir($dir,@udir_fields): finds the normal case filename to $dir
#
sub find_nc_dir
{
	my ($dir,$ucdir) = @_;

        debug_msg("in dir: ".$dir."   check dir: ".$ucdir);
	# prints the directories sorted
	foreach (sort(@{@{$file_cache{$var{'root_dir'}.$dir}}[0]})) {
                debug_msg("test dir: ".$_);
		if(uc($dir."/".$_) eq $ucdir) {
                        $dir_nc= $dir."/".$_;
                        return $dir_nc;
                }
                $dir_nc= find_nc_dir($dir."/".$_,$ucdir);
                unless ($dir_nc eq "/") {
                        return $dir_nc;
                }
	}

        return "/";
}

#
#	list_dir($nick): lists files in current directory for $nick
#
sub list_dir
{
	my ($nick) = @_;

        debug_msg("list_dir: nick: ".$nick."   server: ".$server);

	unless (defined $user_dir{$nick}) {
                debug_msg("user_dir nonexistent, setting default!");
#		IRC::print("user_dir nonexistent, setting default!");
		$user_dir{$nick} = "";
	}

	my $dir = $var{'root_dir'} . $user_dir{$nick};
	my @filelist = ();

	send_user_msg($nick, "Listing [".$c1.((length($user_dir{$nick}) == 0)?"/":"$user_dir{$nick}").$c2."]");

	# prints the directories sorted
	foreach (sort(@{@{$file_cache{$dir}}[0]})) {
		send_user_msg($nick, $c3.$_.$c2."/");
	}

	# prepare the filelist
	for (my $i = 0; $i < @{@{$file_cache{$dir}}[1]}; $i++) {
		my $size = @{@{$file_cache{$dir}}[2]}[$i];
		push(@filelist, $c4.@{@{$file_cache{$dir}}[1]}[$i].$c2." (".$c1.size_to_string($size).$c2.")");
	}
	
	# prints the filenames sorted
	foreach (sort(@filelist)) {
		send_user_msg($nick, $_);
	}
	
	send_user_msg($nick, "End of list [".$c1.((length($user_dir{$nick}) == 0)?"/":"$user_dir{$nick}").$c2."]");
}

#
#	queue_file($nick, $server, $file): queues $file for $nick
#
sub queue_file
{
	my ($nick, $server, $file) = @_;
	my $duplicated = 0;
	my $file_exists = 0;
	my $size = 0;

        my $slots_small = $var{'slots_small'};
        my $slots_med = $var{'slots_med'};
        my $slots_big = $var{'slots_big'};
        my $size_med = $var{'size_med'};
        my $size_big = $var{'size_big'};

        if ($size_big < $size_med) {
                $size_big = $size_med;
                IRC::print($logo." ".$c1."Warning: ".$c2."size_big is smaller then size_med.\n");
        }
        if ($slots_med < $slots_big) {
                $slots_med = $slots_big;
                IRC::print($logo." ".$c1."Warning: ".$c2."slots_med is smaller then slots_big.\n");
        }
        if (($slots_small < $slots_big) && ($slots_small < $slots_med)) {
                $slots_small = $slots_med;
                IRC::print($logo." ".$c1."Warning: ".$c2."slots_small is smaller then slots_big and slots_med.\n");
        }
        if ($slots_small < $slots_med) {
                $slots_small = $slots_med;
                IRC::print($logo." ".$c1."Warning: ".$c2."slots_small is smaller then slots_med.\n");
        }
	# locate the file in cache
	my @files = @{@{$file_cache{$var{'root_dir'}.$user_dir{$nick}}}[1]};
	for ($i = 0; $i < @files; $i++) {
		if ($files[$i] eq $file) {
			$file_exists = 1;
			$size = ${@{$file_cache{$var{'root_dir'}.$user_dir{$nick}}}[2]}[$i];
                        last;
                }
	}
	if (!($file_exists) && !$var{'case_file'}) {
                for ($i = 0; $i < @files; $i++) {
                        if (uc($files[$i]) eq uc($file)) {
                                $file_exists = 1;
                                $size = ${@{$file_cache{$var{'root_dir'}.$user_dir{$nick}}}[2]}[$i];
			        last;
                        }
		}
	}

        if (!($file_exists) && !$var{'case_file'}) {
		send_user_msg_with_server($nick, $server, "'".$c1."$file".$c2."' is not a valid file!");
                return;
	}
        if (!($file_exists)) {
		send_user_msg_with_server($nick, $server, "'".$c1."$file".$c2."' is not a valid file (".$c1."check case".$c2.")!");
                return;
	}
        if (queued_files($nick,0) >= $slots_small) {
		send_user_msg_with_server($nick, $server, "You have filled all your queue slots (".$c1."$slots_small".$c2.")");
                return;
	}
        if ($size >= $size_med && queued_files($nick,$size_med) >= $slots_med) {
		send_user_msg_with_server($nick, $server, "You have filled your big/medium file queue slots (".$c1."$slots_med".$c2.")");
                send_user_msg_with_server($nick, $server, "You can still queue small files.");
                return;
	}
        if ($size >= $size_big && queued_files($nick,$size_big) >= $slots_big) {
		send_user_msg_with_server($nick, $server, "You have filled your big file queue slots (".$c1."$slots_big".$c2.")");
                my $tmpmed = $slots_med - queued_files($nick,$size_med);
                if ($tmpmed > 1) {
                        send_user_msg_with_server($nick, $server, "You have still (".$c1."$tmpmed".$c2.") medium file slots left.");
                } else {
                        if ($tmpmed == 1) {
                                send_user_msg_with_server($nick, $server, "You have still (".$c1."$tmpmed".$c2.") medium file slot left.");
                        }
                        my $tmpsmall = $slots_small - queued_files($nick,0);
                        if ($tmpsmall >= 1) {
                                send_user_msg_with_server($nick, $server, "You can still queue small files.");
                        }
                }
                return;
	}
        if ((@queue_norm + @queue_fail) >= $var{'max_queues'}) {
		send_user_msg_with_server($nick, $server, "All queue slots full (".$c1."$var{'max_queues'}".$c2.")");
                return;
	}
        if (count_queued($nick) >= 1) {
                for (my $slot = 0; $slot < @queue_fail; $slot++) {
                        if (compare_nick($queue_fail[$slot]->{nick},$nick) && $queue_fail[$slot]->{file} eq $file) {
                                $duplicated = 1;
                                last;
                        }
                }
                for (my $slot = 0; $slot < @queue_norm; $slot++) {
                        if (compare_nick($queue_norm[$slot]->{nick},$nick) && $queue_norm[$slot]->{file} eq $file) {
                                $duplicated = 1;
                                last;
                        }
                }
        }
        for (my $slot = 0; $slot < @queue_send; $slot++) {
                if (compare_nick($queue_send[$slot]->{nick},$nick) && $queue_send[$slot]->{file} eq $file) {
                        $duplicated = 1;
                        last;
                }
        }

        if ($duplicated == 1) {
                send_user_msg_with_server($nick, $server, "You have already queued '".$c1."$file".$c2."'!");
                return;
        }
        my %item;
        $item{nick}= $nick;
        $item{path}= $var{'root_dir'} . $user_dir{$nick} . '/';
        $item{file}= $file;
        $item{size}= $size;
        $item{fails}= 0;
        my $ui = $user_info{$nick};
        debug_msg("user_info{nick}: ".$ui);
        my $priority = ($ui & 2) * $var{'priority_op'} + ($user_info{$nick} & 4) * $var{'priority_voice'};
        if ($priority) {
                #@queue_fail = (\%item, @queue_fail);
                push(@queue_fail, \%item);
                send_user_msg_with_server($nick, $server, "Queued file in priority slot ".$c1."#".@queue_fail);
        } else {
                push(@queue_norm, \%item);
                send_user_msg_with_server($nick, $server, "Queued file in slot ".$c1."#".@queue_norm);
        }
}

#
#	queued_files($nick, $server, $size): number of queues file with size greater $size for $nick
#
sub queued_files
{
	my ($nick, $server, $size) = @_;
	my $queued = 0;

        if (count_queued($nick) >= 1) {
                for (my $slot = 0; $slot < @queue_fail; $slot++) {
                        if (compare_nick($queue_fail[$slot]->{nick},$nick) && $queue_fail[$slot]->{size} >= $size) {
                                $queued++;
                        }
                }
                for (my $slot = 0; $slot < @queue_norm; $slot++) {
                        if (compare_nick($queue_norm[$slot]->{nick},$nick) && $queue_norm[$slot]->{size} >= $size) {
                                $queued++;
                        }
                }
        }
        if ($var{'count_sends'}) {
                for (my $slot = 0; $slot < @queue_send; $slot++) {
                        if (compare_nick($queue_send[$slot]->{nick},$nick) && $queue_send[$slot]->{size}  >= $size) {
                                $queued++;
                        }
                }
        }
        return $queued;
}

#
#	dequeue_file($nick, $slot): dequeues file in $slot for $nick
#
sub dequeue_file
{
	my ($nick, $server, $slots) = @_;
        $slots =~ s/\#//g;
        my $slot = $slots;
        if ($slot > 0) {
                $slot--;
                if (defined $queue_norm[$slot]) {
                        if (compare_nick($queue_norm[$slot]->{nick},$nick)) {
                                send_user_msg_with_server($nick, $server, "Removing '".$c1.$queue_norm[$slot]->{file}.$c2."', you now have ".$c1.(count_queued($nick)-1).$c2." file(s) queued!");
                                splice(@queue_norm, $slot, 1);
                        } else {
                                send_user_msg_with_server($nick, $server, "that slot belongs to somebody else!!!");
                        }
                } else {
                        send_user_msg_with_server($nick, $server, "queue slot ".$c1."#$slot".$c2." does not exist!");
                }
        } else {
                $slot = -1-$slot;
                if (defined $queue_fail[$slot]) {
                        if (compare_nick($queue_fail[$slot]->{nick},$nick)) {
                                send_user_msg_with_server($nick, $server, "Removing '".$c1.$queue_fail[$slot]->{file}.$c2."', you now have ".$c1.(count_queued($nick)-1).$c2." file(s) queued!");
                                splice(@queue_fail, $slot, 1);
                        } else {
                                send_user_msg_with_server($nick, $server, "that slot belongs to somebody else!!!");
                        }
                } else {
                        send_user_msg_with_server($nick, $server, "queue slot ".$c1."#$slot".$c2." does not exist!");
                }
        }
}

# FIXME: add server paramter to clr_queues & dequeue

#
#	clr_queue($nick, $server): clear all files queued for $nick
#
sub clr_queue
{
	my ($nick, $server) = @_;

        if (count_queued($nick) == 0) {
		send_user_msg_with_server($nick, $server, "No files queued!");
	} else {
                my $nickcount= count_queued($nick);
		for (my $slot = scalar(@queue_norm) -1; $slot >= 0; $slot--) {
			if (compare_nick($queue_norm[$slot]->{nick},$nick)) {
				splice(@queue_norm, $slot, 1);
			}
		}
		for (my $slot = scalar(@queue_fail) -1; $slot >= 0; $slot--) {
			if (compare_nick($queue_fail[$slot]->{nick},$nick)) {
				splice(@queue_fail, $slot, 1);
			}
		}

		send_user_msg_with_server($nick, $server, "Successfully dequeued ".$c1.$nickcount.$c2." file(s)");
	}
}

#
#	 ($nick, $server, $pattern): tries to find the specified pattern
#				    case insensitive
#
sub find_pattern
{
	my ($nick, $server, $pattern) = @_;
	my $matches = 0;
	my ($to_nick, $net) = split('@', $nick);

	my $msg = $c1."FServe Search".$c2.": found ".$c1;

#	$pattern =~ s/\*//g;	# remove all *
        $pattern =~ s/^\s*|\s*$|^\**|\**$//g;
        my $orig_pattern = $pattern;
        $pattern =~ s/\*/ /g;    # replace all * with spaces
        $pattern =~ s/\s+/.+/g;  # and spaces with a match for any chars
	foreach (keys %file_cache) {
		foreach my $file (@{@{$file_cache{$_}}[1]}) {
			if ($file =~ /$pattern/i) {
				$matches += 1;
			}
		}
	}

        $msg .= $matches.$c2." matches for ".$c1."*$orig_pattern*".$c2;

        if ($var{'ctcptrigger'}) {
              $msg .= " Trigger:(".$c1."/ctcp ".IRC::get_info(1)." $var{'trigger'}) ";
        } else {
              $msg .= " Trigger:(".$c1.$var{'trigger'}.$c2.") ";
        }

	if ($matches > 0) {
                if ($var{'raw_chnl_msg'}) {
                        debug_msg("find response as raw notice to ".$nick);
                        my $cmd = "NOTICE ".$to_nick." :".$msg."\r\n";
                        IRC::send_raw($cmd);
                } else {
                        debug_msg("find response as notice to ".$nick);
                        IRC::command_with_server("/notice $to_nick $msg", $server);
                }
                #send_user_msg_with_server($nick, $msg};
	}
}


#
#	send_dead(n,s): tries to figure out if send #n is dead
#
sub send_dead
{
	my ($s) = @_;
	my @dcc_list = IRC::dcc_list();

        debug_msg("send_dead($s) dcc_list: @dcc_list");


        my $foundsend= 0;

        my $sfile = $queue_send[$s]->{file};
        $sfile =~ s/ /_/g;
        my $i = 0;
        my $active;
        debug_msg("queue_send[\$s]: $queue_send[$s]");
        while ($i < @dcc_list) {
                # check for active and waiting sends 
                # (type = 0, status = 1 || 0)
                # char *dcctypes[] = { "SEND", "RECV", "CHAT", "CHAT" };
                # struct dccstat_info dccstat[] = {
                #	{N_("Waiting"), 1 /*black */ },
                #	{N_("Active"), 12 /*cyan */ },
                #	{N_("Failed"), 4 /*red */ },
                #	{N_("Done"), 3 /*green */ },
                #	{N_("Connect"), 1 /*black */ },
                #	{N_("Aborted"), 4 /*red */ },
                #};
                $active = 0;
                if ($dcc_list[$i+2] == 0 && $dcc_list[$i+3] == 1){$active = 1;}
                if ($dcc_list[$i+2] == 0 && $dcc_list[$i+3] == 0){$active = 1;}
                if ($dcc_list[$i+2] == 0 && $dcc_list[$i+3] == 4){$active = 1;}
                if ($active == 1) {
                        # step through send slots
                        my @temp = split('/', $dcc_list[$i+1]);
                        my $file = $temp[$#temp];
                        $file =~ s/ /_/g;
                        if (compare_nick($queue_send[$s]->{nick},$dcc_list[$i]) && $sfile eq $file) {
                                $foundsend = 1;
                                $i = @dcc_list;
                        }
                }
                $i += 9;
        }
        if ($foundsend) {
                return  0;
        }
        return 1;
}

#
# 	display_path($nick, $server): shows current path to $nick
#
sub display_path
{
        my ($nick, $server) = @_;

	send_user_msg_with_server($nick, $server, $c2."Current path: [".$c1.((length($user_dir{$nick}) == 0)?"/":"$user_dir{$nick}").$c2."]");
}

#
# 	display_queues($nick, $server): shows queued files to $nick
#
sub display_queues
{
        my ($nick, $server) = @_;

	send_user_msg_with_server($nick, $server, $c1.(@queue_norm + @queue_fail)."/$var{'max_queues'}".$c2." queued file(s) in total.");

	for (my $slot = 0; $slot < @queue_norm; $slot++) {
		my $user = $queue_norm[$slot]->{nick};
		my $file = $queue_norm[$slot]->{file};
		my $size = $queue_norm[$slot]->{size};

		send_user_msg_with_server($nick, $server, $c1."#".($slot+1).$c2.": ".$c1."$user".$c2." queued ".$c1."$file".$c2." (".$c1.size_to_string($size).$c2.")");
	}
}

#
# 	display_fail_queues($nick, $server): shows queued failed files to $nick
#
sub display_fail_queues
{
        my ($nick, $server) = @_;

	send_user_msg_with_server($nick, $server, $c1.@queue_fail."/$var{'max_queues'}".$c2." file(s) in the fail/priority queues.");

	for (my $slot = 0; $slot < @queue_fail; $slot++) {
		my $user = $queue_fail[$slot]->{nick};
		my $file = $queue_fail[$slot]->{file};
		my $size = $queue_fail[$slot]->{size};
		my $fails = $queue_fail[$slot]->{fails};

		send_user_msg_with_server($nick, $server, $c1."#".(-$slot-1).$c2.": ".$c1."$user".$c2." queued ".$c1."$file".$c2." (".$c1.size_to_string($size).$c2.") with ".$c1.$fails."/".$var{'max_fails'}.$c2." fails.");
	}
}

#
#	display_sends($nick, $server): shows current sends to $nick
#
sub display_sends
{
        my ($nick, $server) = @_;
	my @dcc_list = IRC::dcc_list();

	if (@queue_send <= 0) {
		send_user_msg_with_server($nick, $server, "No sends in progress!");
		return;
	}

	my $countsends = 0;
	send_user_msg_with_server($nick, $server, $c2."Currently sending ".$c1.@queue_send."/$var{'max_sends'}".$c2." files!");
        debug_msg("dcc_list: @dcc_list");

	for (my $s = 0; $s < @queue_send; $s++) {
		my $sfile = $queue_send[$s]->{file};
                my $ssize = $queue_send[$s]->{size};
		$sfile =~ s/ /_/g;
		my $i = 0;
                debug_msg("queue_send[$s]: $queue_send[$s]");
		while ($i < @dcc_list) {
			# check for active and waiting sends (type = 0, status = 1 || 0)
			if ($dcc_list[$i+2] == 0 && ($dcc_list[$i+3] == 1 || $dcc_list[$i+3] == 0)) {
				# step through send slots
				my @temp = split('/', $dcc_list[$i+1]);
				my $file = $temp[$#temp];
                                $file =~ s/ /_/g;
				if (compare_nick($queue_send[$s]->{nick},$dcc_list[$i]) && $sfile eq $file) {
					if ($dcc_list[$i+3] == 1) {
#                                                my $guesspos = $dcc_list[$i+4] * (time() - $queue_send[$s]->{stime}) + $dcc_list[$i+6];
#                                                my $guesspos = $dcc_list[$i+4] * (time() - $queue_send[$s]->{stime}) + $queue_send[$s]->{resumepos};
                                                my $guesspos = $dcc_list[$i+6];
                                                my $percent = 100;
                                                if ($dcc_list[$i+5] != 0) {
                                                        $percent = ($guesspos/$dcc_list[$i+5])*100;
                                                }
                                                my $time_left = 999999;
                                                if ($dcc_list[$i+4]>=1) {
                                                        $time_left = ($dcc_list[$i+5]-$guesspos)/$dcc_list[$i+4];
                                                }
                                                if ($percent > 99.99) {
                                                        $percent = 99.99;
                                                }
                                                if ($time_left < 5) {
                                                        $time_left = 5;
                                                }
						my $spercent = sprintf("%.2f%%", $percent);
						my $stime_left = sprintf("%d", $time_left);
						send_user_msg_with_server($nick, $server, $c1."#".($s+1).$c2.": Sending ".$c1."$file".$c2." (".$c1.size_to_string($dcc_list[$i+5]).$c2.") to ".$c1."$dcc_list[$i]".$c2." at ".$c1.size_to_string($dcc_list[$i+4])."/s".$c2." (".$c1."$spercent".$c2." done, ".$c1.time_to_string($stime_left).$c2." left)");
					} else {
						send_user_msg_with_server($nick, $server, $c1."#".($s+1).$c2.": Waiting for ".$c1."$dcc_list[$i]".$c2." to accept ".$c1."$file".$c2." (".$c1.size_to_string($dcc_list[$i+5]).$c2.")");
					}

					$i = @dcc_list;
					next;
				}
			}
			$i += 9;
		}
	}
}

#
#	display_who($nick, $server): shows online users to $nick
#
sub display_who
{
        my ($nick, $server) = @_;

	send_user_msg_with_server($nick, $server, $c1.(scalar keys(%user_status)).$c2." user(s) online!");
	foreach (keys(%user_status)) {
		if ($user_status{$_} == -1) {
			send_user_msg_with_server($nick, $server, "$c2   user ".$c1."$_".$c2." is connecting...");
		} else {
                        send_user_msg_with_server($nick, $server, "$c2   user ".$c1."$_".$c2." online for ".$c1.(time() - $user_online_time{$_}).$c2."s, idle: ".$c1.(time() - $user_status{$_}).$c2."s");	
		}
	}
}

#
#	display_stats($nick, $server): shows server information to $nick
#
sub display_stats
{
        my ($nick, $server) = @_;

	send_user_msg_with_server($nick, $server, "Fileserver statistics");
	send_user_msg_with_server($nick, $server, "=====================");
	send_user_msg_with_server($nick, $server, "Online for ".$c1.time_to_string($online_time));
	send_user_msg_with_server($nick, $server, "Logins: ".$c1.$access_count);
	send_user_msg_with_server($nick, $server, " ");
	send_user_msg_with_server($nick, $server, "Users online: ".$c1.scalar values(%user_status));
	send_user_msg_with_server($nick, $server, "Send slots: ".$c1.@queue_send."/$var{'max_sends'}");
	send_user_msg_with_server($nick, $server, "Queue slots: ".$c1.(@queue_norm + @queue_fail)."/$var{'max_queues'}");
	send_user_msg_with_server($nick, $server, " ");
	send_user_msg_with_server($nick, $server, "Files sent: ".$c1.$file_count);
	send_user_msg_with_server($nick, $server, "Bytes sent: ".$c1.size_to_string($byte_count));
	send_user_msg_with_server($nick, $server, "Record CPS: ".$c1.size_to_string($record_cps)."/s");
	send_user_msg_with_server($nick, $server, "Sends failed: ".$c1.$send_fails);
}



#
#	send_next_file(): sends the next file in queue
#			  also prevents a user from having multiple sends
#			  active at the same time
#
sub send_next_file
{
	my ($failqueue) = @_;
	my $nick = "";
	my $file = "";
	my $size = "";
	my $path = "";
	my $server = "";
        my $to_nick = "";
        my $net = "";
        my $item;
        debug_msg("send_next_file: " .$failqueue);
        my @queue;
        if ($failqueue == 0) {
                @queue = @queue_norm;
        } else {
                @queue = @queue_fail;
        }
	# step through the filequeue
	for ($i = 0; $i < @queue; $i++) {
		$item = $queue[$i];
                $nick = $item->{nick};
		my $send_active = send_active_for($nick);

		# remove entry if user isn't in channel
#		if (!($in_channel)) {
#			splice(@$queue, $i, 1);
#                       $i -= 1; # the slot was removed, so next slot
#                       next;	 # will have the same index...
#               }
                # Check next slot if user has a running transfer
                 next if ($send_active);

		($to_nick, $net) = split('@', $nick);
		$server = which_server($net);
		next if ($server == -1);

                $file = $item->{file};
                $size = $item->{size};
                $path = $item->{path};
                if (defined $item->{ftime}) {
                        $time = $item->{ftime};
                } else {
                        $time = 0;
                }
                $item->{checks} = 0;
                # check if slots are used up
		# in previous if statement
                # debug_msg("file: " .$file." size ".$size." path ".$path." time ".$time." nick ".$nick);
		if (length($file) > 0) {
                        if ($time + 120 > time()) {
                                # failtime is not up yet. Try later.
                                # debug_msg("skip_file: " .$time." ".time());
                                $file = "";
                                next;
                        }
                        if (((@queue_send) >= $var{'max_sends'}) &&
                            ((! $var{'send_small_now'}) ||
                             ($size > $var{'size_med'}))) {
                                $file = "";
                                #debug_msg("All sends used and not a small file.");
                                next;    # All sends used and not a small file
                        }
                        if ($failqueue == 0) {
                                splice(@queue_norm, $i, 1);
                        } else {
                                splice(@queue_fail, $i, 1);
                        }
                        last;     # if data found, then we are done
		}
	}
        # debug_msg("file: $file");

	if (length($file) <= 0) {
		return 1;
	}


	if (send_file($item->{nick},
		      $item->{file},
		      $item->{size},
		      $item->{path})) {
	  # add data to current sends
	  $item->{stime} = time();
	  push(@queue_send, $item);
	}
	return 0;
}




#
#       send_file()
#       new send should all be started from the same procedure.
#
sub send_file
{
	my ($nick,$file,$size,$path) = @_;

	my ($to_nick, $net) = split('@', $nick);
	my $server = which_server($net);
	if ($server == -1) {return 0;}
 	my $full_path = $path . $file;
	# check that the file really exist (again)
	if (!(-e $full_path) || !(-f $full_path)) {
                $item->{fails} = $var{'max_fails'};
		remove_from_send_slot($nick, $file, 1);
		return 0;
	}
        # debug_msg("file: $file");

        my $listen_channel= $var{'serve_hidden'}." ".$var{'serve_no_notify'}." ".$var{'notify_to_voice'}." ".$var{'notify_channels'};
        my $channellist= user_is_in_channels($nick, $server, $listen_channel);
        debug_msg("$nick ($to_nick on $net) is in $channellist");
        if ($channellist eq "") {
                return 0;
        }
        # debug_msg("file: $file");

	IRC::command_with_server("/notice $to_nick ".$c2."Sending you your queued file (".$c1.size_to_string($size).$c2.")", $server);
	IRC::command_with_server("/dcc send $to_nick \"$full_path\"", $server);

	return 1;
}


#
#	remove_from_send_slot($nick, $file, $nofail): removes an entry from send slots
#       if nofail is set no attempt to requeue the file will be made regardless of
#       the number of failed sends.
#
sub remove_from_send_slot
{
	my ($nick, $file, $nofail) = @_;
	$file =~ s/ /_/g;	# replace ' ' with '_'

        my $item;
	for (my $i = 0; $i < @queue_send; $i++) {
		my $sfile = $queue_send[$i]->{file};
		$sfile =~ s/ /_/g;	# replace ' ' with '_'
                my $item = $queue_send[$i];
		if (compare_nick($item->{nick},$nick) 
                    && $sfile eq $file) {
			my $size = $item->{size};
			splice(@queue_send, $i, 1);

                        if (($nofail == 0)
                            && ($item->{fails} < $var{'max_fails'})) {
                                $item->{fails}++;
                                $item->{ftime}=time();
                                push(@queue_fail, $item);
                        }
			debug_msg("sends left: " . scalar(@queue_send));
			debug_msg("fail_queue: " . scalar(@queue_fail));
			# return the filesize
			return $size;
		}
	}
	# failure, no match
	return -1;
}


#
#	send_user_msg_with_server($nick, $msg): sends user a message 
#				    (using DCC if available)
#
sub send_user_msg_with_server
{
	my ($nick, $server, $msg) = @_;
	my $cmd;
	my ($to_nick, $net) = split('@', $nick);
	my $server2 = which_server($net);
        debug_msg("send_user_msg_with_server: nick: ".$nick." server: ".$server." server2: ".$server2." msg: ".$msg);
	if ($nick eq "obsidian server") {
		IRC::print($logo." ".$c2.$msg."\n");
	} else {
		# if user connected, send with dcc
		if (defined $user_status{$nick} && $user_status{$nick} >= 0) {
			$cmd = "/msg =".$to_nick . ' ' . $c2 . $msg;
                        debug_msg("IRC::command_with_server($cmd, $server);");
                        IRC::command_with_server($cmd, $server);
		} else {
			$cmd = "/msg ".$to_nick . ' ' . $c2 . $msg;
                        debug_msg("IRC::command_with_server($cmd, $server);");
                        IRC::command_with_server($cmd, $server);
                }
	}
}


#
#	send_user_msg_with_server($nick, $msg): sends user a message 
#				    (using DCC if available)
#
sub send_user_msg
{
	my ($nick, $msg) = @_;
	my $cmd;
	my ($to_nick, $net) = split('@', $nick);
	my $server = which_server($net);
        debug_msg("send_user_msg: nick: ".$nick." server: ".$server." msg: ".$msg);
	if ($nick eq "obsidian server") {
		IRC::print($logo." ".$c2.$msg."\n");
	} else {
		# if user connected, send with dcc
		if (defined $user_status{$nick} && $user_status{$nick} >= 0) {
			$cmd = "/msg =".$to_nick . ' ' . $c2 . $msg;
                        debug_msg("IRC::command_with_server($cmd, $server);");
                        IRC::command_with_server($cmd, $server);
		} else {
			$cmd = "/msg ".$to_nick . ' ' . $c2 . $msg;
                        debug_msg("IRC::command_with_server($cmd, $server);");
                        IRC::command_with_server($cmd, $server);
                }
	}
}


#
#	send_user_raw_msg($nick, $msg): sends user a message
#                  that can't be seen on Your screen
#
sub send_user_raw_msg
{
	my ($nick, $server, $msg) = @_;
	my $cmd = "PRIVMSG ".$nick." :".$c2.$msg."\r\n";
        debug_msg("send user raw msg $cmd");
        IRC::command_with_server("/raw ".$cmd, $server);
#        IRC::send_raw($cmd);
}


#
#	send_active_for($nick): returns 1 if sending file(s) to $nick
#				otherwise returns 0
#
sub send_active_for
{
	my ($nick) = @_;
	
	foreach (@queue_send) {
		if (compare_nick($nick,$_->{nick})) {
			return 1;
		}
	}

	return 0;
}


#
#	user_info($nick, $channels): returns 1 if $nick is in any of
#                                    the $channels and 0 otherwise
#
sub user_info
{
	my ($nick, $channels) = @_;
	my $i = 0;
        my $ui = 0;
        my $found = 0;
        my $op = 0;
        my $voice = 0;
        my @channels= ();
        @channels= split (/[\t ]+/, $channels);
	while ($i < @channels) {
		$ui = user_is_in_channel($nick,$channels[$i]);
                if ($ui) {
                        # found user in channel
                        $found = 1;
                }
                if (($ui & 2) == 2) {
                        $op = 1;
                }
                if (($ui & 4) == 4) {
                        $voice = 1;
                }

                $i++;
        }
        return $found+2*$op+4*$voice;
}


#
#	user_is_in_channels($nick, $channels): returns 1 if $nick is in any of
#                                              the $channels and 0 otherwise
#
sub user_is_in_channels
{
	my ($nick, $server, $channels) = @_;
	my ($nickname, $net) = split('@', $nick);

        debug_msg("user_is_in_channels: $nick: $nickname $net - $channels");

	my $i = 0;
        my $found = 0;
        my $channellist = "";
        my @channels = chans_on_net($net, split (/[\t ]+/, $channels));
#	debug_msg("user_is_in_channels: ".$channels." ".@channels);
	while ($i < @channels) {
		if (user_is_in_channel($nick, $server, $channels[$i])) {
                        # found user in channel
                        $found= 1;
                        $channellist = $channellist." ".$channels[$i];
                }
                $i++;
        }
        return $channellist;
}


#
#	user_is_in_channel($nick, $channel): returns 1 if $nick is in $channel
#					     and 0 otherwise
#
sub user_is_in_channel
{
	my ($nick, $server, $channel) = @_;
#	my $server = IRC::get_info(3);
	my ($nickname, $net) = split('@', $nick);
	my ($aktchan, $net1) = split('@', $channel);

#	debug_msg("user_is_in_channel: Splitted Values $aktchan $net1 - $nickname $net");

#	my $server = which_server($net);
	my @channel_list = ();
	my @aktchanlist = IRC::channel_list();
	my $i = 0;
	
	for($i = 0; $i <= @aktchanlist; $i += 3) {
		if(($aktchanlist[$i] =~ /#.*/) && (lc($aktchanlist[$i+1]) eq lc($server))) {
			@channel_list = (@channel_list, $aktchanlist[$i]);
		}
	}
	
	$i = 0;
	
#	debug_msg("user_is_in_channel: $nick: Channels to check $aktchan - @channel_list");

	while ($i < @channel_list) {
#		debug_msg("user_is_in_channel: $nick: Checking Channel: $channel_list[$i], looking for $aktchan");
		if (lc($aktchan) eq lc($channel_list[$i])) {
#			debug_msg("user_is_in_channel: $nick: Channel found:  $aktchan $channel_list[$i]");
#			debug_msg("user_is_in_channel: Checking userlist for $aktchan on Server: $server");
			my @user_list = IRC::user_list($aktchan, $server);
#			if(@user_list == 0) {debug_msg("user_is_in_channel: Userlist empty");}
			my $u = 0;
#			debug_msg("user_is_in_channel: Userlist $aktchan on Server: $server");

			while ($u < @user_list) {
				if (lc($user_list[$u]) eq lc($nickname)) {
#					debug_msg("user_is_in_channel: $user_list[$u]");
                                        my $op = $user_list[$u+2];
                                        my $voice = $user_list[$u+3];
					return 1 + 2*$op + 4*$voice;
				}
				$u += 5;
			}
		}
		$i++;
	}

	return 0;
}


#
#	check_send_speed($slot): check that send speed for $slot
#				 is greater than min_speed
#				 modified by patrick:
#				 returns speed of the send
#
sub check_send_speed
{
	my ($slot) = @_;
	my @dcc_list = IRC::dcc_list();
	my $i = 0;

        my $sfile = $queue_send[$slot]->{file};
	$sfile =~ s/ /_/g;	# replace ' ' with '_ '

        my $snick = $queue_send[$slot]->{nick};
	my ($sto_nick, $net) = split('@', $snick);

	while ($i < @dcc_list) {
		my @temp = split('/', $dcc_list[$i+1]);
		my $dcc_file = $temp[$#temp];

		# check speed for sending files... (type = 0)
		if ($dcc_list[$i+2] == 0) {
                        $dcc_file =~ s/ /_/g;	# replace ' ' with '_'
			if (compare_nick($dcc_list[$i],$sto_nick)
                            && ($dcc_file eq $sfile)) {
				# found matching DCC send, check speed
				if ($dcc_list[$i+3] == 0) {
					# always return 1 for waiting filetransfers...
					debug_msg("waiting send okay, returning speed 0\n");
					return 0;
				} elsif ($dcc_list[$i+3] == 1) {
					# speed is satisfactory...
					debug_msg("speed: $dcc_list[$i+4] is $dcc_list[$i+4], send okay\n");
					return $dcc_list[$i+4];
				} else {
					debug_msg("speed: $dcc_list[$i+4] is in unknown state, assuming 0\n");
#					my $server = which_server($net);
#					IRC::command_witch_server("/notice $dcc_list[$i] ".$c2."Send was aborted because of an unknown reason.", $server);
#					IRC::command_witch_server("/dcc close send $dcc_list[$i] $dcc_file", $server);
					return 0;
				}
			}
                }
		$i += 9;
	}

	# if we get here, which shouldn't happen, remove the send from slot
	# and return -1
        debug_msg("check_send_speed reachend end of function\n");
        debug_msg("dcc_file ".$dcc_file);
        debug_msg("sfile ".$sfile);
        debug_msg("snick".$snick);
        $i = 0;
	while ($i < @dcc_list) {
		my @temp = split('/', $dcc_list[$i+1]);
		my $dcc_file = $temp[$#temp];

		# check speed for sending files... (type = 0)
		if ($dcc_list[$i+2] == 0) {
                        $dcc_file =~ s/ /_/g;	# replace ' ' with '_'
                        debug_msg("dcc_file[i+1] ".$dcc_file);
                        debug_msg("dcc_file[i] ".$dcc_list[$i]);
                }
                $i += 9;
        }
	debug_msg("remove ghost send $slot of: ".$queue_send[$slot]->{nick}."  send_files[$slot]: ".$queue_send[$slot]->{file});
	remove_from_send_slot($queue_send[$slot]->{nick}, $queue_send[$slot]->{file}, 0);
	return 0;
}


#
#	initialize_user_vars($nick): initialize $nick's variables and send
#				     welcome message
#
sub initialize_user_vars
{
	my ($nick, $server) = @_;

	debug_msg("initializing_vars: nick: $nick  server: $server");

        $user_status{$nick} = time();
	$user_online_time{$nick} = time();
        $user_idle_warning{$nick} = 0;
	$user_dir{$nick} = "";
	$user_server{$nick} = $server;
	#$user_info{$nick} = 0;

        show_welcome($nick, $server);

}

#
#	close_user($nick): close obsidian session for user
#
sub close_user
{
	my ($user, $server) = @_;
 	debug_msg("close user: $user");
	my ($nick, $net) = split('@', $user);

	my ($user) = @_;
	debug_msg("close user: $user");

#   thansen code debug
	my @dcc_list = IRC::dcc_list();
        debug_msg ("close_user: dcc_list before /dcc close chat $nick on $server\n");
        debug_msg ("@dcc_list");
#  end additions
#
#        IRC::command("/dcc close chat $user");
        IRC::command_with_server("/dcc close chat $nick", $server);

#   thansen code debug
	@dcc_list = IRC::dcc_list();
	debug_msg ("close_user: dcc_list after /dcc close chat $nick on $server\n");
	debug_msg ("@dcc_list");
#  end additions

        if ($var{'close_tab_auto'}) {
		debug_msg ("close_user debug:  /close $nick\n");
		IRC::command_with_channel("/close", $nick);
#		IRC::command_with_channel("/close", $nick);
	}
        free_user_vars($user);
	debug_msg ("close_user:  exit\n");
}

#
#	free_user_vars($nick): free variables when user disconnects
#
sub free_user_vars
{
	my ($nick) = @_;

	# free variables specific to user
	delete ($user_dir{$nick});
	delete ($user_info{$nick});
	delete ($user_status{$nick});
	delete ($user_online_time{$nick});
        delete ($user_idle_warning{$nick});
        delete ($user_server{$nick});

	debug_msg("free_user_vars(): nick: $nick users left: " . scalar values(%user_status));
}

#
#	size_to_string($size): returns formatted string containing $size
#
sub size_to_string
{
	my ($size) = @_;

	if ($size < 1000) {
		return "$size B";
	} elsif($size < 1024000) {
		$size = sprintf("%.2f kB", $size/1024);
		return $size; 
	} elsif($size < 1048576000) {
		$size = sprintf("%.2f MB", $size/1048576);
		return $size;
	} else {
		$size = sprintf("%.2f GB", $size/1073741824);
		return $size;
	}
}

#
#	time_to_string($time_in_seconds): returns a formatted time string
#
sub time_to_string
{
	my ($time) = @_;
	my $str;

	my $hours = sprintf("%d", $time/3600);
	$time -= $hours*3600;
	my $minutes = sprintf("%d", $time/60);
	$time -= $minutes*60;
	
	if ($hours > 0) {
		$str = sprintf("%dh %dm %ds", $hours, $minutes, $time);
	} elsif ($minutes > 0) {
		$str = sprintf("%dm %ds", $minutes, $time);
	} else {
		$str = sprintf("%ds", $time);
	}

	return $str;
}

#
#	saveq(): saves the sends and queue to $conf_dir/obsidian.queue
#
sub saveq
{
	my $queue_file = $conf_dir . "/obsidian.queue";
	my $backup_file = $conf_dir . "/obsidian.bqueue";

        if (!open(FILE1,'<'.$queue_file)) {
		IRC::print("$logo Could not open $queue_file for reading...");
	} else {
                if (!open(FILE2, '>'.$backup_file)) {
                        IRC::print("$logo Could not open $backup_file for writing...");
                        return;
                }
                while (<FILE1>) {
                        print FILE2 $_;
                }
                close (FILE1);
                close (FILE2);
	}
	if (!open(FILE, '>'.$queue_file)) {
		IRC::print("$logo Could not open $queue_file for writing...");
		return;
	}

	# save the fail queue to file
	for (my $i = 0; $i < @queue_fail; $i++) {
		print(FILE "$queue_fail[$i]->{nick}".":::");
		print(FILE "$queue_fail[$i]->{path}".":::");
		print(FILE "$queue_fail[$i]->{file}".":::");
		print(FILE "$queue_fail[$i]->{size}".":::");
		print(FILE "$queue_fail[$i]->{fails}".":::"."\n");
	}

	# save the sends to file	
	for (my $i = 0; $i < @queue_send; $i++) {
		print(FILE "$queue_send[$i]->{nick}".":::");
		print(FILE "$queue_send[$i]->{path}".":::");
		print(FILE "$queue_send[$i]->{file}".":::");
		print(FILE "$queue_send[$i]->{size}".":::");
		print(FILE "$queue_send[$i]->{fails}".":::"."\n");
	}


	# save the queue to file
	for (my $i = 0; $i < @queue_norm; $i++) {
		print(FILE "$queue_norm[$i]->{nick}".":::");
		print(FILE "$queue_norm[$i]->{path}".":::");
		print(FILE "$queue_norm[$i]->{file}".":::");
		print(FILE "$queue_norm[$i]->{size}".":::"."\n");
	}

	IRC::print("$logo Sends/queue saved to file...\n");
	close(FILE);
}

#
#	load_backup(): loads the sends and queue from $conf_dir/obsidian.queue
#
sub loadq
{
        my ($location) = @_;
	my $queue_file = $conf_dir . "/$location";

	if (@queue_norm > 0) {
		IRC::print("$logo The queue is not empty, use clr_queues to clear it first!\n");
		return;
	}

	if (!open(FILE, '<'.$queue_file)) {
		IRC::print("$logo Could not open $queue_file for reading...");
		return;
	}

        while(<FILE>)
        {
                my $buf = $_;
                chop $buf;
                my @fields = split(':::', $buf);

                # restore the old sends & queue
                while (@fields >= 4) {
                        my %item;
                        $item{nick}= shift(@fields);
                        $item{path}= shift(@fields);
                        $item{file}= shift(@fields);
                        $item{size}= shift(@fields);
                        my $tmp= shift(@fields);
                        if ($tmp >= 1) {
                                $item{fails}= $tmp;
                                push(@queue_fail, \%item);
                        } else {
                                push(@queue_norm, \%item);
                        }
                }
        }

	IRC::print("$logo Loaded sends/queue from file...\n");
	close(FILE);
}

#
#	clear_queue(): clears the entire queue
#
sub clear_queue
{
	while (@queue_norm) {
                shift(@queue_norm);
	}

	IRC::print("$logo Queue cleared...");
}

#
#	clear_fqueue(): clears the entire queue
#
sub clear_fqueue
{
	while (@queue_fail) {
                shift(@queue_fail);
	}

	IRC::print("$logo Fail/priority queue cleared...");
}

#
#	move_slot($from, $to):  removes the queueslot from pos $from and
#				inserts it at pos $to
#
sub move_a_to_b
{
        my ($as, $bs)= @_;
        $as =~ s/\#//g;
        $bs =~ s/\#//g;
        my $a = $as;
        my $b = $bs;
        if ($a == 0 || $a > @queue_norm || -$a > @queue_fail) {
                return;
        }
        if ($b == 0 || $b > @queue_norm + ($a < 0) || -$b > @queue_fail + ($a > 0)) {
                return;
        }
        IRC::print("$logo Move queued file #".$a." to #".$b);
        #cut item
        my $item;
        if ($a > 0) {
                $item= @queue_norm[$a-1];
                splice(@queue_norm, $a-1, 1);

        } else {
                $item= @queue_fail[-$a-1];
                splice(@queue_fail, -$a-1, 1);
        }
        #paste item
        if ($b > 0) {
                splice(@queue_norm, $b-1, 0, ($item));
        } else {
                splice(@queue_fail, -$b-1, 0, ($item));
        }
}

#
#	clr_slot_a($slot):  removes the queueslot from pos $from
#
sub clr_slot_a
{
	my ($slot) = @_;
        my $orig = $slot;
        if ($slot > 0) {
                $slot--;
                if (defined $queue_norm[$slot]) {
                        my $item = $queue_norm[$slot];
                        splice(@queue_norm, $slot, 1);
                        IRC::print("Removing '".$c1.$item->{file}.$c2."', ".$c1.$item->{nick}.$c2." has now ".$c1.count_queued($item->{nick}).$c2." file(s) queued!");
                } else {
                        IRC::print("queue slot ".$c1."#$orig".$c2." does not exist!");
                }
        } else {
                $slot = -1-$slot;
                if (defined $queue_fail[$slot]) {
                        my $item = $queue_fail[$slot];
                        splice(@queue_fail, $slot, 1);
                        IRC::print("Removing '".$c1.$item->{file}.$c2."', you now have ".$c1.count_queued($item->{nick}).$c2." file(s) queued!");
                } else {
                        IRC::print("fail/priority queue slot ".$c1."#$orig".$c2." does not exist!");
                }
        }
}

#
#	count_queued($nick):  count files queued by $nick
#
sub count_queued
{
	my ($nick) = @_;
        my $count = 0;
        foreach (@queue_norm,@queue_fail) {

                if (compare_nick($nick,$_->{nick})) {
                        $count++;
                }
        }
        return $count;
}

#
#	compare_nick($nick1,$nick2):  compare nicks
#
sub compare_nick
{
	my ($nick1,$nick2) = @_;
	my @temp = split('@', $nick1);
	my @temp1 = split('@', $nick2);
	if((@temp == 2) && (@temp1 == 2)) {
		if (lc($nick1) eq lc($nick2)) { return 1; };
	} else {
		if (lc($temp[0]) eq lc($temp1[0])) { return 1; };
	}
        return 0;
}

#
#      check_ignore($hostname): makes sure to ignore users in xchat ignore list
#
sub check_ignore
{
        my ($hostname) = @_;
        my (@ignore_list) = IRC::ignore_list;

#       debug_msg("check_ignore($host) ignore_list: @ignore_list");

        for ($i=0;$i < @ignore_list;$i++) {
                if (!@ignore_list[$i]) {
                        #no host, skip this
                } else {
                        $reg_hostname = quotemeta($hostname);
                        $reg_hostname =~ s/\\\*/.+/g;
                        $reg_hostname = ".+$reg_hostname.+";
                        $reg_ignore_host = quotemeta(@ignore_list[$i]);
                        $reg_ignore_host =~ s/\\\*/.+/g;
                        if ($reg_hostname =~ /^$reg_ignore_host$/i && @ignore_list[$i+6] eq 0) {
                                if (@ignore_list[$i+1] eq 1 && @ignore_list[$i+3] eq 1) {
                                        #ctcp/priv+hostmask matches, ignore
                                        debug_msg("check_ignore has found a user to ignore.");
                                        return 1;
                                }
                        }
                }
                $i = $i+7; #skip to next hostmask
        }
}


#############################################################################
#
#  Multiserver Environment
#
#############################################################################

sub which_aliases {
	my $net = shift(@_);
#	debug_msg("which_aliases: $var{'netaliases'}");
	@aliases = split(" ", $var{'netaliases'});
#	debug_msg("which_aliases: @aliases");
	my $k = "";
	foreach $k (@aliases) {
		@temp = split(":", $k);
#		debug_msg("which_aliases: @temp");
		my $l = "";
		foreach $l (@temp) {
			if (lc($l) eq lc($net)) {
			debug_msg("which_aliases: returning: $temp[0]");
			return $temp[0];
			}
		}
	}
	debug_msg("which_aliases: returning: $net");
	return ($net);
}

sub which_net {
	my $server = shift(@_);
	my @temp = split(/\./, $server);
	my $net = $temp[$#temp - 1] . "." . $temp[$#temp];
	return $net;
}


sub which_server {
	my $net = shift(@_);
	my @aliases = which_aliases($net);
	my @serverlist = IRC::server_list();
	my $i = '';
	foreach $i (@serverlist) {
		my $k = "";
		foreach $k (@aliases) {
			if($i =~ /.*$k/i) {return $i;}
		}
	}
	return -1;
}

sub get_nickshort {
	my @temp = split('@', @_);
        return $temp[0];
}

sub get_nicknet {
	my @temp = split('@', @_);
        return $temp[1];
}

#
# chans_on_net(net): returns a list with channels on a network
#
sub chans_on_net {
	my $net = shift(@_);
	my @chanlist = @_;
	my @aliases = which_aliases($net);
        push(@aliases,"");
#	debug_msg("chans_on_net: $net  chanlist: @chanlist  aliases: @aliases");
	my @channels = ();
	foreach $i (@chanlist) {
		my @temp = split('@', $i);
		foreach $k (@aliases) {
			if(lc($temp[1]) eq lc($k)) {
				@channels = (@channels, $i);
			}
		}
	}
#	debug_msg("Channnel-list for $net: @channels");
	return @channels;
}



#############################################################################
#
#  Load/Save Status and general outer world comunication
#
#############################################################################

#
#	save_settings(): saves server settings (duh!)
#
sub save_settings
{
        my $config_file = $conf_dir . "/obsidian.conf";
	if (!open(FILE, '>'.$config_file.".new")) {
		IRC::print("$logo Could not open $config_file.new for writing...");
		return;
	}
	
        print(FILE "debug:$var{'debug'}\n");
        print(FILE "ctcptrigger:$var{'ctcptrigger'}\n");
        print(FILE "restorequeues:$var{'restorequeues'}\n");
        print(FILE "autoon:$var{'autoon'}\n");
        print(FILE "notice_as_msg:$var{'notice_as_msg'}\n");
        print(FILE "short_notice:$var{'short_notice'}\n");
        print(FILE "raw_chnl_msg:$var{'raw_chnl_msg'}\n");
        print(FILE "raw_wlcm_msg:$var{'raw_wlcm_msg'}\n");
        print(FILE "open_tab_early:$var{'open_tab_early'}\n");
        print(FILE "close_tab_auto:$var{'close_tab_auto'}\n");
        print(FILE "case_cmd:$var{'case_cmd'}\n");
        print(FILE "case_file:$var{'case_file'}\n");
        print(FILE "fs_sensitive:$var{'fs_sensitive'}\n");
        print(FILE "count_sends:$var{'count_sends'}\n");
        print(FILE "send_small_now:$var{'send_small_now'}\n");
        print(FILE "ads_when_full:$var{'ads_when_full'}\n");
        print(FILE "priority_op:$var{'priority_op'}\n");
        print(FILE "priority_voice:$var{'priority_voice'}\n");

        print(FILE "max_users:$var{'max_users'}\n");
        print(FILE "max_sends:$var{'max_sends'}\n");
        print(FILE "max_queues:$var{'max_queues'}\n");
        print(FILE "slots_big:$var{'slots_big'}\n");
        print(FILE "slots_med:$var{'slots_med'}\n");
        print(FILE "slots_small:$var{'slots_small'}\n");
        print(FILE "size_big:$var{'size_big'}\n");
        print(FILE "size_med:$var{'size_med'}\n");
        print(FILE "max_fails:$var{'max_fails'}\n");
        print(FILE "min_speed:$var{'min_speed'}\n");
        print(FILE "idle_time:$var{'idle_time'}\n");
        print(FILE "trigger:$var{'trigger'}\n");
        print(FILE "serve_no_notify:$var{'serve_no_notify'}\n");
        print(FILE "serve_hidden:$var{'serve_hidden'}\n");
        print(FILE "notify_to_voice:$var{'notify_to_voice'}\n");
        print(FILE "notify_channels:$var{'notify_channels'}\n");
        print(FILE "notify_timeout:$var{'notify_timeout'}\n");
        print(FILE "netaliases:$var{'netaliases'}\n");
        print(FILE "root_dir:$var{'root_dir'}\n");
        print(FILE "note:$var{'note'}\n");
        print(FILE "auto_backup:$var{'auto_backup'}\n");
        print(FILE "record_cps:$record_cps\n");
        print(FILE "record_cps_user:$record_cps_user\n");
        print(FILE "file_count:$file_count\n");
        print(FILE "byte_count:$byte_count\n");
        print(FILE "access_count:$access_count\n");
        print(FILE "send_fails:$send_fails\n");
        print(FILE "logo:$logo\n");
        print(FILE "c1:$c1\n");
        print(FILE "c2:$c2\n");
        print(FILE "c3:$c3\n");
        print(FILE "c4:$c4\n");

	IRC::print("$logo Config file saved...");		
	close(FILE);
        rename "$config_file.new", "$config_file";
        debug_msg("save_settings() done.");
        return 1;
}

#
#	load_settings(): pretty obvious
#
sub load_settings
{
        my $config_file = $conf_dir . "/obsidian.conf";
	if (!open(FILE, '<'.$config_file)) {
		IRC::print("$logo Could not open $config_file for reading...");
		return;
	}

        my @allkeys = "";
        while(<FILE>)
        {
                if ($_ =~ /^\s.*$|^\#.*$/)
                {
                        next;
                }
                elsif (!($_ =~ /^\s.*$|^\#.*$/))
                {
                        my ($key, @reststring) = split(/:/);
                        my $reststring = join(":",@reststring);
                        chop $reststring;
                        $var{$key} = $reststring;
                }
                next;
        }
	IRC::print("$logo Config file loaded...");
	close(FILE);
        if (defined $var{'record_cps'}) {$record_cps = $var{'record_cps'}};
        if (defined $var{'record_cps_user'}) {$record_cps_user = $var{'record_cps_user'}};
        if (defined $var{'file_count'}) {$file_count = $var{'file_count'}};
        if (defined $var{'byte_count'}) {$byte_count = $var{'byte_count'}};
        if (defined $var{'access_count'}) {$access_count = $var{'access_count'}};
        if (defined $var{'send_fails'}) {$send_fails = $var{'send_fails'}};
        if (defined $var{'logo'}) {$logo = $var{'logo'}};
        if (defined $var{'c1'}) {$c1 = $var{'c1'}};
        if (defined $var{'c2'}) {$c2 = $var{'c2'}};
        if (defined $var{'c3'}) {$c3 = $var{'c3'}};
        if (defined $var{'c4'}) {$c4 = $var{'c4'}};
}

#
#	debug_msg($msg): guess =)
#
sub debug_msg
{
	my ($line) = @_;
	my $temp_time = localtime ();

	if ($var{'debug'} == 1) {
		IRC::print_with_channel("$logo <DEBUG> ok ".$line, "obsidian_dbg");
#               IRC::print("$logo <DEBUG> ".$line);
                my $debug_file = $conf_dir . "/obsidian.debug";
                if (!open(FILE, '>>'.$debug_file)) {
                        IRC::print("$logo Could not open $debug_file for reading...");
                } else {
                        print(FILE "$temp_time:  $line\n");
                        close(FILE);
                }
	}
}

#
#	cache_welcome(): caches the welcome message
sub cache_welcome
{
        debug_msg("cache_welcome: enter");
        my $welcome_file = $conf_dir . "/obsidian.welcome";
        @welcome_msg=();
	if (!open(FILE, '<'.$welcome_file)) {
                debug_msg("Could not open $welcome_file for reading. No special welcome msg.");
		return;
	}
        my $i = 0;
        while(<FILE>)
        {
                my $buf = $_;
                chop $buf;
                push(@welcome_msg, $buf);
        }
        close(FILE);
}

#
#	cache_files(): cache all the files & dirs in $root_dir
#
sub cache_files
{
	my $filecount = 0;
        debug_msg("cache_files: enter");

	# cleanup old stuff
	foreach (keys %file_cache) {
		delete ($file_cache{$_});
	}
	foreach (keys %file_cache_uc) {
		delete ($file_cache_uc{$_});
	}

        debug_msg("cache_files: root_dir: $var{'root_dir'}");
	cache_files_in_dir($var{'root_dir'});

	# count files cached
	foreach (keys %file_cache) {
		$filecount += @{@{$file_cache{$_}}[1]};
	}

	IRC::print("$logo Cached ".$filecount." files in ".scalar(keys %file_cache)." directories");
}


#
#	cache_files_in_dir($dir): recursive file caching function
#
sub cache_files_in_dir
{
	my ($dir) = @_;
	my @dirnames = ();
	my @filenames = ();
	my @dirnames_uc = ();
	my @sizes = ();
        my @files_in_dir = ();

        my $direntry = "none at all.";
        local(*DIR);

        debug_msg("cache_files_in_dir: dir: $dir");
	unless (opendir(DIR, "$dir")) {
                debug_msg("cache_files_in_dir: couldn't open shared directory");
                return;
        }
	for (;;) {
                $direntry = readdir(DIR);
                debug_msg("cache_files_in_dir: >>".$direntry."<<");
                last unless defined $direntry;
                next if $direntry eq '';
                next if $direntry eq '.';
                next if $direntry eq '..';
                push(@files_in_dir, $direntry);
        }
        foreach $direntry (sort(@files_in_dir)) {
                my $full_path = $dir.'/'.$direntry;
                if (-d $full_path) {
                        push(@dirnames, $direntry);
                        push(@dirnames_uc, uc($direntry));
                        cache_files_in_dir($full_path);
                } elsif (-f $full_path) {
                        # Workaround because of File::stat
                        my @stats = (stat($full_path));
                        if (defined $stats[7]) {
                                push(@sizes, $stats[7]);
                        } else {
                                push(@sizes, $stats[0]->size);
                        }
                        #push(@sizes, (stat($full_path))[7]);
                        push(@filenames, $direntry);
		}
	}

	closedir(DIR);
	my @data = ([@dirnames], [@filenames], [@sizes]);
	my @data_uc = ([@dirnames_uc], [@filenames], [@sizes]);
	$file_cache{$dir} = [@data];
	$file_cache_uc{uc($dir)} = [@data_uc];
}

#############################################################################
#
#  Help systems and license
#
#############################################################################

#
#	display_help($nick): shows help information to $nick
#
sub display_help
{
	my ($nick, $server, $args) = @_;
        if (length($args) == 0) {
                send_user_msg_with_server($nick, $server, "Help on fileserver commands");
                send_user_msg_with_server($nick, $server, "===========================");
                send_user_msg_with_server($nick, $server, "ls or dir   -   -   list directory content");
                send_user_msg_with_server($nick, $server, "cd <path>   -   -   change directory to path");
                send_user_msg_with_server($nick, $server, "get <file>  -   -   download file");
                send_user_msg_with_server($nick, $server, "pwd     -   -   -   print current path");
                send_user_msg_with_server($nick, $server, "queues  -   -   -   view queues");
                send_user_msg_with_server($nick, $server, "sends   -   -   -   view sends");
                send_user_msg_with_server($nick, $server, "dequeue #n  -   -   clear Your queued file #n");
                send_user_msg_with_server($nick, $server, "clr_queues  -   -   clear all of Your queues");
                send_user_msg_with_server($nick, $server, "stats   -   -   -   print some statistics");
                send_user_msg_with_server($nick, $server, "help    -   -   -   print this help text");
                send_user_msg_with_server($nick, $server, "bye or exit or quit to leave this fserve gracefully");
                if ($var{'case_file'}) {
                        send_user_msg_with_server($nick, $server, "");
                        send_user_msg_with_server($nick, $server, "This fserve runs on a case sensitive file system.");
                }
                return;
        }
        my $help = $args;
        debug_msg("help to command: $help");

        if ($help eq "dir" || $help eq "ls") {
                send_user_msg_with_server($nick, $server, "With this command You can view the content of the actual directory.");
	}
}

#
#	show_help(): shows help on commands, variables and toggles
#
sub show_help
{
	my ($help) = @_;
        debug_msg("help to command: $help");

        if ($help eq "toggle") {
                IRC::print("/fs toggle");
                IRC::print("/fs toggle <toggle> <0|1>");
                IRC::print("With this command You can either view the state of all toggles or set the state of a toggle to 0 for off or 1 for on.");
	} elsif ($help eq "set" || $help eq "vars") {
                IRC::print("/fs set");
                IRC::print("/fs vars");
                IRC::print("With this command You can view the value of all variables.");
                IRC::print("/fs set <var> <value>");
                IRC::print("With this command You can set the value of a variable.");
	} elsif ($help eq "debug") {
                IRC::print("With the debug toggle You can switch on additional output for debugging. This debug output will also be written to obsidian.debug in the xchat resource dir (~/.xchat per default). Be carefull with this because the debug file can get really large.");
                IRC::print("Recommended position: 0");
        } elsif ($help eq "autoon") {
                IRC::print("File server will activate after 5 minutes of admin inactivity.");
                IRC::print("So the server will start on startup and after changes automatically.");
                IRC::print("Recommended position: 1");
        } elsif ($help eq "restorequeues") {
                IRC::print("Decides wether to load and save the queues on startup and unloading of the fserve.");
                IRC::print("Recommended position: 1");
        } elsif ($help eq "priority_op") {
                IRC::print("If set the op's of a channel will get on the fail/priority queue.");
                IRC::print("Recommended value: 0");
        } elsif ($help eq "priority_voice") {
                IRC::print("If set the voiced people of a channel will get on the fail/priority queue.");
                IRC::print("Recommended value: 0");
        } elsif ($help eq "notice_as_msg") {
                IRC::print("If on, the server ad's to the channel are send as messages. This way You can advertise everywhere You can chat.");
                IRC::print("Recommended position: 0");
        } elsif ($help eq "short_notice") {
                IRC::print("If on only print a short advertise notice.");
                IRC::print("Recommended position: 0 but this really depends on the channel policy.");
        } elsif ($help eq "ctcptrigger") {
                IRC::print("If on the trigger will be displayed as /ctcp currentnickname currenttrigger. Regardless of this toggles position both trigger types would be accepted.");
                IRC::print("Recommended position: 1");
        } elsif ($help eq "open_tab_early") {
                IRC::print("Let obsidian handle the opening of tabs. This way the welcome messages don't show up on Your screen which is less annoying. This obsoletes the raw_wlcm_msg feature somewhat.");
                IRC::print("Recommended value: 1");
        } elsif ($help eq "close_tab_auto") {
                IRC::print("Experimental! Let obsidian close tabs when the user types leaves the chat and closes the obsidian session. Currently this is just working for nicely exited sessions, e.g. the user typed quit, bye or exit. Will cause xchat crashes for now.");
                IRC::print("No recommended position.");
        } elsif ($help eq "raw_chnl_msg") {
                IRC::print("Sends the ad's hidden from fserve admin as private message. This way the messages don't show up on Your screen which is less annoying.");
                IRC::print("Recommended value: 1");
        } elsif ($help eq "raw_wlcm_msg") {
                IRC::print("Sends the welcome message as private messages instead of dcc chat messages. This way the messages don't show up on Your screen which is less annoying. On the other hand this increasses the load on the irc server. There is a slight chance that it will be regarded as flooding. Another problem is that some clients display dcc chats and privmsgs in different windows. In this case it will be very confusing for the user that another window just popped up for no particular reason when they type a command. This feature is also a reason that obsidian also parses privmsg for commands.");
                IRC::print("Recommended value: 0");
        } elsif ($help eq "case_cmd") {
                IRC::print("If on the commands are case sensitive. So Dir won't be accepted.");
                IRC::print("Recommended position: 0");
        } elsif ($help eq "case_file") {
                IRC::print("If on the pattern matching for filenames and directory names is case sensitive. This is confusing since some OS don't distinguish upper and lower case filenames. Also a short message is displayed in the welcome message pointing out the case sensitive behaviour.");
                IRC::print("Recommended position: 0");
        } elsif ($help eq "fs_sensitive") {
                IRC::print("If on display a message in the welcome screen pointing out the filesize sensitive queues.");
                IRC::print("Recommended position: 1");
        } elsif ($help eq "count_sends") {
                IRC::print("When counting the number of queued files the sends to a user could be counted as queued files also.");
                IRC::print("Recommended position: 1");
        } elsif ($help eq "send_small_now") {
                IRC::print("Send small files immediatly without queueing.");
                IRC::print("Actually just disregard maxsends for small files.");
                IRC::print("Recommended position: 1");
        } elsif ($help eq "max_users") {
                IRC::print("Maximum number of users at once allowed on this fserve.");
                IRC::print("Recommended value: 10");
        } elsif ($help eq "max_sends") {
                IRC::print("Maximum number of active sends at once.");
                IRC::print("Recommended value: uploadspeed / 10kbyte/sec");
        } elsif ($help eq "max_queues") {
                IRC::print("This is the size of the queues for future sends. It's value depends also on channel policy so feel free to chat about the ideal queue size.");
                IRC::print("Recommended value: 10 for big files, 1000 for small files.");
        } elsif ($help eq "slots_big") {
                IRC::print("Decides how many files bigger than size_big a user can queue.");
                IRC::print("Recommended value: 1");
        } elsif ($help eq "slots_med") {
                IRC::print("Decides how many files bigger than size_med a user can queue. Remember that a big file is also bigger than a medium size file so this value should exceed slots_big.");
                IRC::print("Recommended value: 4");
        } elsif ($help eq "slots_small") {
                IRC::print("Decides how many files smaller than size_med a user can queue. Since small files take just a few seconds to transfer a high value is recomended.");
                IRC::print("Recommended value: max_queues / 2");
        } elsif ($help eq "size_big") {
                IRC::print("All files with a size of at least size_big bytes count as big files and take up one of the slots_big slots for big files. Please note that these files also take up on medium size slot.");
                IRC::print("Recommended value: Bytes that can be uploaded in one hour / max_sends");
        } elsif ($help eq "size_med") {
                IRC::print("All files with a size of at least size_med bytes count as medium files and take up one of the slots_med slots for medium files.");
                IRC::print("Recommended value: 1000000");
        } elsif ($help eq "idle_time") {
                IRC::print("This is the timeout value for users browsing the fserve. After idle_time seconds without any commands the fserve will close the connection.");
                IRC::print("Recommended value: 180");
        } elsif ($help eq "max_fails") {
                IRC::print("How often a file will be resent. Zero means no resents or failqueues switched off.");
                IRC::print("Recommended value: 5");
        } elsif ($help eq "min_speed") {
                IRC::print("All users with an upload speed smaller than min_speed will be disconnected. This is to avoid some very slow sends blocking others forever. If You want to use this consider using a small value and increase the number of max_sends.");
                IRC::print("Recommended value: 0");
        } elsif ($help eq "auto_backup") {
                IRC::print("To prevent a flushed queue when obsidian crashes it's possible to backup all queues after a certain amount of time. Use of restorequeues should be sufficent for most cases.");
                IRC::print("Recommended value: 0");
        } elsif ($help eq "trigger") {
                IRC::print("When someone types Your trigger in the channel or sends this trigger as a ctcp message to You. Obsidian will answer with it's welcome message and accept user commands from this user.");
                IRC::print("Recommended value: !someletters");
        } elsif ($help eq "serve_no_notify") {
                IRC::print("Some channels welcome servers but don't welcome server messages. Add these channels here. The fserve won't be noticed at all except if someone types !list. This must be a space separated list of channels.");
                IRC::print("#channel\@netname, where netname is the last to parts of the servers url (eg. dal.net). For nets with multiple of such identifyers see netaliases");
                IRC::print("No recommended value.");
        } elsif ($help eq "serve_hidden") {
                IRC::print("Maybe You want to run a server just for trusted friends that won't advertise in the channel and won't respond to list or find queries. Add these channels here. The fserve won't be noticed at all. This must be a space separated list of channels.");
                IRC::print("#channel\@netname, where netname is the last to parts of the servers url (eg. dal.net). For nets with multiple of such identifyers see netaliases");
                IRC::print("No recommended value.");
        } elsif ($help eq "notify_to_voice") {
                IRC::print("Some channels voice servers when they see their first ad. In this case You wan't to issue at least one ad on channel join. This must be a space separated list of channels.");
                IRC::print("#channel\@netname, where netname is the last to parts of the servers url (eg. dal.net). For nets with multiple of such identifyers see netaliases");
                IRC::print("No recommended value.");
                IRC::print("Not implemented yet use notify_channels instead.");
        } elsif ($help eq "notify_channels") {
                IRC::print("List of channels to notify every notify_timeout seconds. This must be a space separated list of channels.");
                IRC::print("#channel\@netname, where netname is the last to parts of the servers url (eg. dal.net). For nets with multiple of such identifyers see netaliases");
                IRC::print("No recommended value.");
        } elsif ($help eq "notify_timeout") {
                IRC::print("Timeout in seconds between ad's. Don't forget that there is no use in advertising at all when You can't meet the demand.");
                IRC::print("Recommended value: 3600");
        } elsif ($help eq "root_dir") {
                IRC::print("Absolute value of the shared files directory. Don't share all files. Create a special dir with shared files. You can use softlinks to link to other dirs and files.");
                IRC::print("No recommended value.");
        } elsif ($help eq "note") {
                IRC::print("This note is displayed with every ad. Maybe You want to tell others what's on Your fserve?");
                IRC::print("No recommended value.");
        } elsif ($help eq "netaliases") {
                IRC::print("The last to parts of the servers url are used to figure out which net it belongs to. Unfortunately some nets have servers with more then one \"netnames\".");
                IRC::print("Here you can identify such endings, seperated by ':'. If you need more than one aliaslist, seperate them with spaces.");
                IRC::print("No recommended value.");
        } elsif ($help eq "saveq") {
                IRC::print("/fs saveq");
                IRC::print("Saves the sends and queues to the file obsidian.queue in the xchat resource dir (~/.xchat per default).");
                IRC::print("Makes a backup of the previous file to obsidian.bqueue.");
        } elsif ($help eq "notify") {
                IRC::print("/fs notify [channel]");
                IRC::print("If no parameter is given this command will notify all channels listed in notify_channels. Otherwise it will send a server ad to the given channel. Channel might be a space seperated list of channels.");
        } elsif ($help eq "update_files") {
                IRC::print("/fs update_files");
                IRC::print("Recaches the shared files. You must issue this command whenever You change something in the shared files directory.");
        } elsif ($help eq "loadq") {
                IRC::print("/fs loadq");
                IRC::print("Loads the sends and queues from the file obsidian.queue in the xchat resource dir (~/.xchat per default). You must clear the queues and sends first. Use reset_sends and clr_queues.");
        } elsif ($help eq "load_backup") {
                IRC::print("/fs load_backup");
                IRC::print("Loads the sends and queues from the file obsidian.bqueue in the xchat resource dir (~/.xchat per default). You must clear the queues and sends first. Use reset_sends and clr_queues.");
        } elsif ($help eq "on") {
                IRC::print("/fs on");
                IRC::print("The fserve now listens to !list and trigger commands. Also files from the queue can be send now.");
        } elsif ($help eq "notify") {
                IRC::print("/fs off");
                IRC::print("The fserve now stops listening to !list and trigger commands. Also files from the queue won't be send now.");
        } elsif ($help eq "load") {
                IRC::print("/fs load");
                IRC::print("Load all parameters from obsidian.conf in the xchat resource dir (~/.xchat per default).");
        } elsif ($help eq "save") {
                IRC::print("/fs save");
                IRC::print("Save all parameters to obsidian.conf in the xchat resource dir (~/.xchat per default).");
        } elsif ($help eq "reset_sends") {
                IRC::print("/fs reset_sends");
                IRC::print("Clears send list. Ongoing sends are not stoped. Limited use since the sends array should always contain a valid value now.");
        } elsif ($help eq "clr_queues") {
                IRC::print("/fs clr_queues");
                IRC::print("Flush queue.");
        } elsif ($help eq "dequeue") {
                IRC::print("/fs dequeue <#a>");
                IRC::print("The file in queue pos #a is now dequeued. You may want to use /fs off before issuing this command to avoid movements in queue list.");
        } elsif ($help eq "move") {
                IRC::print("/fs move <#a> <#b>");
                IRC::print("Move file from pos #a to pos #b. You may want to use /fs off before issuing this command to avoid movements in queue list.");
        } elsif ($help eq "stats") {
                IRC::print("/fs stats");
                IRC::print("This command can also be issued by the user. Displays some statistics.");
        } elsif ($help eq "sends") {
                IRC::print("/fs stats");
                IRC::print("This command can also be issued by the user. Displays ongoing sends.");
        } elsif ($help eq "queues") {
                IRC::print("/fs stats");
                IRC::print("This command can also be issued by the user. Displays queued files.");
        } elsif ($help eq "smsg") {
                IRC::print("/fs smsg message");
                IRC::print("Sends a message to all users that have an active DCC transfer.");
        } elsif ($help eq "qmsg") {
                IRC::print("/fs qmsg message");
                IRC::print("Sends a message to all users that have a file in your queue.");
        } elsif ($help eq "amsg") {
                IRC::print("/fs amsg message");
                IRC::print("Sends a message to all users that have either an active DCC transfer or a file in your queue.");
        } elsif ($help eq "who") {
                IRC::print("/fs who");
                IRC::print("Displays all users that are online along with their online and idle time.");
        } elsif ($help eq "show_files") {
                IRC::print("/fs show_files");
                IRC::print("Displays an overview of all the files you are sharing.");
        } elsif ($help eq "queuef") {
                IRC::print("/fs queue_file <nick> <filename>");
                IRC::print("/fs queue_file <nick> <#number>");
                IRC::print("Queues the file named 'filename' or the file referenced by #number for nick. The number coresponds to the positions given by '/fs show_files'.");
        } elsif ($help eq "sendf") {
                IRC::print("/fs send_file <nick> <filename>");
                IRC::print("/fs send_file <nick> <#number>");
                IRC::print("Immediately sends the file named 'filename' or the file referenced by #number to nick. The number coresponds to the positions given by '/fs show_files'.");
        } elsif ($help eq "send_queue") {
                IRC::print("/fs send_queue number");
                IRC::print("Immediately sends the queued file number to the appropriate nick. Disregards send slots limits.");
        } elsif ($help eq "help") {
                IRC::print("/fs help");
                IRC::print("Shows a summary of the basic command of obsidian.");
                IRC::print("/fs help <keyword>");
                IRC::print("Shows a help for the keyword. Valid keywords are variables, toggles and commands.");
        } elsif ($help eq "about") {
                $abouttext = <<"ENDEABOUT";

About Obsidian FServe 0.9.2 for X-Chat 1.7+

This software is copyleft and distributed under the terms of the
GPL version 2. You should have recieved a copy of the GPL with this
program. This copy may be attached to this file so if You havn't
already read this important document jump to the end and read it.

Copyright (c) 2001/2002 Alexander Werth (alexander.werth\@gmx.de)
Copyright (c) 2001 Martin Persson (mep\@passagen.se) (inactive)

If You have any problems with this software please try to gather as
much information as possible before sending a typical \"don't work,
help now.\" email. If You don't know the exact nature and cause of
the problem send as much info as possible. This should at least
contain the version of xchat, perl, obsidian fserve, operation
system, a copy or exception of the file .xchat/obsidian.conf and
any messages that appear when loading this script.
ENDEABOUT
#'
                IRC::print($abouttext);
        } elsif ($help eq "gpl") {
                $gnutext = <<"ENDEGNU";

**********************************************************************
		    GNU GENERAL PUBLIC LICENSE
		       Version 2, June 1991

 Copyright (C) 1989, 1991 Free Software Foundation, Inc.
               59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

			    Preamble

  The licenses for most software are designed to take away your
freedom to share and change it.  By contrast, the GNU General Public
License is intended to guarantee your freedom to share and change free
software--to make sure the software is free for all its users.  This
General Public License applies to most of the Free Software
Foundation's software and to any other program whose authors commit to
using it.  (Some other Free Software Foundation software is covered by
the GNU Library General Public License instead.)  You can apply it to
your programs, too.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
this service if you wish), that you receive source code or can get it
if you want it, that you can change the software or use pieces of it
in new free programs; and that you know you can do these things.

  To protect your rights, we need to make restrictions that forbid
anyone to deny you these rights or to ask you to surrender the rights.
These restrictions translate to certain responsibilities for you if you
distribute copies of the software, or if you modify it.

  For example, if you distribute copies of such a program, whether
gratis or for a fee, you must give the recipients all the rights that
you have.  You must make sure that they, too, receive or can get the
source code.  And you must show them these terms so they know their
rights.

  We protect your rights with two steps: (1) copyright the software, and
(2) offer you this license which gives you legal permission to copy,
distribute and/or modify the software.

  Also, for each author's protection and ours, we want to make certain
that everyone understands that there is no warranty for this free
software.  If the software is modified by someone else and passed on, we
want its recipients to know that what they have is not the original, so
that any problems introduced by others will not reflect on the original
authors' reputations.

  Finally, any free program is threatened constantly by software
patents.  We wish to avoid the danger that redistributors of a free
program will individually obtain patent licenses, in effect making the
program proprietary.  To prevent this, we have made it clear that any
patent must be licensed for everyone's free use or not licensed at all.

  The precise terms and conditions for copying, distribution and
modification follow.

		    GNU GENERAL PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. This License applies to any program or other work which contains
a notice placed by the copyright holder saying it may be distributed
under the terms of this General Public License.  The "Program", below,
refers to any such program or work, and a "work based on the Program"
means either the Program or any derivative work under copyright law:
that is to say, a work containing the Program or a portion of it,
either verbatim or with modifications and/or translated into another
language.  (Hereinafter, translation is included without limitation in
the term "modification".)  Each licensee is addressed as "you".

Activities other than copying, distribution and modification are not
covered by this License; they are outside its scope.  The act of
running the Program is not restricted, and the output from the Program
is covered only if its contents constitute a work based on the
Program (independent of having been made by running the Program).
Whether that is true depends on what the Program does.

  1. You may copy and distribute verbatim copies of the Program's
source code as you receive it, in any medium, provided that you
conspicuously and appropriately publish on each copy an appropriate
copyright notice and disclaimer of warranty; keep intact all the
notices that refer to this License and to the absence of any warranty;
and give any other recipients of the Program a copy of this License
along with the Program.

You may charge a fee for the physical act of transferring a copy, and
you may at your option offer warranty protection in exchange for a fee.

  2. You may modify your copy or copies of the Program or any portion
of it, thus forming a work based on the Program, and copy and
distribute such modifications or work under the terms of Section 1
above, provided that you also meet all of these conditions:

    a) You must cause the modified files to carry prominent notices
    stating that you changed the files and the date of any change.

    b) You must cause any work that you distribute or publish, that in
    whole or in part contains or is derived from the Program or any
    part thereof, to be licensed as a whole at no charge to all third
    parties under the terms of this License.

    c) If the modified program normally reads commands interactively
    when run, you must cause it, when started running for such
    interactive use in the most ordinary way, to print or display an
    announcement including an appropriate copyright notice and a
    notice that there is no warranty (or else, saying that you provide
    a warranty) and that users may redistribute the program under
    these conditions, and telling the user how to view a copy of this
    License.  (Exception: if the Program itself is interactive but
    does not normally print such an announcement, your work based on
    the Program is not required to print an announcement.)

These requirements apply to the modified work as a whole.  If
identifiable sections of that work are not derived from the Program,
and can be reasonably considered independent and separate works in
themselves, then this License, and its terms, do not apply to those
sections when you distribute them as separate works.  But when you
distribute the same sections as part of a whole which is a work based
on the Program, the distribution of the whole must be on the terms of
this License, whose permissions for other licensees extend to the
entire whole, and thus to each and every part regardless of who wrote it.

Thus, it is not the intent of this section to claim rights or contest
your rights to work written entirely by you; rather, the intent is to
exercise the right to control the distribution of derivative or
collective works based on the Program.

In addition, mere aggregation of another work not based on the Program
with the Program (or with a work based on the Program) on a volume of
a storage or distribution medium does not bring the other work under
the scope of this License.

  3. You may copy and distribute the Program (or a work based on it,
under Section 2) in object code or executable form under the terms of
Sections 1 and 2 above provided that you also do one of the following:

    a) Accompany it with the complete corresponding machine-readable
    source code, which must be distributed under the terms of Sections
    1 and 2 above on a medium customarily used for software interchange; or,

    b) Accompany it with a written offer, valid for at least three
    years, to give any third party, for a charge no more than your
    cost of physically performing source distribution, a complete
    machine-readable copy of the corresponding source code, to be
    distributed under the terms of Sections 1 and 2 above on a medium
    customarily used for software interchange; or,

    c) Accompany it with the information you received as to the offer
    to distribute corresponding source code.  (This alternative is
    allowed only for noncommercial distribution and only if you
    received the program in object code or executable form with such
    an offer, in accord with Subsection b above.)

The source code for a work means the preferred form of the work for
making modifications to it.  For an executable work, complete source
code means all the source code for all modules it contains, plus any
associated interface definition files, plus the scripts used to
control compilation and installation of the executable.  However, as a
special exception, the source code distributed need not include
anything that is normally distributed (in either source or binary
form) with the major components (compiler, kernel, and so on) of the
operating system on which the executable runs, unless that component
itself accompanies the executable.

If distribution of executable or object code is made by offering
access to copy from a designated place, then offering equivalent
access to copy the source code from the same place counts as
distribution of the source code, even though third parties are not
compelled to copy the source along with the object code.

  4. You may not copy, modify, sublicense, or distribute the Program
except as expressly provided under this License.  Any attempt
otherwise to copy, modify, sublicense or distribute the Program is
void, and will automatically terminate your rights under this License.
However, parties who have received copies, or rights, from you under
this License will not have their licenses terminated so long as such
parties remain in full compliance.

  5. You are not required to accept this License, since you have not
signed it.  However, nothing else grants you permission to modify or
distribute the Program or its derivative works.  These actions are
prohibited by law if you do not accept this License.  Therefore, by
modifying or distributing the Program (or any work based on the
Program), you indicate your acceptance of this License to do so, and
all its terms and conditions for copying, distributing or modifying
the Program or works based on it.

  6. Each time you redistribute the Program (or any work based on the
Program), the recipient automatically receives a license from the
original licensor to copy, distribute or modify the Program subject to
these terms and conditions.  You may not impose any further
restrictions on the recipients' exercise of the rights granted herein.
You are not responsible for enforcing compliance by third parties to
this License.

  7. If, as a consequence of a court judgment or allegation of patent
infringement or for any other reason (not limited to patent issues),
conditions are imposed on you (whether by court order, agreement or
otherwise) that contradict the conditions of this License, they do not
excuse you from the conditions of this License.  If you cannot
distribute so as to satisfy simultaneously your obligations under this
License and any other pertinent obligations, then as a consequence you
may not distribute the Program at all.  For example, if a patent
license would not permit royalty-free redistribution of the Program by
all those who receive copies directly or indirectly through you, then
the only way you could satisfy both it and this License would be to
refrain entirely from distribution of the Program.

If any portion of this section is held invalid or unenforceable under
any particular circumstance, the balance of the section is intended to
apply and the section as a whole is intended to apply in other
circumstances.

It is not the purpose of this section to induce you to infringe any
patents or other property right claims or to contest validity of any
such claims; this section has the sole purpose of protecting the
integrity of the free software distribution system, which is
implemented by public license practices.  Many people have made
generous contributions to the wide range of software distributed
through that system in reliance on consistent application of that
system; it is up to the author/donor to decide if he or she is willing
to distribute software through any other system and a licensee cannot
impose that choice.

This section is intended to make thoroughly clear what is believed to
be a consequence of the rest of this License.

  8. If the distribution and/or use of the Program is restricted in
certain countries either by patents or by copyrighted interfaces, the
original copyright holder who places the Program under this License
may add an explicit geographical distribution limitation excluding
those countries, so that distribution is permitted only in or among
countries not thus excluded.  In such case, this License incorporates
the limitation as if written in the body of this License.

  9. The Free Software Foundation may publish revised and/or new versions
of the General Public License from time to time.  Such new versions will
be similar in spirit to the present version, but may differ in detail to
address new problems or concerns.

Each version is given a distinguishing version number.  If the Program
specifies a version number of this License which applies to it and "any
later version", you have the option of following the terms and conditions
either of that version or of any later version published by the Free
Software Foundation.  If the Program does not specify a version number of
this License, you may choose any version ever published by the Free Software
Foundation.

  10. If you wish to incorporate parts of the Program into other free
programs whose distribution conditions are different, write to the author
to ask for permission.  For software which is copyrighted by the Free
Software Foundation, write to the Free Software Foundation; we sometimes
make exceptions for this.  Our decision will be guided by the two goals
of preserving the free status of all derivatives of our free software and
of promoting the sharing and reuse of software generally.

			    NO WARRANTY

  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
REPAIR OR CORRECTION.

  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

		     END OF TERMS AND CONDITIONS

	    How to Apply These Terms to Your New Programs

  If you develop a new program, and you want it to be of the greatest
possible use to the public, the best way to achieve this is to make it
free software which everyone can redistribute and change under these terms.

  To do so, attach the following notices to the program.  It is safest
to attach them to the start of each source file to most effectively
convey the exclusion of warranty; and each file should have at least
the "copyright" line and a pointer to where the full notice is found.

    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 19yy  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


Also add information on how to contact you by electronic and paper mail.

If the program is interactive, make it output a short notice like this
when it starts in an interactive mode:

    Gnomovision version 69, Copyright (C) 19yy name of author
    Gnomovision comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type `show c' for details.

The hypothetical commands `show w' and `show c' should show the appropriate
parts of the General Public License.  Of course, the commands you use may
be called something other than `show w' and `show c'; they could even be
mouse-clicks or menu items--whatever suits your program.

You should also get your employer (if you work as a programmer) or your
school, if any, to sign a "copyright disclaimer" for the program, if
necessary.  Here is a sample; alter the names:

  Yoyodyne, Inc., hereby disclaims all copyright interest in the program
  `Gnomovision' (which makes passes at compilers) written by James Hacker.

  <signature of Ty Coon>, 1 April 1989
  Ty Coon, President of Vice

This General Public License does not permit incorporating your program into
proprietary programs.  If your program is a subroutine library, you may
consider it more useful to permit linking proprietary applications with the
library.  If this is what you want to do, use the GNU Library General
Public License instead of this License.
ENDEGNU
#`
              IRC::print($gnutext);
      }
}

#and one for the package
}

1;

__END__
