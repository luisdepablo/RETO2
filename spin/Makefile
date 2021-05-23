CC = gcc
CFLAGS = -Wall - Werror

clean:
	rm -rf ./pan.* ./_spin_nvr.tmp

spin:
	spin -a ./paso1.pml
	gcc -o ./pan ./pan.c
	./pan -a -f -N alarmaActivada
	./pan -a -f -N alarmaDesactivada
	./pan -a -f -N activaAlarma
	./pan -a -f -N desactivaAlarma