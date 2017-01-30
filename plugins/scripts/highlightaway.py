'''
Written on: 2011.08.17
Last update: 2011.08.19
Author: Kulverstukas == http://newage.ql.lt
Shouts: Evilzone.org
Contact: kulverwars@evilzone.org
----
To use it on Linux, put it into ~/.xchat2 and it will load every time you start xchat
or you can do it manualy with: /py load highlightaway.py
----
To use it on Windows, install Python 2.5
and put it into "X:/Documents and Settings/USERNAME/Application Data/X-Chat 2/"
or you can do it manualy with: /py load highlightaway.py
----
When it is loaded up, just mark yourself as Away (ALT+A) and leave it. It will respond to
anyone who will highlight you.
----
For the script to work all the time and not just once, unmark the "Automaticaly unmark away"
in the "Settings -> Preferences -> General" so that you won't be unmarked as not away when
the message gets said.
----
Update notes:
2011.08.19:
* added a timer and a counter so that spamming does not occur when someone is trolling
  with you and highlights you a lot. Different nickname can highlight you all the time,
  but individual nickname can highlight you once every 60 seconds. The list gets cleared
  every 60 seconds.

=================================================
Copyright 2011 Kulverstukas

 "Highlight away" is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation version 3

 "Highlight away" is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
=================================================
'''
'''
\003
    0 white
    1 black
    2 blue
    3 green
    4 red
    5 dark gred
    6 purple
    7 dark yellow
    8 yellow
    9 bright green
    10 dark green
    11 green
    12 blue
    13 bright purple
    14 dark grey
    15 light grey

\017 reset all 
\002 bold 
\026 reverse text 
\007 beep 
\037 underline
'''
import xchat, os
#==============================================
__module_name__ = "Highlight Away"
__module_version__ = "1.1"
__module_description__ = "This script will send a message with the away message if you get highlighted while being away -- command /hihelp"
#==============================================
#Callback when you get highlighted
def SayAway(word, word_eol, userdata):
    global Nicks
    CurrentAwayMsg = xchat.get_info('away')
    if (CurrentAwayMsg != None):
        NickWhoSaid = word[0]
        if (NickWhoSaid not in Nicks):
            xchat.command('say %s, I am away with reason: %s | http://newage.ql.lt/projects/python/highlightaway.py' % (NickWhoSaid, CurrentAwayMsg))
            Nicks.append(word[0])
#==============================================
def OnUnload(userdata):
    xchat.prnt('\002[+]\017 \00312"Highlight away" v%s has been unloaded!' % __module_version__)
#==============================================
def OnCommand(word, word_eol, userdata):
    xchat.prnt('\n\0035\002%s\n\0035\002Tips of using "Highlight away":\n\0035When the script is loaded up, just mark yourself as Away (ALT+A or "Server -> Marked away") and leave it. It will respond to anyone who will highlight you.\n\0035\002----\n\0035For the script to work all the time and not just once, unmark the "Automaticaly unmark away" in the "Settings -> Preferences -> General" so that you won\'t be unmarked as not away when the message is said.\n' % __module_description__)
#==============================================
def OnTimer(userdata):
    global Nicks
    Nicks = [] #empty out the dictionary
    return 1 #keep the timer going
#==============================================
Nicks = []
#Let's hook a print event
xchat.hook_print('Channel Msg Hilight', SayAway) # event grabber
xchat.hook_unload(OnUnload) # prints a mesage when unloaded
xchat.hook_timer(600000, OnTimer) # clear the list every 10 minutes
xchat.hook_command('hihelp', OnCommand) # print the help shit
xchat.prnt('\002[+]\017 \00312"Highlight away" v%s has been loaded! - /hihelp for more info' % __module_version__)
#==============================================
