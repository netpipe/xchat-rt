# Eggdrop-tcl-commands -
# This script gives full support for eggdrop in XChat.
#
# TODO: Create internal banlist
# TODO: Create botnet system
# TODO: Create a list of timers and utimers for resolv [timers] [utimers]

# Avoid conflicting proc names.
if {([info commands channel] ne "") && ([info commands xchat2eggdrop_channel] eq "")} {

    rename channel xchat2eggdrop_channel
}
::tcl::tm::path add $pnp(libraryDir)
if {[catch {package require md5}]} {
    puts "Error: Please install tcllib"
}
# proc bind
on PRIVMSG XChat2Eggdrop {
    global bind
    set userhost [lindex [split [lindex [split $_raw {:}] 1] { }] 0]
    scan $userhost {%[^!]!%[^@]@%s} nick user host
    set line [lindex [split $_raw {:}] 2]
    set trigger [lindex [split $line { }] 0]
    set command [lsearch -nocase -inline -exact [array names bind] privmsg,$trigger]
    set target [lindex [split $_raw] 2]
    if {($command ne "") && ($target eq [me])} {
        $bind($command) $nick $user@$host $nick [join [lrange [split $line] 1 end]]
    }
}


proc bind {args} {
    global bind
    #msg - kickban puppetMsgKB
    set option [lindex $args 0]
    set flag [lindex $args 1]
    set alias [lindex $args 2]
    set command [lindex $args 3]
    switch -exact $option {
        {msg} {
            #:Lady_Sith!Affra@Ani.ahavti.otaj.VeAni.ohevet.otja.VeAni.ohav.otaj.letamid PRIVMSG #literatura :frase
            set bind(privmsg,$alias) $command
        }
        {pub} {
            #puts "Xchat2Eggdrop: $args"
        }
        {join} {
            
        }
        {default} {
            #puts $args
        }
    }
}

proc setudef {command arg} {
    
    switch -exact $command {
        {flag} {
            set ::cfg($arg) 0
        }
    }
}

proc _md5 {string} {
    return [string tolower [::md5::md5 -hex $string]]
}
proc putserv {{text {}} {option {}}} {
    
    global queue
    if {$option eq {-next}} {
        set queue(putserv) [linsert $queue(putserv) 0 $text]
    } elseif {$option eq {-queue}} {
        if {$queue(putserv) ne [list]} {
            putnow [lindex $queue(putserv) 0]
            set queue(putserv) [lreplace $queue(putserv) 0 0]
            set queue(putserv,timer) [utimer 1 [list putserv {} -queue]]
        }
    } else {
        if {$text ne {}} {
            if {[info exists queue(putserv,timer)]} {
                if {[utimerexists $queue(putserv,timer)]} {
                    killutimer $queue(putserv,timer)
                }
            }
            lappend queue(putserv) $text
            set queue(putserv,timer) [utimer 2 [list putserv {} -queue]]
        }
    } 
}

proc puthelp {{text {}} {option {}}} {
    
    global queue
    if {$option eq {-next}} {
        set queue(puthelp) [linsert $queue(puthelp) 0 $text]
    } elseif {$option eq {-queue}} {
        if {$queue(puthelp) ne [list]} {
            putnow [lindex $queue(puthelp) 0]
            set queue(puthelp) [lreplace $queue(puthelp) 0 0]
            set queue(puthelp,timer) [utimer 3 [list puthelp {} -queue]]
        }
    } else {
        if {$text ne {}} {
            if {[info exists queue(puthelp,timer)]} {
                if {[utimerexists $queue(puthelp,timer)]} {
                    killutimer $queue(puthelp,timer)
                }
            }
            lappend queue(puthelp) $text
            set queue(puthelp,timer) [utimer 3 [list puthelp {} -queue]]
        }
    } 
}

