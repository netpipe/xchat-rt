#!/usr/bin/perl
###############
# dns.pl - simple adds /dns lookups. (The supplied C ones I found crashed Xchat if you added a tab by mistake)
# By Flash_ - 2008 - Gnu3
# v.2 - faster query of hostname, gethostbyaddr
###############
###############
use strict;
#use warnings;
my $version = "001";
Xchat::register( "Flashy's /dns", "$version", "DNS", "" );
Xchat::print("Loaded Flashy's DNS Script v.$version");
Xchat::hook_command( 'dns', \&dns, );
#use Net::DNS; # For hostname lookups
use Geo::IP::PurePerl;
my $gi = Geo::IP::PurePerl->new( 'c:/perl/GeoIP.dat' );

sub dns {
	my $ip = $_[0][1];
#	my $hostname;
	my $country;
	Xchat::print "Checking IP: $ip";
#	my $res = Net::DNS::Resolver->new;
#	my $query = $res->query($ip);
#	if ($query) {
#			foreach my $rr ($query->answer) {
#			next unless $rr->type eq "PTR";
#			$hostname =  $rr->ptrdname;
#		  }
#		}
	my $hostname = ( gethostbyaddr( pack( "C4", split( /\./, $ip) ), 2) )[0];
	if (!defined $hostname) { $hostname = "Unknown"; }
	$country = $gi->country_name_by_addr($ip);
	if (!defined $country) { $country = "Unknown"; }
	Xchat::print("IP:\002 $ip\002 Hostname:\002 $hostname\002 Country:\002 $country\002");
	return Xchat::EAT_ALL;
	}