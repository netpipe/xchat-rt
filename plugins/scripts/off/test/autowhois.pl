#!/usr/bin/perl -w

use strict;

Xchat::register ("Xchat2 autowhois script", "0.1.0", "", "");
Xchat::hook_server( 'NOTICE', 'whois');

my %nick;
my $timeout = 10000; # timer in ms
my $whoistext = "WHOIS on you requested by "; # text before nick with space

sub whois {
if ($_[1][1] =~ m/$whoistext(\S*)/o) {
my $whom = $1;
return if exists $nick{$whom};
Xchat::command("whois ".$whom." ".$whom);
$nick{$whom}++;
Xchat::hook_timer( $timeout, 'del', $whom )
}
}

sub del
{
delete $nick{$_[0]};
}