proc putmode {{text {}} {option {}}} {
    
    global queue
    if {$option eq {-next}} {
        set queue(putmode) [linsert $queue(putmode) 0 $text]
    } elseif {$option eq {-queue}} {
        if {$queue(putmode) ne [list]} {
            putnow [lindex $queue(putmode) 0]
            set queue(putmode) [lreplace $queue(putmode) 0 0]
            set queue(putmode,timer) [utimer 3 [list putmode {} -queue]]
        }
    } else {
        if {$text ne {}} {
            if {[info exists queue(putmode,timer)]} {
                if {[utimerexists $queue(putmode,timer)]} {
                    killutimer $queue(putmode,timer)
                }
            }
            lappend queue(putmode) $text
            set queue(putmode,timer) [utimer 3 [list putmode {} -queue]]
        }
    } 
}

proc putquick {{text {}} {option {}}} {
    
    global queue
    if {$option eq {-next}} {
        set queue(putquick) [linsert $queue(putquick) 0 $text]
    } elseif {$option eq {-queue}} {
        if {$queue(putquick) ne [list]} {
            putnow [lindex $queue(putquick) 0]
            set queue(putquick) [lreplace $queue(putquick) 0 0]
            set queue(putquick,timer) [utimer 1 [list putquick {} -queue]]
        }
    } else {
        if {$text ne {}} {
            if {[info exists queue(putquick,timer)]} {
                if {[utimerexists $queue(putquick,timer)]} {
                    killutimer $queue(putquick,timer)
                }
            }
            lappend queue(putquick) $text
            set queue(putquick,timer) [utimer 1 [list putquick {} -queue]]
        }
    } 
}

proc putkick {channel nickList {reason {}}} {
    foreach nick $nickList {
        putserv "KICK $channel $nick :$reason"
    }
}

proc putlog {text} {
    #FixMe: Send this to the bot's logfile.
    print [server] $text
}

proc putcmdlog {text} {
    #FixMe: Send this to the bot's logfile.
    print [server] $text
}

proc procferlog {text} {
    #FixMe: Send this to the bot's logfile.
    print [server] $text
}

proc putloglev {level channel text} {    
    #FixMe: Send this to the bot's logfile.
    print [server] $text
}

proc dumpfile {nick filename} {
    #FixMe: Send this to the bot's logfile.
    print [server] $text
}

proc queuesize {{queueName {}}} {
    
    global queue
    if {$queueName ne {}} {
        if {[info exists queue($queueName)]} {
            return [llength $queue($queueName)]
        } else {
            return 0
        }
    } else {
        set i 0
        foreach q [array names queue] {
            incr i [llength $queue($q)]
        }
        return $i
    }
}

proc clearqueue {queueName} {
    global queue
    set i 0
    if {[info exists queue($queueName)]} {
        set i [llength $queue($queueName)]
        unset -nocomplain queue($queueName)
    }
    return $i
}
proc countusers {} {
    
    set i 0
    foreach server [servers] {
        foreach channel [channels $server] {
           incr i [expr [llength [users $server $channel]] -1]
        }
    }
    return $i
}

proc newignore {hostmask creator comment {lifetime {}}} {
    /IGNORE $hostmask ALL
    if {$lifetime ne {}} {
        timer $lifetime [list /UNIGNORE $hostmask]
    }
}

proc killignore {hostmask} {
    /UNIGNORE $hostmask
}

proc ignorelist {} {
    #FixMe: return a proper eggdrop lit
    foreach entry [ignores] {
      print "Ignoring:"
      print "[lindex $entry 0]: [lindex $entry 1]"
    }
}

proc isignore {hostmask} {
    foreach entry [ignores] {
        set entry [lindex $entry 0]
        if {[string match -nocase $hostmask $entry]} {
            return 1
        }
    }
    return 0
}
proc channel {{command {}} {channel {}} {options {}}} {
    
    if {$command eq {}} {
        return [xchat2eggdrop_channel]
    }
    switch -exact $command {
        {add} {
            /JOIN $channel
        }
        {remove} {
            /PART $channel
        }
        {get} {
            puts ">> $command $channel $options"
        }
        {default} {
            puts "Pendent: $command $channel $options"
        }
    }
}

