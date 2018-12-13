//  $Id: _capi.c 1.16 2007/10/25 06:29:12Z gerritvn Exp $
//
//  *** DO NOT REMOVE THE FOLLOWING NOTICE ***
//
//  Copyright (c) 1996-2007 Datalight, Inc.
//  All Rights Reserved
//
//  Datalight, Inc. is a Washington corporation located at:
//
//  21520 30th Dr SE, Suite 110,          Tel: +1.425.951.8086
//  Bothell, WA  98021                    Fax: +1.425.951.8094
//  USA                                   Http://www.datalight.com
//
//  This software, including without limitation all source code and
//  documentation, is the confidential, trade secret property of
//  Datalight, Inc., and is protected under the copyright laws of
//  the United States and other jurisdictions.  Portions of the
//  software may also be subject to one or more the following US
//  patents: US#5860082, US#6260156.
//
//  In addition to civil penalties for infringement of copyright
//  under applicable U.S. law, 17 U.S.C. 1204 provides criminal
//  penalties for violation of (a) the restrictions on circumvention
//  of copyright protection systems found in 17 U.S.C. 1201 and
//  (b) the protections for the integrity of copyright management
//  information found in 17 U.S.C. 1202.
//
//  U.S. Government Restricted Rights.  Use, duplication,
//  reproduction, or transfer of this commercial product and
//  accompanying documentation is restricted in accordance with
//  FAR 12.212 and DFARS 227.7202 and by a License Agreement.
//
//  IN ADDITION TO COPYRIGHT AND PATENT LAW, THIS SOFTWARE IS
//  PROTECTED UNDER A SOURCE CODE AGREEMENT, NON-DISCLOSURE
//  AGREEMENT (NDA), OR SIMILAR BINDING CONTRACT BETWEEN DATALIGHT,
//  INC. AND THE LICENSEE ("BINDING AGREEMENT").  YOUR RIGHT, IF ANY,
//  TO COPY, PUBLISH, MODIFY OR OTHERWISE USE THE SOFTWARE,IS SUBJECT
//  TO THE TERMS AND CONDITIONS OF THE BINDING AGREEMENT, AND BY
//  USING THE SOFTWARE IN ANY MANNER, IN WHOLE OR IN PART, YOU AGREE
//  TO BE BOUND BY THE TERMS OF THE BINDING AGREEMENT.  CONTACT
//  DATALIGHT, INC. AT THE ADDRESS SET FORTH ABOVE IF YOU OBTAINED
//  THIS SOFTWARE IN ERROR.
//
/************************************************************************
   _CAPI.C

   Copyright (c) 1996-2008 Datalight, Inc.
   Confidential and Proprietary
   All Rights Reserved

   Provides the compiler specific portions of the CAPI module.

   History:
   09/17/2008 gvn split into 2 modules.
                  AsyncNotificationHandler() only compiles when _CAPI_CALLBACK
                  is defined.
   05/17/2008 gvn remove _dpmi_resize_dos_memory()
   02/11/2008 gvn added WATCOM support with major input from Paul Santa Maria
                  of Twelve Strike
   07/19/2002 jmb IR#4032 corrected compilation errors with MSVC
   02/13/2002 jmb IR#3939
                  corrected variable declaration in FindSocketsVector
                  when compiling in C++ mode.
   08/22/2001 jmb added DJGPP support
   04/19/2001 jmb changed ISR_ROUTINE to _loadds
   03/20/2001 jmb Added FindSocketsVector to search the vector table
                  for the Sockets kernel.  We look for the string
                  "TCPTSR" to verify that Sockets is in memory.
                  I removed the code that checked the interrupt
                  vector for a non-zero value.
   03/16/2001 jmb added check for actual interrupt vector being set
                  changed error check on return from Sockets api
                  to check cflag rather than use flags which is not
                  available in MSVC.  I also modified the definition
                  of AsyncNotificationHandler to compile under either
                  SDTK or MSVC.
   1.01 2000-12-09 pkg Factored out non portable code
   1.00 2000-09-08 pkg Created
 ************************************************************************/

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "capi.h"

#ifndef _CAPI_LIBRARY
#define _CAPI_MAIN
#endif

