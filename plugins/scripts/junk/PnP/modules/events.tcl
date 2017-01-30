if {[config [network] [channel] show_clone_join] eq ""} {
    set ::cfg(show_clone_join) 1
}
if {[config [network] [channel] show_banned_user] eq ""} {
    set ::cfg(show_banned_user) 1
}
if {[config [network] [channel] rejoin_to_gain_op] eq ""} {
    set ::cfg( rejoin_to_gain_op) 1
}
if {[config [network] [channel] normalize_users_charset] eq ""} {
    set ::cfg(normalize_users_charset) 1
}
if {[config [network] [channel] delay_autoop] eq ""} {
    set ::cfg(delay_autoop) 1
}

on MODE ban {
    #KADOSH!SaLiTRe@KADOSH MODE #sexo +b *!*@AZkxRM.Bl9aTG.virtual
    if {[xconfig irc_conf_mode]} {
        return
    }
    set mode [lindex [split $_rest { }] 0]
    set arg [join [lrange [split $_rest { }] 1 end]]
    set i 0
    set nick [lindex [split [lindex [split [lindex $_raw 0] ":"] 1] "!"] 0]
    set modes [join [lrange $_raw 2 end]]
    
    set report [list]
    foreach m [split $mode {}] {
        switch -- $m {
            {+} {            
                set type "[lang Banned]-"
                continue
            }
            {-} {
                set type "[lang Unbanned]-"
                continue
            }
            {b} {
                set list [join [hostuser [server] [channel] [lindex $arg $i]] {, }]
                if {$list ne {}} {
                    lappend report "$type \002$list\002\."
                }
                incr i
            }
        }
    }
    if {($report ne {}) && ([config [network] [channel] show_banned_user] == 1)} {
        puts "*** $nick changes modes: \002\[\002$modes\002]\002"
        puts "*** \[[channel]\] [join $report]"
        complete eat_xchat
    }
    
}

on xc_join clone {

    if {[xconfig irc_conf_mode]} {
        return
    }
    set usermask [lindex $_raw 3]
    set mask *@[lindex [split $usermask {@}] 1]
    set clones [userclon [server] [channel] $mask]
    set nick [lindex $_raw 1]
    if {[llength $clones] > 0} {
        if {[config [network] [channel] show_clone_join] == 1} {
            set cloneshow "- [lang ShowClones "\002[llength $clones]\002" [join $clones {, }]]"
            puts "[xevent Join $nick [channel] $usermask] $cloneshow"
        } else {
            puts "[xevent Join $nick [channel] $usermask]"
        }
    } else {
        puts "[xevent Join $nick [channel] $usermask]"
    }

    complete eat_xchat
}

on xc_join UserList {
    global userFile
    set nick [lindex $_raw 1]
    set usermask [lindex $_raw 3]
    set hand [nick2hand $nick!$usermask]
    if {[matchattr $hand b [channel]]} {
        print [channel] "\0036*** [lang AutoKickban $nick \($hand\)]\0036"
        if {[isop $nick [channel]]} {
            /QUOTE MODE [channel] -o+b $nick [userhost [server] [channel] $nick [xconfig irc_ban_type]]
        } else {
            /QUOTE MODE [channel] +b [bantype $nick!$usermask [xconfig irc_ban_type]]
        }
        if {[dict exists $userFile $hand --KREASON]} {
            set kreason [dict get $userFile $hand --KREASON]
        } else {
            set kreason "You are not welcomed."
        }
        /QUOTE KICK [channel] $nick :$kreason

        return
    }
    if {[matchattr [nick2hand $nick!$usermask] o [channel]]} {
        print [channel] "\0036*** [lang AutoOp $nick \($hand\)]\0036"
        set delay [config [network] [channel] delay_autoop]
        if {$delay > 0} {
            timer $delay [list command [channel] "/QUOTE MODE [channel] +o $nick"]
        } else {
            command [channel] "/QUOTE MODE [channel] +o $nick"
        }
    }
    if {[matchattr [nick2hand $nick!$usermask] v [channel]]} {
        print [channel] "\0036*** [lang AutoVoice $nick \($hand\)]\0036"
        set delay [config [network] [channel] delay_autoop]
        if {$delay > 0} {
            timer $delay [list command [channel] "/QUOTE MODE [channel] +v $nick"]
        } else {
            command [channel] "/QUOTE MODE [channel] +v $nick"
        }
        return
    }
        
}
on xc_part part {
    
    if {[xconfig irc_conf_mode]} {
        return
    }    
    set usermask [lindex $_raw 2]
    set mask *@[lindex [split $usermask {@}] 1]
    set nick [lindex $_raw 1]
    set clones [userclon [server] [channel] $mask]
    set id [lsearch -exact $clones [text2pattern $nick]]
    set clones [lreplace $clones $id $id]
    if {[llength $clones] > 0} {
        if {[config [network] [channel] show_clone_join] == 1} {
            set cloneshow "- [lang ShowClones "\002[llength $clones]\002" [join $clones {, }]]"
            puts "[xevent Part $nick $usermask [channel]] $cloneshow"
        } else {
            puts "[xevent Part $nick $usermask [channel]]"
        }
        
    } else {
        puts "[xevent Part $nick $usermask [channel]]"
    }
    if {([llength [chanlist [server] [channel]]] == 2) && ([config [network] [channel] rejoin_to_gain_op] == 1)} {
        print [server] "[lang RejoinGainOp [channel]]"
        /PART [channel] Getting op...
        /JOIN [channel]
    }
    complete eat_xchat    
}

