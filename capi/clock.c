// Use a decent clock() function; the Borland version is very inefficient

/* Copyright (C) 1996 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <time.h>
#include <dos.h>
#include "compiler.h"
#include "clock.h"
#if !defined(_Windows) && !defined(_OSE)
#ifdef __DJGPP__
#include <go32.h>
#include <sys/farptr.h>
#include <pc.h>
#include <libc/farptrgs.h>
#include <dpmi.h>
#include <go32.h>
#define GET_CLOCK _farpeekl(_dos_ds,0x46c)
#define GET_CLOCKX r.h.ah = 0,__dpmi_int(0x1a, &r),((((unsigned)r.x.cx) << 16) + r.x.dx)
#else //__DJGPP__
#define GET_CLOCK *fpctBiosTime
#endif //__DJGPP__

unsigned long Clock(void)
{
   static clock_t ctBase = 0;
   static clock_t ctLastTics = 0;
   clock_t ctTics, ctOtics;
   #ifdef __WATCOMF__
   clock_t *fpctBiosTime = (clock_t *)0x46c;
   #elif !defined(__DJGPP__)
   clock_t far *fpctBiosTime = (clock_t far *)0x46c;
   #elif __DJGPP_MINOR__ > 3
   __dpmi_regs r;
   #endif //XPDEB

   /* Make sure the numbers we get are consistent */
   do {
      #if __DJGPP_MINOR__ > 3
      if (_os_trueversion == 0x532) {	/* Windows NT, 2000, XP */
         r.h.ah = 0;
         __dpmi_int(0x1a, &r);
         ctOtics = (((unsigned)r.x.cx) << 16) + r.x.dx;
         r.h.ah = 0;
         __dpmi_int(0x1a, &r);
         ctTics = (((unsigned)r.x.cx) << 16) + r.x.dx;
      }
      else
      #endif //__DJGPP__
      {
         ctOtics = GET_CLOCK;
         ctTics = GET_CLOCK;
      }
   } while (ctOtics != ctTics);

   if (ctBase == 0L)
      ctBase = ctTics;

   if (ctLastTics > ctTics)
   {
      if (ctLastTics - ctTics < 10)
         ctTics = ctLastTics; // I can't explain it, but it happens :(
      else
      {
         // This is a real midnight-passed event
         ctBase -= 0x1800b0L;
      }
   }
   ctLastTics = ctTics;

   return (ctTics - ctBase);
}
#else //#if !defined(_Windows) && !defined(_OSE)
unsigned long Clock(void)
{
   return clock();
}
#endif //#if !defined(_Windows) && !defined(_OSE)

