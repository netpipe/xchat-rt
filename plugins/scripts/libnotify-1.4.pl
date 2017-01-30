#!/usr/bin/perl
# libnotify.pl for X-Chat, by s0ulslack (Tony Annen)
# Distributed under the terms of the GNU General Public License, v2 or later
# $Header: ~/.xchat2/libnotify.pl, v1.3 2009/04/17 10:21:00 $
# Need help? /msg s0ulslack on Freenode

# uses notification-daemon/libnotify to display queries, nick highlights,
# notices and kicks when XChat isn't focused  (ignores ctcp/dcc events)

# optional:
# show notifications even if xchat is focused
# show only initial notification per event
# play audio on notifications (uses "play" from sox)
# logging highlights/events to a seperate tab
# notification when kicked from channels
# ignoring Nick/ChanServ/X notices

## always show notifications even if xchat is focused? #certain channel togglers
$all=1;
$channelMessages=1;
## only show first notification per nick?
$initial=1;
## notify when kicked from channels?
$kick=1;
## ignore chan/nickserv/x notices?
$ignore=1;
## log events to a seperate tab?
$log=1;
$logtab="\@highlights";

## notification display time? (in milliseconds, 1000ms = 1sec)
$time="10000";
## play audio on events? ($soundfile must exist and "play" (from sox) installed)
$soundfile="$ENV{'HOME'}/.xchat2/plugins/libnotify.wav";
$sounds=0; #channel specific sounds
$log=0;
#$messages
$focusedv=1; #focused messaging on instead of other way around.

IRC::register("libnotify.pl", "1.4", "Notifications of msgs, nick highlights, notices and kicks", "");
IRC::add_message_handler("PRIVMSG", "privmsg_handler");
IRC::add_message_handler("NOTICE", "notice_handler");
IRC::add_message_handler("KICK", "kick_handler");

IRC::add_command_handler("notified", "notified");
IRC::add_command_handler("speech", "speech");

# sanity check, make sure notify-send is found
#if(-e "/sbin/notify-send"){
#  $ns="/sbin/notify-send";
#}elsif(-e "/bin/notify-send"){
  $ns="/bin/notify-send";
#}elsif(-e "/usr/sbin/notify-send"){
#  $ns="/usr/sbin/notify-send";
#}elsif(-e "/usr/bin/notify-send"){
#  $ns="/usr/bin/notify-send";
#}

  #$ns="/usr/bin/mate-notify-send"; # use for mate
  $ns="notify-send"; # use for mate

if(!$ns){
  IRC::print("libnotify-1.4.pl not loaded!\nYou must have libnotify installed.");
  return 0;
}

 system("$ns 'libNotify started YAY!'");
  system("echo 'libnotify loaded' | festival --tts"); #festival test
# lets finger out the sound
if(-e "/sbin/play"){
  $play="/sbin/play";
}elsif(-e "/bin/play"){
  $play="/bin/play";
}elsif(-e "/usr/sbin/play"){
  $play="/usr/sbin/play";
}elsif(-e "/usr/bin/play"){
  $play="/usr/bin/play";
}
if(-e "$soundfile" && -x "$play"){
  $sound=$sounds;								#enable sound or not!
}

# display configuration
if($all == "1"){ $sa="notifying if focused/unfocused"; }else{ $sa="notifying if unfocused"; }
if($sound == "1"){ $s="+audio alerts"; }else{ $s="-audio alerts"; }
if($log == "1"){ $l="+event logging"; }else{ $l="-event logging"; }
IRC::print("libnotify loaded ($sa ($s, $l))");


