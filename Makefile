.PHONY: integrate

integrate: integrate.c
	gcc -Wall -o integrate integrate.c -lpthread -llua
