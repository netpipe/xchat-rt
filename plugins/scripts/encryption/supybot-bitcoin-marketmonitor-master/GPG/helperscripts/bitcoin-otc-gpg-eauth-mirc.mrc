/*
GPG authentication helper script for mIRC.

Version 2013.05.15.220000

Usage: /verify PASSWORD

You can install this script by typing /load -rs <path-to-script>

If you change the script, it should be re-initialized.

Inspiration/Original Source: joric/bitcoin-otc

Rewritten by: imsaguy/bitcoin-otc 
*/

on *:load:{
  window -e @otcgpg
  unset %otcgpg*

  if ($isfile(C:\Program Files\GNU\GnuPG\gpg.exe)) {
    set %otcgpgpath "C:\Program Files\GNU\GnuPG\gpg.exe"
    echo @otcgpg GPG Found At C:\Program Files\GNU\GnuPG\
  }
  elseif ($isfile(C:\Program Files\GNU\GnuPG\pub\gpg.exe)) {
    set %otcgpgpath "C:\Program Files\GNU\GnuPG\pub\gpg.exe"
    echo -t @otcgpg GPG Found At C:\Program Files\GNU\GnuPG\pub\
  }
  elseif ($isfile(C:\Program Files (x86)\GNU\GnuPG\gpg.exe)) {
    set %otcgpgpath "C:\Program Files (x86)\GNU\GnuPG\gpg.exe"
    echo -t @otcgpg GPG Found At C:\Program Files (x86)\GNU\GnuPG\
  }
  elseif ($isfile(C:\Program Files (x86)\GNU\GnuPG\pub\gpg.exe)) {
    set %otcgpgpath "C:\Program Files (x86)\GNU\GnuPG\pub\gpg.exe"
    echo -t @otcgpg GPG Found At C:\Program Files (x86)\GNU\GnuPG\pub
  }
  else {
    set %otcgpgpath $$?="I can't find GPG! Please make sure GPG is installed and enter the path to the directory in which otcgpgexe resides:"
    set %otcgpgpath chr(34) $+ %otcgpgpath $+ chr(34)
  }
  set %otcgpgbot $input(What is the name of the GPG bot?,e,GPG Bot name?,gribble)
  if (%otcgpgbot == $null) {
    set %otcgpgbot gribble
    echo -t @otcgpg Using default GPG bot name of gribble.
  } 
  while (%otcgpguser == $null) set %otcgpguser $input(What is your otc username?,eo,otc username)
  echo -t @otcgpg OTCgpg Helper script successfully setup.  Use /verify password to ident to %otcgpgbot and gain voice in -otc.
}

alias verify {
  window -e @otcgpg
  echo @otcgpg ~~~~~~~~~~
  msg %otcgpgbot ;;eauth %otcgpguser
  set %otcgpgpass $1
}

on *:TEXT:$(Request successful for user %otcgpguser $+ *):?: {
  if ($nick == %otcgpgbot) {
    echo -t @otcgpg %otcgpgbot specified a gpg message url of $wildtok($1-, http://*, 1, 32)
    /otcgpg_download $wildtok($1-, http://*, 1, 32)
  }
}

alias otcgpg_download {
  var %socket $+(otcdl,$chr(46),$nopath($1))
  if (!$sock(%socket)) {
    echo -t @otcgpg opening connection to $gettok($1,2,47)
    sockopen %socket $gettok($1,2,47) 80
    sockmark %socket HEAD $gettok($1,2,47) $+($chr(47),$gettok($1,3,47),$chr(47),$gettok($1,4,47))
    echo -t @otcgpg Beginning to get OTP.
  }
  else {
    echo -t @otcgpg Socket already in use.
  }
}

alias otcgpg_everify {
  if ($isfile($1)) {
    .fopen f $1
    var %s = $fread(f)
    .fclose f
    msg %otcgpgbot ;;everify %s
    .remove $1
    .unset %s
    .unset %otcgpgpass
  }
  else {
    echo -t @otcgpg Unable to decrypt the OTP. Did you type in your password?
  }
}

on *:SOCKOPEN:otcdl.*:{
  hadd -m ticks $sockname $ticks
  var %file = $nopath($gettok($sock($sockname).mark,3,32))
  var %fullfile = $+(",$scriptdir,%file,")
  echo -t @otcgpg file name is %file and directory is $scriptdir for a full destination of %fullfile
  var %sckr = sockwrite -n $sockname, %^ = $gettok($sock($sockname).mark,3,32)
  write -c %fullfile
  %sckr GET %^ HTTP/1.0
  %sckr HOST: $gettok($sock($sockname).mark,2,32)
  %sckr ACCEPT: *.*
  %sckr $crlf
}

on *:SOCKREAD:otcdl.*:{
  if ($sockerr) {
    echo -t @otcgpg Error: $sock($sockname).wsmsg
    return
  }
  var %a
  :begin
  if ($gettok($sock($sockname).mark,1,32) == head) {
    ;echo -t @otcgpg Sockread %a
    sockread %a
  }
  else {
    sockread &b
  }
  if ($sockbr) {
    tokenize 32 $sock($sockname).mark
    if ($1 == HEAD) {
      if (%a) {
        ; Catching the file size, avoiding the data header
        if ($gettok(%a,1,32) == Content-Length:) { var %totsize = $gettok(%a,2,32) }
      }
      else {
        ; When there are no vars, we now we have to start binary downloading
        echo -t @otcgpg Downloading %totsize bytes...
        sockmark $sockname GET $2- %totsize
      }
    }
    elseif ($1 == GET) {
      ; Downloading ...
      var %file = $+(",$scriptdir,$nopath($3),"), %cursize = $file(%file).size
      var %totsize = $gettok($sock($sockname).mark,4,32)
      bwrite %file -1 &b
    }
    goto begin
  }
}

on *:SOCKCLOSE:otcdl.*:{
  var %ticks = $calc(($ticks - $hget(ticks,$sockname)) / 1000)
  var %filename = $nopath($gettok($sock($sockname).mark,3,32))
  echo -t @otcgpg File %filename downloaded in : %ticks seconds. Beginning decryption.
  var %in = $scriptdir $+ %filename
  var %out = $scriptdir $+ stdout.txt
  run -n cmd.exe /C echo %otcgpgpass $+ $chr(124) %otcgpgpath --batch --yes --passphrase-fd 0 --decrypt %in > %out
  .timer 1 3 /otcgpg_everify %out
}

on *:TEXT:$(*You are now authenticated for user %otcgpguser $+ *):?: {
  if ($nick == %otcgpgbot) {
    echo -t @otcgpg Successfully authenticated to %otcgpgbot
    echo -t @otcgpg Requesting +v
    msg %otcgpgbot ;;voiceme
    echo -t @otcgpg Script Complete.
  }
}