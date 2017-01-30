#!/usr/bin/perl

use strict;
use Irssi;
use LWP::UserAgent;
use vars qw($VERSION %IRSSI);
$VERSION = '001i';
%IRSSI = (
    authors     => 'Flash',
    contact     => 'flash@digdilem.org',
    name        => 'Googlefight',
    description => '!googlefight word1 word2',
    license     => 'GNU General Public License 3.0' );

Irssi::signal_add("message public", \&gfight);
Irssi::signal_add("message private", \&gfight);

print("\002Loaded Flash's Googlefight' v.$VERSION\002 (!googlefight word1 word2)");

sub gfight {
    my ($server, $data, $hunter, $mask, $chan) = @_;
	my @words = split(/ /,$data);

	if ((lc($words[0]) eq '!googlefight') or (lc($words[0]) eq '!gfight')) {
		if ((!defined $words[1]) or (!defined $words[2])) { $server->command("msg $chan Usage: !googlefight Word1 Word2"); return; }

		my $ua = LWP::UserAgent->new;
		$ua->agent("aFlashbot/001 ");

		my $req1 = HTTP::Request->new(GET => "http://www.google.com/custom?q=$words[1]");
		my $req2 = HTTP::Request->new(GET => "http://www.google.com/custom?q=$words[2]");
		my $response1 = $ua->request($req1);
		my $response2 = $ua->request($req2);
		my $page1=$response1->content;
		$page1 =~ m/of about <b>(.*?)<\/b>/;
		my $count1 = $1 || 0;
		my $page2=$response2->content;
		$page2 =~ m/of about <b>(.*?)<\/b>/;
		my $count2 = $1 || 0;
		my $num1 = $count1;
		my $num2 = $count2;
		$num1 =~ s/,//g;
		$num2 =~ s/,//g;
		if ($num1 > $num2) {
			$server->command("msg $chan $words[1] is the winner with $count1 hits, beating $words[2] with $count2");

			} elsif ( $num1 < $num2 ) {
			$server->command("msg $chan $words[2] is the winner with $count2 hits, beating $words[1] with $count1");
			} else {
			$server->command("msg $chan Tie! Both $words[1] and $words[2] return $count1 hits!");
			}
		}
	}