#!/usr/bin/perl -w

# rss checker by 1@11001110001101101000011001000110.org
# send me patches!
# try checking http://www.syndic8.com/ for a list of good feeds

# shows you all the news the first time it checks sites
# and from then onwards, only shows you new news.

# only checks one site at a time, otherwise xc begins 
# to "freeze" every so often

# screenshot - http://www.11001110001101101000011001000110.org/rss_shot.gif
 
use strict;
use XML::RSS;
use LWP::Simple;

my %items_seen;

my @check_sites = qw{http://slashdot.org/slashdot.rdf
		   http://www.macslash.com/macslash.rdf
		   http://blogspace.com/rss/feeds/bbcNews/sci/tech
		   http://maccentral.macworld.com/mnn.cgi
		   http://blogspace.com/rss/feeds/bbcNews/world
		   http://www.plastic.com/xml/recentstories.xml
 		   http://boingboing.net/rss.xml
		   };

my $poll_interval = int(216000 / scalar(@check_sites));
                   # each site checked every 60 minutes

my $check_index = 0;

IRC::register('showrss', '1.0', "", "");

IRC::print("--- showrss loaded");

IRC::add_timeout_handler(3000, "timed_handler");

sub timed_handler {
  IRC::add_timeout_handler($poll_interval, "timed_handler");

  &show_rss();
  
  return 1;
}

sub show_rss {
  get_news($check_sites[$check_index]);

  $check_index++;

  $check_index = 0 if ($check_index == scalar(@check_sites));
  
  return 1;
}

sub get_news {
  my ($site) = @_;

  my $rss = new XML::RSS;

  my $content = get($site); # todo - timeout
  
  if ($content) {
    eval {
      $rss->parse($content);
    };
    if (my $error = $@) {
      IRC::print("\0034Unable to parse content for $site");
    }
    
    my $channel = IRC::get_info(2);
    my $server  = IRC::get_info(3);
    
    foreach my $item (@{$rss->{'items'}}) {
      next unless defined($item->{'title'}) && defined($item->{'link'});
      next if defined $items_seen{$item->{'link'}};
      
      my $text = "\0035$item->{'title'} : \0033$item->{'link'}\n";
      
      if ($channel and $server) {
	IRC::print_with_channel($text, $channel, $server);
      } 
      else {
	IRC::print($text);
      }
      
      $items_seen{$item->{'link'}} = 1; # now?
    }
  }
  else {
    IRC::print("\0034Could net got content for $site");
  }
}
