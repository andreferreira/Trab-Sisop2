all:readwrite1 readwrite2

readwrite1:readwrite.o rwmonitor1.o
	gcc -o readwrite1 -pthread readwrite.o rwmonitor1.o

readwrite2:readwrite.o rwmonitor2.o
	gcc -o readwrite2 -pthread readwrite.o rwmonitor2.o

readwrite.o:readwrite.c rwmonitor.h
	gcc -c -o readwrite.o readwrite.c

rwmonitor1.o:rwmonitor1.c rwmonitor.h
	gcc -c -o rwmonitor1.o rwmonitor1.c

rwmonitor2.o:rwmonitor2.c rwmonitor.h
	gcc -c -o rwmonitor2.o rwmonitor2.c

clean:
	rm -f readwrite1 readwrite2 readwrite.o rwmonitor1.o rwmonitor2.o