#ifdef _CAPI_MAIN
/*
   Publicly available error variables that further describe
   the details of failures while talking with the Sockets
   kernel.  These are set immediately following a call into
   the CAPI and are only valid if the previous function
   call indicated a failure.
*/
int iNetErrNo;
int iNetSubErrNo;

INFO_STRUCT_CHAIN (*psInfoChainHead);

/*
   Prototypes
*/
int FindSocketsVector(void);

/*
   The following global is used to tell the CallSocketsDosApi()
   function what interrupt the Sockets stack is hooking.  By
   default this is 0x61 unless overidden on the command line
   load of SOCKETM or SOCKETSP.
*/
int iSocketsApiInterrupt = 0x00;


/*
   Parameters
   px86Regs, pointer to an X86Regs structure which holds the
   registers to call the Sockets interrupt with.  This structure
   is updated with the new register values after the Sockets
   interrupt is made.
   Description
   Calls the Sockets kernel via a software interrupt described
   by iSocketsApiInterrupt with the registers defined in the
   X86Regs structure.
   Returns
   On success, ax value returned by the Sockets call.
   On error (carry flag set), -1 if the Sockets API is not found
*/
int CallSocketsDosApi(X86Regs * px86regs)
{
   /* Clear the current error status */
   iNetErrNo = iNetSubErrNo = 0;

   /* Make certain the sockets interrupt is set */
   if (iSocketsApiInterrupt == 0)
   {
      iSocketsApiInterrupt = FindSocketsVector();
      if (iSocketsApiInterrupt == 0)
      {
         iNetErrNo = ERR_API_NOT_LOADED;
         return -1;
      }
   }

   /*
      Call the sockets interrupt with the appropriate
      registers setup for the sockets API
   */

   #if defined(DPMI_16BIT_STACK)
   // clear stack registers so that DPMI provides the stack
   px86regs->r.x.ss = 0;
   px86regs->r.x.sp = 0;

   // call the dpmi handler
   __dpmi_int(iSocketsApiInterrupt, &px86regs->r);
   #else
   int86x(iSocketsApiInterrupt, &px86regs->r, &px86regs->r, &px86regs->_s);
   #endif

   /*
      Any errors are reported with a carry flag set and the
      network error is in the low eight bits and any
      sub-error codes are in the upper eight bits
   */
   if (CARRY_FLAG_CHECK)
   {
      iNetErrNo = px86regs->r.h.al;
      iNetSubErrNo = px86regs->r.h.ah;
      return -1;
   }
   else
   {
      return px86regs->r.x.ax;
   }
}

#ifdef __WATCOMF__
/*
 * __dpmi_int:
 * This function performs a software interrupt in real mode after filling
 * in *most* the registers from the given structure.  %ss, %esp, and
 * %eflags are automatically taken care of, unlike
 * __dpmi_simulate_real_mode_interrupt::.
 */
int
__dpmi_int(int int_no, __dpmi_regs * regs)  /* DPMI function AX = 0x0300 */
{
  int loc =  FP_OFF(regs);
  WORD dpmi_err = 0;
  __asm {
    cli
    push eax;
    push ebx;
    push ecx;
    push edx;
    push edi;
    //es should already be set properly
    //mov ax,word ptr[regs]
    //mov es,ax
    mov eax,0x300
    mov ebx,int_no
    mov ecx,0
    mov edi,loc
    int 0x31
    jnc No_error
    mov dpmi_err,ax
  No_error:
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    sti
  }
  if (dpmi_err)
    return dpmi_err;
  else
    return 0;
}

unsigned short __dpmi_error;
unsigned short _watcom_ds;

int _dpmi_allocate_real_mode_callback_retf(_dpmi_seginfo *info,__dpmi_regs *regs)
{
//DS:(E)SI = selector:offset of protected mode procedure to call
//ES:(E)DI = selector:offset of 32H-byte buffer for real mode register data structure to be used when calling callback routine.

  union REGS r;
  struct SREGS sr;

  segread(&sr);
  r.x.esi = info->pm_offset;
  sr.es = sr.ds;
  _watcom_ds = sr.ds;
  r.x.edi = (unsigned)regs;
  sr.ds = sr.cs;
  r.w.ax = 0x0303;        /* DPMI allocate realmode callback address */
  int386x(0x31, &r, &r, &sr);

  /* Failure */
  if (r.x.cflag)
  {
    __dpmi_error = r.w.ax;
    return -1;
  }

  /* Success */
  info->rm_segment = r.w.cx;
  info->rm_offset = r.w.dx;
  return 0;
}

