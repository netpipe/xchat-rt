##########################################################################
#                                                                        
# WARNING : This script is only tested in Linux enviroments don't use    
#           it with Windows! It could damage your file system! 
#
# NOTE:     An attempt is underway to test File-Butler with Windows Xchat          
#                                                                        
##########################################################################
#                                                                        
#       File-Butler is a file-server script with ratio support           
#                 for the Linux Xchat IRC client                         
#                                                                        
#                        Version 0.1.1-4
#                 Released under the GNU GPL v.2                              
#                                                                        
#              Xchat version 2.4.5 or higher needed                      
#                                                                        
#      If you find bugs or miss features mail me at brokndodge@yahoo.com   
#               tag the mail subject with [file-butler]
#                  
#                      original 0.1 version
#                               by 
#                        mariokless@gmx.de                  
##########################################################################
#
#        WARNING:  USER INFORMATION AUTOSAVE DOES NOT OCCUR UNLESS
#                             ADVERTISE IS SET TO ON
#
##########################################################################


sub default {

# default configuration values - you can change the commented ones to your needs

# root directory of the files you want to share
$home="$xchat_dir/downloads/";  #you should point this somewhere else example: "/home/user/fb"

# command to start file server
$trigger='!file-butler';

# channel the server listens
$channel='#filebutler';

# number of bytes credit a user gets for every byte he uploads
$ratio=2;

# starting credit for every new user
$credit=100000;

# maximum number of users at the same time
$user_max=5;

# maximum number of files a user can get at the same time
$get_max=5;

# maximum number of files a user can send at the same time
$send_max=5;

# maximum number of files a user can request (although get limit is reached)
$queue_max=5;

# choose if advertising is 'on' or 'off' at server startup
$advertise='1';

# short description of what you offer
$description="File-Butler 0.1.1-1 Beta.  This server is being tested, please do not attempt to connect!!!";

# time between two advertisings in seconds
$ad_time=180;

# seconds until an inactive user will be kicked from server
$idle=90;

# seconds until an inactive user will be warned 
$idle_warning=60;

# decide if users are allowed to upload files that are already on the server (on|off)
$duplicates="0";

# insert a description of what you (don't) want your users to do
# the rules will be shown everytime an user logs in
$rules=(' ');

}

#########################################################################
#                                                                       #
# Don't change anything below here unless you know what you are doing!  #
#                                                                       #
#########################################################################

use File::Find;
use Xchat qw(:all);

#this version of file-butler
$fbver="[0.1.1-4]";
$fbdesc="[file-server with credit ratio support]";
# Per user queue for files to send
%queue=();

# some colour shortcuts
$error="\0034[ERROR]";
$green="\0033";
$red="\0034";
$blue="\0032";
$normal="\017";
$bold="\002";
$under="\037";

# width of directory listing
$format_dir=35;

# hash for user data
%user=();

# cache for files on the server
%files=();

# hack for getting the size of uploaded files
%incoming=();

# Server status (on|off)
$server='off';

# Server logo
$logo="[${blue}FB${normal}${red} ${fbver}${normal}]";

# Timer for advertising
$adv_timeout=0;

# names of active users
%active_users=();

# Server welcome message
@welcome=("$under${blue}Welcome to File-Butler$normal ${red}$fbver ${bold}Alpha",
	  "Released under the GNU GPL v.2",
	  "maintained by brokndodge",
	  " ",
	  "File-Butler is currently under heavy development", 
	  " ",
	  "${under}Commands",
	  "dir               list files",
	  "cd <directory>    change directory",
	  "get <filename>    get file",
	  "credits           show your credits",
	  "help              show help screen",
	  "quit              close connection ",
	  " ",
	  "$boldIt is not neccasary to be connected",
	  "$boldto this server to recieve credit for",
	  "$boldyour sends",
	  " ", 
	  "$bold$red${under}This is a ${blue}Linux$red file server, so all filenames are ${green}case-sensitive!$normal",
	  " "
	  );

# channel for server messages
$c="file-butler";

Xchat::register ("file-butler", "$fbver", "$fbdesc", \&shutdown);

Xchat::hook_command("b","butler");
Xchat::hook_command("bset","bset");


Xchat::hook_server("PRIVMSG","privmsg");

Xchat::hook_print("DCC Chat Text","dcc_chat");
Xchat::hook_print("DCC CHAT Failed","dcc_chat_failed");
Xchat::hook_print("DCC CHAT Connect","dcc_chat_connect");
Xchat::hook_print("DCC SEND Offer","dcc_send_offer");
Xchat::hook_print("DCC SEND Complete","dcc_send_complete");
Xchat::hook_print("DCC SEND Failed","dcc_send_failed");
Xchat::hook_print("DCC RECV Complete","dcc_recv_complete");
Xchat::hook_print("DCC RECV Failed","dcc_receive_failed");
Xchat::hook_print("DCC RECV Connect","dcc_receive_connect");
Xchat::hook_print("Change Nick","change_nick");

Xchat::hook_timer(1000,"timer");
$xchat_dir=Xchat::get_info("xchatdir");#+"/plugins/file-butler/
# set default values
default();

