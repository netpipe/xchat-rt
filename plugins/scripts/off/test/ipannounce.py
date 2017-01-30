######################
# Configuration
#
# Trigger used to access functionality
TRIGGER = "!ip"

# Channels this trigger will be active in
CHANNELS = ["#opers"]

# Required channel access to access functionality
# Available modes are defined in MODES right below
# Please note: Users that score above this level
# will also be allowed (i.e. OPs will get access when 
# REQUIRED_ACCESS is "+")
REQUIRED_ACCESS = "@"

# Modes available on this server
# This should be numbered in order of access power,
# i.e. 0 being the lowest access level, voice being 1, etc.
MODES = {""  : 0,
         "+" : 1, 
         "%" : 2,
         "@" : 3,
         "!" : 4
        }

# Print more verbose debugging messages
DEBUG = True

#
# End of configuration 
######################

import xchat

__module_name__ = "IPannounce" 
__module_version__ = "1.1" 
__module_description__ = "Announces a user's IP address"
__module_author__ = "TwoPoints <2points at gmx dot org>"

WHOIS_HOOK = None        # Used to hook/unhook the WHOIS server response

class Messages:
	ChannelNotAllowed = "IPannounce:\tTriggered on channel %s, but only allowed on those channels: %s"
	UserNotAllowed = "IPannounce:\tTriggered by %s with mode '%s', but only allowed with mode '%s'"
	ModeNotDefined = "IPannounce:\tTriggered by '%s' with an undefined usermode '%s'"
	NoParameter = "IPannounce:\tNo username passed after trigger"
	WhoisNick = "IPannounce:\tSending 'WHOIS %s' to server"


def callback_text(word, word_eol, userdata):
	# Bring word array into a more workable format
	nick, message = xchat.strip(word[0]), word[1]
	if len(word) > 2: mode = word[2]
	else:             mode = ""

	# Skip processing if text doesn't match.
	if not message[:len(TRIGGER)].lower() == TRIGGER.lower():
		return xchat.EAT_NONE

	# Also skip if channel doesn't match a configured channel
	if xchat.get_info("channel").lower() not in \
			[c.lower() for c in CHANNELS]:
		if DEBUG:
			print Messages.ChannelNotAllowed % (xchat.get_info("channel"),
					", ".join(CHANNELS))
		return xchat.EAT_NONE

	# Check if user has sufficient access
	try:
		if MODES[mode] < MODES[REQUIRED_ACCESS]:
			if DEBUG:
				print Messages.UserNotAllowed % (nick, mode, REQUIRED_ACCESS)
			return xchat.EAT_NONE
	except KeyError:
		if DEBUG:
			# Mode was not defined
			print Messages.ModeNotDefined % (nick, mode)
		return xchat.EAT_NONE

	# Make sure a nick was passed after the trigger
	splitted = message.split()
	if len(splitted) < 2:
		if DEBUG:
			print Messages.NoParameter
		return xchat.EAT_NONE

	target_nick = xchat.strip(splitted[1]).strip()

	# Hook WHOIS event
	global WHOIS_HOOK
	if not WHOIS_HOOK:
		WHOIS_HOOK = xchat.hook_server('378', callback_whois,
			userdata=xchat.get_context()) # 378 = WHOIS host/IP
	
	if DEBUG:
		print Messages.WhoisNick % target_nick

	xchat.command("WHOIS %s" % target_nick)

def callback_whois(word, word_eol, context):
	# Set context
	context.command("SAY %s: %s" % (word[3], word[-1]))

	return xchat.EAT_NONE

def callback_endofwhois(word, word_eol, userdata):
	# End of WHOIS reply, unhook event
	global WHOIS_HOOK
	if WHOIS_HOOK:
		xchat.unhook(WHOIS_HOOK)
		WHOIS_HOOK = None
	
	return xchat.EAT_NONE

xchat.hook_server('318', callback_endofwhois) # 318 = End of WHOIS
xchat.hook_print("Channel Message", callback_text)
xchat.hook_print("Channel Msg Hilight", callback_text)
print __module_name__, __module_version__, "loaded."

