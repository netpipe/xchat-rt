 #!/bin/sh
 # The next line restarts with tclsh.\
 exec tclsh "$0" ${1+"$@"}

# PnP installer --
#
# Open Xchat and paste this line without #
# /tcl package require http; eval [http::data [http::geturl http://eggdrop.es/uploads/tcl/PnP/PnP-installer.tcl]]

package require http
http::config -urlencoding [encoding system]
set pnp(list) {
    ../main.tcl    
    addons/CHaN.tcl
    addons/extras.tcl
    addons/puppet.tcl
    addons/rae.tcl
    addons/shortener.tcl
    addons/sysinfo.tcl
    addons/youtube.tcl

    lang/English.tcl
    lang/Spanish.tcl
    
    library/md5-2.0.7.tm

    modules/alias.tcl
    modules/conf.tcl
    modules/events.tcl
    modules/mp3.tcl
    modules/procs.tcl
    modules/protection.tcl
    modules/raws.tcl
    modules/xchat2eggdrop.tcl
}

set url {http://www.eggdrop.es/uploads/tcl/PnP/}
#set target $::argv/PnP

switch -- $::tcl_platform(os) {
    {Linux} {
        set target [file join $::env(HOME) .xchat2 PnP]
    }
    {Darwin} {
        return {This OS is not being supported.}
    }
    {Windows NT} {
        set target [file join $::env(APPDATA) {X-Chat 2} PnP]
    }
}



file mkdir $target

foreach file $pnp(list) {
    set pathFile [file join $target [file dirname $file]]
    if {![file isdirectory $pathFile]} {
        file mkdir $pathFile
        puts "Created directory $pathFile"
    }
    if {[catch {set token [http::geturl $url$file]} msg]} {
        puts "Can't download $file. $msg"
    } else {
        set fs [open [file join $target $file] w]
        puts $fs [encoding convertfrom utf-8] [http::data $token]]
        close $fs
        puts "Downloaded $file ([format %0.2f [expr [file size [file join $target $file]]/1024.0]]Kb)"
    }
}
