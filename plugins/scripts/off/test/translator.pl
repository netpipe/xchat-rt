#!/usr/bin/perl -w

# Usage: /german <english text> or /tgerman <german text>
# Languages Are: German, Dutch, Spanish, French, Portuguese, Italian
# Author: epoch @ irc.darkninja.org

use Lingua::Translate;

Xchat::register('Translator Script','1.0');
Xchat::print"* Translator Script Loaded...";
Xchat::hook_command('german', \&german);
Xchat::hook_command('dutch', \&dutch);
Xchat::hook_command('spanish', \&spanish);
Xchat::hook_command('french', \&french);
Xchat::hook_command('portuguese', \&portuguese);
Xchat::hook_command('italian', \&italian);
Xchat::hook_command('tgerman', \&tgerman);
Xchat::hook_command('tdutch', \&tdutch);
Xchat::hook_command('tspanish', \&tspanish);
Xchat::hook_command('tfrench', \&tfrench);
Xchat::hook_command('tportuguese', \&tportuguese);
Xchat::hook_command('titalian', \&titalian);

sub german {
      my $gitrdun = Lingua::Translate->new(src => "en",dest => "de")       or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $german = $gitrdun->translate($english);
      Xchat::command("say $german");
      return Xchat::EAT_NONE;
}
sub dutch {
      my $gitrdun = Lingua::Translate->new(src => "en",dest => "nl") or die "No translation server available for en -> nl";
      my $english = $_[1][1];
      my $dutch = $gitrdun->translate($english);
      Xchat::command("say $dutch");
      return Xchat::EAT_NONE;
}
sub french {
      my $gitrdun = Lingua::Translate->new(src => "en",dest => "fr") or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $french = $gitrdun->translate($english);
      Xchat::command("say $french");
      return Xchat::EAT_NONE;
}
sub italian {
      my $gitrdun = Lingua::Translate->new(src => "en",dest => "it") or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $italian = $gitrdun->translate($english);
      Xchat::command("say $italian");
      return Xchat::EAT_NONE;
}
sub portuguese {
      my $gitrdun = Lingua::Translate->new(src => "en",dest => "pt") or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $portuguese = $gitrdun->translate($english);
      Xchat::command("say $portuguese");
      return Xchat::EAT_NONE;
}
sub spanish {
      my $gitrdun = Lingua::Translate->new(src => "en",dest => "es") or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $spanish = $gitrdun->translate($english);
      Xchat::command("say $spanish");
      return Xchat::EAT_NONE;
}
sub tgerman {
      my $gitrdun = Lingua::Translate->new(src => "de",dest => "en") or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $german = $gitrdun->translate($english);
      Xchat::command("say $german");
      return Xchat::EAT_NONE;
}
sub tdutch {
      my $gitrdun = Lingua::Translate->new(src => "nl",dest => "en") or die "No translation server available for en -> nl";
      my $english = $_[1][1];
      my $dutch = $gitrdun->translate($english);
      Xchat::command("say $dutch");
      return Xchat::EAT_NONE;
}
sub tfrench {
      my $gitrdun = Lingua::Translate->new(src => "fr",dest => "en") or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $french = $gitrdun->translate($english);
      Xchat::command("say $french");
      return Xchat::EAT_NONE;}
sub titalian {
      my $gitrdun = Lingua::Translate->new(src => "it",dest => "en") or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $italian = $gitrdun->translate($english);
      Xchat::command("say $italian");
      return Xchat::EAT_NONE;
}
sub tportuguese {
      my $gitrdun = Lingua::Translate->new(src => "pt",dest => "en") or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $portuguese = $gitrdun->translate($english);
      Xchat::command("say $portuguese");
      return Xchat::EAT_NONE;
}
sub tspanish {
      my $gitrdun = Lingua::Translate->new(src => "es",dest => "en") or die "No translation server available for en -> de";
      my $english = $_[1][1];
      my $spanish = $gitrdun->translate($english);
      Xchat::command("say $spanish");
      return Xchat::EAT_NONE;
}
 