# load configuration and print startup message
gui_menu();
startup();
sub startup {
    
    Xchat::command("query $c");
    Xchat::print("$logo Loading file-butler ...",$c);
    Xchat::print("$logo  ",$c);
      
      my $error=0;
# load server configuration
	load_config();
        load_user_stats();
      
  }

sub create_user {
    (my $nick,my $ucredit)=@_;
    $user{$nick}={'credits'=>$ucredit,
		  'dir'=>$home,
		  'idle'=>0,
		  'get'=>0,
		  'send'=>0,
		  'queue'=>0,
	      };
    $queue{$nick}=[];
}

# process commands to the server
sub butler {
    	if ($_[0][1] eq "on") {
	    server_on()
	    }
	
	elsif ($_[0][1] eq "off") {
	    if ($server eq "on") {
		$server="off";
		command(qq{gui msgbox "Stopping file server"});
		foreach my $key (keys %active_users) {
		    Xchat::command("msg =$key File server is going down.");
		      dcc_close();
		  }
	    }
	    else {
		command(qq{gui msgbox"File server isn't running!"});
	      }
	}
	elsif ($_[0][1] eq "reset") {
	      if (!defined $_[0][2]) {
		command(qq{getstr "no" "b reset" "Are you sure you want to reset \n File-Butler to the default settings? \n Type yes to reset File-Butler."});
		  return Xchat::EAT_ALL;
	      }
	    
	      elsif ($_[0][2] eq "yes") {
	      default();
	      command(qq{gui msgbox "File-Butler has been reset to the default settings!"});
	      }
	  }
	elsif ($_[0][1] eq "user") {
	    set_context("$c");
	    command(qq{gui focus});
	    Xchat::print("$logo ${under}Non-active user",$c);
	    foreach my $key (sort keys %user) {
		if (! exists $active_users{$key}) {
		    my $string=format_size($user{$key}->{'credits'});
		    Xchat::print("$logo User:$blue $key$normal Credits:$blue $string",$c);
		}
	    }
	    if (! keys %active_users) {
		Xchat::print("$logo",$c);
		  Xchat::print("$logo No active users on the server!",$c);
	      }
	      else {
		  Xchat::print("$logo",$c);
		    Xchat::print("$logo ${under}Active user",$c);
		    foreach my $key (sort keys %active_users) {
			my $string=format_size($user{$key}->{'credits'});
			Xchat::print("$logo User:$blue $key$normal Idle:$blue $user{$key}->{'idle'}$normal s Credits:$blue $string",$c);
		    }
		}
		}
	elsif ($_[0][1] eq "credit") {
	    if (!defined $_[0][2]) {
		command(qq{getstr "nick" "b credit" "user"});
		  return Xchat::EAT_ALL;
		}
	    if (!exists $user{$_[0][2]}) {
		command(qq{gui msgbox "The user $_[0][2] is not in my database!"});
		  return Xchat::EAT_ALL;
	      }
	    if (!defined $_[0][3]) {
		my $ucredit=$user{$_[0][2]}->{'credits'};
		command(qq{getstr "0" "b credit $_[0][2]" "The user has $ucredit credits \n to add credits use + \n to subtract credits use -"});
		  return Xchat::EAT_ALL;
	      }
	    
	    if ($_[0][3]=~/^(\+|-)(\d+)([mgkMGK])?/) {
		my $factor=1;
		if (defined $3) {
		    my $suff=$3;
		    if ($suff eq "G" or $suff eq "g") {
			$factor=1_000_000_000;
		    }
		    elsif ($suff eq "M" or $suff eq "m") {
			$factor=1_000_000;
		    }
		    else {
			$factor=1_000;
		    }
		}
		if ($1 eq "+") {
		    $user{$_[0][2]}->{'credits'}+=$2*$factor;
		}
		else {
		    $user{$_[0][2]}->{'credits'}-=$2*$factor;
		    
		}
	    }
	    if ($_[0][3]=~/^(\d+)/) {
		$user{$_[0][2]}->{'credits'}=$1;
	    }
	    my $string=format_size($user{$_[0][2]}->{'credits'});
	    command(qq{gui msgbox "User $_[0][2] now has $string credits"});
	}
	elsif ($_[0][1] eq "status") {
	      if ($server eq "off") {
	      command(qq{gui msgbox "File server is $server"});
	      }
	      elsif ($server eq "on") {
		  if (my @count=keys %active_users) { 
		  my $number=@count;
		  my @fcount=keys %files;
		  my $count=@fcount;
		  command(qq{gui msgbox "Current $number of $user_max users: @count \n Serving $count unique files"}); 
		  }
	          else {
		  command(qq{gui msgbox "Current users: None \n Serving $count unique files"});
	          }
	      
	      }
	      
	}
	elsif ($_[0][1] eq "kill") {
		if (!defined $_[0][2]) {
		command(qq{getstr "nick" "b kill" "User"});
		  return Xchat::EAT_ALL;
		}
		
		my $nick = $_[0][2];
		dcc_close($nick);
	}
	elsif ($_[0][1] eq "save") {
		save_server_config();
		command(qq{gui msgbox "File-Butler Configuration Saved"});
	}
	elsif ($_[0][1] eq "stat") {
		if (!defined $_[0][2]) { 
			command(qq{getstr "nick" "b stat" "User"});
			return EAT_ALL;
		}
		my $nick=$_[0][2];
		my $ucredit=$user{$nick}->{'credits'};
		my $active="no";
		if (defined $active_users{$nick}) {
			$active="yes";
		}
		command(qq{gui msgbox "User:     $nick\nCredit:   $ucredit\nActive:   $active"});
	}
	elsif ($_[0][1] eq "backup") {
		backup();
		command(qq{gui msgbox "File-Butler Backup Complete\nServer Config: $xchat_dir/file-butler.conf.bk\nUser Stats:    $xchat_dir/file-butler.db.bk"});
	}
	elsif ($_[0][1] eq "restore") {
		restore();
		command(qq{gui msgbox "File-Butler Restore Complete"});
	}
	elsif ($_[0][1] eq "options") {
		set_context("$c");
		command(qq{gui focus});
	    Xchat::print("$logo",$c);
	      Xchat::print("$logo ${under}Server Configuration",$c);
	      Xchat::print("$logo",$c);
	      Xchat::print("$logo ${under}option          current value         ",$c);
	      Xchat::print("$logo",$c);	      
	      Xchat::print("$logo trigger         $trigger",$c);
	      Xchat::print("$logo channel         $channel",$c);
	      Xchat::print("$logo home            $home",$c);
	      Xchat::print("$logo description     $description",$c);
	      Xchat::print("$logo user_max        $user_max",$c);
	      Xchat::print("$logo get_max         $get_max",$c);
	      Xchat::print("$logo send_max        $send_max",$c);
	      Xchat::print("$logo queue_max       $queue_max",$c);
	      Xchat::print("$logo credit          $credit",$c);
	      Xchat::print("$logo ratio           $ratio",$c);
	      Xchat::print("$logo duplicates      $duplicates",$c);
	      Xchat::print("$logo idle            $idle",$c);
	      Xchat::print("$logo advertise       $advertise",$c);
	      Xchat::print("$logo ad_time         $ad_time",$c);
              Xchat::print("$logo rules           $rules",$c);
	      Xchat::print("$logo",$c);
	  }
	elsif ($_[0][1] eq "set") {
	    if (defined $_[0][2] and defined $_[0][3]) {
		if ($_[0][2] eq "trigger") {
		    $trigger = $_[1][3];
		    Xchat::print("$logo trigger was set to $trigger",$c);
		}
		elsif ($_[0][2] eq "channel") {
		    $channel = $_[0][3];
		    Xchat::command(qq{gui msgbox "Channel was set to $channel"});
		}
		elsif ($_[0][2] eq "home") {
		    $home = $_[1][3];
		    if (-d $_[1][3]) {
			$home=$_[1][3];
			foreach my $key (keys %user) {
			    $user{$key}->{'dir'}=$home;
			}
			if ($duplicates eq "off") {
			    %files=();
			    cache_files();
			}
			Xchat::print("$logo home was set to $home",$c);
		    }
		    else {
			Xchat::print("$logo $error $home is not a directory!",$c);
		      }
		}
		elsif ($_[0][2] eq "description") {
		    $description = $_[1][3];
		    Xchat::print("$logo description was set to: $description",$c);
		}
		elsif ($_[0][2] eq "user_max") {
		    if ($_[0][3]=~/^\d+$/) {
			$user_max = $_[0][3];
			Xchat::print("$logo user_max was set to $user_max",$c);
		    }
		    else {
			Xchat::print("$logo $error $_[0][3] is not a correct user_max value!",$c);
		      }
		}
		elsif ($_[0][2] eq "get_max") {
		    if ($_[0][3]=~/^\d+$/) {
			$get_max = $_[0][3];
			Xchat::print("$logo get_max was set to $get_max",$c);
		    }
		    else {
			Xchat::print("$logo $error $_[0][3] is not a correct get_max value!",$c);
		      }
		}
		elsif ($_[0][2] eq "send_max") {
		    if ($_[0][3]=~/^\d+$/) {
			$send_max = $_[0][3];
			Xchat::print("$logo send_max was set to $send_max",$c);
		    }
		    else {
			Xchat::print("$logo $error $_[0][3] is not a correct send_max value!",$c);
		      }
		}
		elsif ($_[0][2] eq "queue_max") {
		    if ($_[0][3]=~/^\d+$/) {
			$queue_max = $_[0][3];
			Xchat::print("$logo queue_max was set to $queue_max",$c);
		    }
		    else {
			Xchat::print("$logo $error $_[0][3] is not a correct queue_max value!",$c);
		      }
		}
		elsif ($_[0][2] eq "credit") {
		    if ($_[0][3]=~/^\d+$/) {
			$credit = $_[0][3];
			Xchat::print("$logo credits was set to $credit",$c);
		    }
		    else {
			Xchat::print("$logo $error $_[0][3] is not a correct credit value!",$c);
		      }
		}
		elsif ($_[0][2] eq "ratio") {
		    if ($_[0][3]=~/^\d+$/) {
			$ratio = $_[0][3];
			Xchat::print("$logo ratio was set to $ratio",$c);
		    }
		    else {
			Xchat::print("$logo $error $_[0][3] is not a correct ratio value!",$c);
		      }
		}
		elsif ($_[0][2] eq "duplicates") {
		    
			$duplicates = $_[0][3];
					    		    
		}
		elsif ($_[0][2] eq "idle") {
		    if ($_[0][3]=~/^\d+$/) {
			$idle = $_[0][3];
			Xchat::print("$logo idle was set to $idle",$c);
		    }
		    else {
			Xchat::print("$logo $error $_[0][3] is not a correct idle value!",$c);
		      }
		}
		elsif ($_[0][2] eq "advertise") {
		    	$advertise = $_[0][3];
			
		}
		elsif ($_[0][2] eq "ad_time") {
		    if ($_[0][3]=~/^\d+$/) {
			$ad_time = $_[0][3];
			$adv_timeout=0;
			Xchat::print("$logo ad_time was set to $ad_time",$c);
		    }
                    else {
			Xchat::print("$logo $error $_[0][3] is not a correct ad_time value!",$c);
		      }
                }
                elsif ($_[0][2] eq "rules") {
		    $rules = $_[1][3];
		    Xchat::print("$logo rules were set to: $rules",$c);
		    
		    
		}
		
		else {
		    Xchat::print("$logo $error You passed an unknown option to the set command",$c);
		  }
	    }
	    else {
		if (!defined $_[0][2]) {
		    Xchat::print("$logo $error You forgot to specify the option you want to set",$c);
		  }
		else {
		    Xchat::print("$logo $error You forgot to specify the value of the option you want to set",$c);
		  }
	    }
	}
	
    
    return Xchat::EAT_ALL;
}

