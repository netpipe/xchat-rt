__module_name__ = "Join Pounce"
__module_version__ = "1.0.2"
__module_description__ = "Sends message to people who haven't joined the channel (or parted) in the last 24 hours."

import xchat, cPickle
from datetime import datetime

# this is where you define what channels get pounce messages.
channels = {
    '#channelname' : {
    
# greet contains the pounce message.
# {{nick}} is replaced with the user's nick
# {{channel}} is replaced with the channel name
        'greet': 'Hi {{nick}}!  Welcome to {{channel}}!',
    
# a list of hostnames that you want to ignore.  These must be exact!
        'ignore': [ 'user@some.host.com', '~another@example.org' ],
    
# type is either 'notice' or 'message'
        'type': 'notice'
    },
    
    '#channel2' : {
        'greet': 'We don\'t need no stinking badges!',
        'ignore': [],
        'type', 'message'
    },
}

for c in channels:
    try:
        f = open(c + '_pounce.dat', 'rb')
        channels[c]['greetz'] = cPickle.load(f)
        f.close()
    except IOError:
        channels[c]['greetz'] = {}
    except EOFError:
        channels[c]['greetz'] = {}
        f.close()

def pounce_cb(word, word_eol, userdata):
    global channels
    
    if word[1] == 'JOIN':
        c = word[2][1:]
    else:
        c = word[2]
        
    try:
        if channels[c] is None:
            return None
    except (KeyError):
        return None
    
    username, hostname = word[0].split('!')
    
    try:
        if channels[c]['greetz'][hostname] + timedelta(1) > datetime.now():
            channels[c]['greetz'][hostname] = datetime.now()
            return
        else:
            channels[c]['greetz'][hostname] = datetime.now()
    except (KeyError):
        #add to the table and see if we should send them the join message
        channels[c]['greetz'][hostname] = datetime.now()
    
    # seems less intensive to check this first than to go through what could be
    # a long ignore list and then check for to see if they are JOIN'ing
    if word[1] != "JOIN":
        return
        
    if not hostname in channels[c]['ignore']:
        if channels[c]['type'] == 'notice':
            xchat.command('notice ' + username[1:] + ' ' + channels[c]['greet'].replace('{{channel}}', c).replace('{{nick}}', username[1:]))
        else:
            xchat.command('msg ' + c + ' ' + channels[c]['greet'].replace('{{channel}}', c).replace('{{nick}}', username[1:]))
    
def unload_cb(userdata): 
    for c in channels:
        f = open(c + '_pounce.dat', 'wb')
        cPickle.dump(channels[c]['greetz'], f)
        f.close()

xchat.hook_server("PART", pounce_cb)
xchat.hook_server("JOIN", pounce_cb)
xchat.hook_unload(unload_cb) 