#!/usr/bin/perl

# Usage: /weather <zipcode>
# Author: epoch @ irc.darkninja.org

use Weather::Underground; 

Xchat::register('Weather Script','1.0');
Xchat::print"* Weather Script Loaded...";
Xchat::hook_command('weather', \&weather);

sub weather {
      $weather = Weather::Underground->new(place => $_[1][1],debug => 0,) || die "Error, could not create new weather object: $@";
      $arrayref = $weather->get_weather() || die "Error, calling get_weather() failed: $@\n";
      Xchat::command("say Weather Report For: $arrayref->[0]->{place} Fahrenheit: $arrayref->[0]->{temperature_fahrenheit} Celsius: $arrayref->[0]->{temperature_celsius} Humidity: $arrayref->[0]->{humidity} Conditions: $arrayref->[0]->{conditions} Wind: $arrayref->[0]->{wind} Pressure: $arrayref->[0]->{pressure} Updated: $arrayref->[0]->{updated}");
      return Xchat::EAT_NONE;
}