proc isbotnick {nick} {
    if {[string compare -nocase $nick [me]] == 0} {
        return 1
    } else {
        return 0
    }
}

proc botisop {channel} {
    set line [lindex [lsearch -exact -index 0 -inline [users [server] $channel] [text2pattern [me]]] 2]
    if {$line eq {@}} {
        return 1
    } else {
        return 0
    }    
}

proc botishalfop {channel} {
    set line [lindex [lsearch -exact -index 0 -inline [users [server] $channel] [text2pattern [me]]] 2]
    if {$line eq {%}} {
        return 1
    } else {
        return 0
    }    
}

proc botisvoice {channel} {
    set line [lindex [lsearch -exact -index 0 -inline [users [server] $channel] [text2pattern [me]]] 2]
    if {$line eq {+}} {
        return 1
    } else {
        return 0
    }    
}

proc botonchan {channel} {
    if {[lsearch -exact [channels] [text2pattern $channel]] != -1} {
        return 1
    } else {
        return 0
    }
}

proc isop {nick channel} {
    set line [lindex [lsearch -exact -index 0 -inline [users [server] $channel] [text2pattern [text2pattern $nick]]] 2]
    if {$line eq {@}} {
        return 1
    } else {
        return 0
    }    
}

proc isvoice {nick channel} {
    set line [lindex [lsearch -exact -index 0 -inline [users [server] $channel] [text2pattern [text2pattern $nick]]] 2]
    if {$line eq {+}} {
        return 1
    } else {
        return 0
    }    
}

proc ishalfop {nick channel} {
    set line [lindex [lsearch -exact -index 0 -inline [users [server] $channel] [text2pattern [text2pattern $nick]]] 2]
    if {$line eq {%}} {
        return 1
    } else {
        return 0
    }    
}

proc onchan {nick channel} {
    
    set chanlist [chanlist [server] $channel]
    if {[lsearch -index 0 [users [server] $channel] [text2pattern $nick]] != -1} {
        return 1
    } else {
        return 0
    }
}

proc getchanhost {nick {channel ""}} {
    
    if {$channel eq ""} {
        foreach c [channels] {
            if {[onchan $nick $c]} {
                set channel $c
                break
            }
        }
    }
    foreach host [lrange [users [server] $channel] 1 end] {
        set uhost [lindex $host 1]
        set user [lindex $host 0]
        if {[string compare $user $nick] == 0} {
            return [lindex $host 1]
        }
    }
}

proc chanlist {{server {}} {chan {}}} {
    if {$chan eq ""} {
        set chan $server
        set server [server]
    }
    set chanlist [list]
    foreach c [lrange [users $server $chan] 1 end] {
        lappend chanlist [lindex $c 0]
    }
    return $chanlist
}

proc pushmode {channel mode {arg ""}} {
    
    putmode "MODE $channel $mode $arg"
}

proc putnow {text} {
    /QUOTE $text
}

proc validchan {channel} {
    if {[lsearch -exact -nocase [channels] [text2pattern $channel]] != -1} {
        return 1
    } else {
        return 0
    }
}
proc utimer {seconds command} {
    timer $seconds $command
}

proc utimerexists {id} {
    if {[lsearch -exact [after info] $id] != -1} {
        return 1
    } else {
        return 0
    }
}

proc timerexists {id} {
    if {[lsearch -exact [after info] $id] != -1} {
        return 1
    } else {
        return 0
    }
}

proc killutimer {id} {
    after cancel $id
}
proc killtimer {id} {
    after cancel $id
}


proc unixtime {} {
    return [clock seconds]
}

