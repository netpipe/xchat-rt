#!/usr/bin/perl -w


# Simple XChat script to filter out unwanted
# messages matching one of the expressions
# given.
#
# David Precious, davidp@preshweb.co.uk
# bigpresh on EFNet, DALnet and undernet
# 20th March 2006


my $script = 'xchat-filter-messages.pl';
my $ver = '0.0.1';

# rules file - each line is a regexp to match
# against incoming messages - if it matches, it'll
# be dropped.
$rules = '/home/davidp/.xchat2/filterrules';
@rules = ();  # will be populated by read_rules

&read_rules; # read in the rules file

IRC::register ($script, $ver, "", "");
IRC::add_message_handler('PRIVMSG', 'parse_line');
IRC::add_message_handler('NOTICE', 'parse_line');
IRC::add_command_handler('filterrules', 'read_rules');
IRC::print("*** \0038,2$script v$ver loaded \003");



sub read_rules {
# read (or re-read) the rules file.

@rules = ();
open(RULES, "<$rules") || IRC::print("FAILED TO READ RULES - $!");
while (chomp($rule = <RULES>))
	{
	next unless $rule;
	push @rules, $rule;
	}
close RULES;
IRC::print("$script loaded ".@rules." rules");
return 1;
} # end of sub read_rules



sub check_filter {
# test each filter rule against this line.
# returns 1 if line should be dropped or 0
# if it didn't match.

my $line = shift;

foreach $rule (@rules)
	{
	return 1 if ($line =~ /$rule/);
	}
return 0;

} # end of sub check_filter



sub parse_line {
# received a PRIVMSG or a NOTICE, so 
# try matching it:
#Line::misspresh!~michelle@80.68.82.38 PRIVMSG #sharosmadhouse :rabbit

my $line = shift;
# replace the funky colour stuff:
$line =~ s/\cc[0-9]{2}//g;

if ($line =~ /:(.+)!(.+)@(.+)\s([A-Z]+)\s(.+)\s?:(.+)/)
	{
	($nick, $user, $host, $msgtype, $to, $msgtxt) = ($1, $2, $3, $4, $5, $6);
	#print PTL "Nick:$nick User:$user Host:$host Type:$msgtype To:$to Msg:$msgtxt\n";
	}

return &check_filter($line);

} # end of sub parse_line





