#!/usr/bin/perl
use warnings;

my $version="1.1.0";
Xchat::register("Away-2005", $version, "DMS",sub{&unload();});

Xchat::print("\n\n\00312.::    Away 2005    ::.\003\n");
Xchat::print("\00312:::  Version $version  :::\003\n");
Xchat::print("\00312:::    © DMS '05    :::\003\n\n"); 

my $awayconf; my $nick; my $away_show_message;
my $goneTime=0; my $idle=0;
my $msgGone; my $repeat; my $goneTimeS;my $goneDateS;
my $backtext="Back";my $awaytext="Away";
my @hooks=(); my %sNicks=();

Xchat::hook_command('goaway','goaway',{help_text => 'usage: /goaway [reson] - go back with reason or default if no reason given'});
Xchat::hook_command('awayStatus','showAwayStatus',{help_text => 'print overview over settings and theme'});
Xchat::hook_command('goback','goback',{help_text => 'come back from away mode'});
Xchat::hook_command('awaySet','awaySet',{help_text => '/awayset [name [value]]'});
Xchat::hook_command('awayReloadTheme','loadTheme',{help_text => 'reload away theme'});
Xchat::hook_print("Your Message",msgSend);
Xchat::hook_print("Connected",hookConnected);

Xchat::hook_print("Channel Msg Hilight",msgHilight);
Xchat::hook_print("Channel Action Hilight",actionHilight);
#Xchat::hook_print("Private Message to Dialog",privateMsg);

&init();

sub unload{
	Xchat::command("set away_show_message ON") if ($away_show_message==1);
	if ($VAR{AUTOBUTTON} eq "ON"){
		Xchat::command("delbutton \"$backtext\"");
		Xchat::command("delbutton \"$awaytext\"");
	}
}

sub setButton{
	$name=shift(@_);
	Xchat::command("delbutton \"$backtext\"");
	Xchat::command("delbutton \"$awaytext\"");
	if ($name eq "away"){Xchat::command("addbutton \"$awaytext\" GETSTR \"\" goaway Away-Reason");}
	if ($name eq "back"){Xchat::command("addbutton \"$backtext\" goback");}
}

sub readFile{
        $path=shift(@_);
        if (!open(DB, "<","$path")){Xchat::print("file not found: $path");return "";}
        @stats=stat(DB);
		my $data;
        read (DB,$data,$stats[7]);
        close (DB);
        return $data;
}

sub printSetHelp{
	Xchat::print("\n\n\002Available Settings for Away2005:");
	Xchat::print("\t\tAWAYNICK \037nick\017 -> Change awaynick to \037nick\017");
	Xchat::print("\t\tAWAYMSG \037msg\017 - Change default awaymessage to \037msg\017 (for /goaway)");
	Xchat::print("\t\tAUTOAWAYMSG \037msg\017 - Change awaymessage for autoaway to \037msg\017");
	Xchat::print("\t\tBACKONINPUT \037[ON/OFF]\017 - enable/disable back on input");
	Xchat::print("\t\tAUTOREPEAT \037[ON/OFF}\017 - enable/disable repeating of awaymessage");
	Xchat::print("\t\tAUTOAWAY \037[ON/OFF]\017 - enable/disable autoaway when idle");
	Xchat::print("\t\tREPEATTIME \037mins\017 - sets time to repeat awaymessage to \037mins\017 minutes");
	Xchat::print("\t\tIDLETIME \037mins\017 - sets time for autoaway to \037mins\017 minutes");
	Xchat::print("\t\tLOG \037[ON/OFF}\017 - enable/disable loging of hilight messages/actions");
	Xchat::print("\t\tANOUNCE \037[ON/OFF}\017 - enable/disable chan anounce of away message");
	Xchat::print("\t\tUSERMSG \037[ON/OFF}\017 - enable/disable messaging user on hilight or private message");
	Xchat::print("\t\tUSENOTICE \037[ON/OFF}\017 - enable/disable notice instead of private message");
	Xchat::print("\t\tAUTOBUTTON \037[ON/OFF}\017 - enable/disable user buttons");
	Xchat::print("\n\002Theme Settings for Away2005:");
	Xchat::print("\t\tAWAYLINEFIRST - sets output for first anounce");
	Xchat::print("\t\tAWAYLINE - sets output for repeated message");
	Xchat::print("\t\tBACKLINE - sets output when coming back");
	Xchat::print("\t\tINFOLINE - info message\n\n");
	
}

