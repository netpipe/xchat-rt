__module_name__="Xchat Location resolver"
__module_description__="Read above"
__module_version__="1.0"
import commands, urllib, xchat
def resolveHost(word,word_eol,userdata):
    users = xchat.get_list("users")
    for u in users:
        if u.nick==word[1]:
            print word[1]+"\n"+"-"*len(word[1])
            ip = u.host[u.host.find("@")+1:]
            sock = urllib.urlopen("http://www.geoiptool.com/en/?IP="+ip)
            source = sock.read()
            sock.close()
            x = source.find("Country:<")
            country = source[source.find("_blank",x)+8:source.find("</a>",x)]
            x = source.find("Region:<")
            region = source[source.find("_blank",x)+8:source.find("</a>",x)]
            x = source.find("City:<")
            city = source[source.find("_bold",x)+7:source.find("</td>",x+16)]
            print "Country: " + country + "\nRegion: " + region + "\nCity: " + city
    return xchat.EAT_ALL

xchat.hook_command("resolve",resolveHost,"/RESOLVE nick\nresolves about where nick lives")
print("Type /RESOLVE <nick> to see where that person lives (according to geoiptool.com)")
