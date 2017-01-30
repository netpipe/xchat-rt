#
# ourTube.tcl 1.0.2 --
# This file search the web target and show relevant information about.
# Is posible define a YouTube account and the bot will log in. Useful
# when the link are for adult people i.e. It will show the first link
# that finds in a whole phrase.
#
# Copyright (c) 2007-2009 Eggdrop Spain 15-november-2009
#   HackeMate (Sentencia) Sentencia@eggdrop.es
#   Andoidsk (Redrum) sincorreo@en@eggdrop.es
#
# This program is free software; you can redistribute it and/or
# modify it _only for your own use_
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
#
# Please, use the email to contact with the author for let him know about all
# what you could do. Everyone wants develop his software as well.
#
#                   Thank you for feed my empiric life.
#
# If you like, you can contact with the author to suggest him features. By the
# way, do not ask him to make Login, he is working on that.

# This is what you need to get this script work:
# Type in partyline: .chanset #channel +ourtube

# Changelog 1.0.2:
# (RedRrum)
#   Fixes:
#	Resolved Tcl error [otPub]: wrong # args: should be "otLog target string"
#	Resolved broken line in the actual youtube html title format (new regsub)
#
#   Changes:
#	Changed description Youtube match.
#	Modify otPub function(minor cost):
#		- Regular expresion by exact uri.
#		- Break before nothing else if not youtube link.

#   Added features:
#	Add category Youtube match.
#	AntiSpam on Results: spliting URIs on points.
#
#   TODO:
#	Add related links match.
#	Move HTTP errors to logs only or disable them.
#	Delete ourtubecolors flag. If want colors write them on ourtube(output)
#	AÃadir opciÃn para detectar links sucios.
#	Permitir a los ops del canal decidir que opciones mostrar (registrar usuarios)

# Changelog 1.0.1:
# (HackeMate)
#   Fixes:
#       Resolved Tcl error [otPub]: can't read "views": no such variable
#       Resolved eternal ignore-protection  issue
#
#   Added features:
#       Now will forward to new location 302 http code received when pasting
#       http://youtube. links
#       Explicit message when 404 error (not found)
#       All non 200, 302, 303, 404 errors will stop the procedure showing proper reason
#
#       -*- IMPORTANT -*-
#       Auto Update checker: You can verify if your ourTube copy is the lastest
#       version available typing .ourtube update in partyline
#
#       FeedBack feature. Type in partyline .ourtube feedback [your email] <message>
#       to send the author any suggestion or comment. Insults are not welcome.
#
#       Added seacher, !youtube string and it will return matches
#       it is customizable (configure ourtube(outputsearch))


# initializes a user defined channel flag
#setudef flag ourtube
#setudef flag ourtubecolors

global ourtube tcl_platform

## START EDITALE ZONE ##
##################



## Configuration for bot admins
#
# Module Record user links


## Configuration for channel operators

## Configuration for users

# Busqueda sucia
set ourtube(dirtysearch) 1

# (1) Enable or (0) disable logs.
set ourtube(logs) 1

# (1-8) Level log.
set ourtube(levellog) 1

# (1) Enable or (0) disable colors
set ourtube(colors) 1

# Flood Protection: after show a link, will ignore all links few seconds
# This means 1 link per 10 seconds.
set ourtube(flood) 10

# What language you can receive the youTube data? (if works heh)
set ourtube(lang) es

# Limit of links at same time
set ourtube(max) 5

# Do you want see all matches at one line? (0) Yes (1) No
set ourtube(multiline) 1

# This is the final output message what you will read in your channel.
#       -*- This is not for search command (only when someone pasted link)
# You can configure all fields that your eggdrop will show.
# <title>       will return the title of the video
# <author>      It was the author himself who had uploaded the video
# <views>       How many views the video has
# <rating>      His rating
# <description> Information by author - This may be disabled because it can
#               contain spam
# <comment>     Will show the last comment if exists - Same as description, take care
#               with spam.
# <category>	
# <link>	link clean

#set ourtube(output) "\002<title>\002. (by <author>) <views> views, <rating> rating. Last comment: <comment>"
set ourtube(output) "\002\0031,0You\0030,4Tube\002\017 <title> (\002<category>\002) - <views> views."
# - <description>"

# This is the output message of search engine
#       -*- This is only for search engine
# You can configure all fields that your eggdrop will show.
# <link>        URL video link
# <time>        video's duration
# <added>       since when it is on line
# <title>       will return the title of the video
# <author>      It was the author himself who had uploaded the video
# <views>       How many views the video has
# <rating>      His rating
# <description> Information by author - This may be disabled because it can
#               contain spam

