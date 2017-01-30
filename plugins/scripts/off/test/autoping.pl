#!/usr/bin/perl
#
# X Chat Autoping Script
# By Steve Grecni (gid[AT]gifpaste.net)
#
# Description:
# Type "!autoping <ip>" in a channel and all people running this script, or a
# compatible script will return a response.  Helpful for determining server for
# clan matches, or simply finding out how latent a host is.
# Type "/autoping <host/ip>" for a manual run.
#
# Requirements:
# debian package libnet-ping-perl  (Net::Ping)
#
# TODO:
# Run this in another thread somehow so it doesn't lock up X-Chat while the
# ping is in progress.

IRC::register("autoping", "0.2", "", "");
IRC::print("\0035Loading autoping 0.2...\003 \n");
IRC::add_message_handler("PRIVMSG", "privmsg_autoping");
IRC::add_command_handler("autoping", "echo_autoping");

use Net::Ping;
use Time::HiRes qw( usleep ualarm gettimeofday tv_interval );

sub privmsg_autoping {
	my $line = shift(@_);
	$line =~ m/\:(.*?)\!(.*?)\sPRIVMSG\s(.*?)\s\:!autoping\s([0-9a-z\-\.]+)?/;
	if($3 && $4) {
		my $text = do_autoping($4);
		IRC::print("$text\n");
		IRC::command("/raw PRIVMSG $3 :\001ACTION $text\001\n");
	}
}

sub echo_autoping {
	my $host = shift;
	IRC::print do_autoping("$host") ."\n";
	return 1;
}

sub do_autoping {
	my ($host) = @_;
	my $numpings = 8;
	$p = Net::Ping->new('external');
	$p->hires();
		
	my $total = 0;
	my $pingsret = 0;
	my $max = 0;
	my $min = 0;
	my $theip;
	for(my $i=0; $i < $numpings; $i++) {
		my ($ret, $t, $ip) = $p->ping($host, .8);
		if($ret) {
			if($t > $max) { $max = $t; }
			if($t < $min || $min == 0) { $min = $t; }
			$theip = $ip;
			$total += $t;
			$pingsret++;
			usleep(750_000);
		}
	}
	#$p->close();
	my $tr;
	my $perc = sprintf('%.1f', 100 - ($pingsret / $numpings * 100));
	if($total) {
		my $avg = sprintf('%.2f', 1000 * $total / $pingsret);
		my $min = sprintf('%.2f', 1000 * $min);
		my $max = sprintf('%.2f', 1000 * $max);
		my $desc, $color;
		if($avg > 150) {
			$desc = '4HPB';
		} elsif ($avg > 120) {
			$desc = '4BAD';
		} elsif($avg > 90) {
			$desc = '4POOR';
		} elsif($avg > 60) {
			$desc = '3OK';
		} elsif($avg > 30) {
			$desc = '3GOOD';
		} elsif($avg > 10) {
			$desc = '3GREAT';
		} else {
			$desc = '3OMG THIS SERVER MUST BE HELLA CLOSE';
		}
		$tr = "($min - $max) $avg ms $desc";
	} else {
		$tr = '4UNPLAYABLE';
		$theip = $host;
	}
	return "4Ping stats:7 $theip 10$perc\x25 loss $tr";
}
