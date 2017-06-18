#!/usr/bin/perl
# Written April 04, 2007 by Chris "ScribbleJ" Jansen
#
# A simple perl script to perform GeoIP lookups and plot maps
# from within Xchat.
#
# REQUIREMENTS:
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
#   An important note!  The latest version of this module uses a terribly
#   inefficient method of placing labels.  I don't have the skill to write 
#   a good label-placer, but this module can  use one called
#   Image::WorldMap::draw_simple() which is available from my site, and
#   seems to help a little, although drawing the map can still take a solid
#   minute sometimes.  
#
# If you wish to use the "large" or "huge" sizes you can find the apprpriate
# images at: http://www.astray.com/WorldMap/
#
#  WARNING!!
#  WARNING!!
#  WARNING!!
#
#  This script can cause Xchat to pause for "quite a while" when drawing 
#  the map.  You can speed it up quite a bit by disabling labels in the settings.
#
#  LICENSE:
#
#  This code is free to use; enjoy.  It comes with no warranties, and I'm not
#  responsible for a damn thing it might do or not do.
#
#  If you wish to modify and redistribute it, you may but please send
#  me a note and say you liked it.  Send the note to: scribblej@scribblej.com
#
use FileHandle;
use Image::WorldMap;
use Geo::IP::PurePerl;

Xchat::register('GeoIP Script', '1.01', 'ScribbleJ\'s GeoIP Lookup Script');

my %userhash = ();
my %settings = (
# path to input maps
input_map => '/home/chris/earth.png',
# path to output
output_map => '/home/chris/xchat-map.png',
# Freetype font and fontsize - notice how font size (10) is appended to
# the path name (the file in my homedir is simply FreeSans.ttf)
ttf_font => '/home/chris/FreeSans.ttf/10',
# Automatically generate map when new users are noticed
auto_redraw => 0,
# Label dots on map
labels => 0,
# display stuff you might not care about
debug => 0,
# automaticlly map people on join?
onjoin => 0,
# set to "regular" for compatibility with standard Image::WorldMap
# draw_style => 'ScribbleJ',
draw_style => 'regular',
);

my $join_hook = undef;
if($settings{onjoin})
{
  $join_hook = Xchat::hook_server('JOIN', \&handle_join);
}

Xchat::print(qq{
ScribbleJ's GeoIP script started.
  Geo commands are:
  geo-set [setting] [value]
  geo-lookup nickname
  geo-lookup-ip ip-address|hostname
  geo-scanchan
  geo-draw
  geo-clear
Use /help <command> for more info.
Please see the documentation in the script source.
});
Xchat::hook_command('geo-set', \&handle_geo_set, 
 {help_text => qq{
 /geo-set [setting] [value]
 use /geo-set with no parameters for list of all current settings.
 use /geo-set [setting] to see current value of one setting.
 use /geo-set [setting] [value] to change a setting.
 See the script file to change defaults and to understand what the
   settings are used for.
 }});
Xchat::hook_command('geo-lookup', \&manual_lookup,
 {help_text => qq{
 /geo-lookup nickname
 Will return the specified user's location, and add them to the map.
 }});
Xchat::hook_command('geo-scanchan', \&channel_lookup,
 {help_text => qq{
 /geo-scanchan 
 Scans the current channel, adding all users to the map.
 }});
Xchat::hook_command('geo-draw', \&handle_draw,
 {help_text => qq{
 /geo-draw
 Redraws the output map file.  If the auto_redraw setting is set
 then this is unneccessary as the map will be drawn whenever data
 is changed.
 }});
Xchat::hook_command('geo-clear', \&handle_clear,
 {help_text => qq{
 /geo-clear
 Clears the cache of data points - this will clear the map.
 }});
 Xchat::hook_command('geo-lookup-ip', \&handle_ip_lookup,
 {help_text => qq{
 /geo-lookup-ip 
 Same as geo-lookup but by hostname or ip rather than nick.
 }});



my $geo = Geo::IP::PurePerl->new("/usr/local/share/GeoIP/GeoIPCity.dat",GEOIP_STANDARD);


sub handle_ip_lookup()
{
  my $params = shift;
  my ($command, $addr) = @{$params};

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

  Xchat::print(qq{
ScribbleJ GeoIP Lookup
  $addr
  City: $city State/Region: $region Country: $country_name 
  Postal Code: $postal_code
  Latitude: $latitude Longitude: $longitude
  });

}

