__module_name__="Xchat Location resolver2"
__module_description__="Read above"
__module_version__="2.0"
import commands, urllib, xchat
def resolveHost(word,word_eol,userdata):
    users = xchat.get_list("users")
    for u in users:
        if u.nick==word[1]:
            print word[1]+"\n"+"-"*len(word[1])
            ip = u.host[u.host.find("@")+1:]
	    #xchat.command("exec ~/.xchat2/plugins/geoipTools/GeoIP-1.4.6/apps/geoiplookup -d ~/.xchat2/plugins/geoipTools/GeoIP-1.4.6/data/ -f ~/.xchat2/plugins/geoipTools/GeoIP-1.4.6/data/GeoLiteCity.dat "+ip)
#xchat.command("exec ~/.xchat2/plugins/geoipTools/GeoIP-1.4.6/apps/geoiplookup -d ~/.xchat2/plugins/geoipTools/GeoIP-1.4.6/data/ -f ~/.xchat2/plugins/geoipTools/GeoIP-1.4.6/data/GeoLiteCity.dat "+ip)
            xchat.command("exec geoiplookup " + ip)
        #    sock = urllib.urlopen("http://www.geoiptool.com/en/?IP="+ip)
        #    source = sock.read()
        #    sock.close()
        #    x = source.find("Country:<")
        #    country = source[source.find("_blank",x)+8:source.find("</a>",x)]
        #    x = source.find("Region:<")
        #    region = source[source.find("_blank",x)+8:source.find("</a>",x)]
        #    x = source.find("City:<")
        #    city = source[source.find("_bold",x)+7:source.find("</td>",x+16)]
        #    print "Country: " + country + "\nRegion: " + region + "\nCity: " + city
    return xchat.EAT_ALL
#resolve hook
xchat.hook_command("gip",resolveHost,"/GIP nick\nresolves about where nick lives")
print("Type /GIP <nick> to see where that person lives (according to geoiptool.com)")


#~/.xchat2/dev/geoipTools/GeoIP-1.4.6/apps ./geoiplookup -d ../data/ -f ../data/GeoLiteCity.dat