# privmsg - basically all messages
sub privmsg_handler{
$mynick=(IRC::get_info(1));
local($line)=@_;
#if($line=~ m/:(.+?)\!.+? PRIVMSG $mynick :(.*)/){
#channels watchlist
if($line=~ m/:(.+?)\!.+? PRIVMSG ($mynick|#luna-dev|#luna|#irrlicht|#electronics) :\s*(.*)/){
  if($line=~ /xdcc\s+.*(?:send|list)/i){ return 0; }
  if($line=~ /\s+.*(?:ping|version|time|finger|xdcc|dcc)/i){ return 0; }
  $user=$1;
  $message=$3; $message=~  s/\+//s; $message=~  s/-//s; $message=~  s/\<//s; $message=~  s/\>//s;
  if(-e "$ENV{'HOME'}/.xchat2/.lmn"){ $oldmn=`cat $ENV{'HOME'}/.xchat2/.lmn`; chomp($oldmn); }
  $window=Xchat::get_info("win_status");
  if($window eq "active" && $all == "1"){ $window="hidden"; }
  if($window eq "hidden" or $window eq "normal"){
    if($user eq $oldmn && $initial == "1"){ return 0; }
 	if(!$channelMessages){ return 0; }
    system("$ns -t $time -i xchat \"Query: $user\" \"$message\"");
	if ($bspeech) {system("echo \"$user says $message\" | festival --tts");}
    if($sound == "1"){ system("$play $soundfile");	}

  }

  system("echo \"$user $message\" >> $ENV{'HOME'}/.xchat2/libNofifyMessages");
}


# nick highlights - channel messages containting my nick
if($line=~ m/:(.+?)\!.+? PRIVMSG (#.*) :\s*(.*$mynick.*)/){
  $user=$1;
  $channel=$2;
  $message=$3; $message=~  s/\+//s; $message=~  s/-//s; $message=~  s/\<//s; $message=~  s/\>//s;
  if(-e "$ENV{'HOME'}/.xchat2/.lhn"){ $oldhl=`cat $ENV{'HOME'}/.xchat2/.lhn`; chomp($oldhl); }
  if(-e "$ENV{'HOME'}/.xchat2/.lahn"){ $oldahl=`cat $ENV{'HOME'}/.xchat2/.lahn`; chomp($oldahl); }
  $window=Xchat::get_info("win_status");
  if($window eq "active" && $all == "1"){ $window="hidden"; }
  if($window eq "hidden" or $window eq "normal"){
    if($message=~/ACTION/){
      $message=~  s/.*ACTION //s; $message=~  s/.\z//s;
	  if($log){
        IRC::command("/query -nofocus $logtab");
        IRC::print_with_channel("\cC8$channel\cO\11$user $message\n","$logtab","");
      }
	  if($user eq $oldahl && $initial == "1"){ return 0; }
	  system("$ns -t $time -i xchat \"Action Highlight: $channel\" \"$user $message\"");
	  if($sound == "1" && $log =="1"){ system("$play $soundfile"); }
	  system("echo \"$user\" > $ENV{'HOME'}/.xchat2/.lahn");
	  return 0;
	}
    if($log){
      IRC::command("/query -nofocus $logtab");
      IRC::print_with_channel("\cC8$user/$channel\cO\11$message\n","$logtab","");
    }
	if($user eq $oldhl && $initial == "1"){ return 0; }
	system("$ns -t $time -i xchat \"Highlight: $channel\" \"$user: $message\"");
    if($sound == "1"){ system("$play $soundfile"); }
	system("echo \"$user\" > $ENV{'HOME'}/.xchat2/.lhn");
  }
 }
}




# notices
sub notice_handler{
  $mynick=(IRC::get_info(1));
  local($line)=@_;
  if($line=~ m/:(.+?)\!.+? NOTICE $mynick :(.*)/){
    $user=$1;
    $message=$2; $message=~  s/\+//s; $message=~  s/-//s; $message=~  s/\<//s; $message=~  s/\>//s;
    if($ignore == "1" && $user eq "NickServ" or $user eq "ChanServ" or $user eq "X"){ return 0; }
    if(-e "$ENV{'HOME'}/.xchat2/.lnn"){ $oldnn=`cat $ENV{'HOME'}/.xchat2/.lnn`; chomp($oldnn); }
    if($log){
      IRC::command("/query -nofocus $logtab");
      IRC::print_with_channel("\cC8($user)\cO\11$message\n","$logtab","");
    }
    if($user eq $oldnn && $initial == "1"){ return 0; }
    $window=Xchat::get_info("win_status");
    if($window eq "active" && $all == "1"){ $window="hidden"; }
    if($window eq "hidden" or $window eq "normal"){
      system("$ns -t $time -i xchat \"Notice: $user\" \"$message\"");
      system("echo \"$user\" > $ENV{'HOME'}/.xchat2/.lnn");
	}
	if($sound == "1"){ system("$play $soundfile"); }
  }
}

# kicks
sub kick_handler{
if($kick == "0"){ return 0; }
$mynick=(IRC::get_info(1));
local($line)=@_;
if($line=~ m/:(.+?)\!.+? KICK (#.*) $mynick :(.*)/){
  $user=$1;
  $channel=$2;
  $message=$3; $message=~  s/\+//s; $message=~  s/-//s; $message=~  s/\<//s; $message=~  s/\>//s;
  system("$ns -t $time -u critical -i xchat \"Kicked from $channel\" \"$user kicked you ($message)\"");
  if($log){
    IRC::command("/query -nofocus $logtab");
    IRC::print_with_channel("\cC8$channel\cO\11$user kicked you ($message)\n","$logtab","");
  }
  if($sound == "1"){ system("$play $soundfile"); }
}


sub notified{
	#$bnotify=true;
	#$all=1;
#festival check or echo to console

	if ($channelMessages=1){$channelMessages=0;system("$ns -t $time -u critical -i xchat \"notify on\""); system("echo \"notify off\" | festival --tts")}
	else {$channelMessages=1;system("$ns -t $time -u critical -i xchat \"notify on\"");system("echo \"notify on\" | festival --tts")}
}

sub speech{
#check for festival

	if ($bspeech=1){	$bspeech=0;system("$ns -t $time -u critical -i xchat \"notify OFF\"");system("echo \"speech off\" | festival --tts")}
		else {$bspeech=1;system("$ns -t $time -u critical -i xchat \"notify ON\""); system("echo \"speech on\" | festival --tts")}

}



}