# listen for trigger message
# fixed by brokndodge 6/24/06
sub privmsg {

    my $line=$_[1][0];

    foreach ($line) {

    if ($server eq "off") {
	return Xchat::EAT_NONE;
    	}

    $line=~/:(\S+)!(\S+) PRIVMSG (\S+) :(.+)/;

    my $nick=$1;
    my $server=$2;
    my $chan=lc($3);
    my $message=$4;

    if (($message eq $trigger) and ($chan eq $channel)) {
	opendcc("$nick");
	}
    else {
	return Xchat::EAT_NONE;
	}
    }
}

sub opendcc {
    my $nick=$_[0];
    if (!(defined $user{$nick})) {
	create_user($nick,$credit);
    }
    if (defined $active_users{$nick}) {
	Xchat::command("msg $channel Sorry ${nick}, I need a few minutes to reset your connection before you try again.");
	  return Xchat::EAT_ALL;
      }
    my $count=keys %active_users;
    if ($count==$user_max) {
	Xchat::command("msg $channel Sorry ${nick}, $red${bold}[${blue}${count}${red}]$normal of $red${bold}[${blue}${user_max}${red}]$normal slots already in use, try again later!");
	  return Xchat::EAT_ALL;
      }
    $active_users{$nick}++;
    $user{$nick}->{'dir'}=$home;
    $user{$nick}->{'idle'}=0;
    Xchat::command("dcc chat $nick");
    return Xchat::EAT_NONE;
}
# end brokndodge's fix
# print welcome message, credit and rules on chat connect
sub dcc_chat_connect {
    my $nick=$_[0][0];
    
    if (!(defined $user{$nick}) or (! exists $active_users{$nick})) {
	return Xchat::EAT_NONE;
    }
    foreach (@welcome) {
	Xchat::command("msg =$nick $_");
    }
    my $string=format_size($user{$nick}->{'credits'});
    Xchat::command("msg =$nick Your credits:$blue $string $normal Ratio:$blue 1:${ratio}");
    foreach ($rules) {
	Xchat::command("msg =$nick $bold The Rules are as follows:  $blue$_${normal}");
    }
}

