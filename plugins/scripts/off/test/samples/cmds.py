
__module_name__ = "commands.py" 
__module_version__ = "1.1" 
__module_description__ = "commands.py" 

import xchat
import string

MyNick = "Nielsen"
Away = ""

def startup(word, word_eol, userdata):
	print "startup..."

	if xchat.get_info("nick") != MyNick:
		print "Nick: "+xchat.get_info("nick")+" - waiting"
		xchat.command("nick "+MyNick)
		xchat.command("timer -refnum 111 10 startup")
		return xchat.EAT_ALL
	if xchat.get_info("nick") == MyNick:
		print "Nick: "+xchat.get_info("nick")
		xchat.command("quote muh read")
		xchat.command("timer -refnum 1 2 autoexec")
		xchat.command("timer -refnum 2 15 botlogin")
		xchat.command("timer -refnum 3 25 smallchans")
	return xchat.EAT_ALL

xchat.hook_command("startup",startup)

def autoexec(word, word_eol, userdata):
	print "autoexec..."
	xchat.command("quote privmsg x@channels.undernet.org :login username password")
	xchat.command("mode "+xchat.get_info("nick")+" +xwis")
	xchat.command("join #my,#favorite key,key2")
	xchat.command("quote AWAY"+Away)
	return xchat.EAT_ALL

xchat.hook_command("autoexec",autoexec)

def botlogin(word, word_eol, userdata):
	print "botlogin..."
	xchat.command("quote privmsg bot1 :login user password")
	xchat.command("quote privmsg bot2 :verify password")
	return xchat.EAT_ALL

xchat.hook_command("botlogin",botlogin)

def autojoin(word, word_eol, userdata):
	print "autojoin..."
	xchat.command("msg bot1 invite #locked")	
	xchat.command("msg bot2 invite #SecretChan")
	xchat.command("timer -refnum 11 5 join #locked,#SecretChan")
	return xchat.EAT_ALL

xchat.hook_command("autojoin",autojoin)

def gone(word, word_eol, userdata):
	print "gone..."
	xchat.command("mode #MyChan +v-o Nielsen Nielsen")
	xchat.command("away "+word_eol[1])
	Away = " "+word_eol[1]
	return xchat.EAT_ALL

xchat.hook_command("gone",gone)

def back(word, word_eol, userdata):
	xchat.command("quote AWAY")
	Away = ""
	return xchat.EAT_XCHAT

xchat.hook_command("back", back)

