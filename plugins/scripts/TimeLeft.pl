#!/usr/bin/perl -w

# TimeLeft script for XChat, by Dan Fruehauf.

use strict;
use Time::Local;

IRC::register ("TimeLeft", "1.0", "", "Prints how much time is left until an event occurs.");

IRC::add_command_handler("timeleft", "timeleft");

sub timeleft {
	# arguments extraction
	my $args = "@_";
	$args =~ s/\s+/ /g;
	my @_ARGS = split / /,$args;
	my $date = shift @_ARGS;
	my $time = shift @_ARGS;
	
	# we shift the first one so we dont have spaces...
	my $my_event = shift @_ARGS;
	foreach ( @_ARGS ) { $my_event = $my_event . " " . $_ ; }

	# time/date argument extraction and verification
	my ($day, $month, $year) = $date =~ /^(\d+)[\/\.](\d+)[\/\.](\d{4})$/ or return &usage();
	my ($hour, $min, $sec) = $time =~ /^(\d+):(\d\d):?(\d*)$/ or return &usage();
	if ( $sec eq "" ) { $sec = 0; } # optional arguement, so we set it if it's not defined
	$month -= 1; # counts between 0 to 11, while we input between 1 to 12
	
	# time/date conversion to seconds
	my $event_time = timelocal($sec, $min, $hour, $day, $month, $year);
	my $curent_time = time();
	my $int_seconds = $event_time - $curent_time;
	my $future_event = 1;
	if ($int_seconds < 0) { $future_event = 0; $int_seconds *= -1 }
	
	# formatting the answer
	my $years = int ( $int_seconds / 31536000 );
	$int_seconds %= 31536000;
	my $months = int ( $int_seconds / 2592000 );
	$int_seconds %= 2592000;
	my $weeks = int ( $int_seconds / 604800 );
	$int_seconds %= 604800;
	my $days = int ( $int_seconds / 86400 );
	$int_seconds %= 86400;
	my $hours = int ( $int_seconds / 3600 );
	$int_seconds %= 3600;
	my $minutes = int ( $int_seconds / 60 );
	$int_seconds %= 60;
	my $seconds = $int_seconds;

	my $string = format_time( $years, $months, $weeks, $days, $hours, $minutes, $seconds );
	# outputting
	if ( $future_event ) {
		$string = "time left till $my_event: " . $string ;
	}
	elsif ( ! $future_event )
	{
		$string = "$my_event " . $string . " ago";
	}
	IRC::command($string);
	return 1;
}

sub format_time {
	my ($years, $months, $weeks, $days, $hours, $minutes, $seconds) = @_;
	my $str = "";
	if( $years > 1 ) { $str = $str . $years . "yrs "; }
	elsif( $years > 0 ) { $str = $str . $years . "yr "; }

	if( $months > 0 ) { $str = $str . $months . "month "; }

	if( $weeks > 1 ) { $str = $str . $weeks . "wks "; }
	elsif( $weeks > 0 ) { $str = $str . $weeks . "wk "; }
	
	if( $days > 1 ) { $str = $str . $days . "days "; }
	elsif( $days > 0 ) { $str = $str . $days . "day "; }

	if( $hours > 1 ) { $str = $str . $hours . "hrs "; }
	elsif( $hours > 0 ) { $str = $str . $hours . "hr "; }
	
	if( $minutes > 1 ) { $str = $str . $minutes . "mins "; }
	elsif( $minutes > 0 ) { $str = $str . $minutes . "min "; }

	if( $seconds > 1 ) { $str = $str . $seconds . "secs"; }
	elsif( $seconds > 0 ) { $str = $str . $seconds . "sec"; }
	return $str;
}
	
sub usage {
	IRC::print("usage : /timeleft date time event");
	IRC::print("example : /timeleft 16/10/2005 8:00:00 i finish my army service");
	IRC::print("example : /timeleft 12/10/1983 5:30:00 i was born");
	return 1;
}
