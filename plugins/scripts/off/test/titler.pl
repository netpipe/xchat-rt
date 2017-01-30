#!/usr/bin/perl
#
# Titler. Sits patiently in a channel and waits for URL's to be posted. If it meets any it knows, it fetches the page and posts a summary
# Users love it as it saves them having to visit a link to see whether they've seen it before.
#
# Comes with matches for Bbc news, youtube, imdb, multiplay, flickr, theRegister, ebuyer
#
# eg:
# [21:36:06] <Flash_> http://www.youtube.com/watch?v=we9_CdNPuJg
# [21:36:09] <Flashbot> YouTube - Fainting Goats
#
# Easy to add new websites to it - just add new sections to %config
# %config values are:
# domain - The domain in the url to "hit" on. If a url contains this string, this section will be used. REQUIRED
# preparse - The section immediately before the text you want in the html source of the web page. Tip: Make sure it's unique :) REQUIRED
# postparse - The bit at the end of the text you want to capture. REQUIRED
# intro - This will be prepended to any results if set. OPTIONAL
# trim - This will be removed from any results if set. OPTIONAL

use strict;
use Irssi;
use LWP::UserAgent;
use vars qw($VERSION %IRSSI);
$VERSION = '002';
%IRSSI = (
    authors     => 'Flash',
    contact     => 'flash@digdilem.org',
    name        => 'Titler',
    description => 'Reports URLs on channels it spies',
    license     => 'GNU General Public License 3.0' );

# List all known pages here, their hit strings and their parsers
my %config = (
	'bbc' => { 'domain' => 'news.bbc.co.uk',
				'preparse' => '<title>',
				'postparse' => '</title>'},
	'youtube' => { 'domain' => 'youtube.com',
				'preparse' => '<title>',
				'postparse' => '</title>' },
	'imdb' => { 'domain' => 'imdb.com',
				'preparse' => '<title>',
				'intro' => 'IMDB: ',
				'postparse' => '</title>' },
	'multiplay' => { 'domain' => 'forums.multiplay.co.uk',
				'preparse' => '<title>',
				'postparse' => '</title>',
				'intro' => 'Thread title: ',
				'trim' => '- Multiplay UK Forums'},
	'flickr' => { 'domain' => 'flickr.com',
				'preparse' => '<title>',
				'postparse' => '</title>',
				'intro' => 'Picture title: ',
				'trim' => 'on Flickr'},
	'ebuyer' => { 'domain' => 'ebuyer.com',
				'preparse' => '<title>',
				'intro' => 'Ebuyer: ',
				'postparse' => '</title>',
				'trim' => ' - Ebuyer'},
	'elreg' => { 'domain' => 'theregister.co.uk',
				'preparse' => '<h2>',
				'postparse' => '</h2>',
				'intro' => 'The Register: ',
				'trim' => '| The Register'},
	'tdm' => { 'domain' => 'thedailymash.co.uk',
				'preparse' => '<title>',
				'postparse' => '</title>',
				'intro' => 'The Daily Mash: ',
				'trim' => ' - The Daily Mash'},
	'slashdot' => { 'domain' => 'slashdot.org',
				'preparse' => '<title>',
				'postparse' => '</title>'},

	);

sub checkline { # use google instead!
	my ($server, $data, $nick, $mask, $chan) = @_;
	my $url = undef;

	if ($data =~ m/(http:.*)/i) { $url = $1; }
	if ($data =~ m/(www.*)/i) { $url = $1; }

	if (defined $url) {
		if ($url =~ m/^(.*) /) { $url = $1; }
		if ($url !~ /http:/i) { $url = "http://$url"; }
		print "Contains a url! ($url) [ $data ]";

		for my $func (keys %config) {
			if ($data =~ /$config{$func}->{'domain'}/i) {
				my $ua = LWP::UserAgent->new;
				$ua->agent("aFlashbot/001 ");
				my $response  = $ua->get($url);
				if ($response->is_success) {
				 print "Got page from $config{$func}->{'domain'}";
				 my $return = $response->decoded_content;
				 if ($return =~ m/$config{$func}->{'preparse'}(.*?)$config{$func}->{'postparse'}/i) {
					my $gotit = $1;
					if (defined $config{$func}->{'intro'}) { $gotit = "$config{$func}->{'intro'}$1"; }
					if (defined $config{$func}->{'trim'}) { $gotit =~ s/$config{$func}->{'trim'}//g; }
					# Trim any new lines or double spaces:
					$gotit =~ s/\n//g;
					$gotit =~ s/\r//g;
					$gotit =~ s/  / /g;
					print "OMG! IT WORKED! ($gotit)";
					$server->command("msg $chan $gotit");
					} # else { print "Not found in: $return"; }
				 }
				 else {
					 print $response->status_line;
					 return;
				 }
				}

			}
		} # else { print "No url"; }
	}

Irssi::signal_add('message public','checkline');
Irssi::signal_add('message own_public','checkline');