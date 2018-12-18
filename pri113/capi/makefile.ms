#
#   $Id: makefile.ms 1.8 2005/03/02 17:50:26Z peterb Exp johnb $
#
#   *** DO NOT REMOVE THE FOLLOWING NOTICE ***
#
#   Copyright (c) 1993 - 2008 Datalight, Inc.
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
# MAKEFILE for Datalight's CAPI Library Routines using MSVC 1.5
#
# This file to build libraries for the small, medium, compact and large
# memory models.
#
# HISTORY
# 09/17/08 gvn split _capi.c
# 02/14/06 gvn add clock
# 01/14/05 gvn fixed _capi.obj
# 12/06/04 gvn Removed need for response file, made maintenance simpler
#
#
CC       = cl
CFLAGS   = /c /Os /Gs /G1 /Zp1 /A$(MEM_MODEL) /D_CAPI_LIBRARY /I..\include /nologo
LIB      = LIB
C =   $(CC) $(CFLAGS) /D_CAPI_$* -Fo$* capi.c
A =  $(LIB) ms_capi$(MEM_MODEL).lib /NOLOGO +$*;

# RELEASE
# -------------
release : clean capis capim capic capil copyrel

copyrel:
		@if exist ..\LIB\ms_capi?.lib del ..\lib\ms_capi?.lib  >nul
		@if not exist ..\LIB\nul md ..\lib >nul
		@copy ms_capi?.lib ..\lib >nul

capis:
  nmake /A -fmakefile.ms MEM_MODEL=S DMEM_MODEL_NAME=SMALL capilib

capim:
  nmake /A -fmakefile.ms MEM_MODEL=M DMEM_MODEL_NAME=MEDIUM capilib

capic:
  nmake /A -fmakefile.ms MEM_MODEL=C DMEM_MODEL_NAME=COMPACT capilib

capil:
  nmake /A -fmakefile.ms MEM_MODEL=L DMEM_MODEL_NAME=LARGE capilib


# Define the modules of the library.
#
OBJ0 = getipvfl.obj
OBJ1 = getsockv.obj disablea.obj enableas.obj issocket.obj getdcsoc.obj
OBJ2 = getsocke.obj getversi.obj convertd.obj eofsockt.obj flushsoc.obj
OBJ3 = releases.obj released.obj abortsoc.obj abortdcs.obj shutdown.obj
OBJ4 = getaddre.obj gtaddrex.obj gtaddrin.obj getbusyf.obj getpeera.obj
OBJ5 = gtpeerex.obj getadcom.obj getkerni.obj icmpping.obj icmppngx.obj
OBJ6 = getkernc.obj getnetin.obj connects.obj listenso.obj listenac.obj
OBJ7 = acceptso.obj selectso.obj readsock.obj readfrom.obj writesoc.obj
OBJ8 = writetos.obj setalarm.obj setasync.obj resolven.obj parseadd.obj
OBJ9 = setsocke.obj joingrou.obj leavegro.obj joingrex.obj leavegre.obj ifaceioc.obj asyncnot.obj
OBJa = dpmicbre.obj arpapi.obj proapi.obj route.obj main.obj callback.obj clock.obj

MODULES = $(OBJ0) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7) $(OBJ8) $(OBJ9) $(OBJa)

#
# build the specified library
#
capilib : $(MODULES)
  @del *.obj
  @if exist *.bak del *.bak
  @echo done $(MEM_MODEL_NAME)

GETIPVFL.OBJ:  capi.c
  $C
  $A

GETSOCKV.OBJ:  capi.c
  $C
  $A

DISABLEA.OBJ:  capi.c
  $C
  $A

ENABLEAS.OBJ:  capi.c
  $C
  $A

ISSOCKET.OBJ:  capi.c
  $C
  $A

GETDCSOC.OBJ:  capi.c
  $C
  $A

GETSOCKE.OBJ:  capi.c
  $C
  $A

GETVERSI.OBJ:  capi.c
  $C
  $A

CONVERTD.OBJ:  capi.c
  $C
  $A

EOFSOCKT.OBJ:  capi.c
  $C
  $A

FLUSHSOC.OBJ:  capi.c
  $C
  $A

RELEASES.OBJ:  capi.c
  $C
  $A

RELEASED.OBJ:  capi.c
  $C
  $A

ABORTSOC.OBJ:  capi.c
  $C
  $A

ABORTDCS.OBJ:  capi.c
  $C
  $A

SHUTDOWN.OBJ:  capi.c
  $C
  $A

GETADDRE.OBJ:  capi.c
  $C
  $A

GTADDREX.OBJ:  capi.c
  $C
  $A

GTADDRIN.OBJ:  capi.c
  $C
  $A

GETBUSYF.OBJ:  capi.c
  $C
  $A

GETPEERA.OBJ:  capi.c
  $C
  $A

GTPEEREX.OBJ:  capi.c
  $C
  $A

GETADCOM.OBJ:  capi.c
  $C
  $A

GETKERNI.OBJ:  capi.c
  $C
  $A

ICMPPING.OBJ:  capi.c
  $C
  $A

ICMPPNGX.OBJ:  capi.c
  $C
  $A

GETKERNC.OBJ:  capi.c
  $C
  $A

GETNETIN.OBJ:  capi.c
  $C
  $A

CONNECTS.OBJ:  capi.c
  $C
  $A

LISTENSO.OBJ:  capi.c
  $C
  $A

LISTENAC.OBJ:  capi.c
  $C
  $A

ACCEPTSO.OBJ:  capi.c
  $C
  $A

SELECTSO.OBJ:  capi.c
  $C
  $A

READSOCK.OBJ:  capi.c
  $C
  $A

READFROM.OBJ:  capi.c
  $C
  $A

WRITESOC.OBJ:  capi.c
  $C
  $A

WRITETOS.OBJ:  capi.c
  $C
  $A

SETALARM.OBJ:  capi.c
  $C
  $A

SETASYNC.OBJ:  capi.c
  $C
  $A

RESOLVEN.OBJ:  capi.c
  $C
  $A

PARSEADD.OBJ:  capi.c
  $C
  $A

SETSOCKE.OBJ:  capi.c
  $C
  $A

JOINGROU.OBJ:  capi.c
  $C
  $A

LEAVEGRO.OBJ:  capi.c
  $C
  $A

JOINGREX.OBJ:  capi.c
  $C
  $A

LEAVEGRE.OBJ:  capi.c
  $C
  $A

IFACEIOC.OBJ:  capi.c
  $C
  $A

ASYNCNOT.OBJ:  capi.c
  $C
  $A

DPMICBRE.OBJ:  capi.c
  $C
  $A

ARPAPI.OBJ:  capi.c
  $C
  $A

PROAPI.OBJ:  capi.c
  $C
  $A

ROUTE.OBJ:  capi.c
  $C
  $A

main.obj:  _capi.c
  $(CC) $(CFLAGS) /D_CAPI_$* -Fo$* _capi.c
  $(LIB) ms_capi$(MEM_MODEL).lib /NOLOGO +main.obj;

callback.obj:  _capi.c
  $(CC) $(CFLAGS) /D_CAPI_$* -Fo$* _capi.c
  $(LIB) ms_capi$(MEM_MODEL).lib /NOLOGO +callback.obj;

clock.obj:  clock.c
  $(CC) $(CFLAGS) clock.c
  $(LIB) ms_capi$(MEM_MODEL).lib /NOLOGO +clock.obj;


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
		  @if exist ..\LIB\ms_capi?.lib del ..\lib\ms_capi?.lib  >nul