#catch all user commands
sub dcc_chat {
    my $nick=$_[0][2];
    my $message=$_[0][3];
    if (!(defined $user{$nick}) or (! exists $active_users{$nick})) {
	command("msg =$nick $logo You must type $trigger in $channel in order to log into $logo.");
	return Xchat::EAT_ALL;
    }
    $user{$nick}->{'idle'}=0;
    if ($message=~/^\s*(dir|ls)/i) {
	list_dir($nick);
    }
    elsif ($message=~/^\s*help/i) {
	help("$nick");
    }
    elsif ($message=~/^\s*get\s+(.+)/) {
	get_file($nick,$1);
    }
    elsif ($message=~/^\s*cd\s*$/i) {
	$user{$nick}->{'dir'}=$home;
	Xchat::command("msg =$nick /");
    }
    elsif ($message=~/^\s*cd..\s*$/i) {
	change_dir_up($nick);
    }
    elsif ($message=~/^\s*cd.\s*$/i) {
	$user{$nick}->{'dir'}=~/^$home(.*)/;
	Xchat::command("msg =$nick $1");
    }
    elsif ($message=~/^\s*cd\s+(.+)/) {
	my $dir=$1;
	
	# legacy compatibility
	if ($message=~/^\s*\.\.\s*$/i) {
	change_dir_up($nick);
        }
	#end legacy compatibility
	if ($dir!~/\.\./) {
	    change_dir($nick,$dir);
	}
	else {
	    Xchat::command("msg =$nick $error Detected a .. in the path - aborting!");
	}
    }
    elsif ($message=~/^\s*credit/i) {
	my $string=format_size($user{$nick}->{'credits'});
	Xchat::command("msg =$nick Your current credit:$blue $string");
    }
    elsif ($message=~/^\s*rules/i) {
	Xchat::command("msg =$nick $blue$bold$rules");
    }
    # i am attempting to build a "call for help" feature
#    elsif ($message=~/^\s*call/i) {
#	Xchat::command("dialog =$nick $logo Welcome to the $trigger help desk.",
#		"The system operator will be with you in a few moments.");
#    }
    elsif ($message=~/^\s*(quit|exit|close)/i) {
	dcc_close("$nick");
	return EAT_ALL;
    }
    else {
	  help($nick);
    }
    return Xchat::EAT_NONE;
}

