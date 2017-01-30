[xchat]  auto-op pour xchat en python-------------------------------------
Url     : http://codes-sources.commentcamarche.net/source/25016-xchat-auto-op-pour-xchat-en-pythonAuteur  : cartoongraphistDate    : 06/09/2013
Licence :
=========

Ce document intitulé « [xchat]  auto-op pour xchat en python » issu de CommentCaMarche
(codes-sources.commentcamarche.net) est mis à disposition sous les termes de
la licence Creative Commons. Vous pouvez copier, modifier des copies de cette
source, dans les conditions fixées par la licence, tant que cette note
apparaît clairement.

Description :
=============

Auto-op Version 0.1
<br />
<br />Ce script permet aux utilisateurs enregistr&e
acute;s de s'auto-oper sans que l'operateur ne fasse quoi que se soit.
<br />

<br />Il fonctionne sur Xchat lorsque celui-ci est install&eacute; avec le plug 
python.
<br />Le script fonctionne sur le clone si celui-ci est op.
<br />Le s
cript doit &ecirc;tre plac&eacute; dans le repertoire de Xchat.
<br />Pour t&ea
cute;l&eacute;charger Xchat : <a href='http://www.xchat.org' target='_blank'>htt
p://www.xchat.org</a>
<br />
<br />Pour l'installer taper dans la fenetre du c
lone : /py load auto-op.py
<br />
<br />Lorsqu'un utilisateur enregistr&eacute
; dans le script veut &ecirc;tre op&eacute;, il tape : !op
<br />Si le clone s'
en va, un message est laiss&eacute; sur le channel expliquant que le script est 
d&eacute;sactiv&eacute;.
<br />
<br />Pour ajouter des utilisateurs, cherchez 
la ligne suivante :
<br />
<br />if word[1] == &quot;!op&quot; and word[0] == 
&quot;un_pseudo&quot;:
<br />       xchat.command(strip(&quot;me --&gt;je te op
 &quot;+word[0]))
<br />       xchat.command(strip(&quot;mode #aspirine +o un_p
seudo&quot;))
<br />       
<br />...et mettez le pseudo de votre ami &agrave;
 la place de un_pseudo.
<br />Vous pouvez mettre autant de pseudos que vous le 
d&eacute;sirez.
<br />       
<br />
<br />Si vous utilisez ce script et qu'i
l vous plait, n'h&eacute;sitez pas &agrave; m'envoyer un petit message ;)
<br /
><a name='source-exemple'></a><h2> Source / Exemple : </h2>
<br /><pre class='
code' data-mode='basic'>
# -*- coding: cp1252 -*-
__module_name__ = &quot;auto
-op&quot;
__module_version__ = &quot;0.1&quot;
__module_description__ = &quot;
auto op - Python&quot;
__module_author__ = &quot;tchoutchou@lexpress.net - cybe
rdivad&quot;

# auto op for all and bot.

import xchat
from string import s
trip

def autoop(word, word_eol, userdata):
    event, pos = userdata
    if
 type(pos) is int:
        pos = (pos,)
    if word[1] == &quot;!op&quot; and 
word[0] == &quot;un_pseudo&quot;:
       xchat.command(strip(&quot;me --&gt;je 
te op &quot;+word[0]))
       xchat.command(strip(&quot;mode #aspirine +o un_ps
eudo&quot;))
    if word[1] == &quot;!op&quot; and word[0] == &quot;un_autre_ps
eudo&quot;:
       xchat.command(strip(&quot;me --&gt;je te op &quot;+word[0]))

       xchat.command(strip(&quot;mode #aspirine +o un_autre_pseudo&quot;))
  
  if word[1] == &quot;!op&quot; and word[0] == &quot;encore_un&quot;:
       xc
hat.command(strip(&quot;me --&gt;je te op &quot;+word[0]))
       xchat.command
(strip(&quot;mode #aspirine +o encore_un&quot;))   
    if word[1] == &quot;!op
&quot; and word[0] == &quot;etc...&quot;:
       xchat.command(strip(&quot;me -
-&gt;je te op &quot;+word[0]))
       xchat.command(strip(&quot;mode #aspirine 
+o etc...&quot;))
    
    return xchat.EAT_NONE

EVENTS = [
  (&quot;Chann
el Message&quot;, 1),
  
 ]
for event in EVENTS:
    xchat.hook_print(event[
0], autoop, event)

    
#-------------------------------------------
# aver
ti tous le monde que l'auto-op est desactivé
def unload_cb(userdata): 
    xch
at.command(&quot;me &gt;&gt;&gt; Auto-op est suspendu...&quot;)
    
xchat.hoo
k_unload(unload_cb)

print &quot;*********************&quot;
print &quot;Auto
-op charge!&quot;

print &quot;Script realise par tchoutchou@lexpress.net - 20
04&quot;
</pre>
<br /><a name='conclusion'></a><h2> Conclusion : </h2>
<br /
>N'oubliez pas : seuls les utilisateurs enregistr&eacute;s dans le script pourro
nt s'auto-oper.
