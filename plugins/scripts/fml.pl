#!/usr/bin/perl

# fml.pl - Query fmylife.com and return a result
#
#[20:45:08] <Flash_> !fml
#
# For irssi

use strict;
use Irssi;
use LWP::UserAgent;
use vars qw($VERSION %IRSSI);

$VERSION = '001';
%IRSSI = (
    authors     => 'Flash',
    contact     => 'flash@digdilem.org',
    name        => 'fml',
    description => 'Return stuff from fmylife',
    license     => 'GNU General Public License 3.0' );

sub query_fml { # use google instead!
	my ($server,$msg,$nick,$address,$target) = @_;
	my @words = split(/ /,$msg);
	if (lc($words[0]) eq '!fml') {
		my $ua = LWP::UserAgent->new;
		$ua->agent("aFlashbot/001 ");
		my $request = HTTP::Request->new(GET => "http://www.fmylife.com/random");
		my $result = $ua->request($request);
		my $content = $result->content;
		#
# summary:'%1',

		#
		$content =~ /summary:'(.*?)',/;
		my $fml = $1;
		# Some prep
		$fml =~ s/&\#39;/'/g;
		$fml =~ s/&quot;/\"/g;
		$fml =~ s/&amp;/&/g;

		$fml =~ s/\\//g;
		if (!defined $fml) { $server->command("/msg $target FML: Sorry - pattern match failed."); }
			else 
			{ $server->command("/msg $target $fml"); }
	} # End, message wasn't for me.
}

IRC::signal_add('message public','query_fml');
IRC::signal_add('message own_public','query_fml');