sub help {
	my $nick=$_[0];
	#set_context($nick);
	Xchat::command("msg =$nick Use the following commands:");
	Xchat::command("msg =$nick  ");
	Xchat::command("msg =$nick cd              change to the home directory ");
	Xchat::command("msg =$nick cd <dir>        change to directory <dir> ");
	Xchat::command("msg =$nick cd.             current directory ");
	Xchat::command("msg =$nick cd..           change to parent directory ");
	Xchat::command("msg =$nick dir             list current directory ");
	Xchat::command("msg =$nick get <name>      get file with name <name> ");
	Xchat::command("msg =$nick credit          show your current credits ");
	Xchat::command("msg =$nick quit            exit from file server ");
        Xchat::command("msg =$nick rules           display the rules of this server ");
	Xchat::command("msg =$nick $bold$red${under}This is a ${blue}Linux$red file server, so all filenames are ${green}case-sensitive!$normal");
	return EAT_ALL;
}

sub cache_files {
    my $start=time();
    Xchat::print("$logo Starting file caching - this can take some time!",$c);
    finddepth(\&search_files,$home);
    my $stop=time();
    my $count=keys %files;
    Xchat::printf("$logo Finished file caching, found %d files in %d seconds ",$count,$stop-$start);
}

sub search_files {
    -f && $files{$_}++;
}

sub dcc_chat_failed {
    my $nick=$_[0][0];
    dcc_close("$nick");
    return Xchat::EAT_ALL;
}

sub dcc_receive_connect {
    my $nick=$_[0][0];
    my $file=$_[0][2];
    if ($files{$file}) {
	Xchat::command("msg =$nick $error $file is already here, upload another one!");
	  Xchat::command("dcc close get $nick $file");
	  if (exists $incoming{$file}) {
	      delete $incoming{$file};
	  }
	  return Xchat::EAT_NONE;
      }
    if ($user{$nick}->{'send'}==$send_max) {
	Xchat::command("msg =$nick $error You're already sending the maximum of $send_max files per user!");
	  Xchat::command("dcc close get $nick $file");
      }
    else {
	$user{$nick}->{'send'}++;
    }
    return Xchat::EAT_NONE;
}


sub dcc_send_complete {
    my $nick=$_[0][1];
    my $file=$_[0][0];
    $user{$nick}->{'get'}--;
    if ($user{$nick}->{'queue'}) {
	my $path=shift @{$queue{$nick}};
	Xchat::command("dcc send $nick \"$path\"");
	$user{$nick}->{'queue'}--;
	$user{$nick}->{'get'}++;
    }
}

sub dcc_receive_failed {
    my $nick=$_[0][2];
    my $file=$_[0][0];
    Xchat::command("msg =$nick $error Your send of $file failed!");
    delete $incoming{$file};
    $user{$nick}->{'send'}--;
}



sub get_file {
    (my $nick,my $file)=@_;
    
    my $path=$user{$nick}->{'dir'} . "/" . $file;
    
    if (! -e $path) {
	Xchat::command("msg =$nick $error File $file does not exist!");
	  return Xchat::EAT_ALL;
      }
    if (! -r $path) {
	Xchat::command("msg =$nick $error You don't have access rights for $file!");
	  return Xchat::EAT_ALL;
      }
    if ( -d $path) {
	Xchat::command("msg =$nick $error You can't download $file it's a directory!");
	  return Xchat::EAT_ALL;
      }
    if ($user{$nick}->{'queue'}==$queue_max) {
	Xchat::command("msg =$nick $error You have reached the queue limit of $queue_limit files!");
	  return Xchat::EAT_ALL;
      }

    my $size= -s $path;
    if (($user{$nick}->{'credits'}-$size)<=0) {
	my $ucredit=format_size($user{$nick}->{'credits'});
	my $f_size=format_size($size);
	Xchat::command("msg =$nick $error You don't have enough credits to get $blue$file$red ($blue $f_size $red), current credits:$blue $ucredit");
	  return Xchat::EAT_ALL;
      }
    $user{$nick}->{'credits'}-=$size;
    $ucredit=format_size($user{$nick}->{'credits'});
    if ($user{$nick}->{'get'}<$get_max) {
	$user{$nick}->{'get'}++;
	Xchat::command("msg =$nick Sending file$blue $1${normal}, credits left:$blue $ucredit");
	Xchat::command("dcc send $nick \"$path\"");
    }
    else {
	$user{$nick}->{'queue'}++;
	Xchat::command("msg =$nick Added file$blue $file$normal to the send queue, credits left:$blue $ucredit");
	push @{$queue{$nick}},$path;
    }
    return Xchat::EAT_ALL;
}

