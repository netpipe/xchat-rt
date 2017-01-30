#!/usr/bin/perl
use warnings;

my $sname="DMS-Whois";
my $version="1.2.5";

Xchat::register($sname, $version, "DMS");

Xchat::print "\n\n\00312.::    DMS-Whois    ::.\003\n";
Xchat::print "\00312:::  Version $version  :::\003\n";
Xchat::print "\00312:::    © DMS '05    :::\003\n\n";

my $whois_started=0;
my $themefile=Xchat::get_info("xchatdir");
$themefile="$themefile/DMS-whois.theme";

Xchat::printf("themefile: %s",$themefile);

&loadtheme;


Xchat::hook_server('RAW LINE','rawHandler',{help_text => 'checks rawlines'});
Xchat::hook_command('setwhois','setCfg',{help_text => 'change settings for DMS-Whois'});

sub readFile{
        $path=shift(@_);
        if (!open(DB, "<","$path")){Xchat::print("file not found: $path");return "";}
        @stats=stat(DB);
		my $data;
        read (DB,$data,$stats[7]);
        close (DB);
        return $data;
}


sub loadtheme{
    Xchat::print("loading theme");
    foreach $line (split "\n", readFile($themefile)){ #`$dbCmd $themefile`){
        my($var0, $value)=(split "=", $line);
        $VAR{$var0}=$value;
    }
    if (!$VAR{PREFIX}){$VAR{PREFIX}="\0034»\003 \002"}
    if (!$VAR{SEPERATOR}){$VAR{SEPERATOR}="\002: "}
    if (!$VAR{POSTFIX}){$VAR{POSTFIX}=""}
    if (!$VAR{HEADER}){$VAR{HEADER}=" ___\00314[\00315\002whois\002\00314]\003____________________________"}
    if (!$VAR{FOOTER}){$VAR{FOOTER}="¯¯¯\00314[\003\00315\002end whois\002\00314]\003¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯"}
	if (!$VAR{WHOWAS}){$VAR{WHOWAS}=" ___\00314[\00315\002whowas\002\00314]\003___________________________"}
}

sub setCfg{
		$name=uc($_[0][1]);
		$val=$_[1][2];
		if (($name eq "HELP") or ($name eq "")){
			Xchat::print("available themesettings");
			Xchat::print("  PREFIX - part in front of information title");
			Xchat::print("  SEPERATOR - part between title and information");
			Xchat::print("  POSTFIX - part after information");
			Xchat::print("  HEADER - printed above list of informations");
			Xchat::print("  FOOTER - printed after complete list of informations");
			Xchat::print("  WHOWAS - printed as header above on whowas");
			return Xchat::EAT_ALL;
		}
                $c3=chr(3);$val=replace($val,"\\003",$c3);$val=replace($val,"%C",$c3);
                $c2=chr(2);$val=replace($val,"\\002",$c2);$val=replace($val,"%B",$c2);
                $c26=chr(26);$val=replace($val,"\\026",$c26);$val=replace($val,"%R",$c26);
                $c17=chr(17);$val=replace($val,"\\017",$c17);$val=replace($val,"%O",$c17);
                $c37=chr(37);$val=replace($val,"\\037",$c37);$val=replace($val,"%U",$c37);
                $VAR{$name}=$val;
		$db= readFile($themefile);#`$dbCmd $themefile`;
		open (DB, ">$themefile");
                foreach $line(split "\n", $db){
                        my($var,$value)=(split "=", $line);
                        print DB "$var=$value\n" if ($var ne $name);
                }
                print DB "$name=$VAR{$name}\n";
                close(DB);
                IRC::print("\00312.:: $name changed to $val ::.\003\n");
		return Xchat::EAT_ALL;
        }

