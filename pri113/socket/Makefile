# Javier Abellán. 4 Septiembre 2003
#
# Makefile para la construcción del ejemplo Cliente/Servidor de socket UDP.

# Poner en esta variable el path de la libreria libChSocket.a
DIRLIBSOCKET=../lib

LIBSOCKET=-lChSocket

all : Servidor Cliente

CPPFLAGS = -g -I.

Servidor : Servidor.c
	cc -g -I. -I$(DIRLIBSOCKET) -L$(DIRLIBSOCKET) Servidor.c \
	$(LIBSOCKET) -o Servidor

Cliente : Cliente.c
	cc -g -I. -I$(DIRLIBSOCKET) -L$(DIRLIBSOCKET) Cliente.c \
	$(LIBSOCKET) -o Cliente

clean :
	rm Cliente Servidor
