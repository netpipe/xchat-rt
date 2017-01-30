CC = gcc
CCFLAGS =
# Eat this! gcc4/MacOSX 
# CCFLAGS = -Wno-pointer-sign


all:    note

	@if [ -f "mirdef_linux.h" ]; then echo; echo "Using mirdef_linux.h ..."; cp mirdef_linux.h mirdef.h;echo; fi

	@make misc
	@make DH
	@make FiSH

	@echo
	@ls -l xfish.so
	@echo
	@echo -e "Compiling finished -->> $(PWD)/\033[1mxfish.so\033[0m"
	@echo


note:
	@echo
	@echo -e "######################## \033[7m\033[1mFiSH for xchat COMPILING NOTES\033[0m ########################"
	@echo "--------------------------------------------------------------------------------"
	@echo    "You'll need MIRACL (http://indigo.ie/~mscott/) to compile FiSH! It is suggested"
	@echo    "to recompile miracl.a on your target machine (compile instructions can be found"
	@echo -e "in miracl.zip/\033[1mlinux.txt\033[0m). Also, make sure you are using the correct mirdef.h"
	@echo -e "(especially in case of exotic systems). On \033[1mAMD64\033[0m systems use 'mirdef_amd64.h'"
	@echo -e "and have a look at miracl.zip/amd64.txt, use 'mirdef_macosx.h' for \033[1mMacOSX\033[0m. You"
	@echo -e "can test MIRACL with \033[1mpk-demo\033[0m."
	@echo
	@echo -e "-- If you plan to use a \033[4mcustom\033[0m password for your blow.ini (via /setinipw),"
	@echo -e "-- you can \033[4mskip\033[0m the following note!"
	@echo    "You are adviced to change default_iniKey in FiSH.h - But remember, changing the"
	@echo    "default blow.ini password will make your old keys in blow.ini unusable!"
	@echo
	@echo -e "\033[1m>> \033[4mUsual procedure to compile FiSH:\033[0m"
	@echo    "   unzip FiSH-xchat.v0.98-source.zip;cd FiSH-xchat.v0.98-source"
	@echo    "   mkdir MIRACL;cd MIRACL;cp ../mir_amd64 amd64;cp ../mir_macosx macosx"
	@echo    "   wget http://indigo.ie/~mscott/miracl.zip (or 'curl -O' instead of wget)"
	@echo -e "   \033[4munzip -j -aa -L miracl.zip\033[0m  (see linux.txt/amd64.txt for more info)"
	@echo -e "   \033[4mbash linux\033[0m  (or '\033[4mbash amd64\033[0m' / '\033[4mbash macosx\033[0m')"
	@echo    "   cp miracl.a ../;cd .."
	@echo -e "   \033[4mmake\033[0m  (or '\033[4mmake macosx\033[0m' / '\033[4mmake amd64\033[0m') - use 'gmake' on BSD"
	@echo    "   cp xfish.so /path/to/xchat/plugins"
	@echo    "--------------------------------------------------------------------------------"
	@echo
	@echo "Press ENTER to continue or CTRL+C to abort..."; read junk

	@if [ ! -f "miracl.a" ]; then echo "miracl.a not found! You didn't compile MIRACL, did you?"; echo; exit 1; fi

	@make -s clean
	@echo "Compiling now..."
	@echo


clean:
	@echo
	@echo Cleaning up...
	@echo
	@rm -f SHA-256.o
	@rm -f base64.o
	@rm -f blowfish.o
	@rm -f cfgopts.o
	@rm -f DH1080.o
	@rm -f FiSH.o
	@rm -f xfish.so
	@rm -f DH1080.so


DH:
	$(CC) $(CCFLAGS) -static -c DH1080.c
	$(CC) $(CCFLAGS) -static -shared DH1080.o miracl.a SHA-256.o base64.o -o DH1080.so


misc:
	$(CC) $(CCFLAGS) -c -static SHA-256.c
	$(CC) $(CCFLAGS) -c -static base64.c
	$(CC) $(CCFLAGS) -c -static blowfish.c
	$(CC) $(CCFLAGS) -c -static cfgopts.c


FiSH:
	$(CC) $(CCFLAGS) -I. -static -O2 -Wall -c FiSH.c -fPIC -DPIC -o FiSH.o
	$(CC) $(CCFLAGS) -static -shared FiSH.o SHA-256.o base64.o blowfish.o cfgopts.o DH1080.o miracl.a -o xfish.so


mac:    macosx
macosx: note

	@if [ -f "mirdef_macosx.h" ]; then echo; echo "Using mirdef_macosx.h ..."; cp mirdef_macosx.h mirdef.h;echo; fi

	$(CC) $(CCFLAGS) -c -fno-common SHA-256.c
	$(CC) $(CCFLAGS) -c -fno-common base64.c
	$(CC) $(CCFLAGS) -c -fno-common blowfish.c
	$(CC) $(CCFLAGS) -c -fno-common cfgopts.c

	ranlib miracl.a
	$(CC) $(CCFLAGS) -fno-common -c DH1080.c
	$(CC) $(CCFLAGS) -bundle -flat_namespace -undefined suppress DH1080.o miracl.a SHA-256.o base64.o -o DH1080.so

	$(CC) $(CCFLAGS) -I. -fno-common -O2 -Wall -c FiSH.c -fPIC -DPIC -o FiSH.o
	$(CC) $(CCFLAGS) -bundle -flat_namespace -undefined suppress FiSH.o SHA-256.o base64.o blowfish.o cfgopts.o DH1080.o miracl.a -o xfish.so

	@echo
	@ls -l xfish.so
	@echo
	@echo -e "Compiling finished -->> $(PWD)/\033[1mxfish.so\033[0m"
	@echo


amd64:  note

	@if [ -f "mirdef_amd64.h" ]; then echo; echo "Using mirdef_amd64.h ..."; cp mirdef_amd64.h mirdef.h;echo; fi

	$(CC) $(CCFLAGS) -c -static -fPIC SHA-256.c
	$(CC) $(CCFLAGS) -c -static -fPIC base64.c
	$(CC) $(CCFLAGS) -c -static -fPIC blowfish.c
	$(CC) $(CCFLAGS) -c -static -fPIC cfgopts.c

	$(CC) $(CCFLAGS) -static -c -fPIC DH1080.c
	$(CC) $(CCFLAGS) -fPIC -shared DH1080.o miracl.a SHA-256.o base64.o -o DH1080.so

	$(CC) $(CCFLAGS) -I. -static -O2 -Wall -c FiSH.c -fPIC -DPIC -o FiSH.o
	$(CC) $(CCFLAGS) -fPIC -shared FiSH.o SHA-256.o base64.o blowfish.o cfgopts.o DH1080.o miracl.a -o xfish.so

	@echo 
	@ls -l xfish.so
	@echo
	@echo -e "Compiling finished -->> $(PWD)/\033[1mxfish.so\033[0m"
	@echo
