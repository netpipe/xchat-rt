SOURCE:=${shell ls *.java}
MAIN=Chess.java

chess: $(SOURCE)
	make clean
	javac $(MAIN) 

all: 
	make chess
	make -C irssi
	make -C test
	
clean:
	rm -f core *.class *~
	make clean -C irssi
	make clean -C test