sub printSet{
	my $field=shift(@_);
	Xchat::print("$field = $VAR{$field}");
}


sub themeSet{
	my $field=shift(@_);
	my $val=shift(@_);
	$VAR{$field}=$val;
		$db= readFile($awaytheme);
		open (DB, ">$awaytheme");
		foreach $line(split "\n", $db){
        	my($var,$value)=(split "=", $line);
            print DB "$var=$value\n" if ($var ne $field);
		}
        print DB "$field=$VAR{$field}\n";
        close(DB);
        IRC::print("\00312.:: $field changed to $val ::.\003\n");
}

sub awaySet{
	if (!$_[0][1]){printSetHelp();return Xchat::EAT_ALL;}
	my $field=uc($_[0][1]);
	if (($field eq "AWAYLINEFIRST")||($field eq "AWAYLINE")||($field eq "BACKLINE")||($field eq "INFOLINE")){
		if ($_[0][2] eq ""){printSet($field);return Xchat::EAT_ALL;}
		my $val=$_[1][2];
		themeSet($field,$val);
		return Xchat::EAT_ALL;
	}
	if (($field eq "AWAYNICK")||($field eq "AWAYMSG")||($field eq "AUTOAWAYMSG")||($field eq "BACKONINPUT")||($field eq "AUTOREPEAT")||($field eq "AUTOAWAY")||($field eq "REPEATTIME")||($field eq "IDLETIME") ||($field eq "ANOUNCE")||($field eq "USERMSG")||($field eq "USENOTICE") ||($field eq "LOG")||($field eq "AUTOBUTTON")){
		if ($_[0][2] eq ""){printSet($field);return Xchat::EAT_ALL;}
		my $val=$_[1][2];
		if (($field eq "BACKONINPUT")||($field eq "AUTOREPEAT")||($field eq "AUTOAWAY")||($field eq "ANOUNCE")||($field eq "USERMSG")||($field eq "USENOTICE")||($field eq "LOG")||($field eq "AUTOBUTTON") ){ #BOOLEAN SETTING
			if ((uc($val)eq "ON") || ($val eq "1") || (uc($val)eq "YES")){$val="ON";}
			else{
				if ((uc($val)eq "OFF") || ($val eq "0") || (uc($val)eq "NO")){$val="OFF";}
				else{Xchat::print("invalid value for this setting");return Xchat::EAT_ALL;}
			}
		}
		
		if (($field eq "REPEATTIME")||($field eq "IDLETIME")){ #numeric setting
			if (int($val)==0){Xchat::print("invalid value for this setting");return Xchat::EAT_ALL;}
		}
		### SETTING VALID SO WRITE TO CONF
		$VAR{$field}=$val;
		$db= readFile($awayconf);
		open (DB, ">$awayconf");
		foreach $line(split "\n", $db){
        	my($var,$value)=(split "=", $line);
            print DB "$var=$value\n" if ($var ne $field);
		}
        print DB "$field=$VAR{$field}\n";
        close(DB);
        IRC::print("\00312.:: $field changed to $val ::.\003\n");
		if (($field eq "AUTOAWAY") &&($val eq "ON") && (!Xchat::get_info("away"))){push @hooks, Xchat::hook_timer(60000,"idleTimer");$idle=0;}
		if ($field eq "AUTOBUTTON"){
			if ($val eq "ON"){if ($goneTime>0){setButton("back");}else {setButton("away");}}
			else{Xchat::command("delbutton \"$awaytext\"");Xchat::command("delbutton \"$backtext\"");}
		}
		removeHooks() if (($field eq "AUTOAWAY") &&($val eq "OFF") && (!Xchat::get_info("away"))) ;
		push @hooks, Xchat::hook_timer(60000,repeatHandler) if (($field eq "AUTOREPEAT") &&($val eq "ON") && (Xchat::get_info("away"))) ;
		removeHooks() if (($field eq "AUTOREPEAT") &&($val eq "OFF") && (Xchat::get_info("away"))) ;
	}
	else{
		Xchat::print("Invalid setting. try /awayset for list of available settings");
	}
	return Xchat::EAT_ALL;
}


