#!/usr/bin/perl
# Written April 07, 2007 by Chris "ScribbleJ" Jansen
#
# A simple perl CGI script that gets the requestor's IP address, 
# looks it up using the MaxMind database and returns a graphic
# file with the address plotted.
# 
# You will almost certainly have to change the values in the 
# settings hash below.
#
# If your apache config is like mine, the cool way to "install" is:
# ScriptAlias /cgi-bin/whereami.jpg /var/www/scribblej.com/cgi-bin/whereami.pl
# or similar.
# 
# NOTES ON REQUIREMENTS:
#
# MaxMind's "GeoLite City" Database:
# http://www.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz
# MaxMind's Geo::IP::PurePerl module
# On CPAN or: http://www.maxmind.com/download/geoip/api/pureperl
#   These must be installed properly, but do not be fooled.  The proper
#   location for the database is: /usr/local/share/GeoIP/GeoIPCity.dat and ignore
#   the "make test" warnings in the perl install -- they are trying to
#   test against the smaller "country" database which you don't want or need.
# Speaking of requirements, I'm required to state:
# "This product includes GeoLite data created by MaxMind, available from http://www.maxmind.com/."
# It doesn't really include the data though; you have to go get it yourself.
#
# Image::WorldMap
#  An important note!  verion .14 of Image::WorldMap has a bug; no matter
#  what output filename you give it the output will be in the same format
#  as the input.  So if you want a .jpg out, you either have to grab 
#  Image::WorldMap off my website, wait for me to update it on CPAN, or
#  fix it yourself.
#
#  LICENSE:
#
#  This code is free to use; enjoy.  It comes with no warranties, and I'm not
#  responsible for a damn thing it might do or not do.
#
#  If you wish to modify and redistribute it, you may but please send
#  me a note and say you liked it.  Send the note to: scribblej@scribblej.com
#
use Image::WorldMap;
use Geo::IP::PurePerl;
use CGI;
use File::Slurp;
use strict;

my %settings = (
# path to input map
input_map => '/var/www/scribblej.com/cgi-bin/earth-small.png',
# path to cache maps; maps are stored named by ip address of requestor.
output_dir => '/var/whereami/',
# UNLESS YOU HAVE A GREATER VERSION OF IMAGE::WORLDMAP THAN .14 
# IT DOESN'T MATTER WHAT OUTPUT EXTENSION YOU SET; the filetype will
# actually be the same as the input map.  
# There isn't a Image::WorldMap greater than .14 on CPAN currently,
# but you can grab a patched version from scribblej.com in the meantime.
output_ext => '.jpg',
# Freetype font and fontsize - notice how font size (10) is appended to
# the path name 
ttf_font => '/var/www/scribblej.com/FreeSans.ttf/10',
);

my $geo = Geo::IP::PurePerl->new("/usr/local/share/GeoIP/GeoIPCity.dat",GEOIP_STANDARD);

my $CGI = new CGI;

my $addr = $CGI->remote_host();

my $fname = "$settings{output_dir}/$addr$settings{output_ext}";

if(!-e $fname)
{

  my ($country_code,
      $country_code3,
      $country_name,
      $region,
      $city,
      $postal_code,
      $latitude,
      $longitude,
      $dma_code,
      $area_code) = $geo->get_city_record($addr);
  
  my $map = Image::WorldMap->new($settings{input_map}, $settings{ttf_font});
  
  $map->add($longitude, $latitude, ($city ? $city : $region ) . ", $country_name");
  $map->draw($fname);
}

my $data = read_file($fname, ('binmode' => 1));

print $CGI->header(-type=>'image/jpeg', 
                   -expires=>'+3d',
                   -x_credit_to=>'This product includes '.
                                 'GeoLite data created by MaxMind, '.
                                 'available from http://www.maxmind.com/.',
                   -x_more_credit_to=>'(c) Christopher Jansen - scribblej@scribblej.com',
                   -content_length=>length($data));

print $data;

exit(0);
