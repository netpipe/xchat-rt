Shortinformation for Nicktrace.pl
Version 1.6.0
Get new versions at http://www.electronic-culture.de/nicktrace.html
Comments and sugestions to DMS@Electronic-Culture.de

Settings:
at the begining of the pl file are 3 lines, where you can change some basic settings

"my $outputType=0;"
change this value to use another kind of output. set to 0 to get aliases in joinmessage, 
set to 1 to print als notice or set to 2 to print as server notice

"my $minMEE=50;"
use this to set how many percent at the end of the hostmask must match to be
handled as same hostmask. 
0   means hostmask will not be ckecked
100 hostmasks must be equal

"my $autoAdd=1;"
set this value to 0 if you don't want to add nicks to db on join


Commands:
/NickTrace_add - add entry manualy to db
usage: /NickTrace_add NICK IDENT HOST/IP
