
xcdns.dll: plugin-dns.obj
	link /dll /out:xcdns.dll /SUBSYSTEM:WINDOWS /nologo /def:dns.def /OPT:NOWIN98 plugin-dns.obj ws2_32.lib
	dir xcdns.dll

plugin-dns.obj: plugin-dns.c makefile xchat-plugin.h thread.c
	gcc -Dsnprintf=_snprintf -Os -MD -c plugin-dns.c

clean:
	del *.obj
	del *.dll
	del *.exp
	del *.lib
