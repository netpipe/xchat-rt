# eggdrop v1.6.20 - Tcl 8.5
# The puppet script!

set puppet(author) {baqilla}
set puppet(version) {1.0}
set puppet(chan) {#eggdrop}
bind msg - kickban puppetMsgKB
bind msg - kick puppetMsgK
proc puppetMsgKB {nick uhost hand text} {
    
    global puppet
    set target [lindex [split $text] 0]
    set target [lsearch -nocase -inline [chanlist $puppet(chan)] $target]
    set msg [join [lrange [split $text] 1 end]]
    if {$target eq {}} {
        puthelp "PRIVMSG $nick :Escribe /msg $::botnick kickban <victima> \[mensaje de expulsión\]"
        return
    }
    if {![matchattr $hand o|o $puppet(chan)]} {
        puthelp "PRIVMSG $nick :¿Y tú quién eres? Ponte tu nick registrado en $puppet(chan)."
        return
    }
    if {![onchan $target $puppet(chan)]} {
        puthelp "PRIVMSG $nick :No veo a $target en $puppet(chan)"
        return
    }
    if {$target eq $::botnick} {
        puthelp "PRIVMSG $puppet(chan) :Joder qué mala leche. $nick Intentaba que yo me banease a mí mismo. ¡Pero si solo soy un bot sin cabea!."
        return
    }
    if {[isop $target $puppet(chan)]} {
        puthelp "PRIVMSG $nick :$target es también un op. Si quieres banearlo hazlo tú. Además se lo voy a chivar."
        puthelp "PRIVMSG $target :Que sepas que $nick intentó que yo te banease del canal."
        return
    }
    puppetKB kickban $nick $target $msg
}

proc puppetMsgK {nick uhost hand text} {
    
    global puppet
    set target [lindex [split $text] 0]
    set target [lsearch -nocase -inline [chanlist $puppet(chan)] $target]
    set msg [join [lrange [split $text] 1 end]]
    
    if {$target eq {}} {
        puthelp "PRIVMSG $nick :Escribe /msg $::botnick kick <victima> \[mensaje de expulsión\]"
        return
    }
    if {![matchattr $hand |o $puppet(chan)]} {
        puthelp "PRIVMSG $nick :¿Y tú quién eres? Ponte tu nick registrado en $puppet(chan)."
        return
    }
    if {![onchan $target $puppet(chan)]} {
        puthelp "PRIVMSG $nick :No veo a $target en $puppet(chan)"
        return
    }
    if {$target eq $::botnick} {
        puthelp "PRIVMSG $puppet(chan) :Joder qué mala leche. $nick Intentaba que yo me echase a mí mismo. ¡Pero si solo soy un bot sin cabea!."
        return
    }
    if {[isop $target $puppet(chan)]} {
        puthelp "PRIVMSG $nick :$target es también un op. Si quieres echarlo hazlo tú. Además se lo voy a chivar."
        puthelp "PRIVMSG $target :Que sepas que $nick intentó que yo te echase del canal."
        return
    }
    puppetKB kick $nick $target $msg
}

proc puppetKB {flag nick target reason} {
    global puppet
    if {$reason eq {}} {
        set reason {El OP siempre tiene la razón.}
    }
    putquick "NOTICE @$puppet(chan) :\002$nick\002 [string toupper $flag] $target \($reason\)"
    if {$flag eq {kickban}} {
        set hostName [getchanhost $target]
        if {$hostName eq {}} {
            set hostName $target!*@*
        } else {
            set hostName "*!*@[lindex [split $hostName {@}] 1]"
        }
        puthelp "MODE $puppet(chan) +b $hostName"
    }
    puthelp "KICK $puppet(chan) $target :$reason"
}
