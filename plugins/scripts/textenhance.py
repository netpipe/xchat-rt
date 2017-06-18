'''
Created on: 2011.08.20
Last update: nil
Author: Kulverstukas
Shouts: Evilzone.org
Contact: kulverwars@evilzone.org
----
To use it on Linux, put it into ~/.xchat2 and it will load every time you start xchat
or you can do it manualy with: /py load highlightaway.py
----
To use it on Windows, install Python 2.5
and put it into "X:/Documents and Settings/USERNAME/Application Data/X-Chat 2/"
or you can do it manualy with: /py load highlightaway.py

=================================================
Copyright 2011 Kulverstukas

 "Text enhance" is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation version 3

 "Text enhance" is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
=================================================

Update notes:
 * nil
'''
'''
\003
    0 white
    1 black
    2 blue
    3 green
    4 red
    5 dark gred
    6 purple
    7 dark yellow
    8 yellow
    9 bright green
    10 dark green
    11 green
    12 blue
    13 bright purple
    14 dark grey
    15 light grey

\017 reset all 
\002 bold 
\026 reverse text 
\007 beep 
\037 underline
'''
import xchat, string, random

#======================================
__module_name__ = "Text enhance"
__module_version__ = "1.0"
__module_description__ = "Script made to print out text in a funny way. Adds more playfulness, and increases trolling - /txtenh"
#======================================
def Mode1(Data):
    '''Reverse words, sentance correct'''
    Words = Data.split()
    String, NewList = ('', [])
    NewList = [Item[::-1] for Item in Words]
    for I in NewList: String += I+' '
    return String
#======================================
def Mode2(Data):
    '''Reverse sentance, words correct'''
    Words = Data.split()
    String = ''
    Words.reverse()
    for I in Words: String += I+' '
    return String
#======================================
def Mode3(Data):
    '''Reverse sentance and words'''
    Words = Data.split()
    String, NewList = ('', [])
    Words.reverse()
    NewList = [Item[::-1] for Item in Words]
    for I in NewList: String += I+' '
    return String
#======================================
def Mode4(Data):
    '''All text randomly mixed'''
    Lst = Data.split()
    NewStr = ''
    NewSent = ''
    for I in Lst:
        while (I != ''):
            NewStr += I[random.randint(0,len(I)-1)]
            I = I.replace(NewStr[len(NewStr)-1], '', 1)
        NewSent += NewStr+' '
        NewStr = ''
    return NewSent.strip()
#======================================
def Mode5(Data):
    '''Capitalize every second word'''
    Lst = Data.split()
    NewStr = ''
    for I in range(len(Lst)):
        if ((I % 2) == 0): NewStr += Lst[I].upper()+' '
        else: NewStr += Lst[I]+' '
    return NewStr.strip()
#======================================
def Mode6(Data):
    '''Capitalize every second letter'''
    NewStr = ''
    for I in range(len(Data)):
        if ((I % 2) == 0): NewStr += Data[I].upper()
        else: NewStr += Data[I]
    return NewStr.strip()
#======================================
def EnhanceText(word, word_eol, userdata):
    if (len(word) <= 3):
        xchat.prnt('\n \02Usage information:'+
                   '\n \02Type: \017\037/txtenh MODE_# s/n text'+
                   '\n\017       Add "\02s\017" after mode number to send to the channel,'+
                   '\n\017       Add "\02n\017" after mode number to not send to channel,'+
                   '\n\017       else it will only be printed to the screen.'+
                   '\n \02Available modes:'+
                   '\n  \02Test string:\017 \0032The quick brown fox jumps over the lazy dog'+
                   '\n    \017\0021\017 - \00310Reverse words, sentance correct\017 \02==\017 \0032ehT kciuq nworb xof spmuj revo eht yzal god\017'+
                   '\n    \017\0022\017 - \00310Reverse sentance, words correct\017 \02==\017 \0032dog lazy the over jumps fox brown quick The\017'+
                   '\n    \017\0023\017 - \00310Reverse sentance and words\017 \02==\017 \0032god yzal eht revo spmuj xof nworb kciuq ehT\017'+
                   '\n    \017\0024\017 - \00310All text randomly mixed\017 \02==\017 \0032hTe cquki rownb xfo umjsp ovre the ylaz dgo\017'+
                   '\n    \017\0025\017 - \00310Capitalize every second word\017 \02==\017 \0032THE quick BROWN fox JUMPS over THE lazy DOG\017'+
                   '\n    \017\0026\017 - \00310Capitalize every second letter\017 \02==\017 \0032ThE QuIcK BrOwN FoX JuMpS OvEr tHe lAzY DoG\017')
    elif (len(word) > 3):
        if (word[1] not in string.digits): xchat.prnt('Mode must be a number!')
        else:
            Mode = int(word[1])
            if (word[2].lower() == 's'): Send = True
            else: Send = False
           #=====
            if (Mode == 1):
                if (Send == True): xchat.command('say %s' % Mode1(word_eol[3]))
                else: xchat.prnt(Mode1(word_eol[3]))
           #=====
            elif (Mode == 2):
                if (Send == True): xchat.command('say %s' % Mode2(word_eol[3]))
                else: xchat.prnt(Mode2(word_eol[3]))
           #=====
            elif (Mode == 3):
                if (Send == True): xchat.command('say %s' % Mode3(word_eol[3]))
                else: xchat.prnt(Mode3(word_eol[3]))
           #=====
            elif (Mode == 4):
                if (Send == True): xchat.command('say %s' % Mode4(word_eol[3]))
                else: xchat.prnt(Mode4(word_eol[3]))
           #=====
            elif (Mode == 5):
                if (Send == True): xchat.command('say %s' % Mode5(word_eol[3]))
                else: xchat.prnt(Mode5(word_eol[3]))
           #=====
            elif (Mode == 6):
                if (Send == True): xchat.command('say %s' % Mode6(word_eol[3]))
                else: xchat.prnt(Mode6(word_eol[3]))
           #=====
            elif (Mode == 7):
                if (Send == True): xchat.command('say %s' % Mode7(word_eol[3]))
                else: xchat.prnt(Mode7(word_eol[3]))
           #=====
        
    return xchat.EAT_ALL
#======================================
def OnUnload(userdata):
    xchat.prnt('\002[+]\017 \00312"Text enhance" %s has been unloaded' % __module_version__)
    return xchat.EAT_NONE
#======================================
try:
    xchat.hook_command('txtenh', EnhanceText)
    xchat.hook_unload(OnUnload)
    xchat.prnt('\002[+]\017 \00312"Text enhance" %s has been loaded - /txtenh' % __module_version__)
except:
    xchat.prnt('\002[-]\017 \0034"Text enhance" %s has failed to load :(' % __module_version__);
#======================================