sub change_dir {
    (my $nick,my $dir)=@_;
    my $path=$user{$nick}->{'dir'} . "/" . $dir;
    if ($path!~/^$home/) {
	  $path=$home;
      }
    if (-d $path) {
	if (!(-r $path) and !(-x $path)) {
	    Xchat::command("msg =$nick $error You don't have access rights to $dir!");
	      return Xchat::EAT_NONE;
	  }
	$user{$nick}->{'dir'}=$path;
	$path=~/^$home(.*)/;
	Xchat::command("msg =$nick $1");
    }
    else {
	Xchat::command("msg =$nick $error $dir is not a directory!");
      }
}

sub change_dir_up {
    (my $nick)=@_;
    my $path=$user{$nick}->{'dir'};
# cut last directory
    $path=~/^(.*)\/.*$/;
    $path=$1;
# only go up to root directory
    if ($path!~/^$home/) {
	$path=$home;
    }
    $user{$nick}->{'dir'}=$path;
    if ($path eq $home) {
	Xchat::command("msg =$nick /");
      }
    else {
	$path=~/^$home(.*)/;
	Xchat::command("msg =$nick $1");
    }
}

sub list_dir {
    my ($nick)=@_;
    my $file;
    my $path=$user{$nick}->{'dir'};
    opendir(DIR,$path);
    if ($path eq $home) {
	Xchat::command("msg =$nick /");
      }
    else {
	$path=~/^$home(.*)/;
	Xchat::command("msg =$nick $]");
    }
    my @dir_list=();
    my %file_list=();
    while (defined (my $entry=readdir(DIR))) {
	my $src=$path . "/" . $entry;
	if (-d $src and -r $src and -x $src) {
	    push @dir_list,$entry;
	}
	else {
	    if (-r $src) {
		$file_list{$entry}=-s $src;
	    }
	}
    }
    foreach $file (sort @dir_list) {
	if ($file eq "." or $file eq "..") {
	    next;
	}
	my $dir="[${blue}DIR${normal}]";
	my $length=$format_dir-length($file)-length($dir);
	if ($length<0) {
	    $length=1;
	}
	my $message=sprintf("%s%s%s",$file," "x$length,$dir);
	Xchat::command("msg =$nick $message");
    }
    foreach $file (sort keys %file_list) {
	$size=format_size($file_list{$file});
	my $length=$format_dir-length($file)-length($size);
	if ($length<0) {
	    $length=1;
	}
	my $message=sprintf("%s%s%s",$file," "x$length,$size);
	Xchat::command("msg =$nick $message");
    }


    closedir(DIR);
}

sub format_size {
    (my $string)=@_;
    if ($string < 1000) {
	$string="$string  B";
    }
    elsif ($string < 1_000_000) {
	$string=sprintf("%.2f KB",$string/1_000);
    }
    elsif ($string < 1_000_000_000) {
	$string=sprintf("%.2f MB",$string/1_000_000);
    }
    else {
	$string=sprintf("%.2f GB",$string/1_000_000_000);
    }
    return $string;
}

sub timer {
    if ($server eq "off") {
	return Xchat::EAT_ALL;
    }
    if (($advertise eq '1') and (++$adv_timeout>=$ad_time)) {
	advert();
    }
    foreach my $nick (keys %active_users) {
	    if (++$user{$nick}->{'idle'}==$idle_warning) {
		my $diff=$idle-$idle_warning;
		Xchat::command("msg =$nick Closing idle connection in $diff seconds");
	    }
	    if ($user{$nick}->{'idle'}>=$idle) {
		  Xchat::command("msg =$nick Closing idle connection in $diff seconds, $nick");
		  dcc_close("$nick");
		  return EAT_ALL;
	      }
    }
    return Xchat::EAT_ALL;
}

sub advert {
    my $count=keys %active_users;
    Xchat::command("msg $channel $logo Trigger: $blue$trigger$normal Ratio:${blue} 1:$ratio ${normal}Start Credit:$blue $credit ${normal}Desc: $description [Users$blue $count/$user_max${normal}]");
    ##  need to perform backup at a specified interval 
    ##  here is as good as any
    backup();
      $adv_timeout=0;
  }

sub dcc_recv_complete {
    my $file=$_[0][0];
    my $nick=$_[0][2];

    if ($duplicates eq "0") {
	$files{$file}++;
    }
    $user{$nick}->{'send'}--;
    $user{$nick}->{'credits'}+=$incoming{$file}*$ratio;
    delete $incoming{$file};
    my $ucredit=format_size($user{$nick}->{'credits'});
    Xchat::command("msg =$nick Upload of$blue $file$normal complete, current credit:$blue $ucredit");
    return Xchat::EAT_NONE;
}