on xc_quit quit {
   if {([llength [chanlist [server] [channel]]] == 2) && ([config [network] [channel] rejoin_to_gain_op] == 1)} {
        print [server] "[lang RejoinGainOp [channel]]"
        /PART [channel] Getting op...
        /JOIN [channel]
    }
}


on xc_ujoin SelfJoin {
    #print [server] [channel] " *** [lang NowTalkInChan [channel]]"
    print [server] [channel] " *** ········································"
    set ::join([channel]) [clock seconds]
    # Because XChat doesn't support an event to catch what you are typing
    # I should do this crap. In this way I can call more than one proc in
    # different modules or addons
    alias @[channel] {
        global aliases
        foreach id [array names aliases] {
            eval {$aliases($id) $_rest}
        }
    }
    complete EAT_XCHAT
}

on xc_upart SelfPart {
    alias @[channel] {}
}
on XC_KICK FixEncoding {
    
    if {[config [network] [channel] normalize_users_charset] != 1} {
        return
    }
    set nick [lindex $_raw 1]
    set knick [lindex $_raw 2]
    set chan [lindex $_raw 3]
    set kickmsg [lindex $_raw 4]
    if {[xconfig irc_conf_mode]} {
        return
    }    
    puts [xevent Kick $nick $knick $chan [encoding convertfrom $kickmsg]]
    complete EAT_XCHAT
}

on XC_CHANMSG FixEncoding {
    if {[config [network] [channel] normalize_users_charset] != 1} {
        return
    }
    set nick [lindex $_raw 1]
    set msg [lindex $_raw 2]
    set nnick [regsub -all -- {\003(\d{1,2})?(,\d{1,2})?} $nick ""]
    set mode [lindex [lsearch -exact -index 0 -inline [users [server] [channel]] $nnick] 2]
    puts [xevent {Channel Message} $nick [encoding convertfrom $msg] $mode]
    complete EAT_XCHAT
}



on INVITE OnInvite {
    #:La_Ley!El_Orden@Viva_España_Viva_El_Rey_Viva_El_Orden_Y_La_Ley INVITE Sentencia :#eggdrop
    set nick [lindex [split $_raw {: }] 1]
    set chan [lindex [split $_raw {:}] 2]
    set ::invite($chan) $nick
    command "/mode $chan"
    complete eat_xchat
}

on 324 OnInvite {
    #:dune.irc-hispano.org 324 Sentencia #eggdrop +tnrC
    set chan [lindex [split $_raw] 3]
    set modes [string trim [join [lrange [split $_raw] 4 end]]]
    if {[info exists ::invite($chan)]} {
        set nick [lindex [split $::invite($chan) {!}] 0]
        set uhost [lindex [split $::invite($chan) {!}] 1]        
        puts "[xevent Invited "$chan ($modes)" "$nick ($uhost)" [server]]"
        unset ::invite($chan)
        complete eat_xchat
    }
}

on 403 OnInvite {
    # :dune.irc-hispano.org 403 Sentencia #mierda :No such channel - Canal inexistente
    set chan [lindex [split $_raw] 3]
    if {[info exists ::invite($chan)]} {
        set nick [lindex [split $::invite($chan) {!}] 0]
        set uhost [lindex [split $::invite($chan) {!}] 1]        
        puts "[xevent Invited "$chan ($modes)" "$nick ($uhost)" [server]]"
        unset ::invite($chan)
        complete eat_xchat
    }
}

# User Lists
