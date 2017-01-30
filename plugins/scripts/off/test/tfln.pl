#!/usr/bin/perl

# tfln.pl - Query textsfromlastnight.com and return a random result
#
#[20:45:08] <Flash_> !tfln !text
#
# For irssi
# v.001 - initial release

use strict;
use Irssi;
use LWP::UserAgent;
use vars qw($VERSION %IRSSI);

$VERSION = '001';
%IRSSI = (
    authors     => 'Flash',
    contact     => 'flash@digdilem.org',
    name        => 'tfln',
    description => 'Return stuff from textsfromlastnight.com',
    license     => 'GNU General Public License 3.0' );

sub query_tfln {
	my ($server,$msg,$nick,$address,$target) = @_;
	my @words = split(/ /,$msg);
	if ( (lc($words[0]) eq '!tfln') or (lc($words[0]) eq '!text') ) {
		my $ua = LWP::UserAgent->new;
		$ua->agent("aFlashbot/001 ");
		my $request = HTTP::Request->new(GET => "http://www.textsfromlastnight.com/Random-Texts-From-Last-Night.html");
		my $result = $ua->request($request);
		my $content = $result->content;
		#
#		$content =~ s/<\/a><a href=(.*?) class="tflnlink">//g; # Stupid mid-links
#</a><a href="/miscellaneous/1167512" class="tflnlink">

		$content =~ /<p><a href="(.*?)">(.*?)<\/a><\/p>/;

		my $tfln = $2;
		# Some prep
#		$tfln =~ s/&\#39;/'/g;
#		$tfln =~ s/\\//g;

		if (!defined $tfln) { $server->command("/msg $target TFLN: Sorry - pattern match failed. Something broke."); }
			else
			{ $server->command("/msg $target $tfln"); }
	} # End, message wasn't for me.
}

Irssi::signal_add('message public','query_tfln');
Irssi::signal_add('message own_public','query_tfln');
