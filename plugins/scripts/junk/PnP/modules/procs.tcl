

proc config {network channel id {value ""}} {
    global cfg

    if {(![info exists cfg($id)]) && (![info exists cfg($network,$id)]) && (![info exists cfg($network,$channel,$id)])} {
        return
    }
    if {$value ne ""} {
        set cfg($network,$id) $value
        return $value
    }
    if {[info exists cfg($network,$channel,$id)]} {
        return $cfg($network,$channel,$id)
    }    
    if {[info exists cfg($channel,$id)]} {
        return $cfg($channel,$id)
    }
    if {[info exists cfg($network,$id)]} {
        return $cfg($network,$id)
    }
    if {[info exists cfg($id)]} {
        return $cfg($id)
    }
}

proc xconfig {value} {
    
    global pnp
    set fs [open $pnp(xchatconf)]
    set buffer [split [read $fs] \n]
    close $fs
    set id [lsearch -inline $buffer "$value =*"]
    return [string trim [join [lrange [split $id {=}] 1 end] {=}]]
}

proc loadUserFile {profileFile} {
    
    global pnp userFile
    set file [file join $pnp(profileDir) $profileFile.user]
    if {![file exists $file]} {
        set fs [open $file w]
        close $fs
    }
    set fs [open $file r]
    set buffer [split [read $fs] \n]
    close $fs
    set userFile [dict create name [lindex $buffer 0]]
    foreach line [lrange $buffer 1 end] {
    #    regsub -all {[ ]+} $line { } $line
    #    set userDelim [lindex [split $line { }] 2]
    #    if {$userDelim eq {-}} {
    #        set userName [string trim [lindex [split $line { }] 0]]
    #        set flags [string trim [lindex [split $line {-}] 1]]
    #    }
    #    dict set $user $userName 
    #    return
        set userName [lindex [split $line] 0]
        set option [lindex [split $line] 1]
        set modes [join [lrange [split $line] 2 end]]
        dict set userFile $userName $option $modes
    }
}

proc userFlag {user} {
    
    global pnp userFile
    if {[dict exists $userFile $user]} {
        dict get $userFile $user --FLAGS
    }
}

proc pnpSave {{name ""}} {
    
    global pnp cfg
    if {$name eq ""} {
        set name [profile file]
    } else {
        set name [file join $pnp(profileDir) $name.tcl]
    }
    set fs [open $name w]
    foreach id [lsort [array names cfg]] {
        puts $fs "set ::cfg($id) [list $cfg($id)]"
    }
    close $fs
    
}
proc xevent {event args} {
    global pnp pevents
    if {![info exists pevents]} {
        set fs [open $pnp(peventsconf)]
        set buffer [split [read $fs] \n]
        close $fs
        foreach {id value n} $buffer {
            set id [lindex [split $id {=}] 1]
            set value [join [lrange [split $value {=}] 1 end] {=}]
            set pevents($id) $value
        }
    }
    return [string map [list {$1} "[lindex $args 0]" {$2} [lindex $args 1] {$3} [lindex $args 2] {$4} [lindex $args 3] {%B} \002 {%C} \003 {%O} \017 {$t} \t] $pevents($event)]
}

proc duration { int_time } {
    set timeList [list]
    foreach div {86400 3600 60 1} mod {0 24 60 60} name {day hr min sec} {
        set n [expr {$int_time / $div}]
        if {$mod > 0} {set n [expr {$n % $mod}]}
        if {$n > 1} {
            lappend timeList "$n ${name}s"
        } elseif {$n == 1} {
            lappend timeList "$n $name"
        }
    }
    return [join $timeList]
}
 

proc matchnick {list nick} {
    foreach l $list {
        if {[nickcmp $l $nick] == 0} {return 1}
    }
}

proc text2pattern {text} {
    regsub -all -- {(\[|\]|\\)} $text {\\\1}
}


 
proc fileRead {file {line ""}} {
    if {![file exists $file]} {
        return
    }
    set fs [open $file r]
    set buffer [split [read $fs] \n]
    close $fs
    if {$line eq ""} {
        set line [rand [expr [llength $buffer] -1]]
    }
    if {$line eq "-1"} {
        return $buffer
    }
    return [lindex $buffer $line]
}

