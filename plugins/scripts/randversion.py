import xchat, random

__module_name__ = "RCVR" 
__module_version__ = "0.2" 
__module_description__ = "random ctcp version reply"
__module_author__ = "afby"

print "loaded %s" % __module_name__
xchat.command("SET irc_hide_version 1")

replys = [
	'VERSION gruiiikk v1.1.0 - running on NetBSD i686',
	'VERSION lut1n v6.6.6 - running on OpenBSD i486',
	'VERSION CHATSKY 1.0.0',
	'VERSION Konversation 0.25 Build 5120 (C) 2001-2008 by the Konversation t3am',
	'VERSION orange taco compiled on May  2 1999, running NetBSD / i386',
	'VERSION cinza v0.0.1 - running on FreeBSD i386',
	'VERSION p4n v0.9.6 - running on Linux i686',
	'VERSION tbksloar v0.5.9-cvs compiled on May  2 2101, running Linux 2.6.12-10-686 / i686',
	'VERSION yAMAZO v1.2.3 - running on NetBSD i386',
	'VERSION Konversation 6.66 Build 1999 (C) 2002-2012 by the Konversation te4m',
	'VERSION pidgin 2.8.9 mUbuntu'
	]

def ctcpgene_cb(word, word_eol, userdata): 
	#print "CTCP RECU" , word[2] 
	#return xchat.EAT_XCHAT # Don't let xchat do its normal printing 
	xchat.command("NCTCP %s %s" % (word[1],random.choice(replys)))

def unload_cb(userdata): 
	print "unloaded %s" % __module_name__
	xchat.command("SET irc_hide_version 0") 

xchat.hook_print("CTCP Generic", ctcpgene_cb) 
xchat.hook_unload(unload_cb) 
