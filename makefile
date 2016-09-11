.SUFFIXES: .c .cpp .o .x .h

bibl=biblioteka
glowna=producent

CC=gcc
libDir=./lib


$(glowna): clearscreen $(glowna).c $(libDir)/$(bibl).o
	$(CC) -Wall -o $(glowna) $(glowna).c $(libDir)/$(bibl).o

clearscreen:
	clear

$(libDir)/$(bibl).o: $(bibl).c
	$(CC) -Wall -c -fpic -o $(bibl).o $(bibl).c
	mkdir -p $(libDir)
	mv $(bibl).o $(libDir)

clean:
	rm -f $(libDir)/*.o
cleanall: clean
	rm -f $(glowna)
	rm -f $(libDir)/*.so

run: $(glowna)
	./$(glowna)

dynamiczna: $(libDir)/$(bibl).o
	$(CC) -shared -o lib$(bibl).so $(libDir)/$(bibl).o 
	mkdir -p $(libDir)
	mv lib$(bibl).so $(libDir)
	$(CC) -o $(glowna) $(glowna).c -L$(libDir) -lbiblioteka -Wl,-R $(libDir) 
