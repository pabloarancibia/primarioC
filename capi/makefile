#
#   $Id: MAKEFILE 1.12 2005/03/02 17:50:26Z peterb Exp $
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
#   under applicable U.S. law, 17 U.S.C. §1204 provides criminal
#   penalties for violation of (a) the restrictions on circumvention
#   of copyright protection systems found in 17 U.S.C. §1201 and
#   (b) the protections for the integrity of copyright management
#   information found in 17 U.S.C. §1202.
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
# MAKEFILE for Datalight's CAPI Library Routines
#
# ROM-DOS Copyright (c) 1991-2005 by Datalight, Inc.
# All Rights Reserved.
# Datalight Confidential and Proprietary
#
# This file to build libraries for the small, medium, compact and large
# memory models.
#
# HISTORY
# 09/17/08 gvn split _capi.c
# 02/14/06 gvn Added Clock function
# 12/06/04 gvn Removed need for response file, made maintenance simpler
#
#
CC       = BCC
CFLAGS   = -P- -c -w -O -Z $(RD_SW) -m$(MEM_MODEL) -D_CAPI_LIBRARY -DIPV6 -I..\include
LFLAGS   = $(RD_SW_L)
LIB      = TLIB
INC      = ..\..\include
C = 	$(CC) $(CFLAGS) -o$* -D_CAPI_$* capi.c
A =	$(LIB) capi$(MEM_MODEL).lib +$*

# RELEASE
# -------------
release : clean capis capim capic capil copyrel

copyrel:
		  @if exist ..\LIB\capi?.lib del ..\lib\capi?.lib  >nul
		  @if not exist ..\LIB\nul md ..\lib >nul
		  @copy capi?.lib ..\lib >nul

capis:
	make -B -DMEM_MODEL=s -DMEM_MODEL_NAME=SMALL capilib

capim:
	make -B -DMEM_MODEL=m -DMEM_MODEL_NAME=MEDIUM capilib

capic:
	make -B -DMEM_MODEL=c -DMEM_MODEL_NAME=COMPACT capilib

capil:
	make -B -DMEM_MODEL=l -DMEM_MODEL_NAME=LARGE capilib


# Define the modules of the library.
#
OBJ0 = GETIPVFL.OBJ
OBJ1 = GETSOCKV.OBJ DISABLEA.OBJ ENABLEAS.OBJ ISSOCKET.OBJ GETDCSOC.OBJ
OBJ2 = GETSOCKE.OBJ GETVERSI.OBJ CONVERTD.OBJ EOFSOCKT.OBJ FLUSHSOC.OBJ
OBJ3 = RELEASES.OBJ RELEASED.OBJ ABORTSOC.OBJ ABORTDCS.OBJ SHUTDOWN.OBJ
OBJ4 = GETADDRE.OBJ GTADDREX.OBJ GTADDRIN.OBJ GETBUSYF.OBJ GETPEERA.OBJ
OBJ5 = GTPEEREX.OBJ GETADCOM.OBJ GETKERNI.OBJ ICMPPING.OBJ ICMPPNGX.OBJ
OBJ6 = GETKERNC.OBJ GETNETIN.OBJ CONNECTS.OBJ LISTENSO.OBJ LISTENAC.OBJ
OBJ7 = ACCEPTSO.OBJ SELECTSO.OBJ READSOCK.OBJ READFROM.OBJ WRITESOC.OBJ
OBJ8 = WRITETOS.OBJ SETALARM.OBJ SETASYNC.OBJ RESOLVEN.OBJ PARSEADD.OBJ
OBJ9 = SETSOCKE.OBJ JOINGROU.OBJ LEAVEGRO.OBJ JOINGREX.OBJ LEAVEGRE.OBJ IFACEIOC.OBJ ASYNCNOT.OBJ
OBJA = DPMICBRE.OBJ ARPAPI.OBJ PROAPI.OBJ ROUTE.OBJ MAIN.OBJ CALLBACK.OBJ CLOCK.OBJ

MODULES = $(OBJ0) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7) $(OBJ8) $(OBJ9) $(OBJA)

#
# build the specified library
#
capilib : $(MODULES)
	@del *.obj
	@if exist *.bak del *.bak
	@echo done $(MEM_MODEL_NAME)