int _dpmi_free_real_mode_callback(_dpmi_seginfo *info)
{
  union REGS r;

  r.w.cx = info->rm_segment;
  r.w.dx = info->rm_offset;
  r.w.ax = 0x0304;        /* DPMI free realmode callback address */
  int386(0x31, &r, &r);

  /* Failure */
  if (r.x.cflag)
  {
    __dpmi_error = r.w.ax;
    return -1;
  }

  /* Success */
  return 0;
}

/*
 * _dpmi_allocate_dos_memory: Allocate a part of the conventional memory area (the first 640K)
 *
 * PARAMETERS:
 * - Set the size field of info to the number of paragraphs requested
 *
 * RETURNS:  returns zero if success, else dpmi error and info->size is max size
 */
int _dpmi_allocate_dos_memory(_dpmi_seginfo *info)
{
  union REGS r;
  r.w.ax = 0x0100;        /* DPMI allocate DOS memory */
  r.w.bx = info->size;    /* Number of paragraphs requested */
  int386 (0x31, &r, &r);

  /* Failure */
  if (r.x.cflag)
  {
    info->size = r.w.bx << 4;    /* bx= largest block (paragraphs) */
    return r.w.ax;               /* ax= error code:
                                    07h  memory control blocks damaged
                                    08h  insufficient memory available
                                    1Fh  no more selectors available */
  }

  /* Success */
  info->rm_segment = r.w.ax;     /* Initial real mode segment of allocated block */
  info->rm_offset = 0;
  info->pm_selector = r.w.dx;    /* DX = Selector for allocated block */
  return 0;
}

/*
 * _dpmi_free_dos_memory: frees memory from "Allocate DOS Memory Block"
 *
 * PARAMETERS:
 * - Pass the same struct used for dpmi_alloc_dos_memory
 *
 * RETURNS:  returns zero if success, else dpmi error#
 */
int _dpmi_free_dos_memory(_dpmi_seginfo *info)
{
  union REGS r;
  r.w.ax = 0x0101;               /* Free DOS Memory Block */
  r.w.dx = info->pm_selector;    /* DX = Selector of block to free */
  int386 (0x31, &r, &r);

  /* Failure */
  if (r.x.cflag)
  {
    return r.w.ax;               /* ax= error code:
                                    07h  memory control blocks damaged
                                    08h  insufficient memory available
                                    1Fh  no more selectors available */
  }

  /* Success */
  return 0;
}

#if 0 // not used any more
/* set new size to bytes/16, call.  If error, max size
  is returned in size as bytes/16 */
int _dpmi_resize_dos_memory(_dpmi_seginfo *info)
{
  union REGS r;
  r.w.ax = 0x0102;               /* Resize DOS Memory Block */
  r.w.bx = (WORD)info->size;     /* BX = New block size in paragraphs */
  r.w.dx = info->pm_selector;    /* DX = Selector of block to modify */
  int386 (0x31, &r, &r);

  /* Failure */
  if (r.x.cflag)
  {
    info->size = r.w.bx;         /* Maximum block size possible in paragraphs */
    return r.w.ax;               /* ax= error code:
                                    07h  memory control blocks damaged
                                    08h  insufficient memory available
                                    09h  incorrect memory segment specified */

  }

  /* Success */
  return 0;
}
#endif //0

/* DPMI 0.9 AX=0200 */
int __dpmi_get_real_mode_interrupt_vector(int vector, __dpmi_raddr * raddr)
{
  WORD tseg = 0;
  WORD tofs = 0;;
  WORD ierr = 0, err_flag = 0;
  __asm {
    pusha;
    mov eax,0x0200;
    mov ebx,vector;
    int 0x31;
    jc do_error;
    mov tseg, cx;
    mov tofs, dx;
    jmp do_pop
do_error:
    mov ierr,ax
    mov ax,-1
    mov err_flag,ax
do_pop:
    popa;
  }

  if ( (err_flag) || (!tseg && !tofs) )
  {
     /* ax= error code */
    ierr = (ierr == 0) ? -1 : ierr;
    return ierr;
  }
  else
  {
    /* Success */
    raddr->offset16 = tofs;
    raddr->segment = tseg;
    return 0;
   }
}
#endif //__WATCOMF__

