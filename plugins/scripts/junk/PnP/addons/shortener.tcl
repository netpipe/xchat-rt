if {[config [network] "" shorten_url] eq ""} {
   set ::cfg(shorten_url) 1
}

global aliases
set aliases(isgd) IsGd

proc gd {url} {
   package require http
   if {![regexp -nocase {^(http://)?([^/:]+)(:([0-9]+))?(/.*)?$} $url]} {return}
   
   set origToken [http::geturl $url]
   #<meta http-equiv="Content-Type"
   set origData [http::data $origToken]
   http::cleanup $origToken
   set origContent "No Content"
   set origTitle "No title"
   #regexp -nocase {<meta http-equiv="Content-Type" content="(.*?)"} $origData -> origContent
   regexp -nocase {<title>(.*?)</title>} $origData -> origTitle
   regsub -all -- {\<[^\>]*\>|\t|\n} $origTitle "" origTitle
   set origTitle [regsub -all {[ ]+} "$origTitle" { }]
   set origTitle [string trim $origTitle { -}]

   set token [http::geturl http://is.gd/api.php?longurl=[http::formatQuery $url]]
   set shortUrl [http::data $token]
   if {$origTitle eq {No title}} {
      set origTitle {Image/Object}
   }
   if {$shortUrl eq {}} {
      return {}
   }
   return "$shortUrl ($origTitle)"
}

#on xc_ujoin IsGd {
   
 #  alias @[channel] {
proc IsGd {_rest} {
   if {$::cfg(shorten_url) != 1} {
      return
   }
   set text $_rest
   set target [getcontext]
   if {[string match -nocase {*http://*} $_rest]} {
      regsub -all -- {\017|\002|\037|\026|\003(\d{1,2})?(,\d{1,2})?} $text "" arg
      set webTarget [lsearch -inline [split $arg] {*http://*}]
      if {([string match "*http://bit.ly/*" $webTarget]) || ([string match "*http://is.gd/*" $webTarget])} { command $target "/say $text"; complete; return}
      if {![regexp -nocase {^(http://)?([^/:]+)(:([0-9]+))?(/.*)?$} $webTarget]} {command $target "/say $text"; complete; return}
      puts "Shorting $webTarget ..."
      set shortUrl [gd $webTarget]
      if {$shortUrl eq {}} {
         puts "No info."
         command $target "/say $lastLine"
         complete EAT_XCHAT
         return
      }
      set lastLine [string map [list $webTarget $shortUrl] $text]
      command $target "/say $lastLine"
      complete EAT_XCHAT; return   
   }

}
#}
