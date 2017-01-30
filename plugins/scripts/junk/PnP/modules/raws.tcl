set ::modes([network],AWAYLEN) 160
set ::modes(uib,CHANMODES) b,k,l,imnpstcrRMCNu
set ::modes(uib,CHANNELLEN) 64
set ::modes(uib,CHANTYPES)  #&+
set ::modes(uib,CHARMAPPING) rfc1459
set ::modes(uib,KICKLEN) 160
set ::modes(uib,MAXBANS) 200
set ::modes(uib,MAXCHANNELS) 25
set ::modes(uib,MAXTARGETS) 20
set ::modes(uib,MODES) 6
set ::modes(uib,NETWORK) ""
set ::modes(uib,NICKLEN) 30
set ::modes(uib,PREFIX) (ov)@+
set ::modes(uib,QUITLEN) 300
set ::modes(uib,SILENCE) 25
set ::modes(uib,TOPICLEN) 240
set ::modes(uib,WATCH) 96
on 005 serverModes {
    set modes [lindex [split $_raw {:}] 1]
    set modes [join [lrange [split $modes { }] 3 end]]
    set id [lsearch -all -inline [split $modes { }] {*=*}]
    foreach nid $id {
        set vid [lindex [split $nid {=}] 0]
        set value [lindex [split $nid {=}] 1]
        set ::modes([network],$vid) $value
    }
}

on 307 Whois {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    print $server [channel] "*** $target [join [lrange [split $_raw {:}] 2 end] {:}]"
    complete eat_xchat
}

on 311 Whois {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set where [xconfig irc_whois_front]
    if {$where == 1} {
        print $server [channel] "*** ········································"
        print $server [channel] "*** $target is [lindex $_raw 4]@[lindex $_raw 5]"
        print $server [channel] "*** $target is «\00312[join [lrange [split $_raw {:}] 2 end] {:}]\003»"

    } else {
        print $server "*** ········································"
        print $server "*** $target is [lindex $_raw 4]@[lindex $_raw 5]"
        print $server "*** $target is «\00312[join [lrange [split $_raw {:}] 2 end] {:}]\003»"

    }
    
    complete eat_xchat
}

on 312 Whois {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set where [xconfig irc_whois_front]
    if {$where == 1} {
        print $server [channel] "*** $target on [lindex $_raw 4] ([join [lrange [split $_raw {:}] 2 end] {:}])"
    } else {
        print $server "*** $target on [lindex $_raw 4] ([join [lrange [split $_raw {:}] 2 end] {:}])"
    }
    complete eat_xchat
}

on 315 who {
    #:dune.irc-hispano.org 315 Sentencia la_ley :End of /WHO list
    set match [lindex [split $_raw] 3]    
    if {[info exists ::who]} {
        set server [lindex [split [lindex [split $_raw {:}] 1]] 0]
        print $server "*** End of /WHO $match list; $::who matches."
        unset -nocomplain ::who
        complete EAT_XCHAT
    }
    if {[info exists ::join($match)]} {
        unset -nocomplain ::join($match)
    }
}
on 317 Whois {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set where [xconfig irc_whois_front]
    if {$where == 1} {
        print $server [channel] "*** $target idle [duration [lindex $_raw 4]], logged in [duration [expr [clock seconds]-[lindex $_raw 5]]] ago"
    } else {
         print $server "*** $target idle [duration [lindex $_raw 4]], logged in [duration [expr [clock seconds]-[lindex $_raw 5]]] ago"
    }
    complete eat_xchat
}

on 318 Whois {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set where [xconfig irc_whois_front]
    if {$where == 1} {
        print $server [channel] "*** ········································"
    } else {
        print $server "*** ········································"
    }
    complete eat_xchat
}
on 319 Whois {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set channels [join [lrange [split $_raw {:}] 2 end] {:}]
    foreach c [channels] {
        set channels [string map [list $c "\002$c\002"] $channels]
    }
    set where [xconfig irc_whois_front]
    if {$where == 1} {     
        print $server [channel] "*** $target on $channels"
    } else {
        print $server "*** $target on $channels"
    }
    complete eat_xchat
}

