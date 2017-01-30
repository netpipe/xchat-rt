Weevil's Away 1.4c
------------------

A *nice* away script for Xchat.

Manifest:
    away.conf       -   Config settings
    away.pl         -   The script itself
    hideaway.conf   -   Config for hiding away messages on certain channels
                        and networks
    readme.txt      -   This file

Features:
    * Set specific autoresponses for people who PM you.
    * Remembers your away reasons as you change them, and can display a
      kind of timeline of away reasons when you return.
    * Sensibly changes your nick. You can have different nicks on several
      servers, which may have differing maximum nick lengths, and the script
      will handle preservation of your identity on each server, while making
      sure your away reason will fit in the allowed space without clipping.
      (It finds the max. allowed nick length from the server 005 message. If
      this is not given, it will check for the longest nick it knows of from
      the userlist on the current server and use that)
    * Exclude certain channels on specific servers from your away/back
      announcements (server can be spacified with a regular expression since
      many networks run several servers)
    * Complete customisation of away, back, and reason change messages.
    * Handles disconnection/reconnection due to network issues
    * Perform (multiple) commands on away/back.
    * Does not set AWAY with server unless you want it to (see below for the
      philosophy here)

Install:
    UNIX:
        You should have perl installed already. XChat needs to be compiled with
        Perl support (it's in by default).

        Place the away.conf and hideaway.conf files in the .xchat directory in
        your home directory or wherever xchat keeps it's data files. Edit them
        to your liking.

        Place the away.pl script in the .xchat directory as well if you want
        it to be auto-loaded when xchat starts.

        Start xchat and (if it's not auto loaded) load the script using the
        scripts & plugins menu.


    Win32:
        TO EDIT CONFIG FILES USE WORDPAD OR A DECENT FREE TEXT EDITOR SUCH AS
        Crimson (http://www.crimsoneditor.com - under 1mb). Notepad will NOT
        be able to display them correctly.

        Get activeperl from www.activeperl.com and install it (use the .msi
        installer, not the zip).

        Put the away.conf and hideaway.conf files in xchat's data/ directory,
        which is usually
        c:/program files/xchat/data/ and edit them to your liking.

        If you want the script to be loaded when you start XChat, put the
        away.pl file in xchat's data/ directory too.

        If any instances of XChat are running, close them and restart them.
        The script should be loaded automatically if you placed it in the
        data/ directory, otherwise you'll have to load it manually using the
        Scripts & Plugins menu. To find if the script is loaded, check your
        server tab for X-Away messages or do /scpinfo

        NOTE TO WINDOWS USERS: xchat seems to dislike loading scripts from
        paths that contain spaces (like c:/program files/xchat/data/away.pl)
        If loading fails, try using the short format of file and directory
        names to load the script, like so:
            /load c:/progra~1/xchat/data/away.pl

Usage:
    There are the following commands.

    /away [-s|-a] [`append] [reason]
    /back [-s|-a]
    /awayrehash
    /awayreset
    /awaymessage [-t<mins>] [<nick> [message]]
    /nicklen [number]

    Specifying -s will cause each command to be silent (not announce your
    away/back reason).

    Specifying -a will force each command to be announced, this is necessary
    should you want to override the silentaway, silentreason and silentback
    configuration settings (these settings make certain away announcements
    silent by default)

    You can't specify -sa - that doesnt work.

    Specifying a `append will cause whatever follows the ` (backtick) to be
    added to the end of your nick. If it won't fit in the max nick length
    allowed on the server (determined from either the server's 005 message
    received on connect or the longest nick in the userlist), then what you
    append will eat into your nick.

    For example if your nick is "MrLazy" and you append "`away" to it on a
    server that only permits 9 characters in a nick, you get "MrLa`away"

    You don't have to specify an `append.

    If you use the /away command when you're already away, the script will
    assume you're changing reasons and will display the appropriate message.

    /awayrehash reloads the configurations.

    /awayreset is there to reset the auto-away timer. This normally happens
    automatically whenever you send a message to IRC, but some people may want
    to add this to their key bindings so when they hit space, the time they've
    been idle for is reset to zero (so the auto-away feature's internal idle
    time counter is reset). The added advantage of this is if you run this
    command while you're automatically set away it will set you as back, which
    is useful if you want to make text entry in the input field set you back
    from auto away. Just bind the space key to Run Command then put /awayreset
    in Data 1 in Xchat's key bindings.

    /awaymessage lets you schedule a message for delivery. If you specify a
    nick and a message, the message will be sent to that nick next time they
    PM you, along with your standard away PM response. Note that you don't
    have to be away for the message to be delivered.

    If you specify /awaymessage with no arguments it lists the messages that
    are delivered or waiting to be sent.

    If you specify -t25 the message will time out after 25 minutes (and thus
    not be delivered) You can check the time remaining on timed messages by
    using /awaymessage with no arguments. You can specify any number for minutes
    (you're not limited to 25 :)

    If you specify a nick but no message, the current message for the specified
    nick is cleared.

    The message can contain any of the mask modifiers explained in the config
    file.

    Note that as with all things in X-Away, the /awaymessage command is
    context sensitive. If there's someone called DrStupid on two networks that
    you're on, and you set an away message for DrStupid on one network, if the
    DrStupid on the OTHER network PMs you, they wont receive the message. Only
    the nick who PMs you on the server on which the /awaymessage command was
    executed will receive the message. So make sure you execute /awaymessage
    commands on the server that the nick you wish to receive the message resides
    on.

    /nicklen [number] lets you specify the maximum nick length on the server if
    it can't be deciphered automatically (usually it defaults to 9 or the length
    of the longest nick it can find in your userlists if it isn't provided in
    the messages when you connect to a server). /nicklen 15 specifies that the
    maximum nick length is 15 characters. This is used when appending your
    away status to your nick - if the status + nick length is too long to fit
    in the given space, the status will eat into the nickname. For example,
    Weevil`away on a server that only supports 9 character nicks will become
    Weev`away


Config:
    The config file should be placed in xchat's configuration directory
    (although this can be changed by editing the SETTINGS hash at the start of
    the script).

    On Unix systems this is:
    ~/.xchat/away.conf

    On Windows systems this is (usually):
    ?:/program files/xchat/data/away.conf

    The script will warn you if it can't find it's config.

    Read the config; it's heavily commented and lets you set up your away
    message masks, timers etc.

Managing Global Messages:
    X-Away uses the same anti-spamming system as my X-Winamp script. This system
    uses a special file that contains various masks that prevent your spam
    appearing in certain channels on certain networks. Both X-Away and X-Winamp
    can share the same file, the location of which is defined by the
    hideawayfile config setting.

    the form is as follows:
    SERVER_REGEXP = [flags]CHANNEL_REGEXP [flags]CHANNEL_REGEXP...

    You can have as many channel regular expressions as you wish after the
    = sign, seperate them with spaces.

    The flags are optional (which is why they're in []. Don't put the [] in the
    actual definitions)

    For an explanation of regular expressions, please read the Regular
    Expressions section at the end of this file.

    The rules defined in this file prevent global messages created by the script
    from appearing in the channels/servers defined.

    The flags are as follows:
    (none)  - The rule applies whatever your status
        o   - The rule doesnt apply if you're opped
        v   - The rule doesnt apply if you're voiced (and NOT opped)
        ov  - The rule doesnt apply if you're voiced OR opped

    So if you had a channel called #sputnik on any network and you wanted to
    hide your spam unless you've got voice but not opped, use this:
        .* = v#sputnik$

    If you want to spam when opped as well, use:
        .* = ov#sputnik$

    Note that the match will always start at the start of the channel name (a
    ^ is prepended automatically).

    THE SCRIPT WILL NOT ANNOUNCE IN CHANNELS THAT IT KNOWS ARE +m IF YOU'RE NOT
    VOICED OR OPPED. This is sensible. It must see the mode change or receive
    a MODE message from the server (XChat requests one when you join a channel)
    for it to know the channel is +m.

    An example is provided. Anything with a # at the start of the line is
    classed as a comment and is ignored by the parser.

    You do NOT need to restart the script or /awayrehash for changes to these
    files to take effect.

Regular Expressions:
    Regular expressions are a way of matching one string of text against
    another. They work very much like the wildcard system you may have used at
    the command line when copying files (putting a * to indicate 'anything').

    A regular expression can contain a number of special characters that I call
    character classes, and a number of other special characters that I call
    quantifiers. They can also contain standard text. Here's an example:

        irc\..*\.somenetwork\.net$

    This is a regexp that matches an IRC server name. It will match the
    following servers:
        irc.de.somenetwork.net
        irc.uk.somenetwork.net
        irc.anywhere.somenetwork.net
        wonderfulirc.whereveryouwant.somenetwork.net

    The .* in that expression is an important bit. The . is a character class,
    and the * is a quantifier. The . means 'any character at all' and the *
    means 'zero or more times'. The expression reads as follows in English:
        'match the string "irc." followed by any number of arbitary characters,
        then ".somenetwork.net" at the end of the server name'

    The $ sign at the end of the expression means the matching should start at
    the END of the server name. If there was no $, it would start anywhere in
    the server name. If you prepend the expression with a ^, the match would
    be forced to start at the beginning of the server name.

    Note that some . signs have \ signs before them. This is because we don't
    want the expression parser to think that the . is a character class, we want
    it to match an actual . character.

    Here's a short list of character classes:
        .   Any character
        \w  Any alphanumeric (A-Z a-z 0-9) character. \W is any non-alphanumeric
        \s  Any whitespace character (space, tab etc). \S is any non-whitespace
        \d  Any digit character (0-9). \D is any non-digit.

    Here's a short list of quantifiers (you dont need to use a quantifier if you
    only want to match the character class once, for one character):
        *   Zero or more times
        +   One or more times

    I have left out very many, but these should suffice here.


Why not set AWAY with server?
    The simple reason not to set AWAY with the server is that it causes the
    server to spam anyone who PMs you while you're away with annoying away
    messages every time they say something. In clients like Xchat you can turn
    this off, or make it only show up once, but in clients such as mIRC it
    isn't easy to disable this. You lose the AWAY reason on /whois, but that's
    a small sacrifice for having this script's more friendly and configurable
    PM away responses.

    In 1.3b up you can enable server-side AWAY messages if you want.

Contact:
    If you have a suggestion or a bug report, contact me:

    Weevil
    xchat@baxpace.com

Enjoy.

Changes:
1.4c
	* Fix for Xchat 2.0 compatibility issues ("Unknown command" errors)
1.4b
    * Added "eatnick" setting in config file, if this is enabled (default)
      then when you change nick and your away append is too long to fit in the
      space allowed on the server, the append will eat into your nickname. If
      it's disabled the append will simply be stuck onto the end of your nick
      name and may therefore be cropped by the server.

1.4
    * Extended the hideaway file format a bit, so now you can check voice and
      op status to determine if the away message is hidden or not
    * Added /awaymessage command and various config settings for it
    * Added /awayreset to reset idle timers or mark back from auto away
    * No longer attempts to announce in channels it knows are +m

1.3c
    * Fixed PM responses with non-lowercase nicks.

1.3b
    * Removed "Nick change from x to y noticed" message - this was a debugging
      message that should have been removed long ago.
    * Added support for sending the server AWAY announcements. This means that
      your AWAY message can now be set on the server, so it shows up in WHOIS
      replies. The drawback of this method is that the server will spam whoever
      PMs you with your away message, instead of showing it only once (as this
      script does if serveraway is off (default) ).
    * You can now disable the response to PMs by setting pmmask empty.
    * You can now set your away, back and reason change messages to be silent
      (as with -s) by default. You can use -a to override this configuration
      setting, e.g. /away -a `afk I want this to be announced.
    * If you PM someone with /msg <nick> <text> while away, they no longer see
      your away reason when they respond (since you initiated the dialog it's
      silly to tell them you're away whenever they respond.) This used to only
      work if you spoke into an open query tab, now it works with /msg too.
    * Some warning messages have been stamped on.
    * You can now tell the script to never change your nick.

1.3a
    * Unreleased - minor tweaks

1.3
    * Major bugfix to do with changing nicks. In 1.2a the script was broken
      so that it always used the nick that you had when you *first* went away
      when returning from away a second or third time.
    * added option to use your local time instead of GMT/UTC. Set timeformat
      to 1 in the config file to use local times.
    * A number of other tweaks and fixes, including sensible discovery of
      the config directory (doesn't always assume "data/")

1.2a
    * Various warning messages fixed (these only showed up if you had
      perlwarnings on anyway)
    * Correct display of local away message on multiple servers with different
      nicks (used to always show your nick as the one for the context in which
      the /away or /back command was issued)
    * You can now specify wildcards in the channel names in hideaway.conf - in
      fact regular expressions are used here, so to hide your away messages on
      all channels beginning with "ugly" on all servers, use:
      .* = #ugly.*
      NOTE: if you know perl regular expressions, the ^ is automatically
      prepended to the start of your expression by the script, meaning matching
      starts from the start of the text (e.g. in the example above, the channel
      name MUST *start* with "#ugly") if this were not the case, #ugly could be
      matched anywhere in the line. If this doesn't make sense, ignore me. It's
      done to make it act like you'd expect it to.
    * Fixed crashing on invalid regular expressions.
    * If any of your masks are empty for back, reason change or away messages,
      they will not be displayed. Previously a blank message was sent, which
      just looked stupid.
    * added appendchar setting. You still use a ` in the /away command though.
    * when you're back and there's no reason, the script used to specify the
      string "(Unknown reason)" as your reason. Now it specifies "Unknown
      reason". This will only show if you unloaded and reloaded the script
      while away, then did /back and backtickpro comes into effect.

1.2
    * Corrected away masks (some identifiers changed - %R is now %l for example)
    * Added awaycommand and backcommand to config file
    * Added backtickpro feature, also added ability to find "back" nick from
      xchat config file if we don't know it.
    * Many small fixes

1.1
    * Never released.

Known issues:
    There's a problem with disconnection and reconnection to the servers. If
    you reconnect to a server while away BEFORE your old nick pings out, your
    nick will be wrong (xchat changes nick to your default), making you look as
    if you've returned.

(c) Weevil 2003.