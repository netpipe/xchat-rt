import xchat
__module_name__ = "inxi" 
__module_version__ = "1.0" 
__module_description__ = "adds buttons for the most common inxi commands " 
 
# delete buttons, just in case
xchat.command("delbutton CPU")
xchat.command("delbutton SYSTEM")
xchat.command("delbutton GFX")
xchat.command("delbutton AUDIO")
xchat.command("delbutton NET")
xchat.command("delbutton SENSOR")

# define the actual commands
def inxis(a,b,c):
    xchat.command("exec -o inxi -Sx")

def inxii(a,b,c):
    xchat.command("exec -o inxi -Ix")

def inxig(a,b,c):
    xchat.command("exec -o inxi -Gx")

def inxia(a,b,c):
    xchat.command("exec -o inxi -Ax")

def inxin(a,b,c):
    xchat.command("exec -o inxi -Nx")

def inxir(a,b,c):
    xchat.command("exec -o inxi -sx")
    
# create a hook cause that's the way it works
xchat.hook_command("inxig", inxig)
xchat.hook_command("inxis", inxis)
xchat.hook_command("inxii", inxii)
xchat.hook_command("inxia", inxia)
xchat.hook_command("inxin", inxin)
xchat.hook_command("inxir", inxir)

# add the buttons
xchat.command("addbutton CPU inxis")
xchat.command("addbutton SYSTEM inxii")
xchat.command("addbutton GFX inxig")
xchat.command("addbutton AUDIO inxia")
xchat.command("addbutton NET inxin")
xchat.command("addbutton SENSOR inxir")

# force the userlist buttons visibility
xchat.command("set gui_ulist_buttons ON") 

    
    