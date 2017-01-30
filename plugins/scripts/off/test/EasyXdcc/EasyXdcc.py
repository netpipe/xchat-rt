# -*- coding: utf-8 -*- 
__module_name__ = "EasyXdcc" 
__module_version__ = "1.0" 
__module_description__ = "Xdcc Queues"
__module_author__ = "Bouliere Tristan <boulieretristan@aliceadsl.fr>"

import pygtk
pygtk.require('2.0')
import gtk, xchat, os, time

class t_bot:
    def __init__(self, name, serv, chan):
        self.name = name
        self.chan = chan
        self.serv = serv
        self.packs = []
        
    def match(self, name, serv, chan):
        return (self.name == name) & (self.chan == chan) & (self.serv == serv)
    
    def equals(self, bot):
        if (isinstance(bot, t_bot)) :
            return bot.match(self.name, self.chan, self.serv)
        else :
            return False
        
    def add_pack(self, num_pack):
        if (type(num_pack) == int):
            if not self.packs.__contains__(num_pack):
                self.packs.append(num_pack)
                self.packs.sort(reverse=True)
                
    def del_pack(self, num_pack):
         if (type(num_pack) == int):
            if self.packs.__contains__(num_pack):
                del self.packs[self.packs.index(num_pack)]
                
    def pop(self):
        return self.packs.pop()
    
    def isEmpty(self):
        return len(self.packs) == 0
    
    def isActive(self):
        list = xchat.get_list("dcc")
        if (list):
            for i in list:
                if i.nick == self.name:
                    return (i.status == 0) | (i.status == 1) | (i.status == 4)
        return False
    
    def echo(self):
        print "Bot : "+self.name+" [ "+self.serv+", "+self.chan+"]"
        for pack in self.packs:
            print "    #"+str(pack)
        
class bot_queue:
    def __init__(self):
        self.bots = []
    
    def search(self, name, chan, serv):
        for i in self.bots:
            if (i.match(name, chan, serv)):
                return i
        return None
       
    def add(self, new_bot):
        if isinstance(new_bot, t_bot):
            for i in self.bots:
                if (i.equals(new_bot)):
                    return
            self.bots.append(new_bot)
    
    def del_bot (self, bot):
         if isinstance(bot, t_bot):
             if self.bots.__contains__(bot):
                 del self.bots[self.bots.index(bot)]
     
    def echo(self):
           print ""
           print "*****************************"
           print "*       Queue EasyXdcc      *"
           print "*****************************"
           print ""
           if len(self.bots) == 0:
               print "No pack(s) queued" 
               print ""
           else:
               for bot in self.bots:
                  bot.echo() 
                  print ""
      
    def save(self, file_name):
        if (type(file_name) == str):
            try: 
                file = open(file_name,'w')
                try:
                    for bot in self.bots:
                        file.write(getattr(bot,"name")+"\n")
                        file.write(getattr(bot,"serv")+"\n")
                        file.write(getattr(bot,"chan")+"\n")
                        for pack in getattr(bot,"packs"):
                            file.write(str(pack)+"\n")
                        file.write("\n")
                finally:
                    file.close()
            except IOError:
                pass
    def load(self,file_name):
        if (type(file_name) == str):
            try: 
                file = open(file_name,'rb')
                try:
                    etat=0
                    for buffer in file.readlines():
                       if etat==0:
                           name = buffer.strip("\n")
                           etat = 1
                       elif etat==1:
                           serv = buffer.strip("\n")
                           etat = 2
                       elif etat==2:
                           chan = buffer.strip("\n")
                           etat = 3
                       elif etat==3:
                           bot = t_bot(name,serv,chan)
                           self.add(bot)
                           pack = buffer.strip("\n")
                           if pack == "":
                               etat=0
                           else:
                               bot.add_pack(int(pack))
                               etat=4
                       else:
                           pack = buffer.strip("\n")
                           if pack == "":
                               etat=0
                           else:
                              bot.add_pack(int(pack)) 
                        
                finally:
                    file.close()
            except IOError:
                pass

    def connect(self):
        servchan=[]
        for bot in self.bots:
            if servchan.__contains__(getattr(bot,"serv")):
                servchan[servchan.index(getattr(bot,"serv")) + 1].append(getattr(bot,"chan"))
            else:
                servchan.append(getattr(bot,"serv"))   
                servchan.append([getattr(bot,"chan")])
 
        for i in range(0,len(servchan),2):
            servs = ""
            for serv in servchan[i+1]:
                servs=servs+serv+","
            servs = servs.strip(",")
            xchat.command("servchan "+servchan[i]+" 6667 "+servs) 
                 
def inputbox(legende=""):
    user_input=None
    dialog = gtk.Dialog("EasyXdcc", buttons=("Ok", gtk.RESPONSE_ACCEPT, "Cancel", gtk.RESPONSE_CANCEL))
    text_input = gtk.Entry()
    text_input.set_text(legende)
    dialog.vbox.add(text_input)
    dialog.show_all()
    response = dialog.run()
    if response == gtk.RESPONSE_ACCEPT:
        user_input = text_input.get_text()
    else:
        user_input = ""
    dialog.destroy()
    return user_input