#if defined(DPMI_16BIT_STACK)

int iLevel = -1;
int aiAvail[NUM_DOSBUF_LEVELS];
int aiUsed[NUM_DOSBUF_LEVELS];
DPMIINFO_STRUCT asLevelInfo[NUM_DOSBUF_LEVELS];

/*
 *----------------------------------------------------------------------------
 * $DocProc: _AllocateCapiDosBuffer
 *
 * OnEntry
 *   iUse 0 - Base level buffer
 *        1 - Asynchronous notification buffer
 *   iSize Max buffer size
 *
 * OnExit
 *   returns 0 if success, -1 on error with iNetErrNo set to error.
 *
 * Modifies
 *   iNetErrNo on error
 *
 *----------------------------------------------------------------------------
 */

int _AllocateCapiDosBuffer(int iUse,int iSize)
{
   int i;
   DPMIINFO_STRUCT *ps;

   if (iLevel > 0)
      return -1;     // only allow direct or at base level
   if ((i = iLevel) == -1)
      i = iUse;
   ps = asLevelInfo + i;
   if (ps->size)
      DPMI_FREE_DOS_MEMORY(ps);
   ps->size = (((iSize) + 15) >> 4) + 3;
   if (DPMI_ALLOCATE_DOS_MEMORY(ps) != 0) {
      ps->size = 0;
      return -1;
   }
   aiAvail[i] = ps->size << 4;
   return 1;
}

void _DosBufFree(int iUse)
{
   int i;
   DPMIINFO_STRUCT *ps;

   if ((i = iUse) < 0)
      i = 0;
   for (;i < NUM_DOSBUF_LEVELS;++i) {
      ps = asLevelInfo + i;
      if (ps->size)
         DPMI_FREE_DOS_MEMORY(ps);
      ps->size = 0;
      if (iUse >= 0)
         break;
   }
}

int _DosBufEnter(int iCount)
{
   if (++iLevel >= NUM_DOSBUF_LEVELS) {
      --iLevel;
      iNetErrNo = ERR_NO_MEM;
      return -1;
   }
   if (aiAvail[iLevel] < iCount) {
      if (_AllocateCapiDosBuffer(iLevel,iCount) < 0) {
         --iLevel;
         iNetErrNo = ERR_NO_MEM;
         return -1;
      }
   }
   aiUsed[iLevel] = 0;
   return 0;
}

void _DosBufSetup(DPMIINFO_STRUCT *psInfo,void *pBuf,int iCount,int iPut)
{
   DPMIINFO_STRUCT *psLevelInfo;

   psLevelInfo = asLevelInfo + iLevel;
   psInfo->rm_segment = psLevelInfo->rm_segment;
   psInfo->rm_offset = psLevelInfo->rm_offset + aiUsed[iLevel];
   aiUsed[iLevel] += iCount;
   if (iPut && pBuf && iCount)
      DOSBUF_PUT((*psInfo),pBuf,iCount);
}

void _DosBufExit(void)
{
   --iLevel;
}

#endif //DPMI_16BIT_STACK

/*
 *----------------------------------------------------------------------------
 * $DocProc: FindSocketsVector
 *
 * OnEntry
 *   Nothing
 *
 * OnExit
 *   returns SOCKETS vector number or 0 if not found.
 *
 * Modifies
 *   none
 *
 *----------------------------------------------------------------------------
 */
