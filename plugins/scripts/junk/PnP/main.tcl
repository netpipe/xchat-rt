# PnP.tcl --
# Use this script at your own risk.
# This script has been created in tribute to pai. Pai, wherever you are, you'll never forget the next ten minutes.
# Peace and Protection script is one of the best works made ​​for IRC, here is his legacy 13 years later.

if {[info tclversion] < 8.5} {
    puts "-"
    puts "\0034You need at least Tcl version 8.5 to use this script."
    puts "-"
    puts "Please download http://xchat.org/files/source/2.8/xchat-2.8.8.tar.bz2"
    puts "Then type tar xvf xchat-2.8.8.tar.bz2 && cd xchat-2.8.8/"
    puts "Now you are ready to compile"
    return
}
set start [clock milliseconds]

global pnp

set pnp(scriptDir) [file join $::env(HOME) Dropbox]
set pnp(langDir) [file join $pnp(scriptDir) PnP lang]
set pnp(moduleDir) [file join $pnp(scriptDir) PnP modules]
set pnp(addonDir) [file join $pnp(scriptDir) PnP addons]
set pnp(profileDir) [file join $pnp(scriptDir) PnP profile]
set pnp(libraryDir) [file join $pnp(scriptDir) PnP library]
set pnp(xchatconf) [file join [xchatdir] xchat.conf]
set pnp(peventsconf) [file join [xchatdir] pevents.conf]
set pnp(profileconf) [file join $pnp(scriptDir) PnP profile profile]

proc lang {string {args}} {
    
    global cfg pnp lang
    if {$string eq {-load-}} {
        if {![info exists cfg(language)]} {
            set cfg(language) English
        }
        set file $cfg(language)
        if {[catch {source [file join $pnp(langDir) $file.tcl]} msg]} {
            puts "There was an error loading language $file. Error message: \0034$msg"
            return
        } 
    } else {
        return [string map [list {<1>} [lindex $args 0] {<2>} [lindex $args 1] {<3>} [lindex $args 2] {<4>} [lindex $args 3] {<5>} [lindex $args 4]] [dict get $lang $string]]
    }
}

proc profile {command args} {
    
    global pnp
    switch $command {
        {get} {
            set fs [open $pnp(profileconf) r]
            set current [gets $fs]
            close $fs
            return $current
        }
        {file} {
            set fs [open $pnp(profileconf) r]
            set current [gets $fs]
            close $fs
            return [file join $pnp(profileDir) $current.tcl]    
        }
        {set} {
            set name [lindex $args 0]
            set fs [open $pnp(profileconf) w]
            puts $fs $name
            close $fs
            puts [lang SetProfile $name]
        }
        {load} {
            set fs [open $pnp(profileconf) r]
            set profile [gets $fs]
            close $fs
            source [file join $pnp(profileDir) $profile.tcl]
        }
        {check} {
            if {![file isdirectory $pnp(profileDir)]} {
                file mkdir $pnp(profileDir)
            }
            if {![file exists $pnp(profileconf)]} {
                set fs [open $pnp(profileconf) w]
                puts $fs {default}
                close $fs
                profile new default
            }
            
        }
        {exists} {
            set profile [lindex $args 0]
            return [file exists [file join $pnp(profileDir) $profile.tcl]]
        }
        {new} {
            set name [lindex $args 0]
            set fs [open $pnp(profileconf) w]
            puts $fs $name
            close $fs
            pnpSave $name
            puts [lang CreatedProfile $name]
        }
    }
}

source [file join $pnp(moduleDir) procs.tcl]


set i 0
foreach file [glob -nocomplain $pnp(moduleDir)/*.tcl] {
    if {[catch {source $file} msg]} {
        puts "[lang FailLoadModule $msg]"
    } else {incr i}
}
lang -load-
profile check
profile load [profile current]

puts "\0032*** [lang StartUp [profile get]]"
loadUserFile [profile get]

puts "\0032*** [lang ModulesLoaded $i]"


set addonList [list]
foreach file [glob -nocomplain $pnp(addonDir)/*.tcl] {
    set fileName [file rootname [file tail $file]]
    if {[lsearch -nocase [config "" "" addons] $fileName] == -1} {
        continue
    }
    if {![catch {source $file} msg]} {
        lappend addonList $fileName
    } 
}
set end [clock milliseconds]
if {[llength $addonList] == 0} {
    set addonList [lang AddonsLoadedNone]
} else {
    set addonList [join $addonList {, }]
}

puts "\0032*** [lang AddonsLoaded $addonList]"
puts "\0032*** [lang StartUpCompleted \0031\002[expr ($end-$start)/1000.0]\002\0032]"
puts "\0032*** ········································"
puts "\0032*** [lang Welcome \003\0021.0\002]"
puts "\0032*** [lang TypeHelp]"
puts "\0032*** [lang RightClick]"
puts "\0032*** [lang UseSet]"
puts "\0032*** ········································"