def get_bot_current_chan(bot_name):
    global queue
    if (type(bot_name) != str):
        return None
    serv = xchat.get_info("server");
    chan = xchat.get_info("channel");
    if serv is None or chan is None:
        print "Not Connected!"
        return xchat.EAT_ALL
    bot = queue.search(bot_name, serv, chan)
    if bot is None:
        bot = t_bot(bot_name, serv, chan)
        queue.add(bot)
    return bot

def search_bot_current_chan(bot_name):
    global queue
    if (type(bot_name) != str):
        return None
    serv = xchat.get_info("server");
    chan = xchat.get_info("channel");
    if serv is None or chan is None:
        print "Not Connected!"
        return xchat.EAT_ALL
    return queue.search(bot_name, serv, chan)

def help():
    print ""
    print "*****************************"
    print "*      Manuel EasyXdcc      *"
    print "*****************************"
    print ""
    print "Queue a pack :"
    print "/XDCC ADD [bot_name] [n°_pack]"
    print ""
    print "Queue a pack list :"
    print "/XDCC ADDL [bot_name] [n°_pack_beg] [n°_pack_end]"
    print ""
    print "See pack queue :"
    print "/XDCC QUEUE"
    print ""
    print "See pack queue for a bot :"
    print "/XDCC QUEUE [bot_name]"
    print ""
    print "Withdraw a pack from queue :"
    print "/XDCC RMP [bot_name] [n°pack]"
    print ""
    print "Withdraw a pack list from queue :"
    print "/XDCC RMPL [bot_name] [n°pack_deb] [N°pack_fin]"
    print ""
    print "Withdraw a bot from queue :"
    print "/XDCC RMBOT [bot_name]"
    print ""
    print "Stop EasyXdcc :"
    print "/XDCC STOP"
    print ""
    print "Start EasyXdcc :"
    print "/XDCC START"
    print ""
    print "Save Queue :"
    print "/XDCC SAVE"
    print ""
    print "Load Queue :"
    print "/XDCC LOAD"
    print ""
    
    return xchat.EAT_ALL

def idx_EasyXdcc(word, word_eol, userdata):
    argc = len(word)
 
    if argc == 2:
        if word[1] == "start":
            return start()
        elif word[1] == "stop":
            return stop()
        elif word[1] == "save":
            return save()
        elif word[1] == "load":
            return load()
        elif word[1] == "queue":
            return seequeue()
        elif word[1] == "help":
            return help()
    elif argc == 3:
        if word[1] == "rmbot":
            return rmbot(word[2])
        elif word[1] == "queue":
            return seebotqueue(word[2])
        elif word[1] == "gtkadd":
            return gtkadd(word[2])
        elif word[1] == "gtkaddl":
            return gtkaddl(word[2])
        elif word[1] == "gtkrmp":
            return gtkrmp(word[2])
        elif word[1] == "gtkrmpl":
            return gtkrmpl(word[2])
    elif argc == 4 :
        if word[3].isdigit():
            if word[1] == "add":
                return add(word[2], int(word[3]))
            elif word[1] == "rmp":
                return rmp(word[2], int(word[3]))
    elif argc == 5:
         if word[3].isdigit() & word[4].isdigit():
             if word[1] == "addl":
                 return addl(word[2], int(word[3]), int(word[4]))
             elif word[1] == "rmpl":
                 return rmpl(word[2], int(word[3]), int(word[4]))
        
    return xchat.EAT_ALL   

def seequeue():
    global queue
    queue.echo()
    return xchat.EAT_ALL

def seebotqueue(bot_name):
    global queue
    if (type(bot_name) != str):
        print "/XDCC QUEUE [BOT_NAME]"
        return xchat.EAT_ALL
    else:
        bot = search_bot_current_chan(bot_name)
        if bot is not None:
            bot.echo()
    return xchat.EAT_ALL
        
def add(bot_name, num_pack):
    global queue
    if (type(bot_name) != str) & (type(num_pack) != int):
        print "/XDCC ADD BOT_NAME NUM_PACK"
    else:
        bot = get_bot_current_chan(bot_name)
        bot.add_pack(num_pack)
        print "EasyXdcc : Pack number #"+str(num_pack)+" add to "+bot_name
    return xchat.EAT_ALL
    
def gtkadd(bot_name):
    global queue
    
    bot = get_bot_current_chan(bot_name)
    pack = inputbox("Pack number, ex: 4")
    if pack.isdigit():
        bot.add_pack(int(pack))
        print "EasyXdcc : Pack number #"+pack+" add to "+bot_name
    return xchat.EAT_ALL

def addl(bot_name, pbeg, pend):
    global queue
    if (type(bot_name) != str) & (type(pbeg) != int) & (type(pend) != int):
        print "/XDCC ADD BOT_NAME NUM_PACK"
    else:
        bot = get_bot_current_chan(bot_name)
        for pack in range(pbeg, pend+1):
            bot.add_pack(pack)
        print "EasyXdcc : Packs number #"+str(pbeg)+" to #"+str(pend)+" add to "+bot_name
    return xchat.EAT_ALL

