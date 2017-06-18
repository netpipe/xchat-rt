#!/usr/bin/env python
#----------------------------------------------------------------------------
"""
 Author: Tx1491 (Tx1491@hyow.eu.org)
 Description: see __module_description__
 Date: 2004-08-12
 Notes: There's a 1-2 second pause in xchat when making the url due to it having
        to retreive the tiny url result page. If anything at all can be done about
        it, it'll get done in 0.2. Threading may be the answer
        Also, why the hell has no one does this yet in python? ack to the perl
        module, well, perl in general ;)
"""

#----------------------------------------------------------------------------

import xchat, urllib2, re

#----------------------------------------------------------------------------

__module_name__ = "pyTinyUrl"
__module_version__ = "0.1"
__module_description__ = "Make tinyurl from specified url and send it to current channel"

def onTinyUrl(word, word_eol, userdata):
	if len(word) < 2: 
		print "you must specify a url eg, /tinyurl http://www.google.com/"
	else:
		# fetch tinyurl result
		sock = urllib2.urlopen("http://tinyurl.com/create.php?url=" + word[1])
		htmlSource = sock.read()
		sock.close()

		# parse out link
		match = re.search('<a href="(.*?)" target="_blank">Open in new window</a>', htmlSource)
		result = match.group(1)

		# display
		xchat.command("MSG " + xchat.get_info("channel") + " " + result)
	return xchat.EAT_ALL

xchat.hook_command("TINYURL", onTinyUrl, help="/TINYURL <url>")