sub handle_clear()
{
  %userhash = ();
  Xchat::print("Geo data cache cleared.\n");
}


sub handle_draw()
{
  redraw_map();
  Xchat::print("Map regenerated.\n");
}
  

sub handle_geo_set()
{
   my $params = shift;
   my ($command, $var, $value) = @{$params};

   if(!defined $var)
   {
     Xchat::print("All available settings:\n");
     foreach my $fname (sort keys(%settings))
     {
       Xchat::print("  $fname = $settings{$fname}\n");
     }
     return Xchat::EAT_ALL;
   }

   if(!defined $value)
   {
     Xchat::print("Geo $var is currently set to: $settings{$var}\n");
     return Xchat::EAT_ALL;
   }

   if($var eq 'onjoin')
   {
     if($value)
     {
       $join_hook = Xchat::hook_server('JOIN', \&handle_join);
     }
     else
     {
       Xchat::unhook($join_hook);
       undef $join_hook;
     }
   }
       
   if(grep $var, keys(%settings))
   {
     $settings{$var} = $value;
     Xchat::print("Geo Set variable $var to $value.\n");
   }

   return Xchat::EAT_ALL;
}
  

sub channel_lookup()
{
   foreach my $user (Xchat::get_list('users'))
   {
     # Xchat::print("Getting user...\n");
     my ($name, $host) = ($user->{nick}, $user->{host});
     # Xchat::print("Got user: $name at $host");
     my $addr;
     if($host =~ m/([^@]+)@(.*)/)
     {
       $addr = $2;
     }
     elsif($settings{debug})
     {
       Xchat::print("GEO ERR:Couldn't parse host in channel_lookup: $name, $host\n");
     }
     # Xchat::print("Got addr: $addr\n");
     if(!$userhash{$name})
     {
       $userhash{$name} = $addr;
       #Xchat::print("Updating map with $name...\n");
     }
   }
   redraw_map() if($settings{auto_redraw});
   Xchat::print("Channel user lookups complete.\n");
   return Xchat::EAT_ALL;
}


sub manual_lookup()
{
  my $params = shift;
  my ($command, $nick) = @{$params};
  my $user = Xchat::user_info($nick);
  if(!defined $user)
  {
    Xchat::print("Geo: Nickname $user not found.\n");
    return Xchat::EAT_ALL;
  }
  my ($name, $host) = ($user->{nick}, $user->{host});
  my $addr;
  if($host =~ m/([^@]+)@(.*)/)
  {
    $addr = $2;
  }
  else
  {
    Xchat::print("Geo:Couldn't parse host in manual_lookup: $name, $host\n");
    return Xchat::EAT_ALL;
  }


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

  Xchat::print(qq{
ScribbleJ GeoIP Lookup
  $name @ $addr
  City: $city State/Region: $region Country: $country_name 
  Postal Code: $postal_code
  Latitude: $latitude Longitude: $longitude
  });

  if(!$userhash{$name})
  {
    $userhash{$name} = $addr;
    redraw_map() if($settings{auto_redraw});
  }
  else
  {
    Xchat::print("User $name already plotted.\n") if($settings{debug});
  }
  return Xchat::EAT_ALL;
}


sub handle_join()
{
  my $foo = shift;
  my $user = $foo->[0];

  if($user =~ m/:([^!]+)!([^@]+)@(.*)/)
  {
    if(!$userhash{$1})
    {
      $userhash{$1} = $3;
      redraw_map() if($settings{auto_redraw});
    }
  }
  else
  {
    Xchat::print("Geo Couldn't parse line: $foo->[0]\n") if($settings{debug});
  }
  return Xchat::EAT_NONE;
}


sub redraw_map()
{
  my $map = Image::WorldMap->new($settings{input_map}, $settings{ttf_font});

  foreach my $name (keys(%userhash))
  {
    my $addr = $userhash{$name};
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
    if($longitude != 0 and $latitude != 0)
    {
      if($settings{labels})
      {
        $map->add($longitude, $latitude, $name);
      }
      else
      {
        $map->add($longitude, $latitude);
      }
    }
    elsif($settings{debug})
    {
      Xchat::print("Not plotting $name ($city,$region,$country) because no long/lat.\n");
    } 
  }

  if($settings{draw_style} eq 'regular')
  {
    $map->draw($settings{output_map});
  }
  else
  {
    $map->draw_simple($settings{output_map});
  }
  
}

  

