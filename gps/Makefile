Test : main.o sbp.o gps.o edc.o
	gcc -o Test main.o sbp.o gps.o edc.o -lpthread
main.o : main.c
	gcc -c main.c
sbp.o : sbp.c sbp.h edc.h
	gcc -c sbp.c
gps.o : gps.c gps.h sbp.h
	gcc -c gps.c
edc.o : edc.c edc.h
	gcc -c edc.c

