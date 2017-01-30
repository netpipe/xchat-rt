#!/usr/bin/perl
#
# Random rude word of the day.
#
#
Xchat::register( "RudeWord", 'v.001', "Rudeword", "" );
Xchat::hook_print('Channel Message', "rude");
Xchat::hook_print('Your Message', "rude");
Xchat::hook_print('Private Message', "rude");
my @pt1 = split(/ /,"arse tit twat fuck shit anal piss wank jism ass dick crap wee penis vagina cock thadge rug poo willy");
my @pt2 = split(/ /,"knocker face bags socks features sticks monkeys head licker fiddler puller yanker gobbler fondler");
srand();

sub rude {
	my @pubwords = split(/ /,$_[0][1]);
	if ($pubwords[0] eq '!rude') {
		my $word = uc($pt1[rand(scalar(@pt1))]) .  uc($pt2[rand(scalar(@pt2))]);
		Xchat::command("say $word");
		}
	return Xchat::EAT_NONE;
	}
