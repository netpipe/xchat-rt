#########################################################################
#			Exaile Now Playing				#
#          	     Python Plugin For X-Chat				#
#									#
#		Written by Sonic <sonic88@sonic88.org>			#
#		    IRC: Sonic @ irc.azzurra.org			#
#########################################################################

#! /usr/bin/env python

import xchat, dbus, string
__module_name__ = "Exaile np"
__module_version__ = "1.2"
__module_description__ = "Tell which song are you currently playing in exaile"

exa = ""

print "Exaile Now Playing Plugin by Sonic <sonic@sonic88.org> loaded clean."
print "You can find me on irc: Sonic @ irc.azzurra.org"
def get_position():
	global exa
	return exa.current_position()
def get_length():
	global exa
	return exa.get_length()

def get_title():
	global exa
	return exa.get_title()

def get_artist():
	global exa
	return exa.get_artist()

def show_song(word, word_eol, userdata):
	global exa
	try:
		bus = dbus.SessionBus()
		obj = bus.get_object("org.exaile.DBusInterface","/DBusInterfaceObject")
		exa = dbus.Interface(obj,"org.exaile.DBusInterface")
	except:
		print "Error during trying to comunicate with exaile!"
		return xchat.EAT_ALL
	if len(word) > 1:
		if string.lower(word[1]) == "play":
			exa.play()
		elif string.lower(word[1]) == "pause":
			exa.play_pause()
		elif string.lower(word[1]) == "stop":
			exa.stop()
		elif string.lower(word[1]) == "next":
			exa.next_track()
		elif string.lower(word[1]) == "prev":
			exa.prev_track()
		elif string.lower(word[1]) == "vol-":
			exa.decrease_volume(5)
		elif string.lower(word[1]) == "vol+":
			exa.increase_volume(5)
		elif string.lower(word[1]) == "version":
			print "Exaile Now Playing Plugin by \2Sonic\2 <sonic88@sonic88.org> version \2" + __module_version__ + "\2"
			print "Exaile Version: \2" + str(exa.get_version()) + "\2"
		elif string.lower(word[1]) == "help":
			print "Exaile Now Playing Plugin for XChat HELP:"
			print "Type \2/" + word[0] + "\2 to say what are you listening to."
			print "Other Functions are:"
			print "\2/" + word[0] + " play\2 to put your player in play mode."
			print "\2/" + word[0] + " pause\2 to play/pause your player."
			print "\2/" + word[0] + " stop\2 to stop your player."
			print "\2/" + word[0] + " next\2 to go to the next track."
			print "\2/" + word[0] + " prev\2 to go to the previous track."
			print "\2/" + word[0] + " vol+\2 to increase the volume."
			print "\2/" + word[0] + " vol-\2 to decrease the volume."
			print "\2/" + word[0] + " version\2 to show plugin and player version."
			print "\2/" + word[0] + " help\2 to show this help."
			print "\2***\2 End Of Help \2***\2"
		else:
			print "Unknow command type: \2/" + word[0] + " help\2 for more information."
		return xchat.EAT_ALL
	try:
		position = int(get_position())
	except:
		print "Error during trying to comunicate with exaile!"
		return xchat.EAT_ALL
	if not position:
		print "Exaile is stopped or not running!"
		return xchat.EAT_ALL
	title = get_title()
	title = title.encode("utf-8")
	artist = get_artist()
	artist = artist.encode("utf-8")
	length = get_length()
	xchat.command ("me is listening to " + artist + " - " + title + " [" + str(length) + " (" + str(position) + "%)]")
	return xchat.EAT_ALL



xchat.hook_command("song", show_song, help="/song Tell which song are you currently playing in exaile, /song help for more infos") 
xchat.hook_command("exaile", show_song, help="/exaile Tell which song are you currently playing in exaile, /exaile help for more infos") 
	
