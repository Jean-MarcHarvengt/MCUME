/* Emulations of the ED operations of the Z80 instruction set.
 * Copyright (C) 1994 Ian Collier.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define input(var) {  unsigned short u;\
                      var=u=in(b,c);\
                      tstates+=u>>8;\
                      f=(f&1)|(var&0xa8)|((!var)<<6)|parity(var);\
                   }
#define sbchl(x) {    unsigned short z=(x);\
                      unsigned long t=(hl-z-cy)&0x1ffff;\
                      f=((t>>8)&0xa8)|(t>>16)|2|\
                        (((hl&0xfff)<(z&0xfff)+cy)<<4)|\
                        (((hl^z)&(hl^t)&0x8000)>>13)|\
                        ((!(t&0xffff))<<6)|2;\
                      l=t;\
                      h=t>>8;\
                   }

#define adchl(x) {    unsigned short z=(x);\
                      unsigned long t=hl+z+cy;\
                      f=((t>>8)&0xa8)|(t>>16)|\
                        (((hl&0xfff)+(z&0xfff)+cy>0xfff)<<4)|\
                        (((~hl^z)&(hl^t)&0x8000)>>13)|\
                        ((!(t&0xffff))<<6)|2;\
                      l=t;\
                      h=t>>8;\
                 }

#define neg (a=-a,\
            f=(a&0xa8)|((!a)<<6)|(((a&15)>0)<<4)|((a==128)<<2)|2|(a>0))

{
   unsigned char op=fetch(pc&0x7fff);
   pc++;
   radjust++;
   switch(op){
instr(0x40,8);
   input(b);
endinstr;

instr(0x41,8);
   tstates+=out(b,c,b);
endinstr;

instr(0x42,11);
   sbchl(bc);
endinstr;

instr(0x43,16);
   {unsigned short addr=fetch2(pc);
    pc+=2;
    store2b(addr,b,c);
   }
endinstr;

instr(0x44,4);
   neg;
endinstr;

instr(0x45,4);
   iff1=iff2;
   ret;
endinstr;

instr(0x46,4);
   im=0;
endinstr;

instr(0x47,5);
   i=a;
endinstr;

instr(0x48,8);
   input(c);
endinstr;

instr(0x49,8);
   tstates+=out(b,c,c);
endinstr;

instr(0x4a,11);
   adchl(bc);
endinstr;

instr(0x4b,16);
   {unsigned short addr=fetch2(pc);
    pc+=2;
    c=fetch(addr);
    b=fetch(addr+1);
   }
endinstr;

instr(0x4c,4);
   neg;
endinstr;

instr(0x4d,4);
   ret;
endinstr;

instr(0x4e,4);
   im=1;
endinstr;

instr(0x4f,5);
   r=a;
   radjust=r;
endinstr;

instr(0x50,8);
   input(d);
endinstr;

instr(0x51,8);
   tstates+=out(b,c,d);
endinstr;

instr(0x52,11);
   sbchl(de);
endinstr;

instr(0x53,16);
   {unsigned short addr=fetch2(pc);
    pc+=2;
    store2b(addr,d,e);
   }
endinstr;

instr(0x54,4);
   neg;
endinstr;

instr(0x55,4);
   ret;
endinstr;

instr(0x56,4);
   im=2;
endinstr;

instr(0x57,5);
   a=i;
   f=(f&1)|(a&0xa8)|((!a)<<6)|(iff2<<2);
endinstr;

instr(0x58,8);
   input(e);
endinstr;

instr(0x59,8);
   tstates+=out(b,c,e);
endinstr;

instr(0x5a,11);
   adchl(de);
endinstr;

instr(0x5b,16);
   {unsigned short addr=fetch2(pc);
    pc+=2;
    e=fetch(addr);
    d=fetch(addr+1);
   }
endinstr;

instr(0x5c,4);
   neg;
endinstr;

instr(0x5d,4);
   ret;
endinstr;

instr(0x5e,4);
   im=3;
endinstr;

instr(0x5f,5);
   r=(r&0x80)|(radjust&0x7f);
   a=r;
   f=(f&1)|(a&0xa8)|((!a)<<6)|(iff2<<2);
endinstr;

instr(0x60,8);
   input(h);
endinstr;

instr(0x61,8);
   tstates+=out(b,c,h);
endinstr;

instr(0x62,11);
   sbchl(hl);
endinstr;

instr(0x63,16);
   {unsigned short addr=fetch2(pc);
    pc+=2;
    store2b(addr,h,l);
   }
endinstr;

instr(0x64,4);
   neg;
endinstr;

instr(0x65,4);
   ret;
endinstr;

instr(0x66,4);
   im=0;
endinstr;

instr(0x67,14);
   {unsigned char t=fetch(hl);
    unsigned char u=(a<<4)|(t>>4);
    a=(a&0xf0)|(t&0x0f);
    store(hl,u);
    f=(f&1)|(a&0xa8)|((!a)<<6)|parity(a);
   }
endinstr;

instr(0x68,8);
   input(l);
endinstr;

instr(0x69,8);
   tstates+=out(b,c,l);
endinstr;

instr(0x6a,11);
   adchl(hl);
endinstr;

instr(0x6b,16);
   {unsigned short addr=fetch2(pc);
    pc+=2;
    l=fetch(addr);
    h=fetch(addr+1);
   }
endinstr;

instr(0x6c,4);
   neg;
endinstr;

instr(0x6d,4);
   ret;
endinstr;

instr(0x6e,4);
   im=1;
endinstr;

instr(0x6f,5);
   {unsigned char t=fetch(hl);
    unsigned char u=(a&0x0f)|(t<<4);
    a=(a&0xf0)|(t>>4);
    store(hl,u);
    f=(f&1)|(a&0xa8)|((!a)<<6)|parity(a);
   }
endinstr;

instr(0x70,8);
   {unsigned char x;input(x);}
endinstr;

instr(0x71,8);
   tstates+=out(b,c,0);
endinstr;

instr(0x72,11);
   sbchl(sp);
endinstr;

instr(0x73,16);
   {unsigned short addr=fetch2(pc);
    pc+=2;
    store2(addr,sp);
   }
endinstr;

instr(0x74,4);
   neg;
endinstr;

instr(0x75,4);
   ret;
endinstr;

instr(0x76,4);
   im=2;
endinstr;

instr(0x78,8);
   input(a);
endinstr;

instr(0x79,8);
   tstates+=out(b,c,a);
endinstr;

instr(0x7a,11);
   adchl(sp);
endinstr;

instr(0x7b,16);
   {unsigned short addr=fetch2(pc);
    pc+=2;
    sp=fetch2(addr);
   }
endinstr;

instr(0x7c,4);
   neg;
endinstr;

instr(0x7d,4);
   ret;
endinstr;

instr(0x7e,4);
   im=3;
endinstr;

instr(0xa0,12);
   {unsigned char x=fetch(hl);
    store(de,x);
    if(!++l)h++;
    if(!++e)d++;
    if(!c--)b--;
    f=(f&0xc1)|(x&0x28)|(((b|c)>0)<<2);
   }
endinstr;

instr(0xa1,12);
   {unsigned char carry=cy;
    cpa(fetch(hl));
    if(!++l)h++;
    if(!c--)b--;
    f=(f&0xfa)|carry|(((b|c)>0)<<2);
   }
endinstr;

instr(0xa2,12);
   {unsigned short t=in(b,c);
    store(hl,t);
    tstates+=t>>8;
    if(!++l)h++;
    b--;
    f=(b&0xa8)|((b>0)<<6)|2|((parity(b)^c)&4);
   }
endinstr;

instr(0xa3,12); /* I can't determine the correct flags outcome for the
                   block OUT instructions.  Spec says that the carry
                   flag is left unchanged and N is set to 1, but that
                   doesn't seem to be the case... */
   {unsigned char x=fetch(hl);
    tstates+=out(b,c,x);
    if(!++l)h++;
    b--;
    f=(f&1)|0x12|(b&0xa8)|((b==0)<<6);
   }
