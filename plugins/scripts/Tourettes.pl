#!/usr/bin/perl -w
#
# Purpose: When enabled, inserts random swear words into your own text.
# Don't blame me if you get banned...
# (Please, no hate mail - I know Tourettes and Coprolalia are serious conditions.)
#
# Toggle with /tourettes
#
# Change these to suit your tastes;
#
my $chance = 15; # percentage change to trigger between each word. 100% = every other word will be a swear
my $global_on = 0; # Default state of toggle on start. Recommend 0
my $shout = 1; # Whether to shout the rudewords. 0 = no, 1 = yes.
my $exclamation = 1; # Whether to add an exclamation mark after each word. (!)
my $badness = 3; # Which word set to use (see below). 1=Mild. 2=Medium. 3=Strong. 4=Bizarre
#
# If you want to change the swear words, add as many as you like here (within the brackets)
# Badness level 1: (Mild)
my @level1words = ('boom','piddle','fart','burp','moo','fiddlesticks','darn','crikey','poot','dagnabbit','pump','stupid','idiot','monkey');
# Badness level 2: (Medium - sexual)
my @level2words = ('bumhugger','thigh-stroker','bra-snapper','uphill gardener','rug-muncher','gay','queer','homo','donkeydick');
# Badness level 3: (Strong)
my @level3words = ('cunt','dick','dildo','foreskin','honky','motherfucker','horsefucker','piss','prick','pussy','slut','cocksucker','twat','wank',
	'whore','bitch','bastard','damn','shit','fuck','assmuncher','arsehole','dyke','turd');
# Badness level 4 (Bizarre)
my @level4words = ('donkey','trumpet','squid','pump','bottle','marshmallow','zuccini','pony','trout','kipper','Bill Clinton','munchkin','dragon','banana',
			'yacht','time-machine','grass','lipstick','orange','yoghurt','burger','tazer','pig','dog','cat','bull','aeroplane','zeppelin','car','computer','parrot',
			'camera','book','curtains','elephant');

#
# End config stuff
############

Xchat::register( "Flashy's Tourettes Script", "v.1", "Tourettes", "" );
Xchat::hook_command('', "my_msg"); # Sadly I need to hook a null command. 
Xchat::hook_command('tourettes', "toggle");
Xchat::print("Tourettes loaded. Type /tourettes to toggle on and off. Word list chosen: $badness");

sub my_msg {
if ($global_on == 0) { return Xchat::EAT_NONE; }

my $mychan = Xchat::get_info('channel');
my $mynick = Xchat::get_info('nick');
my $out = $_[1][0];
my $fixed=undef;
my @words = split(/ /,$out);

foreach(@words) {
	$fixed .= "$_ ";
	my @wordlist = @level1words; # Safe default
	if ($badness == 2) { @wordlist = @level2words; }
	if ($badness == 3) { @wordlist = @level3words; }
	if ($badness == 4) { @wordlist = @level4words; }
	if (rand(100) <= $chance) {
		my $rude = @wordlist[rand(scalar @wordlist)];
		if ($exclamation == 1) { $rude .= '!'; }
		if ($shout == 0) { 
			$fixed .= lc("$rude "); } else { $fixed .= uc("$rude "); }
		}
	}

chop($fixed);
Xchat::command("PRIVMSG $mychan :$fixed");
# Also output locally
Xchat::emit_print('Your Message',$mynick,$fixed);

return Xchat::EAT_ALL;
}

sub toggle { if ($global_on == 0) { $global_on = 1; Xchat::print("Tourettes now active!"); } else { $global_on = 0; Xchat::print("Tourettes disabled!"); } return Xchat::EAT_NONE; }
