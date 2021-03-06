#
#   $Id: makefile.ms 1.4 2005/03/02 17:49:40Z peterb Exp $
#
#   *** DO NOT REMOVE THE FOLLOWING NOTICE ***
#
#   Copyright (c) 1993 - 2005 Datalight, Inc.
#   All Rights Reserved
#
#   Datalight, Inc. is a Washington corporation with an address at
#   21520 30TH DR SE MS110 Bothell, WA, 98021
#   Tel: 800.221.6630  Fax: 425.951.8091
#   Http://www.datalight.com
#
#   This software, including without limitation all source code and
#   documentation, is the confidential, trade secret property of
#   Datalight, Inc., and is protected under the copyright laws of
#   the United States and other jurisdictions. Portions of the
#   software may also be subject to one or more the following US
#   patents: US#5860082, US#6260156.
#
#   In addition to civil penalties for infringement of copyright
#   under applicable U.S. law, 17 U.S.C. �1204 provides criminal
#   penalties for violation of (a) the restrictions on circumvention
#   of copyright protection systems found in 17 U.S.C. �1201 and
#   (b) the protections for the integrity of copyright management
#   information found in 17 U.S.C. �1202.
#
#   U.S. Government Restricted Rights. Use, duplication,
#   reproduction, or transfer of this commercial product and
#   accompanying documentation is restricted in accordance with
#   FAR 12.212 and DFARS 227.7202 and by a License Agreement.
#
#   IN ADDITION TO COPYRIGHT AND PATENT LAW, THIS SOFTWARE IS
#   PROTECTED UNDER A SOURCE CODE AGREEMENT, NON-DISCLOSURE
#   AGREEMENT (NDA), OR SIMILAR BINDING CONTRACT BETWEEN DATALIGHT,
#   INC. AND THE LICENSEE ("BINDING AGREEMENT"). YOUR RIGHT, IF ANY,
#   TO COPY, PUBLISH, MODIFY OR OTHERWISE USE THE SOFTWARE,IS SUBJECT
#   TO THE TERMS AND CONDITIONS OF THE BINDING AGREEMENT, AND BY
#   USING THE SOFTWARE IN ANY MANNER, IN WHOLE OR IN PART, YOU AGREE
#   TO BE BOUND BY THE TERMS OF THE BINDING AGREEMENT. CONTACT
#   DATALIGHT, INC. AT THE ADDRESS SET FORTH ABOVE IF YOU OBTAINED
#   THIS SOFTWARE IN ERROR.
#
#
# MAKEFILE for Datalight's Socket Library Routines
#
# ROM-DOS Copyright (c) 1991-2005 by Datalight, Inc.
# All Rights Reserved.
# Datalight Confidential and Proprietary
#
# This file to build libraries for the small, medium, compact and large
# memory models.
#
# HISTORY
# 12/06/04 gvn Removed need for response file, made maintenance simpler
#
#
CC       = cl
CFLAGS   = /c /Os /Gs /G1 /Zp1 /A$(MEM_MODEL) /D_SAPI_LIBRARY /I..\include /nologo
LIB      = LIB
C = 	$(CC) $(CFLAGS) -Fo$* /D_SAPI_$* socket.c
A =	$(LIB) ms_sock$(MEM_MODEL).lib /NOLOGO +$*;

# RELEASE
# -------------
release : clean sockets socketm socketc socketl copyrel

copyrel:
		  @if exist ..\LIB\ms_sock?.lib del ..\lib\ms_sock?.lib  >nul
		  @if not exist ..\LIB\nul md ..\lib >nul
		  @copy ms_sock?.lib ..\lib >nul

sockets:
	nmake /A -fmakefile.ms MEM_MODEL=S MEM_MODEL_NAME=SMALL socklib

socketm:
	nmake /A -fmakefile.ms MEM_MODEL=M MEM_MODEL_NAME=MEDIUM socklib

socketc:
	nmake /A -fmakefile.ms MEM_MODEL=C MEM_MODEL_NAME=COMPACT socklib

socketl:
	nmake /A -fmakefile.ms MEM_MODEL=L MEM_MODEL_NAME=LARGE socklib