endinstr;

instr(0xa8,12);
   {unsigned char x=fetch(hl);
    store(de,x);
    if(!l--)h--;
    if(!e--)d--;
    if(!c--)b--;
    f=(f&0xc1)|(x&0x28)|(((b|c)>0)<<2);
   }
endinstr;

instr(0xa9,12);
   {unsigned char carry=cy;
    cpa(fetch(hl));
    if(!l--)h--;
    if(!c--)b--;
    f=(f&0xfa)|carry|(((b|c)>0)<<2);
   }
endinstr;

instr(0xaa,12);
   {unsigned short t=in(b,c);
    store(hl,t);
    tstates+=t>>8;
    if(!l--)h--;
    b--;
    f=(b&0xa8)|((b>0)<<6)|2|((parity(b)^c^4)&4);
   }
endinstr;

instr(0xab,12);
   {unsigned char x=fetch(hl);
    tstates+=out(b,c,x);
    if(!l--)h--;
    b--;
    f=(f&1)|0x12|(b&0xa8)|((b==0)<<6);
   }
endinstr;

/* Note: the Z80 implements "*R" as "*" followed by JR -2.  No reason
   to change this... */

instr(0xb0,12);
   {unsigned char x=fetch(hl);
    store(de,x);
    if(!++l)h++;
    if(!++e)d++;
    if(!c--)b--;
    f=(f&0xc1)|(x&0x28)|(((b|c)>0)<<2);
    if(b|c)pc-=2,tstates+=5;
   }
