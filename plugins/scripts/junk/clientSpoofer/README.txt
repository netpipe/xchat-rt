Client Spoofer v1.0
-------------------

Pretty simple.
Move clientSpoofer.so into your plugin or .xchat2 directory
/SPOOF <text> to set <text> as your client response
/SPOOFX shows you what string will be sent to others

It creates a file in your home directory (.clientSpoof) to remember your settings

Recommended that you do /IGNORE * CTCP when you've installed the script, otherwise you could just set a CTCP reply, and xchat will still send it's defaul VERSION string.

If anyone has any advice, or can tell me if this works on windows, please email:
bluberry939@gmail.com


gcc -Wall -Os -DWIN32 -c
gcc -Wall -Os -c