sub rawHandler {
	@words=$_[0];
	my $code=$_[0][1];
	return Xchat::EAT_NONE if(isNumeric($code)==0);
	#Xchat::printf("code: %s",$code);
	$count=0;
	do{
		$val=shift(@words);
		$count++;
	}while ($val);
	
	if ($code==311) {
		Xchat::printf("%s",$VAR{HEADER}) if ($VAR{HEADER} ne "");
		Xchat::printf("%sNickname%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][3],$VAR{POSTFIX});
		Xchat::printf("%sName%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},substr($_[1][7],1),$VAR{POSTFIX});
		Xchat::printf("%sHostmask%s%s@%s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][4],$_[0][5],$VAR{POSTFIX});
		$whois_started=1;
		return Xchat::EAT_ALL;
	}
	
	if ($code==314) {
		Xchat::printf("%s",$VAR{WHOWAS}) if ($VAR{WHOWAS} ne "");
		Xchat::printf("%sNickname%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][3],$VAR{POSTFIX});
		Xchat::printf("%sName%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},substr($_[0][7],1),$VAR{POSTFIX});
		Xchat::printf("%sHostmask%s%s@%s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][4],$_[0][5],$VAR{POSTFIX});
		$whois_started=1;
		return Xchat::EAT_ALL;
	}
	
	if ($code==318) {#whois footer
		Xchat::printf("%s",$VAR{FOOTER})if ($VAR{FOOTER} ne "");
		$whois_started=0;
		return Xchat::EAT_ALL;
    }
	
	if ($code==369) {#whowas footer
		Xchat::printf("%s",$VAR{FOOTER})if ($VAR{FOOTER} ne "");
		$whois_started=0;
		return Xchat::EAT_ALL;
    }
	
	if ($code==401){ #nosuch nick/chanel whois
		Xchat::printf("%s",$VAR{HEADER}) if ($VAR{HEADER} ne "");
		Xchat::printf("%sInfo%s%s %s%s",$VAR{PREFIX},$VAR{SEPERATOR},substr($_[1][4],1),$_[0][3],$VAR{POSTFIX});
		$whois_started=0;
		return Xchat::EAT_ALL;
	}
	
	if ($code==406){ #no such nick whowas
		Xchat::printf("%s",$VAR{WHOWAS}) if ($VAR{WHOWAS} ne "");
		Xchat::printf("%sInfo%s%s %s%s",$VAR{PREFIX},$VAR{SEPERATOR},substr($_[1][4],1),$_[0][3],$VAR{POSTFIX});
		$whois_started=0;
		return Xchat::EAT_ALL;
	}
	
	return Xchat::EAT_NONE if ($whois_started==0);
	
	if ($code==301) {
                #Xchat::printf("\0034»\003 \002\0034away\003\002: \0034 %s \003",substr($_[0][4],1));
		Xchat::printf("%saway%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},substr($_[1][4],1),$VAR{POSTFIX});
		return Xchat::EAT_ALL;
        }

	if ($code==307) {
		Xchat::printf("%sNickstate%s%s %s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][3],substr($_[1][4],1),$VAR{POSTFIX});
		return Xchat::EAT_ALL;
	}

	if ($code==310) {# should be helpOp
                Xchat::printf("%sHelpOp%s%s %s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][3],substr($_[1][4],1),$VAR{POSTFIX});
                return Xchat::EAT_ALL;
        }

	if ($code==312) {
		Xchat::printf("%sServer%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},substr($_[0][4],0,length($_[0][4])),$VAR{POSTFIX});
		return Xchat::EAT_ALL;
	}
	if ($code==313) {
		Xchat::printf("%sStatus%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},substr($_[1][6],0,length($_[1][6])),$VAR{POSTFIX});
                return Xchat::EAT_ALL;
    }
	
	if ($code==317) {
                Xchat::printf("%sTime idle%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},dif_str($_[0][4]),$VAR{POSTFIX});
		Xchat::printf("%sTime online%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},dif_str(time - $_[0][5]),$VAR{POSTFIX}); # does not work @ MacOS
		return Xchat::EAT_ALL;
    }
    if ($code==319) {
        Xchat::printf("%sChannels%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},substr($_[1][4],1),$VAR{POSTFIX});
		return Xchat::EAT_ALL;
	}
	if ($code==320) {
                Xchat::printf("%sSpecial%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},substr($_[1][4],1),$VAR{POSTFIX});
                return Xchat::EAT_ALL;
        }

	if ($code==330) { #not in rfc therefor unknown format :(
                Xchat::printf("%saccount%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][4],$VAR{POSTFIX});#$+ $3 $+
		# return Xchat::EAT_ALL;
	}
	if ($code==338) { #not in rfc therefor unknown format :(
                Xchat::printf("%saddress stuff%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][4],$VAR{POSTFIX}); #$3-
		# return Xchat::EAT_ALL;
	}
	
	if ($code==378) {
		Xchat::printf("%sConnection%s%s %s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][3],substr($_[1][4],1),$VAR{POSTFIX});
		return Xchat::EAT_ALL;
    }
	if ($code==379) {
                Xchat::printf("%sUsermodes%s%s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[1][7],$VAR{POSTFIX});
                return Xchat::EAT_ALL;
        }
	if ($code==671) {
		Xchat::printf("%sSSL%s%s %s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][3],substr($_[1][4],1),$VAR{POSTFIX});
        	return Xchat::EAT_ALL;
	}

	if ($code==335) {
		Xchat::printf("%sBot%s%s %s%s",$VAR{PREFIX},$VAR{SEPERATOR},$_[0][3],substr($_[1][4],1),$VAR{POSTFIX});
                return Xchat::EAT_ALL;
	}

	return Xchat::EAT_NONE;
}

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
	
	sub isNumeric{
		my $num=shift(@_);
		for ($i=0;$i<length($num);$i++){
			return 0 if (ord(substr($num,$i))<48);
			return 0 if (ord(substr($num,$i))>57);
		}
		return 1;
	}
