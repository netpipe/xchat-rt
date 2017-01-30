# Extras.tcl

if {[config [network] "" show_history_on_join] eq ""} {
   set ::cfg(show_history_on_join) 1
}

global nhistory
if {![info exists nhistory]} {
    set nhistory [dict create]
}
proc nhistory {command host {nick {}}} {
    global nhistory
    
    switch -- $command {
        {exists} {
            if {$nick eq ""} {
                return [dict exists $nhistory $host]
            } else {
                if {[dict exists $nhistory $host]} {
                    set buffer [dict get $nhistory $host]
                } else {
                    return 0
                }
                if {[lsearch -nocase $buffer [text2pattern $nick]] != -1} {
                    return 1
                } else {
                    return 0
                }
            }
        }
        {set} {
            if {[dict exists $nhistory $host]} {
                set list [dict get $nhistory $host]
                if {[lsearch -nocase $list [text2pattern $nick]] == -1} {
                    set nhistory [dict lappend nhistory $host $nick]
                }
            } else {
                set nhistory [dict lappend nhistory $host $nick]
            }
        }
        {get} {
            if {[nhistory exists $host]} {
                return [dict get $nhistory $host]
            }
        }
        {save} {
            set fs [open $::cfg(history_extras_file) w]
            puts $fs $nhistory
            close $fs
        }
        {load} {
            set fs [open $::cfg(history_extras_file) r]
            set nhistory [read $fs]
            close $fs
        }
    }
}

on NICK history {
    
    #:soltero_solo!4@Ay83xn.CWOrqS.virtual NICK :casado_solitario
    scan [lindex [split [lindex $_raw 0] {:}] 1] {%[^!]!%[^@]@%s} nick ident host
    set newnick [lindex [split $_raw {:}] 2]
    nhistory set $host $nick
    nhistory set $host $newnick
}

on QUIT history {
    scan [lindex [split [lindex $_raw 0] {:}] 1] {%[^!]!%[^@]@%s} nick ident host
    nhistory set $host $nick
}

on PART history {
    #:RBS!13Bor@AH9IgB.B2UkmI.virtual PART #sexo
    scan [lindex [split [lindex $_raw 0] {:}] 1] {%[^!]!%[^@]@%s} nick ident host
    nhistory set $host $nick 
}

on XC_JOIN history {
    set channel [lindex $_raw 2]
    if {[config [network] $channel show_history_on_join] == 1} {
            
       set nick [lindex $_raw 1]
       set host [lindex [split [lindex $_raw 3] {@}] 1]
       if {[nhistory exists $host]} {
           set saw [nhistory get $host]
           set id [lsearch -nocase $saw [text2pattern $nick]]
           set saw [lreplace $saw $id $id]
           if {($saw eq $nick) || ($saw eq "")} {
               return
           }
           print $channel " [lang JoinBeforeAs \00312\002$nick\002 [join [lsort $saw] {, }]]."
       }
    }
}