sub loadTheme{
	foreach $line (split "\n", readFile($awaytheme)){
		my($var0, $value)=(split "=", $line);
		$value=replace($value,"%C",chr(3));
		$value=replace($value,"%B",chr(2));
		#$value=replace($value,"%U",chr(37));
		#$value=replace($value,"%O",chr(17));
		#$value=replace($value,"%R",chr(26));
		$VAR{$var0}=$value;
	}
	if (!$VAR{BACKLINE}){$VAR{BACKLINE}="is back and was gone for <gone>, reason <reason>"}
	if (!$VAR{AWAYLINEFIRST}){$VAR{AWAYLINEFIRST}="is away reason: <reason>"}
	if (!$VAR{AWAYLINE}){$VAR{AWAYLINE}="is away reason: <reason> gone: <gone> ago"}
	if (!$VAR{INFOLINE}){$VAR{INFOLINE}="is away reason: <reason> gone: <gone> ago"}
}


sub init(){
	$away_show_message=Xchat::get_prefs("away_show_message");
	if ($away_show_message==1){
		Xchat::print("\002disabling away anounce by X-Chat (will be enabled again when unloading)");
		Xchat::command("set away_show_message OFF");
	}
	$awayconf=Xchat::get_info("xchatdir");
	$awayconf="$awayconf/Away2005.cfg";
	$awaytheme=Xchat::get_info("xchatdir");
	$awaytheme="$awaytheme/Away2005.theme";
	$nick=Xchat::get_info("nick" );
	$goneTime=time if (Xchat::get_info("away"));
	$goneTimeS=timeStr();
	$goneDateS=dateStr();
	#$awayTime=0;
	$idle=0;
	foreach $line (split "\n", readFile($awayconf)){
		my($var0, $value)=(split "=", $line);
		$VAR{$var0}=$value;
	}
	if (!$VAR{AWAYNICK}){$VAR{AWAYNICK}="iAmAway"}
	if (!$VAR{AWAYMSG}){$VAR{AWAYMSG}="simple away"}
	if (!$VAR{BACKONINPUT}){$VAR{BACKONINPUT}="ON"}
	if (!$VAR{AUTOREPEAT}){$VAR{AUTOREPEAT}="ON"}
	if (!$VAR{REPEATTIME}){$VAR{REPEATTIME}=30}
	if (!$VAR{IDLETIME}){$VAR{IDLETIME}=15}
	if (!$VAR{AUTOAWAY}){$VAR{AUTOAWAY}="OFF"} #"OFF"}
	
	if (!$VAR{ANOUNCE}){$VAR{ANOUNCE}="ON"}
	if (!$VAR{USERMSG}){$VAR{USERMSG}="ON"}
	if (!$VAR{USENOTICE}){$VAR{USENOTICE}="OFF"}
	if (!$VAR{LOG}){$VAR{LOG}="ON"}
	if (!$VAR{AUTOBUTTON}){$VAR{AUTOBUTTON}="OFF"}
	
	if (!$VAR{AUTOAWAYMSG}) {$VAR{AUTOAWAYMSG}="AutoIdle after 15 minutes"};
	&loadTheme();
	if ($VAR{AUTOBUTTON} eq "ON"){
		if ($goneTime>0){setButton("back");}
		else{setButton("away");}
	}
	push @hooks, Xchat::hook_timer(60000,"idleTimer") if ($VAR{AUTOAWAY}eq "ON");
}

sub removeHooks{
	my $hook;
	while((my $hook=pop (@hooks))){Xchat::unhook($hook);}
}