endinstr;

instr(0xb1,12);
   {unsigned char carry=cy;
    cpa(fetch(hl));
    if(!++l)h++;
    if(!c--)b--;
    f=(f&0xfa)|carry|(((b|c)>0)<<2);
    if((f&0x44)==4)pc-=2,tstates+=5;
   }
endinstr;

instr(0xb2,12);
   {unsigned short t=in(b,c);
    store(hl,t);
    tstates+=t>>8;
    if(!++l)h++;
    b--;
    f=(b&0xa8)|((b>0)<<6)|2|((parity(b)^c)&4);
    if(b)pc-=2,tstates+=5;
   }
endinstr;

instr(0xb3,12);
   {unsigned char x=fetch(hl);
    tstates+=out(b,c,x);
    if(!++l)h++;
    b--;
    f=(f&1)|0x12|(b&0xa8)|((b==0)<<6);
    if(b)pc-=2,tstates+=5;
   }
endinstr;

instr(0xb8,12);
   {unsigned char x=fetch(hl);
    store(de,x);
    if(!l--)h--;
    if(!e--)d--;
    if(!c--)b--;
    f=(f&0xc1)|(x&0x28)|(((b|c)>0)<<2);
    if(b|c)pc-=2,tstates+=5;
   }
endinstr;

instr(0xb9,12);
   {unsigned char carry=cy;
    cpa(fetch(hl));
    if(!l--)h--;
    if(!c--)b--;
    f=(f&0xfa)|carry|(((b|c)>0)<<2);
    if((f&0x44)==4)pc-=2,tstates+=5;
   }
endinstr;

instr(0xba,12);
   {unsigned short t=in(b,c);
    store(hl,t);
    tstates+=t>>8;
    if(!l--)h--;
    b--;
    f=(b&0xa8)|((b>0)<<6)|2|((parity(b)^c^4)&4);
    if(b)pc-=2,tstates+=5;
   }
endinstr;

instr(0xbb,12);
   {unsigned char x=fetch(hl);
    tstates+=out(b,c,x);
    if(!l--)h--;
    b--;
    f=(f&1)|0x12|(b&0xa8)|((b==0)<<6);
    if(b)pc-=2,tstates+=5;
   }
endinstr;

/* save/load patches */

instr(0xfc,4);
#ifdef SZ81	/* Added by Thunor */
  if(!zx80 && hl < 0x8000)
    {
    sdl_load_file(hl,LOAD_FILE_METHOD_NAMEDLOAD);
    }
  else /* if((!zx80 && hl >= 0x8000) || zx80) */
    {
    sdl_load_file(hl,LOAD_FILE_METHOD_SELECTLOAD);
    }
#else
  load_p(hl);
#endif
  framewait=1;
endinstr;

instr(0xfd,4);
#ifdef SZ81	/* Added by Thunor */
  if(zx80)
    {
    sdl_save_file(hl,SAVE_FILE_METHOD_UNNAMEDSAVE);
    }
  else
    {
    sdl_save_file(hl,SAVE_FILE_METHOD_NAMEDSAVE);
    }
#else
  save_p(hl);
#endif
  framewait=1;
endinstr;

default: tstates+=4;

}}
