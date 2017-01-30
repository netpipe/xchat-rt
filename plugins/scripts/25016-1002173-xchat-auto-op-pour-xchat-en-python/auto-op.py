# -*- coding: cp1252 -*-
__module_name__ = "auto-op"
__module_version__ = "0.1"
__module_description__ = "auto op - Python"
__module_author__ = "tchoutchou@lexpress.net - cyberdivad"

# auto op for all and bot.

import xchat
from string import strip

def autoop(word, word_eol, userdata):
    event, pos = userdata
    if type(pos) is int:
        pos = (pos,)
    if word[1] == "!op" and word[0] == "sumawa":
       xchat.command(strip("me -->je te op "+word[0]))
       xchat.command(strip("mode #aspirine +o sumawa"))
    if word[1] == "!op" and word[0] == "CyberDiv":
       xchat.command(strip("me -->je te op "+word[0]))
       xchat.command(strip("mode #aspirine +o CyberDiv"))
    if word[1] == "!op" and word[0] == "yelosubma":
       xchat.command(strip("me -->je te op "+word[0]))
       xchat.command(strip("mode #aspirine +o yelosubma"))   
    if word[1] == "!op" and word[0] == "remydel":
       xchat.command(strip("me -->je te op "+word[0]))
       xchat.command(strip("mode #aspirine +o remydel"))
    if word[1] == "!op" and word[0] == "mancholivier":
       xchat.command(strip("me -->je te op "+word[0]))
       xchat.command(strip("mode #aspirine +o mancholivier"))
    return xchat.EAT_NONE

EVENTS = [
  ("Channel Message", 1),
  
 ]
for event in EVENTS:
    xchat.hook_print(event[0], autoop, event)

    
#-------------------------------------------
# averti tous le monde que l'auto-op est desactivé
def unload_cb(userdata): 
    xchat.command("me >>> Auto-op est suspendu...")
    
xchat.hook_unload(unload_cb)


print "*********************"
print "Auto-op charge!"

print "Script realise par tchoutchou@lexpress.net - 2004"    