# Define the modules of the library.
#
OBJ1 = ACCEPT.OBJ BIND.OBJ CLOSESOC.OBJ CONNECT.OBJ IOCTLSOC.OBJ
OBJ2 = GETPEERN.OBJ GETSOCKN.OBJ GETSOCKO.OBJ HTONL.OBJ HTONS.OBJ
OBJ3 = INETADDR.OBJ INETNTOA.OBJ LISTEN.OBJ NTOHL.OBJ NTOHS.OBJ
OBJ4 = RECV.OBJ RECVFROM.OBJ SELECT.OBJ SEND.OBJ SENDTO.OBJ
OBJ5 = SETSOCKO.OBJ SHUTDOWN.OBJ SOCKET.OBJ GHOSTBYA.OBJ GHOSTBYN.OBJ
OBJ6 = GHOSTNAM.OBJ GSERVBYP.OBJ GSERVBYN.OBJ GPROTONU.OBJ GPROTONA.OBJ
OBJ7 = WSASTART.OBJ WSASETER.OBJ WSAGETER.OBJ
OBJ8 = GETADDRI.OBJ INETNTOP.OBJ GAISTRER.OBJ
OBJ9 = ISSET.OBJ FDCLR.OBJ FDSET.OBJ MAPERROR.OBJ GETREADC.OBJ
OBJ10 = CNVRT128.OBJ GETNEXTP.OBJ CHECKUDP.OBJ DATABASE.OBJ BASE.OBJ

MODULES = $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7) $(OBJ8) $(OBJ9) $(OBJ10)


#
# build the specified library
#
socklib : $(MODULES)
	@del *.obj
	@if exist *.bak del *.bak
	@echo done $(MEM_MODEL_NAME)

ACCEPT.OBJ:	socket.c
	$C
	$A
BIND.OBJ:	socket.c
	$C
	$A
CLOSESOC.OBJ:	socket.c
	$C
	$A
CONNECT.OBJ:	socket.c
	$C
	$A
IOCTLSOC.OBJ:	socket.c
	$C
	$A
GETPEERN.OBJ:	socket.c
	$C
	$A
GETSOCKN.OBJ:	socket.c
	$C
	$A
GETSOCKO.OBJ:	 socket.c
	$C
	$A
HTONL.OBJ:	socket.c
	$C
	$A
HTONS.OBJ:	socket.c
	$C
	$A
INETADDR.OBJ:	socket.c
	$C
	$A
INETNTOA.OBJ:	socket.c
	$C
	$A
LISTEN.OBJ:	socket.c
	$C
	$A
NTOHL.OBJ:	socket.c
	$C
	$A
NTOHS.OBJ:	socket.c
	$C
	$A
RECV.OBJ:	socket.c
	$C
	$A
RECVFROM.OBJ:	socket.c
	$C
	$A
SELECT.OBJ:	socket.c
	$C
	$A
SEND.OBJ:	socket.c
	$C
	$A
SENDTO.OBJ:	socket.c
	$C
	$A
SETSOCKO.OBJ:	socket.c
	$C
	$A
SHUTDOWN.OBJ:	socket.c
	$C
	$A
SOCKET.OBJ:	socket.c
	$C
	$A
GHOSTBYA.OBJ:	socket.c
	$C
	$A
GHOSTBYN.OBJ:	socket.c
	$C
	$A
GHOSTNAM.OBJ:	socket.c
	$C
	$A
GSERVBYP.OBJ:	socket.c
	$C
	$A
GSERVBYN.OBJ:	socket.c
	$C
	$A
GPROTONU.OBJ:	socket.c
	$C
	$A
GPROTONA.OBJ:	socket.c
	$C
	$A
WSASTART.OBJ:	socket.c
	$C
	$A
WSASETER.OBJ:	socket.c
	$C
	$A
WSAGETER.OBJ:	socket.c
	$C
	$A
GETADDRI.OBJ:	socket.c
	$C
	$A
INETNTOP.OBJ:	socket.c
	$C
	$A
GAISTRER.OBJ:	socket.c
	$C
	$A
CNVRT128.OBJ:	socket.c
	$C
	$A
ISSET.OBJ:	socket.c
	$C
	$A
FDCLR.OBJ:	socket.c
	$C
	$A
FDSET.OBJ:	socket.c
	$C
	$A
MAPERROR.OBJ:	socket.c
	$C
	$A
GETREADC.OBJ:	socket.c
	$C
	$A
GETNEXTP.OBJ:	socket.c
	$C
	$A
CHECKUDP.OBJ:	socket.c
	$C
	$A
DATABASE.OBJ:	socket.c
	$C
	$A
BASE.OBJ:	socket.c
	$C
	$A

# HOUSEKEEPING
# -----------------
clean :
	@if exist *.obj del *.obj
	@if exist *.map del *.map
	@if exist *.lnk del *.lnk
	@if exist *.xrf del *.xrf
	@if exist *.lst del *.lst
	@if exist *.exe del *.exe
	@if exist *.pt del *.pt
	@if exist *.td del *.td
	@if exist *.tr del *.tr
	@if exist *.bak del *.bak
	@if exist *.lib del *.lib
	@if exist x.y del x.y
	@if exist err.txt del err.txt
	@if exist status.me del status.me

cleanall : clean
	@if exist status.me del status.me
	@if exist build.log del build.log
		  @if exist ..\lib\ms_sock?.lib del ..\lib\ms_sock?.lib >nul
