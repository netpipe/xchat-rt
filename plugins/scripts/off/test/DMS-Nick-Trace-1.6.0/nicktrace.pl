#!/usr/bin/perl
use warnings;

my $sname="DMS-Nick-Trace";
my $version="1.6.0";

Xchat::register($sname, $version, "DMS");

Xchat::print "\n\n\00312.::  DMS-Nick-Trace ::.\003\n";
Xchat::print "\00312:::  Version $version  :::\003\n";
Xchat::print "\00312:::    © DMS '05    :::\003\n\n";

my $outputType=0; #0=Join Message, 1=Notice, 2=Server Notice
#my $lessOut=0;
my $autoAdd=1;
my $minMEE=50;
my $hostmasks=Xchat::get_info("xchatdir");

$hostmasks="$hostmasks/hostmasks.db";

Xchat::hook_print('Join', 'joinHandler',{help_text => 'handles nickchanges'});
Xchat::hook_server('NICK','nickHandler',{help_text => 'handles joins'});
Xchat::hook_command('NickTrace','nicktraceHandler',{help_text => 'check for aliases for given nick'});
Xchat::hook_command('NickTrace_add','nicktraceAddHandler',{help_text => 'usage: /NickTrace_add NICK IDENT HOST/IP'});

sub readFile{
	$path=shift(@_);
	#if (open(DB, "<","$path")==undef){Xchat::print("file not found: $path");return "";}
	if (!open(DB, "<","$path")){Xchat::print("file not found: $path");return "";}
	@stats=stat(DB);
	my $data;
	read (DB,$data,$stats[7]);
	close (DB);
	return $data;
}

sub nicktraceHandler{
	$nick=$_[0][1];
	if (!$nick){Xchat::print("you have to give nick as argument");return Xchat::EAT_ALL;}
	my $data=Xchat::user_info("$nick");
	if (!$data){
		Xchat::print("invalid user");
		return Xchat::EAT_ALL;
	}
	$host=$data->{"host"};
	@parts=split("@",$host);
	$akas=checkAlias($nick,$parts[0],$parts[1]);
	Xchat::printf("$nick$akas");
	return Xchat::EAT_ALL;
}


sub checkAlias{
	$nick=shift(@_);
	$ident=shift(@_);
	$hostmask=shift(@_);
	$aliases="";
	foreach $line (split "\n", readFile($hostmasks)){
		my($n, $i, $h)=(split "=", $line);
		if (($i eq $ident)||($h eq $hostmask)){
			if ($n ne $nick){
				#if (($i eq $ident)&&($h eq $hostmask)){$aliases="$aliases aka. $n" if (index($aliases,"aka. $n")==-1);}
				#else {if ($lessOut==1){$aliases="$aliases (aka. $n)" if (index($aliases,"aka. $nick")==-1);}}
				if (($i eq $ident)&&(mee($h, $hostmask)>=$minMEE)){$aliases="$aliases aka. $n" if (index($aliases,"aka. $n")==-1);}
			}
		}
	}
	return $aliases;
}

sub addAlias{
	$nick=shift(@_);
	$ident=shift(@_);
	$hostmask=shift(@_);
	$db=readFile($hostmasks);
	open(DB, ">$hostmasks");
	foreach $line(split "\n", $db){
		my($n,$i, $h)=(split "=", $line);
		print DB "$n=$i=$h\n" if (($n ne $nick)||($i ne $ident)||($h ne $hostmask));
	}
	print DB "$nick=$ident=$hostmask\n";
	close(DB);
}

sub nicktraceAddHandler{addAlias($_[0][1],$_[0][2],$_[0][3]);Xchat::print("Added to hostmask.db");return Xchat::EAT_ALL;}

sub joinHandler{
	$mask=$_[0][2];
	$nick=$_[0][0];
	$chan=$_[0][1];
	if (index($nick,"\002\002")==0){return Xchat::EAT_NONE;}
	@parts2=split("@",$mask);
	$ident=$parts2[0];
	$hostmask=$parts2[1];
	$alias=checkAlias($nick,$ident,$hostmask);
	&addAlias($nick,$ident,$hostmask)if ($autoAdd==1);
	if ($outputType==0){
		my @ret=();
		push(@ret,"\002\002$nick$alias");push(@ret,$chan);push(@ret,"$mask");
		$rVal=Xchat::emit_print('Join',@ret);
		if ($rVal!=1){ #emit_print fails (why ever)
			Xchat::print("emit_print failed (returned $rVal)");
			Xchat::print("\002$nick$alias\002 ($mask) has joined $chan");
		}
		return Xchat::EAT_ALL;
	}
	if ($outputType==1){ #notice
		my @ret=();
		push(@ret,"nickTrace");push(@ret,"\002\002$nick$alias");
		Xchat::set_context("(notices)");
		$rVal=Xchat::emit_print('Notice',@ret);
		if ($rVal!=1){ #emit_print fails (why ever)
			Xchat::print("emit_print failed (returned $rVal)");
			#Xchat::print("\002$nick$alias\002 ($mask) has joined $chan");
		}
	}
	if ($outputType==2){ #server notice
		my @ret=();
		push(@ret,"\002\002$nick$alias");push(@ret,"nickTrace");
		Xchat::set_context("(snotices)");
		$rVal=Xchat::emit_print('Server Notice',@ret);
		if ($rVal!=1){ #emit_print fails (why ever)
			Xchat::print("emit_print failed (returned $rVal)");
			#Xchat::print("\002$nick$alias\002 ($mask) has joined $chan");
		}
	}
	
	return Xchat::EAT_NONE;
}

sub nickHandler{
	$mask=$_[0][0];
	$nick=substr($_[0][2],1);
	@parts1=split("!",$mask);
	@parts2=split("@",$parts1[1]);
	$ident=$parts2[0];
	$hostmask=$parts2[1];
	addAlias($nick,$ident,$hostmask);
	return Xchat::EAT_NONE;
}

sub mee{
        my $mask1=uc(shift(@_));my $mask2=uc(shift(@_));my $maxLen; my $minLen;
        if (length($mask1)>length($mask2)){$maxLen=length($mask1);$minLen=length($mask2);}
        else {$maxLen=length($mask2);$minLen=length($mask1);}
        for ($i=1;$i<=$minLen;$i++){return int(($i-1)*100/$maxLen) if (substr($mask1,length($mask1)-$i) ne substr($mask2,length($mask2)-$i));}
        return 100;
}
