__module_name__ = "IRCOp Scan"
__module_version__ = "1.0"
__module_description__ = "Scans a channel for IRC operators. Triggered with /ircopscan."
__module_author__ = "2points <2points at gmx.org>"

import xchat

def ircopscan(word, word_eol, userdata):
   # Unhook the command to prevent it from being used before
   # the WHO command finished
   global gCommandHook
   xchat.unhook(gCommandHook)
   # Hook IRC messages 352 (RPL_WHOREPLY) and
   # 315 (RPL_ENDOFWHO)
   global gWhoHook, gEndWhoHook
   ircOpList = []
   gWhoHook = xchat.hook_server("352", rpl_whoreply, ircOpList)
   gEndWhoHook = xchat.hook_server("315", rpl_endofwho, ircOpList)
   # Start scan
   xchat.command("WHO %s o" % xchat.get_info("channel"))
   return xchat.EAT_ALL

def rpl_whoreply(word, word_eol, userdata):
   raw_message = word[3:]
   user, host, server, nick, modes = raw_message[1:6]
   if "*" in modes:
      # Only append nickname
      userdata.append(nick)
   
   return xchat.EAT_XCHAT

def rpl_endofwho(word, word_eol, userdata):
   if len(userdata) == 0:
      print "\0034\002IRCOp Scan\017:\tNo IRCOps found."
   else:
      userdata.sort()
      while len(userdata) > 0:
         # Print only 15 nicks per line to prevent crash
         print "\0034\002IRCOp Scan\017:\t%s" % \
               " ".join(userdata[:15])
         userdata = userdata[15:]
   
   # Unhook all WHO hooks
   global gWhoHook, gEndWhoHook
   xchat.unhook(gWhoHook)
   xchat.unhook(gEndWhoHook)
   # Re-register command
   global gCommandHook
   gCommandHook = xchat.hook_command("IRCOPSCAN", ircopscan,
      help="/IRCOPSCAN Lists all IRC operators in a channel.")

   return xchat.EAT_XCHAT

gCommandHook = xchat.hook_command("IRCOPSCAN", ircopscan,
   help="/IRCOPSCAN Lists all IRC operators in the current channel.")
print __module_name__, __module_version__, "loaded."