proc maskhost {server channel userName type} {
    foreach name [lrange [users $server $channel] 1 end] {
        if {[nickcmp [lindex $name 0]  $userName] == 0} {
            scan [lindex $name 0]![lindex $name 1] {%[^!]!%[^@]@%s} nick user host
            # This proc is in order to XChat.
           if [string match {[023]} $type] {
              if [string match {*[0-9]} $host] {
                 set host [join [lrange [split $host .] 0 2] .].*
              } elseif {[string match *.*.* $host]} {
                 set host *.[join [lrange [split $host .] end-1 end] .]
              }
           }
           if [string match {[23]} $type] {
              set user *[string trimleft $user ~]
           } elseif {[string match {[01]} $type]} {
              set user *
           }
           if [string match {[01234]} $type] {
              set nick *
           }
           return $nick!$user@$host
        } 
    }
}

proc rand {max} {
    return [expr {int(rand()*$max)}]
}

proc stripcodes {flags string} {
    set list [list]
    if {[string match *b* $flags]} {
        lappend list {\002}
    }
    if {[string match *c* $flags]} {
        lappend list {\003(\d{1,2})?(,\d{1,2})?}
    }
    if {[string match *r* $flags]} {
        lappend list {\026}
    }
    if {[string match *u* $flags]} {
        lappend list {\037}
    }
    if {[string match *a* $flags]} {
        lappend list {\017}
    }
    if {[string match *g* $flags]} {
        lappend list {\037}
    }
    set list [join $list {|}]
    
    return [regsub -all -- "$list" $string ""]
}


# All tools

proc putmsg {dest text} {
  puthelp "PRIVMSG $dest :$text"
}

proc putchan {dest text} {
  puthelp "PRIVMSG $dest :$text"
}

proc putnotc {dest text} {
  puthelp "NOTICE $dest :$text"
}

proc putact {dest text} {
  puthelp "PRIVMSG $dest :\001ACTION $text\001"
}

proc strlwr {string} {
  string tolower $string
}

proc strupr {string} {
  string toupper $string
}

proc strcmp {string1 string2} {
  string compare $string1 $string2
}

proc stricmp {string1 string2} {
  string compare [string tolower $string1] [string tolower $string2]
}

proc strlen {string} {
  string length $string
}

proc stridx {string index} {
  string index $string $index
}

proc iscommand {command} {
  if {[string compare "" [info commands $command]]} then {
    return 1
  }
  return 0
}


proc randstring {length {chars abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789}} {
  set count [string length $chars]
  for {set index 0} {$index < $length} {incr index} {
    append result [string index $chars [rand $count]]
  }
  return $result
}

proc putdccall {text} {
  foreach i [dcclist CHAT] {
    putdcc [lindex $i 0] $text
  }
}

proc putdccbut {idx text} {
  foreach i [dcclist CHAT] {
    set j [lindex $i 0]
    if {$j != $idx} then {
      putdcc $j $text
    }
  }
}

proc killdccall {} {
  foreach i [dcclist CHAT] {
    killdcc [lindex $i 0]
  }
}

proc killdccbut {idx} {
  foreach i [dcclist CHAT] {
    set j [lindex $i 0]
    if {$j != $idx} then {
      killdcc $j
    }
  }
}

proc nick2hand {nickname {channel ""}} {
    
    global pnp userFile
    
    foreach id [dict keys $userFile] {
        if {![dict exists $userFile $id {--HOSTS}]} {
            continue
        } else {
            set hosts [dict get $userFile $id {--HOSTS}]
            foreach host $hosts {
                if {[string match $host $nickname]} {
                    return $id
                }
            }
        }
        
    }
    return *
}

proc iso {nick chan} {
  matchattr [nick2hand $nick $chan] o|o $chan
}

proc realtime {args} {
  switch -exact -- [lindex $args 0] {
    time {
      return [strftime %H:%M]
    }
    date {
      return [strftime "%d %b %Y"]
    }
    default {
      return [strftime "%I:%M %P"]
    }
  }
}