GETIPVFL.OBJ:	capi.c
	$C
	$A

GETSOCKV.OBJ:	capi.c
	$C
	$A

DISABLEA.OBJ:	capi.c
	$C
	$A

ENABLEAS.OBJ:	capi.c
	$C
	$A

ISSOCKET.OBJ:	capi.c
	$C
	$A

GETDCSOC.OBJ:	capi.c
	$C
	$A

GETSOCKE.OBJ:	capi.c
	$C
	$A

GETVERSI.OBJ:	capi.c
	$C
	$A

CONVERTD.OBJ:	capi.c
	$C
	$A

EOFSOCKT.OBJ:	capi.c
	$C
	$A

FLUSHSOC.OBJ:	capi.c
	$C
	$A

RELEASES.OBJ:	capi.c
	$C
	$A

RELEASED.OBJ:	capi.c
	$C
	$A

ABORTSOC.OBJ:	capi.c
	$C
	$A

ABORTDCS.OBJ:	capi.c
	$C
	$A

SHUTDOWN.OBJ:	capi.c
	$C
	$A

GETADDRE.OBJ:	capi.c
	$C
	$A

GTADDREX.OBJ:	capi.c
	$C
	$A

GTADDRIN.OBJ:	capi.c
	$C
	$A

GETBUSYF.OBJ:	capi.c
	$C
	$A

GETPEERA.OBJ:	capi.c
	$C
	$A

GTPEEREX.OBJ:	capi.c
	$C
	$A

GETADCOM.OBJ:	capi.c
	$C
	$A

GETKERNI.OBJ:	capi.c
	$C
	$A

ICMPPING.OBJ:	capi.c
	$C
	$A

ICMPPNGX.OBJ:	capi.c
	$C
	$A

GETKERNC.OBJ:	capi.c
	$C
	$A

GETNETIN.OBJ:	capi.c
	$C
	$A

CONNECTS.OBJ:	capi.c
	$C
	$A

LISTENSO.OBJ:	capi.c
	$C
	$A

LISTENAC.OBJ:	capi.c
	$C
	$A

ACCEPTSO.OBJ:	capi.c
	$C
	$A

SELECTSO.OBJ:	capi.c
	$C
	$A

READSOCK.OBJ:	capi.c
	$C
	$A

READFROM.OBJ:	capi.c
	$C
	$A

WRITESOC.OBJ:	capi.c
	$C
	$A

WRITETOS.OBJ:	capi.c
	$C
	$A

SETALARM.OBJ:	capi.c
	$C
	$A

SETASYNC.OBJ:	capi.c
	$C
	$A

RESOLVEN.OBJ:	capi.c
	$C
	$A

PARSEADD.OBJ:	capi.c
	$C
	$A

SETSOCKE.OBJ:	capi.c
	$C
	$A

JOINGROU.OBJ:	capi.c
	$C
	$A

LEAVEGRO.OBJ:	capi.c
	$C
	$A

JOINGREX.OBJ:	capi.c
	$C
	$A

LEAVEGRE.OBJ:	capi.c
	$C
	$A

IFACEIOC.OBJ:	capi.c
	$C
	$A

ASYNCNOT.OBJ:	capi.c
	$C
	$A

DPMICBRE.OBJ:	capi.c
	$C
	$A

ARPAPI.OBJ:	capi.c
	$C
	$A

PROAPI.OBJ:	capi.c
	$C
	$A

ROUTE.OBJ:	capi.c
	$C
	$A

MAIN.OBJ:	_capi.c
	$(CC) $(CFLAGS) -o$* -D_CAPI_$* _capi.c
	$(LIB) capi$(MEM_MODEL).lib +main.obj

CALLBACK.OBJ:	_capi.c
	$(CC) $(CFLAGS) -o$* -D_CAPI_$* _capi.c
	$(LIB) capi$(MEM_MODEL).lib +callback.obj

clock.obj:	clock.c
	$(CC) $(CFLAGS) clock.c
	$(LIB) capi$(MEM_MODEL).lib +clock.obj

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
        @if exist ..\LIB\capi?.lib del ..\lib\capi?.lib  >nul
