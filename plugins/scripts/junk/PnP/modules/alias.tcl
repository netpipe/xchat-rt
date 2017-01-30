alias pnp {
    source [xchatdir]/main.tcl
    complete
}
alias conf {
    exec wish8.5 $::pnp(scriptDir)/PnP/config.tcl &
    complete
}

alias dxkicks {
    bgExec [list wish8.5 $::pnp(scriptDir)/PnP/dialogs.tcl dxkicks] bgExecGenericHandler pCount
    complete    
}

alias bug {
    bgExec [list wish8.5 $::pnp(scriptDir)/PnP/dialogs.tcl bug] bgExecGenericHandler pCount
    complete    
}

proc echo {args} {puts $args}

alias profile {
    
    if {$_rest eq ""} {
        puts "[lang CurrentProfile [profile get]]"
    } else {
        set name [lindex [split $_rest] 0]
        if {[profile exists $name]} {
            profile set $name
        } else {
            profile new $name
        }
        /pnp
    }
    complete
}

alias addon {

    set args $_rest
    global pnp
    if {![info exists ::cfg(addons)]} {
        set ::cfg(addons) [list]
    }
    if {$args eq ""} {
        set addons [glob $pnp(addonDir)/*.tcl]
        foreach addon $addons {
            lappend list [file rootname [file tail $addon]]
        }
        puts [lang AvailableAddon [join $list {, }].]
    } else {
        set cmd [lindex $args 0]
        set file [lindex $args 1]
        switch $cmd {
            {load} {
                if {![file exists [file join $pnp(addonDir) $file.tcl]]} {
                    puts [lang NoSuchAddon $file]
                } else {
                    set id [lsearch -nocase -exact [config "" "" addons] $file]
                    if {$id == -1} {
                        lappend ::cfg(addons) $file
                    }
                    source [file join $pnp(addonDir) $file.tcl]
                    puts [lang LoadedAddon $file]
                    pnpSave
                }
            }
            {unload} {
                set id [lsearch -nocase -exact [config "" "" addons] $file]
                if {$id == -1} {
                    puts [lang NoSuchAddon $file]
                    return
                }
                set ::cfg(addons) [lreplace $::cfg(addons) $id $id]
                puts [lang UnloadedAddon $file]
                pnpSave 
            }
        }
    }
    complete
}




alias j {
    set chans [split $_rest {,}]
    foreach c $chans {
        set prefix [string index $c 0]
        if {$prefix ne {#}} {
            /QUOTE JOIN #$c
        } else {
            /QUOTE JOIN $c
        }
    }
    complete
}

alias oop {
    set user [lindex $_rest 0]
    if {[isop $user [channel]]} {
        /deop $_rest
    } else {
        /op $_rest
    }
    complete
}

alias vvoice {
    
    set user [lindex $_rest 0]
    if {[isvoice $user [channel]]} {
        /devoice $_rest
    } else {
        /voice $_rest
    }
    complete    
}

alias op {
    
    set max $::modes([network],MODES)
    set nicklist $_rest
    set chanlist [chanlist [server] [channel]]
    foreach nick [split $nicklist { }] {
        set id [lsearch -all -nocase $chanlist [text2pattern $nick]]
        if {[llength $id] > 0} {
            set list [list]
            foreach nid $id {
                lappend list [lindex $chanlist $nid]
                if {[llength $list] == $max} {
                    /QUOTE MODE [channel] +[string repeat o $max] [join $list { }]
                    set list [list]
                }
            }
            /QUOTE MODE [channel] +[string repeat o $max] [join $list { }]
            set list [list]
        }
    }
    complete
}
alias deop {
    
    set max $::modes([network],MODES)
    
    set nicklist $_rest
    set chanlist [chanlist [server] [channel]]
    foreach nick [split $nicklist { }] {
        set id [lsearch -all -nocase $chanlist [text2pattern $nick]]
        if {[llength $id] > 0} {
            set list [list]
            foreach nid $id {
                lappend list [lindex $chanlist $nid]
                if {[llength $list] == $max} {
                    /QUOTE MODE [channel] -[string repeat o $max] [join $list { }]
                    set list [list]
                }
            }
            /QUOTE MODE [channel] -[string repeat o $max] [join $list { }]
            set list [list]
        }
    }
    complete
}

alias voice {
    
    set max $::modes([network],MODES)
    
    set nicklist $_rest
    set chanlist [chanlist [server] [channel]]
    foreach nick [split $nicklist { }] {
        set id [lsearch -all -nocase $chanlist [text2pattern $nick]]
        if {[llength $id] > 0} {
            set list [list]
            foreach nid $id {
                lappend list [lindex $chanlist $nid]
                if {[llength $list] == $max} {
                    /QUOTE MODE [channel] +[string repeat v $max] [join $list { }]
                    set list [list]
                }
            }
            /QUOTE MODE [channel] +[string repeat v $max] [join $list { }]
            set list [list]
        }
    }
    complete
}

alias devoice {
    
    set max $::modes([network],MODES)
    
    set nicklist $_rest
    set chanlist [chanlist [server] [channel]]
    foreach nick [split $nicklist { }] {
        set id [lsearch -all -nocase $chanlist [text2pattern $nick]]
        if {[llength $id] > 0} {
            set list [list]
            foreach nid $id {
                lappend list [lindex $chanlist $nid]
                if {[llength $list] == $max} {
                    /QUOTE MODE [channel] -[string repeat v $max] [join $list { }]
                    set list [list]
                }
            }
            /QUOTE MODE [channel] -[string repeat v $max] [join $list { }]
            set list [list]
        }
    }
    complete
}

alias k {
    
    if {[validchan [lindex [split $_rest { }] 0]]} {
        set target [lindex [split $_rest { }] 0]
        set _rest [join [lrange [split $_rest] 1 end]]
    } else {
        set target [channel]
    }
    set nicklist [lindex [split $_rest { }] 0]
    set reason [join [lrange [split $_rest { }] 1 end]]
    set chanlist [chanlist [server] $target]
    foreach nick [split $nicklist {,}] {
        set id [lsearch -all -nocase $chanlist [text2pattern $nick]]
        if {[llength $id] > 0} {
            foreach nid $id {
                if {$reason eq ""} {
                    set kreason [fileRead [file join $::pnp(scriptDir) PnP kicks.txt]]
                } else {
                    set kreason $reason
                }
                /QUOTE KICK $target [lindex $chanlist $nid] \:$kreason
            }
        } 
    }
    complete
}
 
alias kb {
    set nicklist [lindex [split $_rest { }] 0]
    set reason [join [lrange [split $_rest { }] 1 end]]
    set chanlist [chanlist [server] [channel]]
    foreach nick [split $nicklist {,}] {
        set id [lsearch -all -nocase $chanlist [text2pattern $nick]]
        if {[llength $id] > 0} {
            foreach nid $id {
                if {$reason eq ""} {
                    set kreason [fileRead [file join $::pnp(scriptDir) PnP kicks.txt]]
                } else {
                    set kreason $reason
                }
                /QUOTE MODE [channel] +b [userhost [server] [channel] [lindex $chanlist $nid] [xconfig irc_ban_type]]
                /QUOTE KICK [channel] [lindex $chanlist $nid] \:$kreason
            }
        } 
    }
    complete
}

alias kbc {
    set nicklist [lindex [split $_rest { }] 0]
    set reason [join [lrange [split $_rest { }] 1 end]]
    foreach nick [split $nicklist {,}] {
        set maskhost [userhost [server] [channel] $nick [xconfig irc_ban_type]
        set id [userclon [server] [channel] $maskhost]
        if {[llength $id] > 1} {
            puts [lang KickBannning [llength $id] *!*@[lindex [split $maskhost {@}] 1]]
            /QUOTE MODE [channel] +b $maskhost
            foreach nid $id {
                if {$reason eq ""} {
                    set kreason [fileRead [file join $::pnp(scriptDir) PnP kicks.txt]]
                } else {
                    set kreason $reason
                }
                /QUOTE KICK [channel] $nid \:$kreason
            }
        } else {
            puts "[lang NoClones *!*@[lindex [split $maskhost {@}] 1]]"
        }
    }
    complete
}

alias kbh {
    set nicklist [lindex [split $_rest { }] 0]
    set reason [join [lrange [split $_rest { }] 1 end]]
    foreach nick [split $nicklist {,}] {
        set id [userclon [server] [channel] $nick]
        /QUOTE MODE [channel] +b $nick
        if {[llength $id] > 0} {
            foreach nid $id {
                if {$reason eq ""} {
                    set kreason [fileRead [file join $::pnp(scriptDir) PnP kicks.txt]]
                } else {
                    set kreason $reason
                }
                /QUOTE KICK [channel] $nid \:$kreason
            }
        } 
    }
    complete
}

alias bnlist {
    
    set nicklist [join [lrange [split $_rest { }] 0 end-1]]
    set type [lindex [split $_rest { }] end]
    foreach nick $nicklist {
        /QUOTE MODE [channel] +b [userhost [server] [channel] $nick $type]
    }
    complete
}

alias set {
    if {($_rest eq "pnp") || ($_rest eq "")} {
        foreach id [lsort [array names ::cfg]] {
            set length [expr 29- [string length $id]]
            if {([string is boolean $::cfg($id)]) && ($::cfg($id) ne "")} {
                set result [expr ($::cfg($id) == 1) ? ON : OFF]
            } else {
                set result $::cfg($id)
            }
            print "$id\0033[string repeat {.} $length]\00312:\003 $result"
        }
    } else {
        set var [lindex [split $_rest] 0]
        set value [join [lrange [split $_rest] 1 end]]
        if {$value eq ""} {
            set line [lsearch -nocase -all -inline [array names ::cfg] $var]
            foreach i $line {
                set length [expr 29- [string length $i]]
                
                if {([string is boolean $::cfg($i)]) && ($::cfg($i) ne "")} {
                    set result [expr ($::cfg($i) == 1) ? ON : OFF]
                } else {
                    set result $::cfg($i)
                }
                puts "$i\0033[string repeat {.} $length]\00312:\003 $result"
            }
            return
        }
        if {[lsearch -exact [array names ::cfg] [text2pattern $var]] != -1} {
            if {[trueorfalse $value] == -1} {
                set result $value    
            } else {
                set result [trueorfalse $value]
            }
            set ::cfg($var) $result
            puts "[lang SetSaved $var $result]"
            pnpSave
            complete
        }
    }
    if {$_rest eq {pnp}} {
        complete EAT_ALL
    } else {
        print "\00312[string repeat {.} 30]"

    }
}