on 333 topicend {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set by [lindex $_raw 4]
    set when [lindex $_raw 5]
    print $server $target " *** \[$target\] topic- set by $by ([clock format $when])"
    complete eat_xchat
}

on 338 whois {

    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set where [xconfig irc_whois_front]
    if {$where == 2221} {
        print $server [channel]
    }
}

on 352 who {
    #:dune.irc-hispano.org 352 Sentencia #eggdrop hxm 237.Red-213-98-70.staticIP.rima-tde.net irc.irc-hispano.org Sentencia H@xr :0 Tengo un humor, negro.
    
    set line [lindex [split $_raw {:}] 1]
    set server [lindex [split $line] 0]
    set chan [lindex [split $line] 3]
    if {[info exists ::join($chan)]} {
        complete
        return
    }
    set ident [lindex [split $line] 4]
    set host [lindex [split $line] 5]
    set nick [lindex [split $line] 7]
    set modes [lindex [split $line] 8]
    set mode ""
    if {[string match {*@*} $modes]} {
        set mode @
    }
    if {[string match {*+*} $modes]} {
        set mode [append $mode +]
    }
    if {[string match {*G*} $modes]} {
        lappend status [lang Away]
    } else {
        lappend status [lang Present]
    }
    if {[string match {*r*} $modes]} {
        lappend status [lang Registered]
    } else {
        lappend status [lang Unregistered]
    }
    if {[string match {*\**} $modes]} {
        lappend status [lang Ircop]
    }
    if {[string match {*R*} $modes]} {
        lappend status [lang MessageRestricted]
    }
    if {[string match {*k*} $modes]} {
        lappend status [lang Protected]
    }
        
    set realname [join [lrange [split [join [lrange [split $_raw {:}] 2 end] {:}] { }] 1 end] { }]
    print $server -
    print $server "*** [lang WhoOn $nick ($ident@$host) $mode$chan ([join $status {; }])]"
    print $server "*** [lang WhoOnIs $nick «\00312$realname\00312»]"
    incr ::who
    complete 
}
on 366 namesjoin {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    if {[info exists ::join($target)]} {
        set users [llength [lrange [users $server $target] 1 end]]
        set ops [llength [lsearch -exact -index 2 -all [users [server] $target] {@}]]
        set regular [expr $users - $ops]
        set percentOp [expr ($ops * 100.0) / $users]
        set percentRegular [expr ($regular * 100.0) / $users]
        print $server $target " *** [lang JoinUsers \[$target\] $users]"
        print $server $target " *** [lang JoinUsersResult \[$target\] $ops [format %.2f $percentOp]% $regular [format %.2f $percentRegular]%]"
        print $server $target " *** ········································"
    }
}

on 379 Whois {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set where [xconfig irc_whois_front]
    if {$where == 1} {
        print $server [channel] "*** [join [lrange [split $_raw {:}] 2 end] {:}]"
    } else {
        print $server "*** [join [lrange [split $_raw {:}] 2 end] {:}]"
    }
    complete eat_xchat
}

on 401 Whois {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set where [xconfig irc_whois_front]
    if {$where == 1} {
        print $server [channel] "*** ········································"
        print $server [channel] "*** \[\002$target\002\] no such user."
    } else {
        print $server "*** ········································"
        print $server "*** \[\002$target\002\] no such user."
    }
    complete eat_xchat
}

on 402 Whois {
    set target [lindex $_raw 3]
    set server [lindex [split $_raw {: }] 1]
    set where [xconfig irc_whois_front]
    if {$where == 1} {
        print $server [channel] "*** ········································"
        print $server [channel] "*** \[\002$target\002\] no such server."
    } else {
        print $server "*** ········································"
        print $server "*** \[\002$target\002\] no such server."
    }
    complete eat_xchat
}
