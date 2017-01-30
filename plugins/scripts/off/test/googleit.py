__module_name__ = "let me google that"
__module_version__ = "0.8"
__module_description__ = "makes something googleable "

import xchat
import urllib

print "\0034",__module_name__, __module_version__,"loading...\003"
def letmegoogleit(word, word_eol, userdata):
	wordStr = "+".join(word[1:])
	print "googling for "+wordStr
	endUrl = urllib.urlopen('http://tinyurl.com/api-create.php?url=http://letmegooglethatforyou.com?q='+wordStr).read()
	print endUrl
	xchat.command("PRIVMSG " +  xchat.get_info("channel") + " " + endUrl)

xchat.hook_command('lmgi', letmegoogleit)