sub dcc_send_offer {
    my $nick=$_[0][0];
    my $file=$_[0][1];
    my $size=$_[0][2];

    Xchat::command("dcc get $nick $file");
    $incoming{$file}=$size;
    return Xchat::EAT_NONE;
}

#  we are going to close the chat connection and the chat window
sub dcc_close {
        
        my $nick=$_[0];
	delete $active_users{$nick};
	set_context("$nick");
        command("msg $nick Closing Connection!  Good Bye!");
	command("dcc close chat $nick");
	command("close $nick");
	return EAT_ALL;
}


# save all data before unloading the script
sub shutdown {
    my $path=Xchat::get_info("xchatdir");
    my $error=0;
    Xchat::command(qq{gui msgbox "Shutting down File-Butler..."});
    set_context($c);
    command("close $c");
    
# kicking active users
    foreach my $key (keys %active_users) {
	set_context($key);
	command("msg =$key File server is going down. Closing connection. Bye!");
	command("dcc close chat $key");
        command("close $key");
      }
    save_server_config();
    save_user_stats();
    return Xchat::EAT_ALL;
}
sub load_config {
my $conffile = "file-butler.conf";
if (defined $_[0]) {
    $conffile="$_[0]";
 
}
my $file="$xchat_dir/$conffile";
      if (open (FD,$file)) {
	  while (<FD>) {
	      if (/^trigger\s*=\s*(.+)/) {
		  $trigger=$1;
	      }
		  if (/^home\s*=\s*(.+)/) {
		      if (-d $1) {
			  $home=$1;
		      }
		      else {
			  command(qq{gui msgbox "Configured home directory is not available, using default!"});
			}
		  }
		  elsif (/^channel\s*=\s*(\S+)/) {
		      $channel=$1;
		  }
		  elsif (/^description\s*=\s*(.+)/) {
		      $description=$1;
		  }
		  elsif (/^ratio\s*=\s*(\d+)/) {
		      $ratio=$1;
		  }
		  elsif (/^user_max\s*=\s*(\d+)/) {
		      $user_max=$1;
		  }
		  elsif (/^get_max\s*=\s*(\d+)/) {
		      $get_max=$1;
		  }
		  elsif (/^send_max\s*=\s*(\d+)/) {
		      $send_max=$1;
		  }
		  elsif (/^idle\s*=\s*(\d+)/) {
		      $idle=$1;
		  }
		  elsif (/^idle_warning\s*=\s*(\d+)/) {
		      $idle_warning=$1;
		  }
		  elsif (/^advertise\s*=\s*(\S+)/) {
		      $advertise=$1;
		  }
		  elsif (/^ad_time\s*=\s*(\d+)/) {
		      $ad_time=$1;
		  }
		  elsif (/^duplicates\s*=\s*(\S+)/) {
		      $duplicates=$1;
		  }
		  elsif (/^rules\s*=\s*(.+)/) {
		      $rules=$1;
		  }
	  }
	  close FD;
      }
      else {
	  command(qq{gui msgbox "Configuration file was not found, using defaults!"});
	    $error++;
	}
return EAT_ALL;
}

sub load_user_stats {
my $conffile = "file-butler.db";
if (defined $_[0]) {
    $conffile = "$_[0]";
}
# load user statistics      
      my $file="$xchat_dir/$conffile";
      if (open (FD,$file)) {
	  while (<FD>) {
	      if (/(\S+)\s+(\d+)/) {
		  create_user($1,$2);
	      }
	  }
	  close FD;
      }
      else {
	  command(qq{gui msgbox "User statistics file was not found, using new one!"});
	    $error++;
      }
return EAT_ALL;
}
# we are going to backup the server 
# config and user stats
sub backup {
    save_server_config("file-butler.conf.bk");
    save_user_stats("file-butler.db.bk");
    return EAT_ALL;
}
sub restore {
    load_config("file-butler.conf.bk");
    load_user_stats("file-butler.db.bk");
    save_server_config();
    save_user_stats();
	
    return EAT_ALL;
}
sub save_server_config {
#save server configuration

my $conffile = "file-butler.conf";
if (defined $_[0]) {
    $conffile = "$_[0]";
	
}    
    if (open (file,">$xchat_dir/$conffile")) {
        #Xchat::print("$logo accessing $xchat_dir/butler_server");
	print file "trigger = $trigger\n";
	print file "channel = $channel\n";
	print file "home = $home\n";
	print file "description = $description\n";
	print file "ratio = $ratio\n";
	print file "user_max = $user_max\n";
	print file "get_max = $get_max\n";
	print file "send_max = $send_max\n";
	print file "idle = $idle\n";
	print file "idle_warning = $idle_warning\n";
	print file "advertise = $advertise\n";
	print file "ad_time = $ad_time\n";
	print file "duplicates = $duplicates\n";
        print file "rules = $rules\n";
	close (file);
        #Xchat::print("$logo server config backup sucessfull.\n",$c);
        }
        else {
		command(qq{gui msgbox "Couldn't write server configuration file!"});
	 	$error++;
        }
    return Xchat::EAT_NONE;
    
}  
# save user stats
sub save_user_stats {
my $conffile = "file-butler.db";
if (defined $_[0]) {
    $conffile = "$_[0]";
} 
    if (open (file,">$xchat_dir/$conffile")) {
        #Xchat::print("$logo accessing $xchat_dir/butler_user");
	foreach my $nick (sort keys %user) {
	    print file "$nick $user{$nick}->{'credits'}\n";
	}
	close (file);
        
	#Xchat::print("$logo userstats backup sucessfull.\n",$c);
      }
    else {
	command(qq{gui msgbox "Couldn't write user statistics file!"});
	  $error++;
      }
    
    
return Xchat::EAT_NONE;
}