proc testip {ip} {
  set tmp [split $ip .]
  if {[llength $tmp] != 4} then {
    return 0
  }
  set index 0
  foreach i $tmp {
    if {(([regexp \[^0-9\] $i]) || ([string length $i] > 3) || \
         (($index == 3) && (($i > 254) || ($i < 1))) || \
         (($index <= 2) && (($i > 255) || ($i < 0))))} then {
      return 0
    }
    incr index
  }
  return 1
}

proc number_to_number {number} {
  switch -exact -- $number {
    0 {
      return Zero
    }
    1 {
      return One
    }
    2 {
      return Two
    }
    3 {
      return Three
    }
    4 {
      return Four
    }
    5 {
      return Five
    }
    6 {
      return Six
    }
    7 {
      return Seven
    }
    8 {
      return Eight
    }
    9 {
      return Nine
    }
    10 {
      return Ten
    }
    11 {
      return Eleven
    }
    12 {
      return Twelve
    }
    13 {
      return Thirteen
    }
    14 {
      return Fourteen
    }
    15 {
      return Fifteen
    }
    default {
      return $number
    }
  }
}


proc isnumber {string} {
  if {([string compare "" $string]) && \
      (![regexp \[^0-9\] $string])} then {
    return 1
  }
  return 0
}

proc ispermowner {hand} {
  global owner

  if {([matchattr $hand n]) && \
      ([lsearch -exact [split [string tolower $owner] ", "] \
        [string tolower $hand]] != -1)} then {
    return 1
  }
  return 0
}

proc matchbotattr {bot flags} {
  foreach flag [split $flags ""] {
    if {[lsearch -exact [split [botattr $bot] ""] $flag] == -1} then {
      return 0
    }
  }
  return 1
}

proc matchbotattrany {bot flags} {
  foreach flag [split $flags ""] {
    if {[string first $flag [botattr $bot]] != -1} then {
      return 1
    }
  }
  return 0
}

proc ordnumber {string} {
  if {[isnumber $string]} then {
    set last [string index $string end]
    if {[string index $string [expr [string length $string] - 2]] != 1} then {
      if {$last == 1} then {
        return ${string}st
      } elseif {$last == 2} then {
        return ${string}nd
      } elseif {$last == 3} then {
        return ${string}rd
      }
    }
    return ${string}th
  }
  return $string
}

proc matchattr {handle flags {channel {}}} {
    
    #This will make the script fail in most of cases
    if {$channel eq {}} {
        set channel [channel]
    }
    global pnp userFile
    if {[dict exists $userFile $handle]} {
        if {[dict exists $userFile $handle --FLAGS$channel]} {
            set userFlag [dict get $userFile $handle --FLAGS$channel]
        } else {
            set userFlag [dict get $userFile $handle --FLAGSglobal]
        }
        set i 0
        foreach flag [split $flags {}] {
            if {![string match "*$flag*" $userFlag]} {
                    return 0
                }
            }
        return 1
    }
    return 0
}

# *** GLOBAL VARIABLES ***

set ::botnick [me]
proc botname {} {
    foreach host [lrange [users $server $channel] 1 end] {
        set mask [lindex $host 1]
        set nick [lindex $host 0]
        if {[string compare $nick [me]] == 0} {
            return $nick!$mask
        }
    }    
}

set ::server [server]
set ::version [getinfo version]
set ::numversion "xchat $::version"
set ::uptime [unixtime]
set ::{server-online} [unixtime]
set ::isjuped #TODO
set ::handlen #TODO
set ::config [info script] 
set ::nick [me]
# *** eggdrop configuration ***
set username {hXm}
set admin {Sentencia}
set network ""
set timezone "GMT"
set offset "1"
#set env(TZ) "$timezone $offset"
#set my-hostname
#set my-ip
#addlang "english"
