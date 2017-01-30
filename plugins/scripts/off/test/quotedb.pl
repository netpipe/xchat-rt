#!/usr/bin/perl -w

use strict;

##############################################################
## ######################################################## ##
## quotedb.pl                                               ##
## for xchat 2                                              ##
## creator: Christian Kuester <christian@scriptkiddie.de>   ##
## author: NiteFall <nitefall@box201.com>                   ##
## version: 0.1.9                                           ##
## last changes:  May 25 2008      i                        ##
## credits: joachim for his perl knowledge                  ##
## Christian Kuester for his inspiration to build ver 0.0.3 ##
## License: GPL <http://www.gnu.org/copyleft/gpl.html>      ##
## ######################################################## ##
## Copy quotedb.pl into ~/.xchat2/ to enable autoloading    ##
## when starting xchat. touch the file ~/quotes             ##
## You can have your quotes anywhere on your system. just   ##
## change the paths in this file to the correct position.   ##

my $dir=$ENV{'HOME'};
my $QUOTESFILE = "$dir/quotes";
# my $QUOTESFILE = "$dir/.xchat2/quotes";

## this script provides the following commands:             ##
## /addquote string                                         ##
## which adds the string to your quotes file                ##
## /pquote regexp                                           ##
## which greps for the regexp and chooses randomly a string ##
## from the results and /says it to the current channel     ##
## /lquote                                                  ##
## this just prints the current quotes in the quotes file   ##
## /dquote regexp                                           ##
## An added quotedb line removal tool.                      ##
##                                                          ##
## ######################################################## ##
##############################################################

IRC::register ("Xchat2 quotedb script", "0.1.8", "", "");
IRC::add_command_handler("addquote", "addquote");
IRC::add_command_handler("pquote", "printquote");
IRC::add_command_handler("lquote", "listquote");
IRC::add_command_handler("dquote", "delquote");
IRC::add_command_handler("quote", "how2quote");

sub addquote  {## not much of a change here I just added linenumbers to the db
    if (!open(QUOTESCOUNT,"< $QUOTESFILE")) {
        IRC::print("quotedb.pl: Can't open Quote-File! $!\n");
        return 1;
    }
    my @quotecount = <QUOTESCOUNT>;
    close QUOTESCOUNT;
    my $listsize = @quotecount;
    my $linenum = $listsize +1;
    ## I get the number in the quotes list then add 1 so we keep the 
    ## sequence correct. so if @quotes has 3 lines my next line number
    ## will be "4"
    if (!open(QUOTES,">> $QUOTESFILE")) {
        IRC::print("quotedb.pl: Can't open Quote-File! $!\n");
	     return 1;
    }
    if ($_[0] eq /[\s,'']/){ ##just in case the inbound line contained nothing.
        IRC::print("quotedb.pl nothing to quote");
        return 1;
    }
    ## now I grab the first field from the line which should be the nick.
    ## $_[0] contains the whole quote nick and all. I assume we collect the nick
    ## when we cut&paste the copied quote. I split it by spaces and use the first
    ## field which should be the nick. Then say Nick's quote added to db.   
    my ($name,$line) = split (/ /, $_[0], 2);
    print QUOTES "$linenum:$name:", $line, "\n"; 
    IRC::print("$name\'s quote added to database\n"); ## now just tell user we are done.
    close QUOTES;
    return 1;
}


sub printquote { ## this worked well too. Only dded the nick to the quote.
    my $pat = shift;
    if (!open(QUOTES,"< $QUOTESFILE")) {
        IRC::print("quotedb.pl: Can't open Quote-File! $!\n");
	return 1;
    }
    my @quotes = <QUOTES>;
    close QUOTES;

    my @matches = grep(/$pat/i, @quotes);
    my $matchessize = scalar @matches;

    if ($matchessize == 0) {
          IRC::print("quotedb.pl: No matches found for pattern: $pat\n");
	  return 1;
    }
    ## randomize the quotes if more than one.
    my $nn = int rand $matchessize;
    ## now we remove the recordID or linenumber from the quote said
    ## into the room. I use "#:nick quote" as the table format in the db so 
    ## if we split the line on ":" I have $trimmatch[0] = linenumber
    ## and $trimmatch[1] = the nick&quote
    my @trimmatch = split(/:/,$matches[$nn]);
    chomp $trimmatch[2]; ## prevent the lines "\n" from printing
    
    IRC::command("/say [quotedb #$trimmatch[0]]: \<$trimmatch[1]\> \"$trimmatch[2]\"");
    return 1;
}

