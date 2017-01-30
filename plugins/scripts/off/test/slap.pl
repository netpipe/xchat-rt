Xchat::register("slap ","1.0");
Xchat::print"Loaded RandomSlap 1.0";
Xchat::hook_command("slap", \&Slap);

sub Slap {

my@objects = (
"around a bit with a large trout",
"a rather large squid",
"a hydraulic pump",
"a book by Stephen King",
"a 10mbit network card",
"the Win2k Buglist"
);
my $obj = $objects[rand(@objects)];
$person = $_[1][1];

Xchat::command "me slap $person with $obj.";
return Xchat::EAT_XCHAT;
} 