sub sendMsg{
	return if (($VAR{USERMSG}ne"ON")||($goneTime==0));
	my $resiver=shift(@_);
	my $line=fillIn($VAR{INFOLINE});
	if ($VAR{USENOTICE}eq "ON"){Xchat::command("notice $resiver $line");}
	else {
		my $tmp=$VAR{BACKONINPUT};$VAR{BACKONINPUT}="OFF";
		Xchat::command("msg $resiver \002\002$line");
		$VAR{BACKONINPUT}=$tmp;
	}
}

sub msgHilight{
	my $a=Xchat::get_info("away");
	return Xchat::EAT_NONE if ((!Xchat::get_info("away"))||($goneTime<=0)) ;
	my $chan=Xchat::get_info("channel");
	my $netw=Xchat::get_info("network");
	sendMsg($_[0][0]);
	if ($VAR{LOG} eq "ON"){
		Xchat::command("query awaylog",undef,$netw) if (!Xchat::find_context("awaylog",$netw));
		my @ret=();
		push(@ret,"$_[0][0] \[$chan\]");
		push(@ret,"\002\002$_[0][1]");
		push(@ret,"");
		Xchat::set_context("awaylog",$netw);
		Xchat::emit_print('Private Message to Dialog',@ret);
	}
	return Xchat::EAT_NONE;	
}

sub actionHilight{
	return Xchat::EAT_NONE if ((!Xchat::get_info("away"))||($goneTime<=0)) ;
	my $chan=Xchat::get_info("channel");
	my $netw=Xchat::get_info("network");
	sendMsg($_[0][0]);
	if ($VAR{LOG} eq "ON"){
		Xchat::command("query awaylog",undef,$netw) if (Xchat::find_context("awaylog",$netw)==undef);
		my @ret=();
		push(@ret,"$_[0][0] \[$chan\]");
		push(@ret,$_[0][1]);
		Xchat::set_context("awaylog",$netw);
		Xchat::emit_print('Channel Action',@ret);
	}
	return Xchat::EAT_NONE;	
}

sub goaway{goawaySub($_[1][1]);}
	
sub goawaySub{
	my $msg=shift(@_);
	$msg=$VAR{AWAYMSG} if (!$msg);
	if (Xchat::get_info("away")){
		Xchat::print("you are still marked being away");
		return Xchat::EAT_ALL;
	}
	$goneTime=time;
	$goneTimeS=timeStr();
	$goneDateS=dateStr();
	$msgGone=$msg;
	printAway($msg) if ($VAR{ANOUNCE} eq "ON");
	removeHooks();
	push @hooks, Xchat::hook_timer(60000,repeatHandler) if ($VAR{AUTOREPEAT} eq "ON");
	$repeat=0;
	###save nick for all servers
	my @chans = Xchat::get_list("channels");
	%sNicks=();
	foreach $chan (@chans){
		my $network=$chan->{"network"};
		if (!exists $sNicks{$network}){
			Xchat::set_context(undef,$network); #SERVER
			my $sNick=Xchat::get_info("nick");
#Xchat::print("Adding nick $sNick to list for nw $network and go away with reason $msg");
			$sNicks{$network}=$sNick;
			Xchat::command("nick $VAR{AWAYNICK}",undef,$network);
			Xchat::command("away $msg",undef,$network);
		}
		#else {Xchat::print("207");}
	}
	setButton("back") if ($VAR{AUTOBUTTON} eq "ON");
	#################
	return Xchat::EAT_ALL;
}

sub printAway{
	$msg = shift(@_);
	my $mTime=time;
	my $line;
	if ($mTime == $goneTime){ #just gone
		$line=fillIn($VAR{AWAYLINEFIRST});
	}
	else{$line=fillIn($VAR{AWAYLINE});}
	Xchat::command("ALLCHAN action $line");
}