set ourtube(outputsearch) "***\t\00312\037<link>\003\037 \00315\(<time> rating: <rating>\)\003 \"\0036<title> \0031<author> said\0036: <description>\003\" <views>,  added <added>"

# Do you want auto update this file when it is possible?
# (1) Yes (0) No - It is recommended, if youtube changes his tags probably this script will broke
set ourtube(autoupdate) 0



## END EDITABLE ZONE #############
##################################

# This is not required to edit, or yes.

set ourtube(author) "HackeMate"
set ourtube(contact) "HackeMate <Sentencia@eggdrop.es>"
set ourtube(name) "ourTube"
set ourtube(fileName) [info script]
set ourtube(projectName) "ourTube"
set ourtube(version) "1.0.2"
set ourtube(package.http) [package require http]
## No inicializar (uso interno)
set ourtube(protection) ""
if {$tcl_platform(os) eq "Linux"} {
    set platfrm "X11"
} else {
    set platfrm $tcl_platform(os)
}
http::config -useragent "Mozilla/5.0 ($platfrm; U; $tcl_platform(os) $tcl_platform(machine); $ourtube(lang); rv:1.9.0.3) ourTube 1.0" -accept "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"

if {[config [network] "" show_youtube_info] eq ""} {
   set ::cfg(show_youtube_info) 1
}
bind pubm - * otPub
on XC_CHANMSG YouTube {
    if {[config [network] [channel] show_youtube_info] == 1} {
	set channel [getcontext]
        otPub [lindex $_raw 1] host hand $channel [lindex $_raw 2]
    }
}
proc otPub {nick uhost hand chan text} {
    
	global ourtube

	# Clean all colour marks
	regsub -all -- {\017|\002|\037|\026|\003(\d{1,2})?(,\d{1,2})?} $text "" text
	if {$ourtube(dirtysearch)} {
	
		set id [regexp -inline -nocase -- {youtube.*watch\?v=.{11}} $text]
		regsub -nocase -- {.*watch\?v=} $id "" id
		if {$id eq ""} {
		    set id [regexp -inline -nocase -- {http://youtu.be/.{11}} $text]
		    set id [lindex [split $id {/}] 3]
		}
		if {$id eq ""} {
		    return
		}
		set uri "http://www.youtube.com/watch?v=$id"

	} else {
		set uri [regexp -inline -nocase -- {http://www\.youtube\.com/watch\?v=.{11}} $text]
		if {$uri eq ""} { 
		    set uri [regexp -inline -nocase -- {http://youtu.be/.{11}} $text]
		}
		if {$uri eq ""} {
		    return
		}
	}

	if {[string is digit -strict $ourtube(protection)]} {
		set flood [expr [clock seconds]-$ourtube(protection)]
		if {$flood >= $ourtube(flood)} {
			set ourtube(protection) ""
		} else {
			otLog $chan "Resting... (flood protection) [duration [expr ($ourtube(flood) - [expr ([clock seconds]-$ourtube(protection))])]] left"
			return
		}
	} 
	
	set ourtube(protection) [clock seconds]
	set data [otGet $uri]

	##pasar a otGet
	if {![string length $data]} {
		set data "I was not able to reach $uri. Probably I get a timeout. Try again."
	} 
	
	if {!$ourtube(colors)} {
		regsub -all -- {\017|\002|\037|\026|\003(\d{1,2})?(,\d{1,2})?} $data "" data
	}		
	
	# Este formato de log facilita el paso a xml para posterior posible procesado.
        print $chan "[string repeat { } [string length $nick]] $data"

	utimer $ourtube(flood) [list set ourtube(protection) ""]
}

proc otGet {web {relocation ""}} {
	global ourtube
	set token [http::geturl $web -timeout 4000]
	upvar #0 $token state
	set lastcode $state(http)

#	set ncode ""
	set ncode [regexp -inline -- {[0-9]{3}} $lastcode]
#	regexp {[0-9]{3}} $lastcode ncode
#	if {$ncode eq ""} {
#		set ncode $lastcode
#	}
	switch -- $ncode {
		"200" { }
		"302" - "303" {
		    foreach {flag value} $state(meta) {
			if {$flag eq "Location"} {
			    # Due to invalid youtube link but valid url syntax we can
			    # receive an url forward. this handles that
			    http::cleanup $token
			    return [otGet $value "(Relocated)"]
			}
		    }
		}
		"404" {
			return "$web - No such webpage."
			http::cleanup $token
			return "$web - No such webpage"
		}
		default {
			http::cleanup $token
			return "unforeseen circumstances. Server responded: Time out."
		}
	}
	set data [string map {"&quot\;" "\"" "&amp\;quot\;" "\"" "&amp;" "&"} $state(body)] 
	http::cleanup $token
	set title ""
	set author ""
	set description ""
	set views ""
	set rating ""
	set comment ""
	set category ""
	set desc ""

	regexp {<title>(.*?)</title>} $data "" title
	regexp {class="hLink fn n contributor">(.*?)</a><br>} $data "" author

	#regexp {<meta name=\"description\" content=\"(.*?)\">.*} $data "" description
	#regexp {<div([:blank:]+)class="watch-video-desc description">(.*?)</div>} $data "" desc


	# Description
	regexp {class="watch-video-desc description">(.*?)</div>} $data "" description
	
	regsub -all -- {<a href=(.*?)</a>} $description "" description
	regsub -all -- {\<(.*?)\>} $description "" description
	regsub -all -- {\n|\t} $description "" description
	regsub -all -- {\. *} $description ". " description
	regsub -all -- {  +} $description " " description
	regsub -all -- {^(- *)} $description "" description
	regsub -all -- {\.*( *)$} $description "" description

	set description ${description}.

#	if {[string length $description] > 300} {
#
#	} else {
#
#	}

	regexp {<p id="eow-category">(.*?)</a>} $data "" category
	regexp {<span class="watch-view-count">(.*?)</span>} $data "" views
        regexp {<strong>(.*?)</strong>} $views "" views
        #set views [string map [list {,} {}] $views]
	regexp {<div id=\"defaultRatingMessage\">(.*?)</span>.*} $data "" rating
	# This is not so smart way. I know, sorry about :)
#	set comments ""
#	set description ""

#	regexp {<div id="recent_comments" class="comments">(.*?)<div id="div_comment_form_id} $data "" comments
#	if {$comments ne ""} {
#		regexp { rel="nofollow">(.*?)</a>} $comments "" user
#		regexp {<span class="watch-comment-time">(.*?)</span>} $comments "" timeago
#		regexp {<div class="watch-comment-body">(.*?)</div>} $comments "" comment
#		set comment [string map {\n " " "<br>" ""} $comment]
#		regsub -all -- {\<[^\>]*\>|\t} $comment "" comment
#		regsub -all {\s+} $comment " " comment
#		set comment "\<$user [string trim $timeago]\> [string trim $comment]"
#	} else {
#		set comment ""
#	}
	regsub -all -- {\<[^\>]*\>|\t|\n|YouTube} $title "" title
	regsub -all -- {^- } $title "" title
	regsub -all -- {( *)\.( *)} $title ". " title
	#regsub -all -- {http://[^ ]* } $description "" description
	#regsub -all -- {\<[^\>]*\>|\t} $description "" description
	regsub -all -- {\<[^\>]*\>|\t} $views "" views
	regsub -all -- {\<[^\>]*\>|\t} $rating "" rating
	regsub -all -- {\<[^\>]*\>|\t|\>} $category "" category
	set rating [lindex [split $rating] 0]
	set title "$relocation $title"
	set title [string trim $title { -}]
	if {![string is digit [string map [list {,} {}] $views]]} {
		set views "no"
	}
	if {![string is digit -strict $rating]} {
		set rating "no"
	}
	set output [string map [list "<title>" $title "<author>" $author "<description>" $description "<views>" $views "<rating>" $rating "<comment>" $comment "<category>" $category <link> $web] $ourtube(output)]
	return $output
}



# upvar #0 $token state

proc otLog {target string} {
    global ourtube
    if {![validchan $target]} {
	if {$ourtube(logs) == "1"} {
	    print $target " $ourtube(name)\: $string"
	}
    }
}


if {![info exists ourtube(loaded)]} {
    if {$ourtube(autoupdate) == "1"} {
        set ourtube(status) [communication::Update $ourtube(fileName) $ourtube(projectName) $ourtube(version) $ourtube(autoupdate)]
        set ourtube(result) [lindex [split $ourtube(status)] 0]
        set ourtube(info) [join [lrange [split $ourtube(status)] 1 end]]
        otLog log $ourtube(info)
    }
} 

set ourtube(loaded) 1
