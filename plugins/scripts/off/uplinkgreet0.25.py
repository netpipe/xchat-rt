## This is the UplinkGreeter script for XChat-Python
## Written by Justin Vermillion
## Released under the GPL
## If you distribute the unmodified code, please
## Give credit where credit is due.
## This script will automatically send a user-defined greeting
## Whenever anyone with the specified hostmask joins your channel.
## The greeting can also be sent manually by using /greet <nick>

__module_name__ = "UplinkGreeter" 
__module_version__ = "0.25" 
__module_description__ = "Send a notice when someone joins whose hostmask matches the given pattern" 
 
import re
import xchat

greeting = "Hello there! Please be sure to view the Uplink guide (http://guide.modlink.net) before posting questions to the channel."

## Edit these patterns to reflect whatever hostmask and channel you want.
## You must edit this in order for the script to work on your server.
## gmask can be set to ".*" to greet everyone.
## gmask and gchan are REGEXPs, and case sensitive.
gmask = ".*Uplink.*@.*"
gchan = "#Uplink"

## Anything below this line is fine as-is, and shouldn't be edited
## Unless you know what you're doing. :)

def send(word, word_eol, userdata):
   xchat.command("privmsg %s :%s" % (word[1], greeting))
   xchat.emit_print("Notice", "guide", "Greeting sent to %s (%s)." % (word[1], word[0]))
   return xchat.EAT_ALL

def grab(word, word_eol, userdata):
   if re.search(gmask, word[2]) and re.search(gchan, word[1]):
	   sendinfo = [word[2], word[0]]
	   send(sendinfo, word_eol, userdata)
	   return xchat.EAT_NONE
   else:
	return xchat.EAT_NONE

## Don't edit between these markers ##

xchat.hook_command('greet', send, help="/GREET <nick> manually sends your greeting to the desired nick")
xchat.hook_print("JOIN", grab)

xchat.prnt("%s v%s loaded" % (__module_name__, __module_version__))