sub goback{
	if (!Xchat::get_info("away")){
		Xchat::print("you are not marked being away");
		return Xchat::EAT_ALL;
	}
	#if ($goneTime>0){$tdif=dif_str(time-$goneTime);} else {$tdif="?";}
	my @chans = Xchat::get_list("channels");
	my $line=fillIn($VAR{BACKLINE});
	##my @sDone=();##################
	foreach $chan (@chans){
		my $network=$chan->{"network"};
		Xchat::set_context(undef,$network); #?SERVER
		#if (!exists $sDone{$network}){############
		if (exists $sNicks{$network}){
			my $sNick=$sNicks{$network};
			if ($sNick ne Xchat::get_info("nick")){Xchat::command("nick $sNick");}
		}
		if (Xchat::get_info("away")){
			Xchat::command("away") ;
			Xchat::command("allchanl action $line") if ($VAR{ANOUNCE} eq "ON");
		}
	}
	$goneTime=0;
	removeHooks();
	$idle=0;
	setButton("away") if ($VAR{AUTOBUTTON} eq "ON");;
	push @hoocks, Xchat::hook_timer(60000,"idleTimer") if ($VAR{AUTOAWAY}eq "ON");
	return Xchat::EAT_ALL;
}

sub idleTimer{
	$idle++;
	#Xchat::print("idle since $idle minutes");
	if (($idle==$VAR{IDLETIME}-10)&&(!Xchat::get_info("away"))&&($VAR{AUTOAWAY}eq "ON")){Xchat::print("will be go autoaway in 10 minutes");}
	if (($idle==$VAR{IDLETIME}-5)&&(!Xchat::get_info("away"))&&($VAR{AUTOAWAY}eq "ON")){Xchat::print("will be go autoaway in 5 minutes");}
	if (($idle==$VAR{IDLETIME}-1)&&(!Xchat::get_info("away"))&&($VAR{AUTOAWAY}eq "ON")){Xchat::print("will be go autoaway in 1 minute");}
	if (($idle>=$VAR{IDLETIME})&&(!Xchat::get_info("away"))&&($VAR{AUTOAWAY}eq "ON")){
		goawaySub($VAR{AUTOAWAYMSG});
	}
	return Xchat::KEEP if (($VAR{AUTOAWAY}eq "ON")&&(!Xchat::get_info("away")));
	#Xchat::print("removing idle timer");
	return Xchat::REMOVE;
}

sub repeatHandler{
	$repeat++;
	if ($repeat>=int($VAR{REPEATTIME})){
		printAway($msgGone) if ((Xchat::get_info("away")) &&($VAR{ANOUNCE} eq "ON"))  ;
		$repeat=0;
	}
	return Xchat::KEEP if (Xchat::get_info("away"));
	#Xchat::print("removing repeat timer");
	return Xchat::REMOVE;
}

sub hookConnected{
	Xchat::hook_timer(60000,connectedTimer) if ($goneTime>0);
	return Xchat::EAT_NONE;
}

sub connectedTimer{
	return Xchat::REMOVE if ($goneTime<=0); 
	my @chans = Xchat::get_list("channels");
	my $ok=1;#false;
	my %networks=();
	foreach $chan (@chans){
		my $network=$chan->{"network"};
		if ((exists $sNicks{$network})&&(!exists $networks{$network})) {
			#Xchat::print("Handle reconnect for $network");
			if (Xchat::get_info("nick")ne $VAR{AWAYNICK}){
				$ok=0;Xchat::command("nick $VAR{AWAYNICK}",undef,$network);
			}
			if (!Xchat::get_info("away")){
				$ok=0;Xchat::command("away $msgGone",undef,$network);
			}
			$networks{$network}="TRYED";
			#Xchat::print("Status: $ok\n\n");
		}
	}
	if ($ok==1){return Xchat::REMOVE;}
	Xchat::print("\002Handle Reconnect failed. Tring again later.");
	return Xchat::KEEP;
}

sub msgSend{
	goback() if (($VAR{BACKONINPUT}eq"ON")&&(Xchat::get_info("away")));
	$idle=0;
	return Xchat::EAT_NONE;
}