def gtkaddl(bot_name):
    global queue
    
    bot = get_bot_current_chan(bot_name)
    packs = inputbox("Pack list, ex: 4,21")
    packs = packs.split(",")
    if (len(packs) == 2):
        if packs[0].isdigit() & packs[1].isdigit():
            for pack in range(int(packs[0]),int(packs[1])+1):
                bot.add_pack(pack)    
            print "EasyXdcc : Packs number #"+packs[0]+" to #"+packs[1]+" add to "+bot_name
    return xchat.EAT_ALL

def rmp(bot_name,num_pack):
    if (type(bot_name) != str) & (type(num_pack) != int):
        print "/XDCC RMP BOT_NAME NUM_PACK"
    else:
        bot = search_bot_current_chan(bot_name)
        if bot is not None:
            bot.del_pack(num_pack)
            print "EasyXdcc : Pack number #"+str(num_pack)+" remove from "+bot_name
    return xchat.EAT_ALL

def gtkrmp(bot_name):
    global queue
    bot = search_bot_current_chan(bot_name)
    pack = inputbox("Pack number, ex: 4")
    if pack.isdigit():
        bot.del_pack(int(pack))
        print "EasyXdcc : Pack number #"+pack+" remove from "+bot_name
    return xchat.EAT_ALL

def rmpl(bot_name,pbeg,pend):
    global queue
    if (type(bot_name) != str) & (type(pbeg) != int) & (type(pend) != int):
        print "/XDCC RMPL BOT_NAME PACK_BEG PACK_END"
    else:
        bot = search_bot_current_chan(bot_name)
        if bot is not None:
            for pack in range(pbeg,pend + 1):
                bot.del_pack(pack)
            print "EasyXdcc : Pack number #"+str(pbeg)+" to #"+str(pend)+" remove from "+bot_name

def gtkrmpl(bot_name):
    global queue
    
    bot = get_bot_current_chan(bot_name)
    packs = inputbox("Pack list, ex: 4,21")
    packs = packs.split(",")
    if (len(packs) == 2):
        if packs[0].isdigit() & packs[1].isdigit():
            for pack in range(int(packs[0]),int(packs[1])+1):
                bot.del_pack(pack)
            print "EasyXdcc : Packs number #"+packs[0]+" to #"+packs[1]+" remove from "+bot_name
    return xchat.EAT_ALL
        
def rmbot(bot_name):
    if (type(bot_name) != str):
        print "/XDCC RMBOT BOT_NAME"
    else:
        bot = search_bot_current_chan(bot_name)
        if bot is not None:
            queue.del_bot(bot)
    return xchat.EAT_ALL

def save():
    global queue,sav_file
    queue.save(sav_file)
    print "Queue(s) state saved"
    return xchat.EAT_ALL

def load():
    global queue,sav_file
    queue.load(sav_file)
    queue.connect()
    print "Queue(s) state loaded"
    return xchat.EAT_ALL

def start():
    global my_hook
    if my_hook is None:
        my_hook = xchat.hook_timer(10000, lauch_dl)
        print "EasyXdcc started"
        lauch_dl(None)
    return xchat.EAT_ALL

def stop():
    global my_hook
    if my_hook is not None:
        xchat.unhook(my_hook)
        my_hook = None
        print "EasyXdcc stoped"
    return xchat.EAT_ALL

def lauch_dl(userdata):
    global queue
    for bot in getattr(queue, 'bots'):
        if not bot.isActive():
            bot_context = xchat.find_context(getattr(bot, 'serv'), getattr(bot, 'chan'))
            bot_context.command('msg '+getattr(bot, 'name')+' xdcc send #'+str(bot.pop()))
        if bot.isEmpty():
            queue.del_bot(bot)
    return 1
    

my_hook = None
queue = bot_queue()
try:
    cmd = os.popen("whoami")
    try:
        user = cmd.readlines()
        user = user[0].strip("\n")
    finally:
        cmd.close()
except IOError:
    pass
sav_file = "/home/"+user+"/.xchat2/EasyXdcc.queue"
xchat.hook_command("XDCC", idx_EasyXdcc, help="/XDCC <cmd>") 
xchat.command ("MENU -p5 ADD EasyXdcc")
xchat.command ("MENU ADD \"EasyXdcc/Start\" \"xdcc start\"")
xchat.command ("MENU ADD \"EasyXdcc/Stop\" \"xdcc stop\"")
xchat.command ("MENU ADD \"EasyXdcc/Queue(s)\" \"xdcc queue\"")
xchat.command ("MENU ADD \"EasyXdcc/Save\" \"xdcc save\"")
xchat.command ("MENU ADD \"EasyXdcc/Load\" \"xdcc load\"")
print "Plugin EasyXdcc loaded!"
print "/XDCC HELP"