sub listquote { ##I just needed a way to list the current db here 
    if (!open(QUOTES,"< $QUOTESFILE")) {
        IRC::print("quotedb.pl: Can't open Quote-File! $!\n");
	return 1;
    }
    my @quotes = <QUOTES>;
    close QUOTES;
    IRC::print("== quoted database list ==\n");
    IRC::print("--------------------------\n");
    foreach my $quoteline (@quotes){
          IRC::print("$quoteline");
    }
    return 1;
}

sub delquote {
    ## we got our pattern to delete in $_ so I'm just going to
    ## shift it into $pat for pattern matching. Next I open the db
    ## and load the table into the QUOTES file handle. 
    my $pat = shift;
    chmod $pat; ##to strip the /n
    if (!open(QUOTES,"< $QUOTESFILE")) {
    IRC::print("quotedb.pl: Can't open Quote-File! $!\n");
	 return 1;
    }
    ## now to move it to the @quotes array.
    my @quotes = <QUOTES>;
    ## building another array of the matched values in @quotes
    my @matches = grep(/$pat/i, @quotes);
    my $matchessize = scalar @matches;
    close QUOTES; ## fh cleanup
    ## this just bounces out of the delete function if we 
    ## don't have a match. No matches no delete. 
    if ($matchessize == 0) {
          IRC::print("quotedb.pl: No matches found for pattern: $pat\n");
	  return 1;
    }
    ## here is the tricky part. we have 2 arrays. 1 is the whole db
    ## and one is the array of lines that matched the $pat pattern.
    ## we set our counters to zero:     
    my $pointer = 0;
    my $matchnum = 0;
    ## this reads "while the db pointer <= the total number of fields in
    ## the array, do..."
    while ($pointer <= $#quotes) {
    ## I need exact matches here so we use eq for string matching.
    if ($quotes[$pointer] eq $matches[$matchnum]) {
       chomp $matches[$matchnum]; ## to remove the \n from the match
       IRC::print("\n== $matches[$matchnum] removed. ==\n");
       splice(@quotes, $pointer, 1); ## splice line $pointer from @quotes and only that "1" field.
       $matchnum++; ## move to the next match
    }
    else {
       $pointer++;  ## move to the next line in @array
    }
    close QUOTES; ## cleanup the fh again.
    }
   ## time to recreate the db. I open the file via ">" instead of
   ## appending ">>" the file because I want to re-write the whole list
   ## the ">" overwrites the db flatfile so we can start fresh.
   my $newlinenum = 1;
   if (!open(QUOTES,"> $QUOTESFILE")) {
        IRC::print("quotedb.pl: Can't open Quote-File! $!\n");
	return 1;
   }
   ## ok now to print. I split the linenumbers from the quotes array
   ## one line at a time and re-add the new linenumber to the new array 
   ## list. That way if we delete a quote from the middle of the db
   ## then the db will resequence the list thus prventing gaps in the list 
   foreach my $rawline (@quotes){
   my ($linenum,$name,$quote) = split(/:/,$rawline,3); 
   print QUOTES "$newlinenum:$name:$quote";
   $newlinenum++;
   }
   close QUOTES;
   ## I noticed everytime I deleted I wanted to see the result so
   ## time to reopen the db. Read in the new list and print to the screen.
   &listquote;
   ## and since &listquote has it's own "return 1;" we don't need one
}

sub how2quote { ## simple instructions to remind me how to quote everytime I do a "/quote" command
    IRC::print("\n== Quoted Database Command List \n");
    IRC::print("== \"/quote\" prints this help \n");
    IRC::print("== \"/addquote <string>\" adds string to the db\n");
    IRC::print("== \"/lquote\" lists the quotes in the db\n");
    IRC::print("== \"/pquote <regexp>\" prints rand quote in channel\n");
    IRC::print("== \"/dquote <regexp>\" remove matching quotes in db\n");
    return 1;
}


