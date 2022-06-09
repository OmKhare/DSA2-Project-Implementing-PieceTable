main : main.o gui.o piecetable.o
		gcc -Wall -o main main.o gui.o piecetable.o

main.o : main.c gui.h
		gcc -c -Wall main.c

gui.o : gui.c piecetable.h
		gcc -c -Wall gui.c

piecetable.o : piecetable.c gui.h 
		gcc -c -Wall piecetable.c
		
clean :
		rm -f main *.o