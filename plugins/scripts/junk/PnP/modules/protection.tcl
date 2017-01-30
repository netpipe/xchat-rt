# Peace and Protection --
# This script gives full control in your channel.
# Please, don't abuse and be civil.
# This script doesn't works without PnP enviroment.

if {[config [network] "" flood_protect] eq ""} {
    set ::cfg(flood_protect) 0
    set ::cfg(flood_protect_lines) 3
    set ::cfg(flood_protect_seconds) 3
    set ::cfg(flood_protect_remind_minutes) 5
    set ::cfg(flood_action_1) [list {NOTICE <nick> You're typing too fast. Please calm down.}]
    set ::cfg(flood_action_2) [list {KICK <nick> Please stop flooding.}]
    set ::cfg(flood_action_3) [list {TEMPKICKBAN 5 <nick> [5mins] Stop flooding.}]
}

if {[config [network] "" spam_protect] eq ""} {
    set ::cfg(spam_protect) 0
    set ::cfg(spam_protect_regexp) 1
    set ::cfg(spam_protect_matches) [list "me.*ace.*recarga" "Las putas A LA CALLE." "arga.*saldo" "Las aprendices de putas a la CALLE." "\[1-999\]e " "Aquí de negocios nada." "\[1-999\].*eur" "Aquí de negocios nada." "cam.*euro" "Las aprendizas de putas a la CALLE."]    
    set ::cfg(spam_action_1) [list {KICKBAN <nick> <reason>}]
}
if {[config [network] "" repeat_protect] eq ""} {
    set ::cfg(repeat_protect) 0
    set ::cfg(repeat_protect_seconds) 3
    set ::cfg(repeat_protect_lines) 3
    set ::cfg(repeat_action_1) [list {KICK <nick> Please don't repeat yourself.}]
    set ::cfg(repeat_action_2) [list {KICKBAN <nick> Stop repeating yourself.}]
}
#
on PRIVMSG Repeatprotect {
    #:SocialMedia!SM@Manager.SocialMedia.irc-hispano.org PRIVMSG #eggdrop :asi ganaba espacio xD
    global eGuard
    set channel [lindex [split $_raw { }] 2]
    if {[config [network] $channel repeat_protect] != 1} {
        return
    }
    set nick [lindex [split $_raw {:!}] 1]
    set uhost [lindex [split $_raw {:! }] 2]
    set text [join [lrange [split $_raw {:}] 2 end] {:}]
    set text [stripcodes bcruag $text]
    #regsub -all {[ ]|[0-9]|,|\.|,|\;|\:|-\_|\"|^|\?|\¿|!|¡} $text "" text
    set tlength [string length $text]
    set md5 [_md5 [server],$channel,$nick]
    if {![info exists eGuard(repeat,$md5)]} {
        set eGuard(repeat,$md5) $text
        timer [config [network] $channel repeat_protect_seconds] "array unset ::eGuard repeat,*$md5"
	return
    } else { 
        set diff [levenshteinDistance $text $eGuard(repeat,$md5)]
        set total [expr $tlength + [string length $eGuard(repeat,$md5)]]
        set rediff [expr $total - $diff]
        set calc [expr ($rediff*100)/$total]
        if {$calc > 89} {
            if {![info exists eGuard(repeat,lines,$md5)]} {
                set eGuard(repeat,lines,$md5) 1
            }
            incr eGuard(repeat,lines,$md5)
            if {$eGuard(repeat,lines,$md5) >= [config [network] $channel repeat_protect_lines]} {
                eGuardAction "repeat ([lang RepeatCalc $calc%])" [server] $channel $nick $uhost
                #print [server] "$nick $total $rediff $calc"
                #print [server] "$text"
                #print [server] "$eGuard(repeat,$md5)"
                #unset eGuard(repeat,$md5) eGuard(repeat,lines,$md5)
            }
        } else {
            array unset eGuard(repeat,*$md5)
            
        }
    }
    #puts ":: $distance - $tlength // $text \\ $eGuard(repeat,$channel,$nick)"
}

proc levenshteinDistance {s t} {
    # Edge cases
    if {![set n [string length $t]]} {
	return [string length $s]
    } elseif {![set m [string length $s]]} {
	return $n
    }
    # Fastest way to initialize
    for {set i 0} {$i <= $m} {incr i} {
	lappend d 0
	lappend p $i
    }
    # Loop, computing the distance table (well, a moving section)
    for {set j 0} {$j < $n} {} {
	set tj [string index $t $j]
	lset d 0 [incr j]
	for {set i 0} {$i < $m} {} {
	    set a [expr {[lindex $d $i]+1}]
	    set b [expr {[lindex $p $i]+([string index $s $i] ne $tj)}]
	    set c [expr {[lindex $p [incr i]]+1}]
	    # Faster than min($a,$b,$c)
	    lset d $i [expr {$a<$b ? $c<$a ? $c : $a : $c<$b ? $c : $b}]
	}
	# Swap
	set nd $p; set p $d; set d $nd
    }
    # The score is at the end of the last-computed row
    return [lindex $p end]
}

# Flood protection

on PRIVMSG FloodProtect {
    #:SocialMedia!SM@Manager.SocialMedia.irc-hispano.org PRIVMSG #eggdrop :asi ganaba espacio xD
    global eGuard
    set channel [lindex [split $_raw { }] 2]
    if {[config [network] $channel flood_protect] != 1} {
        return
    }
    set nick [lindex [split $_raw {:!}] 1]
    set uhost [lindex [split $_raw {:! }] 2]
    if {![info exists eGuard(flood,$channel,$nick)]} {
        timer [config [network] [channel] flood_protect_seconds] [list unset -nocomplain ::eGuard(flood,$channel,$nick)]
    } 
    incr eGuard(flood,$channel,$nick)
    if {$eGuard(flood,$channel,$nick) >= [config [network] [channel] flood_protect_lines]} {
        eGuardAction "flood" [server] $channel $nick $uhost
	unset eGuard(flood,$channel,$nick)
    }
}

on PRIVMSG SpamProtect {
    
    global eGuard
    set channel [lindex [split $_raw { }] 2]
    if {[config [network] $channel spam_protect] != 1} {
        return
    }
    set nick [lindex [split $_raw {:!}] 1]
    set uhost [lindex [split $_raw {:! }] 2]
    set line [config [network] [channel] spam_protect_matches]
    set text [join [lrange [split $_raw {:}] 2 end] {:}]
    foreach {regexp reason} $line {
        if {[regexp "$regexp" $text]} {
            eGuardAction "spam ($regexp)" [server] $channel $nick $uhost $reason
            break
        }
    }
}

proc eGuardAction {event server channel nick uhost {reason ""}} {
    global eGuard
    
    set eventInfo [lindex [split [string tolower $event]] 0]
    set log [eGuardLog $eventInfo $server $channel $nick]
    set command [lindex [split [lindex $log 0] ] 0]
    if {[matchattr [nick2hand $nick!$uhost $channel] {f} $channel]} {
	print [server] [lang IgnoreOffence \002$event\002 $nick "\002f\002"]
	return
    } else {
        print [server] "[lang Offence $eGuard(log,$server,$channel,$nick) \[$event\] $nick $channel $command]"
    }
    foreach execute $log {
        set execute [string map [list {<channel>} $channel {<nick>} $nick {<reason>} $reason] $execute]
        set commandExecute [lindex [split $execute] 0]
        set paramExecute [join [lrange [split $execute] 1 end]]
        print $server $channel "\0036*** [lang Punnishing \002$nick\002 $event $commandExecute]..."
        # valid commands: TEMPMODE, MODE, TEMPBAN, BAN, TEMPKICKBAN, KICKBAN, KICK, NOTICE, MSG, IPKICK, IPKICKBAN, 
        switch -nocase -exact $commandExecute {
            {KICKBAN} {
                command $server $channel "/kickban $paramExecute"
                #print [server] $log
            }
            {KICK} {
                command $server $channel "/kick $paramExecute"
            }
            {BAN} {
                command $server $channel "/ban $paramExecute"
            }
            {MODE} {
		command $server $channel "/mode $paramExecute"
	    }
	    
            {spam} {
                
            }
        }
    }
}

proc eGuardLog {event server channel nick} {
    global eGuard
    if {![info exists eGuard(log,$server,$channel,$nick)]} {
        set eGuard(log,$server,$channel,$nick) 0
    }
    set i [expr $eGuard(log,$server,$channel,$nick) +1]
    if {[config [network] [channel] ${event}_action_$i] ne ""} {
        incr eGuard(log,$server,$channel,$nick)
    }
    set n $eGuard(log,$server,$channel,$nick)
    timer [config [network] $channel flood_protect_remind_minutes] [list unset -nocomplain ::eGuard(log,$server,$channel,$nick)]
    return [config [network] $channel ${event}_action_$n]
}


