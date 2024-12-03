.SILENT:  clean compile link  run

all: clean compile link

link:
	gcc -g -m32 -Wall -o out mypipe.c

compile:
	gcc -g -m32 -Wall -c -o mypipe.o mypipe.c

clean:
	rm -f *.o out

run:
	out -d 