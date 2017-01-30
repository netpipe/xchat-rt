#!/usr/bin/perl

### Xchat registers and hooks.

Xchat::register("Alex's away script", "1.0", "Alex's away script", "");
Xchat::hook_command("iaway", "setaway");
Xchat::hook_command("iback", "setback");

use warnings;
use strict;

### Main routine.


sub setaway {
  Xchat::set_context("#norway", "[Banned-URL]");
  Xchat::command("NICK Alex[Away]");
  Xchat::command("MSG #Norway .status away");
  Xchat::set_context("#iphone", "WyldRyde");
  Xchat::command("NICK iAlex[Away]");
}

sub setback {
  Xchat::set_context("#norway", "[Banned-URL]");
  Xchat::command("NICK Alex");
  Xchat::command("MSG #Norway .status here");
  Xchat::set_context("#iphone", "WyldRyde");
  Xchat::command("NICK iAlex");
}

