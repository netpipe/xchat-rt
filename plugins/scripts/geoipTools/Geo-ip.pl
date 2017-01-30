#!/usr/bin/perl
# By flash http://digdilem.org
## Geo-IP. Public responder to find which country belongs to a country.
#
# Requires the perl modules:
# Net::DNS - To find the hostname of an ip if rdns is present.
# Geo::IP::PurePerl - To do the Geo lookup.
# As well as;
# GeoIP.dat - The file containing the country database. You can get it here:  http://www.maxmind.com/download/geoip/database/GeoIP.dat.gz and other places.

my $trigger_word='!ip'; # What public trigger to respond to. 
my $database_filename = '~/.xchat2/GeoLiteCity.dat'; # Path to the geoip database
my @trigger_chans = qw(\#channel1 \#channel2); # Chans to respond to triggers. 
# End user config

use strict;
use NET::DNS;

Xchat::register( "Flashy's geo-ip", "V.0.1", "geo-ip", "" );
Xchat::hook_print('Channel Message', "geoip");
Xchat::hook_print('Your Message', "geoip"); 
Xchat::print "Started: Flash's Geo-IP 0.1";

use Geo::IP::PurePerl;
my $gi = Geo::IP::PurePerl->new( $database_filename );

sub geoip {
	my $cur_chan = Xchat::get_info('channel');
	if ((grep { $cur_chan =~ /$_/} @trigger_chans)) { # String contains a trigger @		
		$_[0][1] =~ s/\s+/ /g; # Remove multiple spaces	
		my @rowr = split(/ /,$_[0][1]);
#		Xchat::print("Geo-IP: Here ($rowr[0])");	
		if (lc($rowr[0]) eq $trigger_word) { 
			if (not defined $rowr[1]) { Xchat::command("say No IP supplied - use $trigger_word IP"); return Xchat::EAT_NONE; }
			my $ip = $rowr[1];
			
			# Get dns if avail	
			my $rr, my $hostname;
			my $res   = Net::DNS::Resolver->new;
			my $query = $res->query($ip);
			if ($query) {     				
					foreach $rr ($query->answer) {
					next unless $rr->type eq "PTR";
					$hostname =  $rr->ptrdname;						
					}
				}
			# look up IP address '24.24.24.24'
			my $country = $gi->country_name_by_addr($ip);

			Xchat::command("say Geo-IP: $country ($ip / $hostname)");
			}
		}
return Xchat::EAT_NONE;
}
