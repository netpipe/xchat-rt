##########################################################
# RAW.tcl                                                #
# Version 0.2                                            #
# Alchera <www.ballarat.us>                              #
##########################################################

# Formatted RAWs received from the server (RFC1459)

#########################################################################
##################### DO NOT EDIT BELOW THIS LINE!! #####################
#########################################################################

# ERR_NOSUCHSERVER
on 402 no_server {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\t[bold][lindex $_rest 0][bold]: No Such Server"
    complete EAT_ALL
}

# ERR_NOSUCHCHANNEL
on 403 no_channel {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tNo Such Channel [bold][lindex $_rest 0]"
    complete EAT_ALL
}

# ERR_CANNOTSENDTOCHAN
on 404 cannot_send {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tCan't Send to Channel [bold][lindex $_rest 0]"
    complete EAT_ALL
}

# ERR_TOOMANYCHANNELS
on 405 2many_channels {
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tYou have joined too many channels!"
    complete EAT_ALL
}

# ERR_WASNOSUCHNICK
on 406 no_nick {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tThere was no such nick [bold][lindex $_rest 0]"
    complete EAT_ALL
}

# ERR_NOTEXTTOSEND
on 412 no_text {
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tNo Text to Send"
    complete EAT_ALL
}

# ERR_UNKNOWNCOMMAND
on 421 unknown_cmd {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tUnknown Command: [lindex $_rest 0]"
    complete EAT_ALL
}

# ERR_NONICKNAMEGIVEN
on 431 no_nick {
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tNo Nickname Given!"
    complete EAT_ALL
}

# ERR_NICKNAMEINUSE
on 433 nick_inuse {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\t[bold][lindex $_rest 0][bold] is in use!"
    complete
}

# ERR_BANNICKCHANGE
on 437 ban_nickchange {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tCan't Change Nick while [bold]banned[bold] or [bold]moderated[bold] on [lindex $_rest 0]"
    complete EAT_ALL
}

# ERR_NCHANGETOOFAST
on 438 change_toofast {
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tNick change too fast!  Wait 20 seconds!"
    complete
}

# ERR_SERVICESDOWN
on 440 no_services {
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tServices are down! Please Wait a while!"
    complete EAT_ALL
}

# ERR_USERNOTINCHANNEL
on 441 not_inchannel {
    set rawdata [split $_rest " "]
    set i [expr {[string first " :" $_rest]+2}]
    set info [string range $_rest $i end]
    set servertab [findcontext [server]]
    print $servertab "[color 13]*\t[color 12]\[[reset][lindex $rawdata 0][color 12]\][reset] [lindex $rawdata 1]: $info"
    complete EAT_ALL
}

# ERR_ONLYSERVERSCANCHANGE
on 468 only_server {
    set i [expr {[string first " :" $_rest]+2}]
    set info [string range $_rest $i end]
    set servertab [findcontext [server]]
    print $servertab "[color 13]*\t[reset]$info"
    complete EAT_ALL
}

# ERR_CHANNELISFULL
on 471 channel_full {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\t[bold][lindex $_rest 0][bold] is full!"
    complete EAT_ALL
}

# ERR_UNKNOWNMODE
on 472 unknown_mode {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tUnknown Mode: [lindex $_rest 0]"
    complete EAT_ALL
}

# ERR_INVITEONLYCHAN
on 473 invite_only {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\t[bold][lindex $_rest 0][bold] is Invite Only!!"
    complete EAT_ALL 
}

# ERR_BADCHANNELKEY
on 475 wrong_key {
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\t[lindex $_rest 0] Requires the correct key!"
    complete EAT_ALL
}

# ERR_NEEDREGGEDNICK
on 477 need_regnick {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    if {[string match -nocase "*abjects.net*" [getinfo server]]} {complete}
    print $servertab "[color 2]*\tYou need a registered nick to join [bold][lindex $_rest 0]"
    complete EAT_ALL
}

# ERR_CHANOPRIVSNEEDED
on 482 im_notop {
    set _rest [split $_rest " "]
    set servertab [findcontext [server]]
    print $servertab "[color 2]*\tYou are not a channel operator on [bold][lindex $_rest 0]"
    complete EAT_ALL
}

# ERR_UMODEUNKNOWNFLAG
on 501 unknown_flag {
	print "[color 4]*\tUnknown Mode Flag"
	complete EAT_ALL
}

# ERR_USERSDONTMATCH
on 502 users_dontmatch {
    print "[color 2]*\tYou can't change modes for other users!"
    complete EAT_ALL
}

# RPL_WATCHOFF
on 602 watch_off {
    set _rest [split $_rest " "]
    print "[color 13]*\t[reset]No longer watching: [bold][lindex $_rest 0]"
    complete EAT_ALL
}

proc color { {arg {}} } {
  return "\003$arg"
}

proc bold { } {
  return "\002"
}

proc underline { } {
  return "\037"
}

proc reverse { } {
  return "\026"
}

proc reset { } {
  return "\017"
}