proc userhost {server channel userName type} {
    foreach name [lrange [users $server $channel] 1 end] {
        if {[nickcmp [lindex $name 0]  $userName] == 0} {
            scan [lindex $name 0]![lindex $name 1] {%[^!]!%[^@]@%s} nick user host
            #0: *!user@full.host.tld
            #1: *!*user@full.host.tld
            #2: *!*@full.host.tld
            #3: *!*user@*.host.tld
            #4: *!*@*.host.tld
            #5: nick!user@full.host.tld
            #6: nick!*user@full.host.tld
            #7: nick!*@full.host.tld
            #8: nick!*user@*.host.tld
            #9: nick!*@*.host.tld
           if [string match {[024]} $type] {
              if [string match {*[1-9]} $host] {
                 set host [join [lrange [split $host .] 0 2] .].*
              } elseif {[string match *.*.* $host]} {
                 set host *.[join [lrange [split $host .] end-1 end] .]
              }
           }
           if [string match {[23]} $type] {
              set user *[string trimleft $user ~]
           } elseif {[string match {[012]} $type]} {
              set user *
           }
           if [string match {[0123]} $type] {
              set nick *
           }
           return $nick!$user@$host
        } 
    }
}

proc bantype {name type} {
    scan $name {%[^!]!%[^@]@%s} nick user host
    if [string match {[024]} $type] {
       if [string match {*[1-9]} $host] {
          set host [join [lrange [split $host .] 0 2] .].*
       } elseif {[string match *.*.* $host]} {
          set host *.[join [lrange [split $host .] end-1 end] .]
       }
    }
    if [string match {[23]} $type] {
       set user *[string trimleft $user ~]
    } elseif {[string match {[012]} $type]} {
       set user *
    }
    if [string match {[0123]} $type] {
       set nick *
    }
    return $nick!$user@$host  
}
# Setting:
set maskhostDefaultType 3

# The proc:
proc maskhost [list name [list type $maskhostDefaultType]] {
   if {[scan $name {%[^!]!%[^@]@%s} nick user host]!=3} {
      error "Usage: maskhost <nick!user@host> \[type\]"
   }

}

proc hostuser {server channel maskhost} {
    
    set list [list]
    foreach host [lrange [users $server $channel] 1 end] {
        set mask [lindex $host 1]
        set nick [lindex $host 0]
        if {[string match $maskhost $nick!$mask]} {
            lappend list $nick
        }
    }
    return $list
}

proc userclon {server channel maskhost} {
    set list [list]
    foreach host [lrange [users $server $channel] 1 end] {
        set mask [lindex $host 1]
        set nick [lindex $host 0]
        if {[string match $maskhost $nick!$mask]} {
            lappend list $nick
        }
    }
    return $list
}

proc trueorfalse {str} {
  if { ![string is boolean -strict $str] } {
       return -1; # not a boolean
     }
  return [string is true $str]
}




 #
 proc bgExec {prog readHandler pCount {timeout 0} {toExit ""}} {
      upvar #0 $pCount myCount
      set myCount [expr {[info exists myCount]?[incr myCount]:1}]
      set p [expr {[lindex [lsort -dict [list 8.4.7 [info patchlevel]]] 0] == "8.4.7"?"| $prog 2>@1":"| $prog 2>@stdout"}]
      set pH [open $p r]
      fconfigure $pH -blocking 0; # -buffering line (does it really matter?!)
      set tID [expr {$timeout?[after $timeout [list bgExecTimeout $pH $pCount $toExit]]:{}}]
      fileevent $pH readable [list bgExecGenericHandler $pH $pCount $readHandler $tID]
      return $pH
 }
 #-------------------------------------------------------------------------------
 proc bgExecGenericHandler {chan pCount readHandler tID} {
      upvar #0 $pCount myCount
      if {[eof $chan]} {
         after cancel $tID;   # empty tID is ignored
         catch {close $chan}; # automatically deregisters the fileevent handler
                              # (see Practical Programming in Tcl an Tk, page 229)
         incr myCount -1
      } elseif {[gets $chan line] != -1} {
         # we are not blocked (manpage gets, Practical... page.233)
         lappend readHandler $line
         if {[catch {uplevel $readHandler}]} {
            # user-readHandler ended with error -> terminate the processing
            after cancel $tID
            catch {close $chan}
            incr myCount -1
         }
      }
 }
 #-------------------------------------------------------------------------------
 proc bgExecTimeout {chan pCount toExit} {
      upvar #0 $pCount myCount
      if {[string length $toExit]} {
         catch {uplevel [list $toExit [pid $chan]]}
      }
      catch {close $chan}
      incr myCount -1
 }