sub dcc_send_failed {
    my $file=$_[0][0];
    my $nick=$_[0][1];
    Xchat::command("msg =$nick $error Failed to send$blue $file$normal!");
}

sub change_nick {
    my $old=$_[0][0];
    my $new=$_[0][1];
    if (exists $active_users{$old}) {
	delete $active_users{$old};
	$active_users{$new}++;
	$user{$new}=$user{$old};
	delete $user{$old};
    }	
}

sub server_on {
    	    set_context("$c");
	    command(qq{gui focus});
	    if ($server eq "on") {
		Xchat::command(qq{gui msgbox "File server is already running!"});
		return EAT_ALL;
	    }
	    else {
		#if ($duplicates eq "0") {
		#    %files=();
		#    cache_files();
		#}
		$server="on";
		cache_files();
		Xchat::command("join $channel");
		
		if ($advertise eq "1") {
		    advert();
	    }
return Xchat::EAT_ALL;
}

sub bset {
	my $type=$_[0][1];
	my $cmd=$_[0][2];
	my $default=${$cmd};
	my $prompt=$cmd;
	if ($prompt=~/_/) {
		$prompt =~ s/(\S+)_(\S+)/$1 $2/g;
	}
	$prompt =~ s/(\w+)/\u\L$1/g;
	if ($type =~ /^i$/) {
		command(qq{getint "$default" "b set $cmd" "$prompt"});
	}
	else {
	command(qq{getstr "$default" "b set $cmd" "$prompt"});
	
	}
}

sub gui_menu {
	command(qq{menu -p4 ADD "File-_Butler"});
	command(qq{menu -t0 ADD "File-Butler/Enabled" "b on" "b off"});
	command(qq{menu ADD File-Butler/-});
	command(qq{menu ADD "File-Butler/_Kill User" "b kill"});
	command(qq{menu ADD "File-Butler/_List Users" "b user"});
	command(qq{menu ADD "File-Butler/_User Stats" "b stat"});
	command(qq{menu ADD "File-Butler/_Credit" "b credit"});
	command(qq{menu ADD "File-Butler/S_tatus" "b status"});
	command(qq{menu ADD "File-Butler/Show C_onfiguration" "b options"});
	command(qq{menu ADD "File-Butler/_Server Set"});
	command(qq{menu ADD "File-Butler/Server Set/_Trigger" "bset s trigger"});
	command(qq{menu ADD "File-Butler/Server Set/_Channel" "bset s channel"});
	command(qq{menu ADD "File-Butler/Server Set/_Home" "bset s home"});
	command(qq{menu ADD "File-Butler/Server Set/_Description" "bset s description"});
	command(qq{menu ADD "File-Butler/Server Set/_User Max" "bset i user_max"});
	command(qq{menu ADD "File-Butler/Server Set/_Get Max" "bset i get_max"});
	command(qq{menu ADD "File-Butler/Server Set/_Send Max" "bset i send_max"});
	command(qq{menu ADD "File-Butler/Server Set/_Queue Max" "bset i queue_max"});
	command(qq{menu ADD "File-Butler/Server Set/Starting C_redit" "bset s credit"});
	command(qq{menu ADD "File-Butler/Server Set/R_atio" "bset i ratio"});
	command(qq{menu -t$duplicates ADD "File-Butler/Server _Set/Duplicates" "b set duplicates on" "b set duplicates on"});
	command(qq{menu ADD "File-Butler/Server _Set/_Idle" "bset i idle"});
	command(qq{menu -t$advertise ADD "File-Butler/Server _Set/Advertise" "b set advertise 1" "b set advertise 0"});
	command(qq{menu ADD "File-Butler/Server _Set/Ad Ti_me" "bset i ad_time"});
	command(qq{menu ADD "File-Butler/Server _Set/Ru_les" "bset s rules"});
	command(qq{menu ADD File-Butler/-});
	command(qq{menu ADD "File-Butler/Save" "b save"});
	command(qq{menu ADD "File-Butler/Backup" "b backup"});
	command(qq{menu ADD "File-Butler/Restore" "b restore"});
	command(qq{menu ADD File-Butler/-});
	command(qq{menu ADD "File-Butler/Reset" "b reset"});

	return EAT_ALL;
}

}
