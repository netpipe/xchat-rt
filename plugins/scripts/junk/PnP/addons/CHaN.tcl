#iRC-Hispano bot services.

on MODE CHaNService {

    set mode [lindex [split $_rest { }] 0]
    set arg [join [lrange [split $_rest { }] 1 end]]
    set i 0
    set nick [lindex [split [lindex [split [lindex $_raw 0] ":"] 1] "!"] 0]
    set modes [join [lrange $_raw 2 end]]
    
    set report [list]
    foreach m [split $mode {}] {
        switch -- $m {
            {+} {            
                set type "op"
                continue
            }
            {-} {
                set type "deop"
                continue
            }
            {o} {
                set target [lindex $arg $i]
                if {($type eq {deop}) && ($target eq [me]) && ($nick ne [me])} {
                    puts "\0036*** [lang CHaNOp \002$nick\002 \002[channel]\002]"
                    /quote privmsg CHaN :op [channel] [me]
                }
                incr i
            }
        }
    }
}

proc chanMenu {} {
    /menu ADD {$NICK/CHaN}
    /menu ADD {$NICK/CHaN/Op} "tcl /msg CHaN op [channel] %s"
    /menu ADD {$NICK/CHaN/Deop} "tcl /msg CHaN deop [channel] %s"
}

timer 10 chanMenu