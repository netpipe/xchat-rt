try:
  import xchat
except:
  print "O modulo xchat soh roda com o xchat"

try:
  import GeoIP
except:
  print "GeoIP module not installed."

import os
import re
import time
import string
# Identificacao
__module_name__ = "xSalada - seu xchat automatico"
__module_version__ = "0.1"
__module_description__ = "Script que retorna a origem do host"
__module_author__ = "jack at icatorze dot com dot br"

###############################
# Get nick country, if possible
###############################
def ipFormatChk(ip_str):
    try:
        num = string.split(ip_str,'.')
	if not len(num) == 4:
	    return False
	else:
	    for n in num:
	    	try:
		    if int(n)  255:
		        return False
		except:
		    return False
            return True
    except:
        return False

def getCountry(word, word_eol, userdata):
    print word
    users = xchat.get_list("users")
    try:
        gi = GeoIP.new(GeoIP.GEOIP_MEMORY_CACHE)
        try:
            nicktoquery = word[1]
        except:
            print "Choose a nick"
	    nicktoquery = ""
        for user in users:
            if user.nick == nicktoquery:
	       partes_host = string.split(user.host,'@',1)
	       hostid = partes_host[1]
	       if ipFormatChk(hostid):
	           print "%s from %s" % (nicktoquery,gi.country_name_by_addr(hostid))
               elif re.match(r'.*/.*', hostid):
		   print "%s cloaked as %s" % (nicktoquery,hostid)
               else:
                   print "%s from %s" % (nicktoquery,gi.country_name_by_name(hostid))
    except:
    	print "GeoIP nao instalado"
    return xchat.EAT_ALL

def getCountryJoin(word, word_eol, userdata):
    #print word[2]
    try:
        gi = GeoIP.new(GeoIP.GEOIP_MEMORY_CACHE)
        partes_host = string.split(word[2],'@',1)
        hostid = partes_host[1]
        if ipFormatChk(hostid):
            print "%s from %s" % (word[0],gi.country_name_by_addr(hostid))
        elif re.match(r'.*/.*', hostid):
	    print "%s cloaked as %s" % (word[0],hostid)
        else:
            print "%s from %s" % (word[0],gi.country_name_by_name(hostid))
    except:
        print "GeoIP nao instalado"
    return xchat.EAT_NONE

try:
    xchat.hook_print("Join", getCountryJoin)
    xchat.hook_command("geoip",getCountry)

except:
    print "modo ajuda ou deu merda!!!"

if __name__ == '__main__':
    pass
