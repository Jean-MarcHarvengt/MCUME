/* Emulations of the CB operations of the Z80 instruction set.
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

#define var_t   unsigned char t
#define rlc(x)  (x=(x<<1)|(x>>7),rflags(x,x&1))
#define rrc(x)  do{var_t=x&1;x=(x>>1)|(t<<7);rflags(x,t);}while(0)
#define rl(x)   do{var_t=x>>7;x=(x<<1)|(f&1);rflags(x,t);}while(0)
#define rr(x)   do{var_t=x&1;x=(x>>1)|(f<<7);rflags(x,t);}while(0)
#define sla(x)  do{var_t=x>>7;x<<=1;rflags(x,t);}while(0)
#define sra(x)  do{var_t=x&1;x=((signed char)x)>>1;rflags(x,t);}while(0)
#define sll(x)  do{var_t=x>>7;x=(x<<1)|1;rflags(x,t);}while(0)
#define srl(x)  do{var_t=x&1;x>>=1;rflags(x,t);}while(0)

#define rflags(x,c) (f=(c)|(x&0xa8)|((!x)<<6)|parity(x))

#define bit(n,x) (f=(f&1)|((x&(1<<n))?0x10:0x54)|(x&0x28))
#define set(n,x) (x|=(1<<n))
#define res(n,x) (x&=~(1<<n))

{
   /* reg/val are initialised to stop gcc's (incorrect) warning,
    * and static to save initialising them every time.
    */
   static unsigned char reg=0,val=0;
   unsigned short addr;
   unsigned char op;
   if(ixoriy){
      addr=(ixoriy==1?ix:iy)+(signed char)fetch(pc);
      pc++;
      tstates+=8;
      op=fetch(pc);
      reg=op&7;
      op=(op&0xf8)|6;
   }
   else{
      op=fetch(pc);
      tstates+=4;
      radjust++;
      addr=hl;
   }
   pc++;

   if(op<64)switch(op){
   case  0: rlc(b); break;
   case  1: rlc(c); break;
   case  2: rlc(d); break;
   case  3: rlc(e); break;
   case  4: rlc(h); break;
   case  5: rlc(l); break;
   case  6: tstates+=7;val=fetch(addr);rlc(val);store(addr,val);break;
   case  7: rlc(a); break;
   case  8: rrc(b); break;
   case  9: rrc(c); break;
   case 10: rrc(d); break;
   case 11: rrc(e); break;
   case 12: rrc(h); break;
   case 13: rrc(l); break;
   case 14: tstates+=7;val=fetch(addr);rrc(val);store(addr,val);break;
   case 15: rrc(a); break;
   case 0x10: rl(b); break;
   case 0x11: rl(c); break;
   case 0x12: rl(d); break;
   case 0x13: rl(e); break;
   case 0x14: rl(h); break;
   case 0x15: rl(l); break;
   case 0x16: tstates+=7;val=fetch(addr);rl(val);store(addr,val);break;
   case 0x17: rl(a); break;
   case 0x18: rr(b); break;
   case 0x19: rr(c); break;
   case 0x1a: rr(d); break;
   case 0x1b: rr(e); break;
   case 0x1c: rr(h); break;
   case 0x1d: rr(l); break;
   case 0x1e: tstates+=7;val=fetch(addr);rr(val);store(addr,val);break;
   case 0x1f: rr(a); break;
   case 0x20: sla(b); break;
   case 0x21: sla(c); break;
   case 0x22: sla(d); break;
   case 0x23: sla(e); break;
   case 0x24: sla(h); break;
   case 0x25: sla(l); break;
   case 0x26: tstates+=7;val=fetch(addr);sla(val);store(addr,val);break;
   case 0x27: sla(a); break;
   case 0x28: sra(b); break;
   case 0x29: sra(c); break;
   case 0x2a: sra(d); break;
   case 0x2b: sra(e); break;
   case 0x2c: sra(h); break;
   case 0x2d: sra(l); break;
   case 0x2e: tstates+=7;val=fetch(addr);sra(val);store(addr,val);break;
   case 0x2f: sra(a); break;
   case 0x30: sll(b); break;
   case 0x31: sll(c); break;
   case 0x32: sll(d); break;
   case 0x33: sll(e); break;
   case 0x34: sll(h); break;
   case 0x35: sll(l); break;
   case 0x36: tstates+=7;val=fetch(addr);sll(val);store(addr,val);break;
   case 0x37: sll(a); break;
   case 0x38: srl(b); break;
   case 0x39: srl(c); break;
   case 0x3a: srl(d); break;
   case 0x3b: srl(e); break;
   case 0x3c: srl(h); break;
   case 0x3d: srl(l); break;
   case 0x3e: tstates+=7;val=fetch(addr);srl(val);store(addr,val);break;
   case 0x3f: srl(a); break;
   }
   else{
      unsigned char n=(op>>3)&7;
      switch(op&0xc7){
      case 0x40: bit(n,b); break;
      case 0x41: bit(n,c); break;
      case 0x42: bit(n,d); break;
      case 0x43: bit(n,e); break;
      case 0x44: bit(n,h); break;
      case 0x45: bit(n,l); break;
      case 0x46: tstates+=4;val=fetch(addr);bit(n,val);store(addr,val);break;
      case 0x47: bit(n,a); break;
      case 0x80: res(n,b); break;
      case 0x81: res(n,c); break;
      case 0x82: res(n,d); break;
      case 0x83: res(n,e); break;
      case 0x84: res(n,h); break;
      case 0x85: res(n,l); break;
      case 0x86: tstates+=4;val=fetch(addr);res(n,val);store(addr,val);break;
      case 0x87: res(n,a); break;
      case 0xc0: set(n,b); break;
      case 0xc1: set(n,c); break;
      case 0xc2: set(n,d); break;
      case 0xc3: set(n,e); break;
      case 0xc4: set(n,h); break;
      case 0xc5: set(n,l); break;
      case 0xc6: tstates+=4;val=fetch(addr);set(n,val);store(addr,val);break;
      case 0xc7: set(n,a); break;
      }
   }
   if(ixoriy)switch(reg){
      case 0:b=val; break;
      case 1:c=val; break;
      case 2:d=val; break;
      case 3:e=val; break;
      case 4:h=val; break;
      case 5:l=val; break;
      case 7:a=val; break;
   }
}

#undef var_t
#undef rlc
#undef rrc
#undef rl
#undef rr
#undef sla
#undef sra
#undef sll
#undef srl
#undef rflags
#undef bit
#undef set
#undef res