int FindSocketsVector()
{
   #if defined(DPMI_16BIT_STACK)
   __dpmi_raddr api_info;
   char cTempTSRID[10];
   #else
   #ifdef __cplusplus
   void (interrupt *piAPIInterruptVector)(...);
   #else
   void (interrupt *piAPIInterruptVector)();
   #endif
   #endif
   static char cSockTSRID[] = "TCPTSR";
   int iAPIInterrupt, i;
   char D_FAR *pTSRID;

   for (iAPIInterrupt = 0x20; iAPIInterrupt < 0x100; iAPIInterrupt++)
   {
      #if defined(DPMI_16BIT_STACK)
      __dpmi_get_real_mode_interrupt_vector(iAPIInterrupt, &api_info);
      #else
      piAPIInterruptVector = _dos_getvect(iAPIInterrupt);
      #endif

      #if defined(DPMI_16BIT_STACK)
      if (api_info.segment == 0 && api_info.offset16 == 0)
      #else
      if (piAPIInterruptVector == 0)
      #endif
         continue;

      #if defined(DPMI_16BIT_STACK)
      dosmemget((DWORD)(api_info.segment << 4) + api_info.offset16,
               10, cTempTSRID);
      pTSRID = &cTempTSRID[3];
      #else
      pTSRID = (char far *)piAPIInterruptVector + 3;
      #endif
      for (i = 0; i < 6; i++)
      {
         if (pTSRID[i] != cSockTSRID[i])
            break;
      }
      if (i == 6)
         break;
   }
   if (iAPIInterrupt == 0x100)
      iAPIInterrupt = 0;
   return iAPIInterrupt;
}
#endif //_CAPI_MAIN

#ifdef _CAPI_CALLBACK
DPMI_CB_REGS(cb_regs);

typedef int (D_FAR *FH)(int,int,DWORD);
/*
   Stack size of stack for asynchronous callbacks
*/
#define HANDLER_STACK_SIZE 5000

static char acStack[HANDLER_STACK_SIZE];
char *_pcRmCbStackTop = acStack + HANDLER_STACK_SIZE - 2;

#ifdef __WATCOM16__
   // check for -zdp option
   #ifndef __SW_ZDP
      #error The -zdp compiler switch must be specified
   #endif
   #ifndef __SW_S
      #error The -s compiler switch must be specified
   #endif
#endif

void CAPI_CB AsyncNotificationHandler(void)
{
   static int iSocket, iEvent;
   static WORD wArgLo, wArgHi, wHintLo, wHintHi;

   static DWORD dwArg;
   static FH Handler;
   static int iSaveNetErrNo, iSaveNetSubErrNo;

   #ifdef DPMI_16BIT_STACK
   #if DPMI_DEBUG
   int Vprintf(int iAttribute,char *szFormat, ...);
   static int iNotifications;
   #endif

   ENTER_RMCB();
   RMCB_PARAMS();
   #else  //DPMI_16BIT_STACK
   static int iSaveSS, iSaveSP;

   // Save the error variables just incase we interrupted while they have
   // been set, but not used yet and our user calls CAPI.

   iSaveNetErrNo = iNetErrNo;
   iSaveNetSubErrNo = iNetSubErrNo;

   _ASM {

      // It is safe to use local variables as async notifications don't nest
      // We can safely ignore BP as long as we do not use any auto variables
      // Save arguments in registers into local variables
      mov   iSocket,bx;
      mov   iEvent,cx;
      mov   wArgLo,dx;
      mov   wArgHi,si;
      mov   wHintHi,es;
      mov   wHintLo,di;

      // Save stack registers
      mov   iSaveSS,ss;
      mov   iSaveSP,sp;

      // Switch stack to local stack
      mov   ax,ds;
      mov   ss,ax
      mov   sp,word ptr _pcRmCbStackTop;
   }
   #endif

   ((WORD *)&Handler)[0] = wHintLo;
   ((WORD *)&Handler)[1] = wHintHi;
   ((WORD *)&dwArg)[0] = wArgLo;
   ((WORD *)&dwArg)[1] = wArgHi;

   #if DPMI_DEBUG
   Vprintf(6,"%5d AsyncNotificationHandler %d %d %lx %lx",++iNotifications,
               iSocket,iEvent,dwArg,Handler);
   #endif //DPMI_DEBUG
   if (Handler) {
      PUSH_DS();
      (*Handler)(iSocket,iEvent, dwArg);
      POP_DS();
   }
   // restore error variables
   iNetErrNo = iSaveNetErrNo;
   iNetSubErrNo = iSaveNetSubErrNo;
   #ifdef DPMI_16BIT_STACK
   EXIT_RMCB();
   #else
   _ASM
   {
      // Restore stack registers
      mov     ss,iSaveSS;
      mov     sp,iSaveSP;
   }
   #endif
}

#endif //_CAPI_CALLBACK

/*
 * Local Variables:
 * tab-width:3
 * End:
 */

/*  EOF: _capi.c */

