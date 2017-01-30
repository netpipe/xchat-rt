# ¿Activar colores en la salida del texto? (1) Sí (0) No
set ::rae(colors) 1

# Ignorar en segundos las siguientes peticiones para evitar flood
set ::rae(rest) 10

# Mostrar en varias líneas como si fuese copiar-pegar de la página. (Recomendado)
set ::rae(multiline) 1

# ¿Cuántas acepciones debe mostrar como máximo? Hay algunas palabras polisémicas
# que tienen un montón de acepciones y ejemplos que podrían llenar dos páginas
# del monitor. Hay que poner un número razonable.
set ::rae(max) 8

# ¿Quieres que este script que actualice automaticamente? (1) (0) No
set ::rae(autoupdate) 1

alias rae {
	foreach line [raeGet $_rest] {
		print $line
	}
	complete
}
proc raeGet {ask {research ""}} {
	
	package require http
	global rae tcl_platform plat
	if {$research eq ""} {
		regsub -all -- {\s+} $ask " " word
		set word [string tolower $word]
		set word [string trim $word]
	} else {
		set word $ask
	}
	set word [http::formatQuery $word]
	if {$tcl_platform(os) eq "Linux"} {
	    set platfrm "X11"
	} else {
		set platfrm $tcl_platform(os)
	}
	http::config -useragent "Mozilla/5.0 ($platfrm; U; $tcl_platform(os) $tcl_platform(machine); en-ES; rv:1.9.0.3) Firefox 3.0.7" -accept "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
	set token [http::geturl http://buscon.rae.es/draeI/SrvltGUIBusUsual?LEMA=$word\&origen=RAE&TIPO_BUS=3 -timeout 10000]
	upvar #0 $token state
    set lastcode $state(http)
	set ncode ""
    regexp {[0-9]{3}} $lastcode ncode
    if {$ncode eq ""} {
        set ncode $lastcode
    }
    switch -- $ncode {
		"200" {
			
		}
		"404" {
			return "Error 404 obteniendo informacion de la palabra $word"
			return [list "Error 404"]
		}
		default {
			return "Error no identificado. El servidor respondió: $lastcode"
			return [list "Error no identificado. El servidor respondió: $lastcode"]
		}
	}
	set data $state(body)
	if {[string match -nocase "*Las que se muestran a continuaci*n tienen una escritura cercana*" $data]} {
		set data [encoding convertfrom utf-8 $data]
		set info {}
		set li [regexp -all -inline -- {<li align="left">(.*?)</li>} $data]
		regsub -all -- {\<[^\>]*\>|\t} $li "" li
		foreach {id liword} $li {
			lappend info [string trim $liword]
		}
		if {[llength $info] == 1} {
			return [raeGet $info "(Redirigido)"]
		}
		return "[list "La palabra $ask no está registrada en el Diccionario. ¿Quizá quisiste decir... [join $info ", "]\?."]"
	}
	if {[string match -nocase "*<TITLE>RAE. DRAE. Aviso de error.</TITLE>*" $data]} {
		return [list "\0034La palabra \002$ask\002 no está en el diccionario.\003"]
	}
	set acepLema ""
	set eEtimo ""
	set eLema ""
	# Sé que no es una manera elegante de deshacerse de este \u016 que iso8859-15
	# no es capaz de interpretar y termina en un \026 usado por irc para enviar
	# texto en color invertido, pero hasta ahora es la única manera efectia que
	# me ha funcionado. Si conoces la manera ideal, un email sería apreciado.
	regsub -all {â } $data "" data
	set data [encoding convertfrom utf-8 $data]
	set eLema ""
	regexp {<span class=\"eLema\">(.*?)</span>} $data "" eLema
	regsub -all -- {\<[^\>]*\>|\t} $eLema "" eLema
    set acepLema [regexp -all -inline {<span class="eOrdenAcepLema">(.*?)</p>} $data]
	if {$acepLema eq ""} {
		set acepLema [regexp -all -inline {<p style="margin-left:[0-9]em; margin-bottom:-0.5em">(.*?)</p>} $data]
		set eEtimo ""
		regexp {^.*?<span class=\"eEtimo\">(.*?)</p>} $data "" eEtimo
		regsub -all -- {\<[^\>]*\>|\t} $eEtimo "" eEtimo
		set total [expr [llength $acepLema] / 2]
		if {$total > $rae(max)} {
			set newline "\002\0032$eLema\0033\002 $eEtimo\003 (Se mostrarán $rae(max) de $total acepciones. El resto en: \00312\037http://rae.es/$word\003\037 $research"
		} else {
			set newline "\002\0032$eLema\0033\002 $eEtimo\003 $research"
			if {$eEtimo ne ""} {
				set newline "\002\0032$eLema\0033\002 $eEtimo\003 $research"
			} else {
				set newline "\002\0032$eLema\003\002 $research"
			}
		}
		lappend result $newline
		set i 2
		foreach {id line} $acepLema {
			set num ""
			set genre ""
			set abrv ""
			set acep ""
			set ecomp ""
			regsub -all -nocase {<B>|</B>} $line "" line
			regexp -nocase {<span class="eOrdenAcepFC">(.*?)</span>} $line "" num
			regsub -all {\s+} $num " " num
			regexp -all {<span class="eFCompleja"> (.*?)</span>} $line "" ecomp
			regsub -all {\s+} $ecomp " " ecmop
			regexp -all {<span class="eAbrv" title=\"(.*?)\">} $line "" abrv
			regexp {<span class="eAcep">(.*?)</span>$} $line "" acep
			regsub -all -- {\<[^\>]*\>|\t} $line "" line
			regsub -all -- {<span class="eAbrv" title=\"(.*?)</span>} $acep "" acep
			set newline "[string trim $num][string trim $ecomp][string trim $abrv] $acep"
			putlog [string length $newline]
			if {[string length $newline] > 1} {
				lappend result $newline
			}
			if {$i > $rae(max)} {
				break
			}
			incr i
		}
		set ref [regexp -all -inline {<span class="eReferencia">(.*?)</p>} $data]
		set resultref {}
		foreach {id references} $ref {
			regsub -all -- {\<[^\>]*\>|\t} $references "" references
			lappend resultref [string trim $references]
		}
		lappend result "\0032Véase\003: [join $resultref ", "]"
		return $result
	}
	regexp {^.*?<span class=\"eEtimo\">(.*?)</p>} $data "" eEtimo
	regsub -all -- {\<[^\>]*\>|\t} $eEtimo "" eEtimo
	set total [expr [llength $acepLema] / 2]
	if {$total > $rae(max)} {
		set newline "\002\0032$eLema\0033\002 $eEtimo\003 (Se mostrarán $rae(max) de $total acepciones. El resto en: \00312\037http://rae.es/$word\003\037 $research"
	} else {
		set newline "\002\0032$eLema\0033\002 $eEtimo\003 $research"
	}
	lappend result $newline
	set i 2
	foreach {id line} $acepLema {
		set num ""
		set genre ""
		set abrv ""
		set acep ""
		set info {}
		regsub -all {\s+} $line " " line
		regexp -nocase {<B>(.*?)</B>} $line "" num
		regexp {<span class=\"eAbrvNoEdit\" title=\"(.*?)\">} $line "" genre
		regexp -all {<span class="eAbrv" title=\"(.*?)\">} $line "" abrv
		regexp {<span class="eAcep">(.*?)$} $line "" acep
		set acep [string map -nocase {"<b>" "\002" "</b>" "\002"} $acep]
		regsub -all -- {<span class="eAbrv" title=\"(.*?)</span>} $acep "" acep
		regsub -all -- {\<[^\>]*\>|\t} $acep "" acep
		if {$genre ne ""} {
			lappend info \00314$genre\003
		}
		if {$abrv ne ""} {
			lappend info \0032$abrv\003
		}
		lappend result "\002\00312$num\002\003 ([join $info ", "]) \0031$acep\003"
		if {$i > $rae(max)} {
			break
		}
		incr i
	}
	if {$rae(multiline) != "1"} {
		set result [join $result " | "]
	}
	return $result
}