#sub fillIn ($line){
sub fillIn{
	my $line=shift(@_);
	my $dif="";
	if ($goneTime>0){$dif=dif_str(time-$goneTime);} else {$dif="?";}
	$line=replace($line,"<reason>",$msgGone);
	$line=replace($line,"<gone>",$dif);
	$line=replace($line,"<time>",$goneTimeS);
	$line=replace($line,"<date>",$goneDateS);
	$line=replace($line,"<log>",lc($VAR{LOG}));
	$line=replace($line,"<LOG>",uc($VAR{LOG}));
	$line=replace($line,"<Log>",ucfirst(lc($VAR{LOG})));
	return $line;	
}

#sub dif_str($dif){
sub dif_str{
	$dif=shift(@_);
    $m=0;$h=0;$d=0;
    if ($dif>=86400){$d=int($dif/86400);$dif=$dif-$d*86400;}
    if ($dif>=3600){$h=int($dif/3600);$dif=$dif-$h*3600;}
    if ($dif>=60){$m=int($dif/60);$dif=$dif-$m*60;}
    $s=$dif;
    return "$d days $h hrs $m mins $s secs" if ($d>0);
    return "$h hrs $m mins $s secs" if ($h>0);
    return "$m mins $s secs" if ($m>0);
    return "$s secs";
}

sub timeStr{
	my @lt= localtime(time);
	my $ret=sprintf("%.2i:%.2i:%.2i",$lt[2],$lt[1],$lt[0]);
	return $ret;
}

sub dateStr{
	my @lt= localtime(time);
	my $ret=sprintf("%.2i.%.2i.%i",$lt[3],$lt[4]+1,$lt[5]+1900);
	return $ret;
}

sub showAwayStatus{
	Xchat::print ("\n\n\00312.:: Away-2005 ::.\003\n");
    Xchat::print ("\00312:::    © DMS '05    :::\003\n");
    Xchat::print ("    Away-Nick: $VAR{AWAYNICK}\n");
    Xchat::print ("    Away-Msg: $VAR{AWAYMSG}\n");
    Xchat::print ("    Back on Input: $VAR{BACKONINPUT}\n");
    Xchat::print ("    Auto-Repeat: $VAR{AUTOREPEAT}\n");
    Xchat::print ("    Repeat-Time: $VAR{REPEATTIME}\n");
	Xchat::print ("    Auto-Away: $VAR{AUTOAWAY}\n");
	Xchat::print ("    Auto-Away-Message: $VAR{AUTOAWAYMSG}\n");
	Xchat::print ("    Idle-Time: $VAR{IDLETIME}\n");
	Xchat::print ("    Announce: $VAR{ANOUNCE}\n");
	Xchat::print ("    User-Message: $VAR{USERMSG}\n");
	Xchat::print ("    User notice: $VAR{USENOTICE}\n");
	Xchat::print ("    Loging: $VAR{LOG}\n");
	Xchat::print ("    Autobuttons: $VAR{AUTOBUTTON}\n");
	#Xchat::print ("    Time since last repeat: $repeat\n") if ($VAR{AUTOREPEAT} eq "ON"); ### for debug only
	#Xchat::print ("    Idle since: $idle mins\n\n") if ($VAR{AUTOAWAY} eq "ON");
	Xchat::print ("    AWAYLINEFIRST: $VAR{AWAYLINEFIRST}\n");
	Xchat::print ("    AWAYLINE: $VAR{AWAYLINE}\n");
	Xchat::print ("    BACKLINE: $VAR{BACKLINE}\n");
	Xchat::print ("    INFOLINE: $VAR{INFOLINE}\n\n");
	
    return Xchat::EAT_ALL;
}

#sub replace($text,$pattern,$newText){
sub replace{
	my $text=shift(@_);
	my $pattern=shift(@_);
	my $newText=shift(@_);
	my $left=""; 
	my $right="";
	while (index($text,$pattern) ne -1){
		$left=substr($text,0,index($text,$pattern));
		$right=substr($text,index($text,$pattern)+length($pattern));
		$text="$left$newText$right";
	}
	return $text;
}
