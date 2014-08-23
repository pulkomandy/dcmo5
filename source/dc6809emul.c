//////////////////////////////////////////////////////////////////////////
// DC6809EMUL.C - Motorola 6809 micropocessor emulation
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcmo5.free.fr
// Created  : 1997-08
// Version  : 2006-10-25
//
// This file is part of DCMO5 v11.
// 
// DCMO5 v11 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DCMO5 v11 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DCMO5 v11.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////

//global variables
int n;                           //cycle count
int CC = 0x10;                   //condition code
short PC, D, X, Y, U, S, DA, W;  //6809 two byte registers + W work register
char *PCHp, *PCLp, *Ap, *Bp;     //pointers to register bytes
char *XHp, *XLp, *YHp, *YLp;     //pointers to register bytes
char *UHp, *ULp, *SHp, *SLp;     //pointers to register bytes
char *DPp, *DDp, *WHp, *WLp;     //pointers to register bytes
#define PCH *PCHp
#define PCL *PCLp
#define A   *Ap
#define B   *Bp
#define XH  *XHp
#define XL  *XLp
#define YH  *YHp
#define YL  *YLp
#define UH  *UHp
#define UL  *ULp
#define SH  *SHp
#define SL  *SLp
#define DP  *DPp
#define DD  *DDp
#define WH  *WHp
#define WL  *WLp

/*condition code masks (CC=EFHINZVC)*/
#define  C1 0x01  /* carry */
#define  V1 0x02  /* overflow */
#define  Z1 0x04  /* zero */
#define  Z0 0xfb  /* ~Z1 */
#define  N1 0x08  /* negative */
#define  I1 0x10  /* irq mask */
#define  H1 0x20  /* half-carry */
#define  F1 0x40  /* firq mask */
#define  E1 0x80  /* extended registers save */
#define  NV 0x0a  /* negative | overflow */
#define  ZC 0x05  /* zero | carry */
#define NZC 0x0e  /* negative | zero | carry */

/*
conditional jump summary

C BCC BCS      Z BNE BEQ      V BVC BVS     NZV BGT BLE
0 yes no       0 yes no       0 yes no      000 yes no
1 no  yes      1 no  yes      1 no  yes     100 no  yes
                                            010 no  yes
N BPL BMI     ZC BHI BLS     NV BGE BLT     001 no  yes
0 yes no      00 yes no      00 yes no      110 no  no
1 no  yes     10 no  yes     10 no  yes     101 yes no
              01 no  yes     01 no  yes     011 no  no
              11 no  no      11 yes no      111 no  yes
*/
#define BHI (CC&ZC)==0
#define BLS (CC&ZC)==Z1||(CC&ZC)==C1
#define BCC (CC&C1)==0  // BCC = BHS
#define BCS (CC&C1)==C1 // BCS = BLO
#define BNE (CC&Z1)==0
#define BEQ (CC&Z1)==Z1
#define BVC (CC&V1)==0
#define BVS (CC&V1)==V1
#define BPL (CC&N1)==0
#define BMI (CC&N1)==N1
#define BGE (CC&NV)==0||(CC&NV)==NV
#define BLT (CC&NV)==N1||(CC&NV)==V1
#define BGT (CC&NZC)==0||(CC&0x0e)==NV
#define BLE (CC&NZC)==N1||(CC&NZC)==Z1||(CC&NZC)==V1||(CC&NZC)==NZC
#define BRANCH {PC+=Mgetc(PC);}
#define LBRANCH {PC+=Mgetw(PC);n++;}

//repetitive code
#define IND Mgeti()
#define DIR DD=Mgetc(PC++)
#define EXT W=Mgetw(PC);PC+=2
#define SETZERO {if(W) CC &= Z0; else CC |= Z1;}

//external functions (or pointers to functions)
//memory is accessed through :
//Mgetc : reads one byte from address a
//Mgetw : reads two bytes from address a
//Mputc : writes one byte to address a
//Mputw : writes two bytes to address a
extern char (*Mgetc)(unsigned short);       //pointer to function
extern void (*Mputc)(unsigned short, char); //pointer to function
extern short Mgetw(unsigned short);         //function
extern void  Mputw(unsigned short, short);  //function

// Init pointers to register bytes ///////////////////////////////////////////
void Init6809registerpointers()
{
 int i = 1;                //integer used to test endianness
 char *c = (char*)&i;      //left byte of integer i  

 PCHp = PCLp = (char*)&PC; //PC msb & lsb
 Ap   = Bp   = (char*)&D;  // D msb & lsb
 XHp  = XLp  = (char*)&X;  // X msb & lsb
 YHp  = YLp  = (char*)&Y;  // Y msb & lsb
 UHp  = ULp  = (char*)&U;  // U msb & lsb
 SHp  = SLp  = (char*)&S;  // S msb & lsb
 DPp  = DDp  = (char*)&DA; //DP msb & lsb
 WHp  = WLp  = (char*)&W;  // W msb & lsb

 switch(c[0]) //test endianness: 1=little-endian, 0=big-endian
 {
  case 1:  PCHp++; Ap++; XHp++; YHp++; UHp++; SHp++; DPp++; WHp++; break;
  default: PCLp++; Bp++; XLp++; YLp++; ULp++; SLp++; DDp++; WLp++; break;
 }                         
}
  
// Get memory (indexed) //////////////////////////////////////////////////////
void Mgeti()
{
 int i;
 short *r;
 i = Mgetc(PC++);
 switch (i & 0x60)
 {
  case 0x00: r = &X; break;
  case 0x20: r = &Y; break;
  case 0x40: r = &U; break;
  case 0x60: r = &S; break;
  default: r = &X;
 }
 switch(i &= 0x9f)
 {
  case 0x80: n = 2; W = *r; *r += 1; return;                    // ,R+       
  case 0x81: n = 3; W = *r; *r += 2; return;                    // ,R++      
  case 0x82: n = 2; *r -= 1; W = *r; return;                    // ,-R       
  case 0x83: n = 3; *r -= 2; W = *r; return;                    // ,--R      
  case 0x84: n = 0; W = *r; return;                             // ,R        
  case 0x85: n = 1; W = *r + B; return;                         // B,R       
  case 0x86: n = 1; W = *r + A; return;                         // A,R       
  case 0x87: n = 0; W = *r; return;                             // invalid       
  case 0x88: n = 1; W = *r + Mgetc(PC++); return;               // char,R    
  case 0x89: n = 4; EXT; W += *r; return;                       // word,R    
  case 0x8a: n = 0; W = *r; return;                             // invalid       
  case 0x8b: n = 4; W = *r + D; return;                         // D,R       
  case 0x8c: n = 1; W = Mgetc(PC++); W += PC; return;           // char,PCR  
  case 0x8d: n = 5; EXT; W += PC; return;                       // word,PCR  
  case 0x8e: n = 0; W = *r; return;                             // invalid       
  case 0x8f: n = 0; W = *r; return;                             // invalid       
  case 0x90: n = 3; W = Mgetw(*r); return;                      // invalid       
  case 0x91: n = 6; *r += 2; W = Mgetw(*r - 2); return;         // [,R++]    
  case 0x92: n = 3; W = Mgetw(*r); return;                      // invalid       
  case 0x93: n = 6; *r -= 2; W = Mgetw(*r); return;             // [,--R]    
  case 0x94: n = 3; W = Mgetw(*r); return;                      // [,R]      
  case 0x95: n = 4; W = Mgetw(*r + B); return;                  // [B,R]     
  case 0x96: n = 4; W = Mgetw(*r + A); return;                  // [A,R]     
  case 0x97: n = 3; W = Mgetw(*r); return;                      // invalid       
  case 0x98: n = 4; W = Mgetw(*r + Mgetc(PC++)); return;        // [char,R]  
  case 0x99: n = 7; EXT; W = Mgetw(*r + W); return;             // [word,R]  
  case 0x9a: n = 3; W = Mgetw(*r); return;                      // invalid       
  case 0x9b: n = 7; W = Mgetw(*r + D); return;                  // [D,R]     
  case 0x9c: n = 4; W = Mgetw(PC+1+Mgetc(PC)); PC++; return;    // [char,PCR]
  case 0x9d: n = 8; EXT; W = Mgetw(PC + W); return;             // [word,PCR]
  case 0x9e: n = 3; W = Mgetw(*r); return;                      // invalid       
  case 0x9f: n = 5; EXT; W = Mgetw(W); return;                  // [word]    
  default  : n = 1; if(i & 0x10) i -= 0x20; W = *r + i; return; // 5 bits,R  
  //Assumes 0x84 for invalid bytes 0x87 0x8a 0x8e 0x8f
  //Assumes 0x94 for invalid bytes 0x90 0x92 0x97 0x9a 0x9e
 }
}

// PSH, PUL, EXG, TFR /////////////////////////////////////////////////////////
void Pshs(char c)
{
 //Pshs(0xff) = 12 cycles   
 //Pshs(0xfe) = 11 cycles   
 //Pshs(0x80) = 2 cycles   
 if(c & 0x80) {Mputc(--S,PCL); Mputc(--S,PCH); n += 2;}
 if(c & 0x40) {Mputc(--S, UL); Mputc(--S, UH); n += 2;}
 if(c & 0x20) {Mputc(--S, YL); Mputc(--S, YH); n += 2;}
 if(c & 0x10) {Mputc(--S, XL); Mputc(--S, XH); n += 2;}
 if(c & 0x08) {Mputc(--S, DP); n += 1;}
 if(c & 0x04) {Mputc(--S,  B); n += 1;}
 if(c & 0x02) {Mputc(--S,  A); n += 1;}
 if(c & 0x01) {Mputc(--S, CC); n += 1;}
}

void Pshu(char c)
{
 if(c & 0x80) {Mputc(--U,PCL); Mputc(--U,PCH); n += 2;}
 if(c & 0x40) {Mputc(--U, SL); Mputc(--U, SH); n += 2;}
 if(c & 0x20) {Mputc(--U, YL); Mputc(--U, YH); n += 2;}
 if(c & 0x10) {Mputc(--U, XL); Mputc(--U, XH); n += 2;}
 if(c & 0x08) {Mputc(--U, DP); n += 1;}
 if(c & 0x04) {Mputc(--U,  B); n += 1;}
 if(c & 0x02) {Mputc(--U,  A); n += 1;}
 if(c & 0x01) {Mputc(--U, CC); n += 1;}
}

void Puls(char c)
{
 if(c & 0x01) {CC = Mgetc(S); S++; n += 1;}
 if(c & 0x02) { A = Mgetc(S); S++; n += 1;}
 if(c & 0x04) { B = Mgetc(S); S++; n += 1;}
 if(c & 0x08) {DP = Mgetc(S); S++; n += 1;}
 if(c & 0x10) {XH = Mgetc(S); S++; XL = Mgetc(S); S++; n += 2;}
 if(c & 0x20) {YH = Mgetc(S); S++; YL = Mgetc(S); S++; n += 2;}
 if(c & 0x40) {UH = Mgetc(S); S++; UL = Mgetc(S); S++; n += 2;}
 if(c & 0x80) {PCH= Mgetc(S); S++; PCL= Mgetc(S); S++; n += 2;}
}

void Pulu(char c)
{
 if(c & 0x01) {CC = Mgetc(U); U++; n += 1;}
 if(c & 0x02) { A = Mgetc(U); U++; n += 1;}
 if(c & 0x04) { B = Mgetc(U); U++; n += 1;}
 if(c & 0x08) {DP = Mgetc(U); U++; n += 1;}
 if(c & 0x10) {XH = Mgetc(U); U++; XL = Mgetc(U); U++; n += 2;}
 if(c & 0x20) {YH = Mgetc(U); U++; YL = Mgetc(U); U++; n += 2;}
 if(c & 0x40) {SH = Mgetc(U); U++; SL = Mgetc(U); U++; n += 2;}
 if(c & 0x80) {PCH= Mgetc(U); U++; PCL= Mgetc(U); U++; n += 2;}
}

void Exg(char c)
{
 switch(c & 0xff)
 {
  case 0x01: W = D; D = X; X = W; return;    //D-X
  case 0x02: W = D; D = Y; Y = W; return;    //D-Y
  case 0x03: W = D; D = U; U = W; return;    //D-U
  case 0x04: W = D; D = S; S = W; return;    //D-S
  case 0x05: W = D; D = PC; PC = W; return;  //D-PC
  case 0x10: W = X; X = D; D = W; return;    //X-D
  case 0x12: W = X; X = Y; Y = W; return;    //X-Y
  case 0x13: W = X; X = U; U = W; return;    //X-U
  case 0x14: W = X; X = S; S = W; return;    //X-S
  case 0x15: W = X; X = PC; PC = W; return;  //X-PC
  case 0x20: W = Y; Y = D; D = W; return;    //Y-D
  case 0x21: W = Y; Y = X; X = W; return;    //Y-X
  case 0x23: W = Y; Y = U; U = W; return;    //Y-U
  case 0x24: W = Y; Y = S; S = W; return;    //Y-S
  case 0x25: W = Y; Y = PC; PC = W; return;  //Y-PC
  case 0x30: W = U; U = D; D = W; return;    //U-D
  case 0x31: W = U; U = X; X = W; return;    //U-X
  case 0x32: W = U; U = Y; Y = W; return;    //U-Y
  case 0x34: W = U; U = S; S = W; return;    //U-S
  case 0x35: W = U; U = PC; PC = W; return;  //U-PC
  case 0x40: W = S; S = D; D = W; return; 
  case 0x41: W = S; S = X; X = W; return; 
  case 0x42: W = S; S = Y; Y = W; return; 
  case 0x43: W = S; S = U; U = W; return; 
  case 0x45: W = S; S = PC; PC = W; return; 
  case 0x50: W = PC; PC = D; D = W; return; 
  case 0x51: W = PC; PC = X; X = W; return; 
  case 0x52: W = PC; PC = Y; Y = W; return; 
  case 0x53: W = PC; PC = U; U = W; return; 
  case 0x54: W = PC; PC = S; S = W; return; 
  case 0x89: W = A; A = B; B = W; return; 
  case 0x8a: W = A; A = CC; CC = W; return; 
  case 0x8b: W = A; A = DP; DP = W; return; 
  case 0x98: W = B; B = A; A = W; return; 
  case 0x9a: W = B; B = CC; CC = W; return; 
  case 0x9b: W = B; B = DP; DP = W; return; 
  case 0xa8: W = CC; CC = A; A = W; return; 
  case 0xa9: W = CC; CC = B; B = W; return; 
  case 0xab: W = CC; CC = DP; DP = W; return; 
  case 0xb8: W = DP; DP = A; A = W; return; 
  case 0xb9: W = DP; DP = B; B = W; return; 
  case 0xba: W = DP; DP = CC; CC = W; return; 
 }
}

void Tfr(char c)
{
 switch(c & 0xff)
 {
  case 0x01: X = D; return; 
  case 0x02: Y = D; return; 
  case 0x03: U = D; return; 
  case 0x04: S = D; return; 
  case 0x05: PC = D; return; 
  case 0x10: D = X; return; 
  case 0x12: Y = X; return; 
  case 0x13: U = X; return; 
  case 0x14: S = X; return; 
  case 0x15: PC = X; return; 
  case 0x20: D = Y; return; 
  case 0x21: X = Y; return; 
  case 0x23: U = Y; return; 
  case 0x24: S = Y; return; 
  case 0x25: PC = Y; return; 
  case 0x30: D = U; return; 
  case 0x31: X = U; return; 
  case 0x32: Y = U; return; 
  case 0x34: S = U; return; 
  case 0x35: PC = U; return; 
  case 0x40: D = S; return; 
  case 0x41: X = S; return; 
  case 0x42: Y = S; return; 
  case 0x43: U = S; return; 
  case 0x45: PC = S; return; 
  case 0x50: D = PC; return; 
  case 0x51: X = PC; return; 
  case 0x52: Y = PC; return; 
  case 0x53: U = PC; return; 
  case 0x54: S = PC; return; 
  case 0x89: B = A; return; 
  case 0x8a: CC = A; return; 
  case 0x8b: DP = A; return; 
  case 0x98: A = B; return; 
  case 0x9a: CC = B; return; 
  case 0x9b: DP = B; return; 
  case 0xa8: A = CC; return; 
  case 0xa9: B = CC; return; 
  case 0xab: DP = CC; return; 
  case 0xb8: A = DP; return; 
  case 0xb9: B = DP; return; 
  case 0xba: CC = DP; return; 
 }
}

// CLR, NEG, COM, INC, DEC  (CC=EFHINZVC) /////////////////////////////////////
char Clr()
{
 CC &= 0xf0;
 CC |= Z1;
 return 0;
}

char Neg(char c)
{
 CC &= 0xf0;
 if(c == -128) CC |= V1;
 c = - c;
 if(c != 0) CC |= C1;
 if(c < 0) CC |= N1;
 if(c == 0) CC |= Z1; 
 return c;
}

char Com(char c)
{
 CC &= 0xf0;
 c = ~c;
 CC |= C1;
 if(c < 0) CC |= N1;
 if(c == 0) CC |= Z1;
 return c;
}

char Inc(char c)
{
 CC &= 0xf1;
 if(c == 127) CC |= V1;
 c++;
 if(c < 0) CC |= N1;
 if(c == 0) CC |= Z1;
 return c;
}

char Dec(char c)
{
 CC &= 0xf1;
 if(c == -128) CC |= V1;
 c--;
 if(c < 0) CC |= N1;
 if(c == 0) CC |= Z1;
 return c;
}

// Registers operations  (CC=EFHINZVC) ////////////////////////////////////////
void Mul()
{
 D = (A & 0xff) * (B & 0xff);
 CC &= 0xf2;
 if(D < 0) CC |= C1;
 if(D == 0) CC |= Z1;
}

void Addc(char *r, char c)
{
 int i = *r + c;
 CC &= 0xd0;
 if(((*r & 0x0f) + (c & 0x0f)) & 0x10) CC |= H1;
 if(((*r & 0xff) + (c & 0xff)) & 0x100) CC |= C1;
 *r = i & 0xff;
 if(*r != i) CC |= V1;
 if(*r < 0) CC |= N1;
 if(*r == 0) CC |= Z1;
}

void Adc(char *r, char c)
{
 int carry = (CC & C1);
 int i = *r + c + carry;
 CC &= 0xd0;
 if(((*r & 0x0f) + (c & 0x0f) + carry) & 0x10) CC |= H1;
 if(((*r & 0xff) + (c & 0xff) + carry) & 0x100) CC |= C1;
 *r = i & 0xff;
 if(*r != i) CC |= V1;
 if(*r < 0) CC |= N1;
 if(*r == 0) CC |= Z1;
}

void Addw(short *r, short w)
{
 int i = *r + w;
 CC &= 0xf0;
 if(((*r & 0xffff) + (w & 0xffff)) & 0xf0000) CC |= C1;
 *r = i & 0xffff;
 if(*r != i) CC |= V1;
 if(*r < 0) CC |= N1;
 if(*r == 0) CC |= Z1;
}

void Subc(char *r, char c)
{
 int i = *r - c;
 CC &= 0xf0;
 if(((*r & 0xff) - (c & 0xff)) & 0x100) CC |= C1;
 *r = i & 0xff;
 if(*r != i) CC |= V1;
 if(*r < 0) CC |= N1;
 if(*r == 0) CC |= Z1;
}

void Sbc(char *r, char c)
{
 int carry = (CC & C1);
 int i = *r - c - carry;
 CC &= 0xf0;
 if(((*r & 0xff) - (c & 0xff) - carry) & 0x100) CC |= C1;
 *r = i & 0xff;
 if(*r != i) CC |= V1;
 if(*r < 0) CC |= N1;
 if(*r == 0) CC |= Z1;
}

void Subw(short *r, short w)
{
 int i = *r - w;
 CC &= 0xf0;
 if(((*r & 0xffff) - (w & 0xffff)) & 0x10000) CC |= C1;
 *r = i & 0xffff;
 if(*r != i) CC |= V1;
 if(*r < 0) CC |= N1;
 if(*r == 0) CC |= Z1;
}

void Daa()
{
 int i = A & 0xff;
 if((CC & H1) || ((i & 0x00f) > 0x09)) i += 0x06;
 if((CC & C1) || ((i & 0x1f0) > 0x90)) i += 0x60;
 A = i & 0xff;
 i = (i >> 1 & 0xff) | (CC << 7);
 CC &= 0xf0;
 if(i & 0x80) CC |= C1;
 if((A ^ i) & 0x80) CC |= V1;
 if(A < 0) CC |= N1;
 if(A == 0) CC |= Z1;
}

// Shift and rotate  (CC=EFHINZVC) ////////////////////////////////////////////
char Lsr(char c)
{
 CC &= 0xf2;
 if(c & 1) CC |= C1;
 c = (c & 0xff) >> 1; 
 if(c == 0) CC |= Z1;
 return c;
}

char Ror(char c)
{
 int carry = CC & C1; 
 CC &= 0xf2;
 if(c & 1) CC |= C1;
 c = ((c & 0xff) >> 1) | (carry << 7);
 if(c < 0) CC |= N1;
 if(c == 0) CC |= Z1;
 return c;
}

char Rol(char c)
{
 int carry = CC & C1;
 CC &= 0xf0;
 if(c < 0) CC |= C1;
 c = ((c & 0x7f) << 1) | carry;
 if((c >> 7 & 1) ^ (CC & C1)) CC |= V1;
 if(c < 0) CC |= N1;
 if(c == 0) CC |= Z1;
 return c;
}

char Asr(char c)
{
 CC &= 0xf2;
 if(c & 1) CC |= C1;
 c = ((c & 0xff) >> 1) | (c & 0x80);
 if(c < 0) CC |= N1;
 if(c == 0) CC |= Z1;
 return c;
}

char Asl(char c)
{
 CC &= 0xf0;
 if(c < 0) CC |= C1;
 c = (c & 0xff) << 1;
 if((c >> 7 & 1) ^ (CC & C1)) CC |= V1;
 if(c < 0) CC |= N1;
 if(c == 0) CC |= Z1;
 return c;
}

// Test and compare  (CC=EFHINZVC) ////////////////////////////////////////////
void Tstc(char c)
{
 CC &= 0xf1;
 if(c < 0) CC |= N1;
 if(c == 0) CC |= Z1;
}

void Tstw(short w)
{
 CC &= 0xf1;
 if(w < 0) CC |= N1;
 if(w == 0) CC |= Z1;
}

void Cmpc(char *reg, char c)
{
 char r = *reg;
 int i = *reg - c;
 CC &= 0xf0;
 if(((r & 0xff) - (c & 0xff)) & 0x100) CC |= C1;
 r = i & 0xff;
 if(r != i) CC |= V1;
 if(r < 0) CC |= N1;
 if(r == 0) CC |= Z1;
}

void Cmpw(short *reg, short w)
{
 short r = *reg;
 int i = *reg - w;
 CC &= 0xf0;
 if(((r & 0xffff) - (w & 0xffff)) & 0x10000) CC |= C1;
 r = i & 0xffff;
 if(r != i) CC |= V1;
 if(r < 0) CC |= N1;
 if(r == 0) CC |= Z1;
}

// Interrupt requests  (CC=EFHINZVC) //////////////////////////////////////////
void Swi(int n)
{
 CC |= E1;
 Pshs(0xff);
 CC |= I1;
 if(n == 1) {PC = Mgetw(0xfffa); return;}
 if(n == 2) {PC = Mgetw(0xfff4); return;}
 if(n == 3) {PC = Mgetw(0xfff2); return;}
} 

void Irq()
{
 if((CC & I1) == 0)
 {CC |= E1; Pshs(0xff); CC |= I1; PC = Mgetw(0xfff8);}
}

void Firq()
{
 if((CC & F1) == 0)
 {CC &= ~E1; Pshs(0x81); CC |= F1; CC |= I1; PC = Mgetw(0xfff6);}
}

// RTI ////////////////////////////////////////////////////////////////////////
void Rti() {Puls(0x01); if(CC & E1) Puls(0xfe); else Puls(0x80);}

// Execute one operation at PC address and set PC to next opcode address //////
int Run6809()
/*
Return value is set to :
- cycle count for the executed instruction when operation code is legal
- negative value (-code) when operation code is illegal
*/
{
 int precode, code;
 n = 0; //par defaut pas de cycles supplementaires
 precode = 0; //par defaut pas de precode
 while(1)
 {   
  code = Mgetc(PC++) & 0xff;
  if(code == 0x10) {precode = 0x1000; continue;}           
  if(code == 0x11) {precode = 0x1100; continue;}           
  code |= precode; break;
 }

 switch(code)
 {
  case 0x00: DIR; Mputc(DA, Neg(Mgetc(DA))); return 6;      /* NEG  /$ */
  case 0x01: DIR; return 3;                            /*undoc BRN     */
//case 0x02: 
// if(CC&C1){Mputc(wd, Com(Mgetc(DA))); return 6;}     /*undoc COM  /$ */
//      else{Mputc(DA, Neg(Mgetc(DA))); return 6;}     /*undoc NEG  /$ */
  case 0x03: DIR; Mputc(DA, Com(Mgetc(DA))); return 6;      /* COM  /$ */
  case 0x04: DIR; Mputc(DA, Lsr(Mgetc(DA))); return 6;      /* LSR  /$ */
//case 0x05: Mputc(DA, Lsr(Mgetc(DA))); return 6;      /*undoc LSR  /$ */
  case 0x06: DIR; Mputc(DA, Ror(Mgetc(DA))); return 6;      /* ROR  /$ */
  case 0x07: DIR; Mputc(DA, Asr(Mgetc(DA))); return 6;      /* ASR  /$ */
  case 0x08: DIR; Mputc(DA, Asl(Mgetc(DA))); return 6;      /* ASL  /$ */
  case 0x09: DIR; Mputc(DA, Rol(Mgetc(DA))); return 6;      /* ROL  /$ */
  case 0x0a: DIR; Mputc(DA, Dec(Mgetc(DA))); return 6;      /* DEC  /$ */
  case 0x0c: DIR; Mputc(DA, Inc(Mgetc(DA))); return 6;      /* INC  /$ */
  case 0x0d: DIR; Tstc(Mgetc(DA)); return 6;                /* TST  /$ */
  case 0x0e: DIR; PC = DA; return 3;                        /* JMP  /$ */
  case 0x0f: DIR; Mputc(DA, Clr()); return 6;               /* CLR  /$ */

  case 0x12: return 2;                                      /* NOP     */
  case 0x13: return 4;                                      /* SYNC    */
  case 0x16: PC += Mgetw(PC) + 2; return 5;                 /* LBRA    */
  case 0x17: EXT; Pshs(0x80); PC += W; return 9;            /* LBSR    */
  case 0x19: Daa(); return 2;                               /* DAA     */
  case 0x1a: CC |= Mgetc(PC++); return 3;                   /* ORCC #$ */
  case 0x1c: CC &= Mgetc(PC++); return 3;                   /* ANDC #$ */
  case 0x1d: Tstw(D = B); return 2;                         /* SEX     */
  case 0x1e: Exg(Mgetc(PC++)); return 8;                    /* EXG     */
  case 0x1f: Tfr(Mgetc(PC++)); return 6;                    /* TFR     */

  case 0x20: BRANCH; PC++; return 3;                        /* BRA     */
  case 0x21: PC++; return 3;                                /* BRN     */
  case 0x22: if(BHI) BRANCH; PC++; return 3;                /* BHI     */
  case 0x23: if(BLS) BRANCH; PC++; return 3;                /* BLS     */
  case 0x24: if(BCC) BRANCH; PC++; return 3;                /* BCC     */
  case 0x25: if(BCS) BRANCH; PC++; return 3;                /* BCS     */
  case 0x26: if(BNE) BRANCH; PC++; return 3;                /* BNE     */
  case 0x27: if(BEQ) BRANCH; PC++; return 3;                /* BEQ     */
  case 0x28: if(BVC) BRANCH; PC++; return 3;                /* BVC     */
  case 0x29: if(BVS) BRANCH; PC++; return 3;                /* BVS     */
  case 0x2a: if(BPL) BRANCH; PC++; return 3;                /* BPL     */
  case 0x2b: if(BMI) BRANCH; PC++; return 3;                /* BMI     */
  case 0x2c: if(BGE) BRANCH; PC++; return 3;                /* BGE     */
  case 0x2d: if(BLT) BRANCH; PC++; return 3;                /* BLT     */
  case 0x2e: if(BGT) BRANCH; PC++; return 3;                /* BGT     */
  case 0x2f: if(BLE) BRANCH; PC++; return 3;                /* BLE     */

  case 0x30: IND; X = W; SETZERO; return 4 + n;           /* LEAX    */
  case 0x31: IND; Y = W; SETZERO; return 4 + n;           /* LEAY    */
  //d'apres Prehisto, LEAX et LEAY positionnent aussi le bit N de CC
  //il faut donc modifier l'émulation de ces deux instructions
  case 0x32: IND; S = W; return 4 + n; /*CC not set*/       /* LEAS    */
  case 0x33: IND; U = W; return 4 + n; /*CC not set*/       /* LEAU    */
  case 0x34: Pshs(Mgetc(PC++)); return 5 + n;               /* PSHS    */
  case 0x35: Puls(Mgetc(PC++)); return 5 + n;               /* PULS    */
  case 0x36: Pshu(Mgetc(PC++)); return 5 + n;               /* PSHU    */
  case 0x37: Pulu(Mgetc(PC++)); return 5 + n;               /* PULU    */
  case 0x39: Puls(0x80); return 5;                          /* RTS     */
  case 0x3a: X += B & 0xff; return 3;                       /* ABX     */
  case 0x3b: Rti(); return 4 + n;                           /* RTI     */
  case 0x3c: CC &= Mgetc(PC++); CC |= E1; return 20;        /* CWAI    */
  case 0x3d: Mul(); return 11;                              /* MUL     */
  case 0x3f: Swi(1); return 19;                             /* SWI     */

  case 0x40: A = Neg(A); return 2;                          /* NEGA    */
  case 0x43: A = Com(A); return 2;                          /* COMA    */
  case 0x44: A = Lsr(A); return 2;                          /* LSRA    */
  case 0x46: A = Ror(A); return 2;                          /* RORA    */
  case 0x47: A = Asr(A); return 2;                          /* ASRA    */
  case 0x48: A = Asl(A); return 2;                          /* ASLA    */
  case 0x49: A = Rol(A); return 2;                          /* ROLA    */
  case 0x4a: A = Dec(A); return 2;                          /* DECA    */
  case 0x4c: A = Inc(A); return 2;                          /* INCA    */
  case 0x4d: Tstc(A); return 2;                             /* TSTA    */
  case 0x4f: A = Clr(); return 2;                           /* CLRA    */

  case 0x50: B = Neg(B); return 2;                          /* NEGB    */
  case 0x53: B = Com(B); return 2;                          /* COMB    */
  case 0x54: B = Lsr(B); return 2;                          /* LSRB    */
  case 0x56: B = Ror(B); return 2;                          /* RORB    */
  case 0x57: B = Asr(B); return 2;                          /* ASRB    */
  case 0x58: B = Asl(B); return 2;                          /* ASLB    */
  case 0x59: B = Rol(B); return 2;                          /* ROLB    */
  case 0x5a: B = Dec(B); return 2;                          /* DECB    */
  case 0x5c: B = Inc(B); return 2;                          /* INCB    */
  case 0x5d: Tstc(B); return 2;                             /* TSTB    */
  case 0x5f: B = Clr(); return 2;                           /* CLRB    */

  case 0x60: IND; Mputc(W, Neg(Mgetc(W))); return 6 + n;    /* NEG  IX */
  case 0x63: IND; Mputc(W, Com(Mgetc(W))); return 6 + n;    /* COM  IX */
  case 0x64: IND; Mputc(W, Lsr(Mgetc(W))); return 6 + n;    /* LSR  IX */
  case 0x66: IND; Mputc(W, Ror(Mgetc(W))); return 6 + n;    /* ROR  IX */
  case 0x67: IND; Mputc(W, Asr(Mgetc(W))); return 6 + n;    /* ASR  IX */
  case 0x68: IND; Mputc(W, Asl(Mgetc(W))); return 6 + n;    /* ASL  IX */
  case 0x69: IND; Mputc(W, Rol(Mgetc(W))); return 6 + n;    /* ROL  IX */
  case 0x6a: IND; Mputc(W, Dec(Mgetc(W))); return 6 + n;    /* DEC  IX */
  case 0x6c: IND; Mputc(W, Inc(Mgetc(W))); return 6 + n;    /* INC  IX */
  case 0x6d: IND; Tstc(Mgetc(W)); return 6 + n;             /* TST  IX */
  case 0x6e: IND; PC = W; return 3 + n;                     /* JMP  IX */
  case 0x6f: IND; Mputc(W, Clr()); return 6 + n;            /* CLR  IX */

  case 0x70: EXT; Mputc(W, Neg(Mgetc(W))); return 7;        /* NEG  $  */
  case 0x73: EXT; Mputc(W, Com(Mgetc(W))); return 7;        /* COM  $  */
  case 0x74: EXT; Mputc(W, Lsr(Mgetc(W))); return 7;        /* LSR  $  */
  case 0x76: EXT; Mputc(W, Ror(Mgetc(W))); return 7;        /* ROR  $  */
  case 0x77: EXT; Mputc(W, Asr(Mgetc(W))); return 7;        /* ASR  $  */
  case 0x78: EXT; Mputc(W, Asl(Mgetc(W))); return 7;        /* ASL  $  */
  case 0x79: EXT; Mputc(W, Rol(Mgetc(W))); return 7;        /* ROL  $  */
  case 0x7a: EXT; Mputc(W, Dec(Mgetc(W))); return 7;        /* DEC  $  */
  case 0x7c: EXT; Mputc(W, Inc(Mgetc(W))); return 7;        /* INC  $  */
  case 0x7d: EXT; Tstc(Mgetc(W)); return 7;                 /* TST  $  */
  case 0x7e: EXT; PC = W; return 4;                         /* JMP  $  */
  case 0x7f: EXT; Mputc(W, Clr()); return 7;                /* CLR  $  */

  case 0x80: Subc(&A, Mgetc(PC++)); return 2;               /* SUBA #$ */
  case 0x81: Cmpc(&A, Mgetc(PC++)); return 2;               /* CMPA #$ */
  case 0x82: Sbc(&A, Mgetc(PC++)); return 2;                /* SBCA #$ */
  case 0x83: EXT; Subw(&D, W); return 4;                    /* SUBD #$ */
  case 0x84: Tstc(A &= Mgetc(PC++)); return 2;              /* ANDA #$ */
  case 0x85: Tstc(A & Mgetc(PC++)); return 2;               /* BITA #$ */
  case 0x86: Tstc(A = Mgetc(PC++)); return 2;               /* LDA  #$ */
  case 0x88: Tstc(A ^= Mgetc(PC++)); return 2;              /* EORA #$ */
  case 0x89: Adc(&A, Mgetc(PC++)); return 2;                /* ADCA #$ */
  case 0x8a: Tstc(A |= Mgetc(PC++)); return 2;              /* ORA  #$ */
  case 0x8b: Addc(&A, Mgetc(PC++)); return 2;               /* ADDA #$ */
  case 0x8c: EXT; Cmpw(&X, W); return 4;                    /* CMPX #$ */
  case 0x8d: DIR; Pshs(0x80); PC += DD; return 7;           /* BSR     */
  case 0x8e: EXT; Tstw(X = W); return 3;                    /* LDX  #$ */

  case 0x90: DIR; Subc(&A, Mgetc(DA)); return 4;            /* SUBA /$ */
  case 0x91: DIR; Cmpc(&A, Mgetc(DA)); return 4;            /* CMPA /$ */
  case 0x92: DIR; Sbc(&A, Mgetc(DA)); return 4;             /* SBCA /$ */
  case 0x93: DIR; Subw(&D, Mgetw(DA));return 6;             /* SUBD /$ */
  case 0x94: DIR; Tstc(A &= Mgetc(DA)); return 4;           /* ANDA /$ */
  case 0x95: DIR; Tstc(A & Mgetc(DA)); return 4;            /* BITA /$ */
  case 0x96: DIR; Tstc(A = Mgetc(DA)); return 4;            /* LDA  /$ */
  case 0x97: DIR; Mputc(DA, A); Tstc(A); return 4;          /* STA  /$ */
  case 0x98: DIR; Tstc(A ^= Mgetc(DA)); return 4;           /* EORA /$ */
  case 0x99: DIR; Adc(&A, Mgetc(DA)); return 4;             /* ADCA /$ */
  case 0x9a: DIR; Tstc(A |= Mgetc(DA)); return 4;           /* ORA  /$ */
  case 0x9b: DIR; Addc(&A, Mgetc(DA)); return 4;            /* ADDA /$ */
  case 0x9c: DIR; Cmpw(&X, Mgetw(DA)); return 6;            /* CMPX /$ */
  case 0x9d: DIR; Pshs(0x80); PC = DA; return 7;            /* JSR  /$ */
  case 0x9e: DIR; Tstw(X = Mgetw(DA)); return 5;            /* LDX  /$ */
  case 0x9f: DIR; Mputw(DA, X); Tstw(X); return 5;          /* STX  /$ */

  case 0xa0: IND; Subc(&A, Mgetc(W)); return 4 + n;         /* SUBA IX */
  case 0xa1: IND; Cmpc(&A, Mgetc(W)); return 4 + n;         /* CMPA IX */
  case 0xa2: IND; Sbc(&A, Mgetc(W)); return 4 + n;          /* SBCA IX */
  case 0xa3: IND; Subw(&D, Mgetw(W)); return 6 + n;         /* SUBD IX */
  case 0xa4: IND; Tstc(A &= Mgetc(W)); return 4 + n;        /* ANDA IX */
  case 0xa5: IND; Tstc(Mgetc(W) & A); return 4 + n;         /* BITA IX */
  case 0xa6: IND; Tstc(A = Mgetc(W)); return 4 + n;         /* LDA  IX */
  case 0xa7: IND; Mputc(W,A); Tstc(A); return 4 + n;        /* STA  IX */
  case 0xa8: IND; Tstc(A ^= Mgetc(W)); return 4 + n;        /* EORA IX */
  case 0xa9: IND; Adc(&A, Mgetc(W)); return 4 + n;          /* ADCA IX */
  case 0xaa: IND; Tstc(A |= Mgetc(W)); return 4 + n;        /* ORA  IX */
  case 0xab: IND; Addc(&A, Mgetc(W)); return 4 + n;         /* ADDA IX */
  case 0xac: IND; Cmpw(&X, Mgetw(W)); return 4 + n;         /* CMPX IX */
  case 0xad: IND; Pshs(0x80); PC = W; return 5 + n;         /* JSR  IX */
  case 0xae: IND; Tstw(X = Mgetw(W)); return 5 + n;         /* LDX  IX */
  case 0xaf: IND; Mputw(W, X); Tstw(X); return 5 + n;       /* STX  IX */

  case 0xb0: EXT; Subc(&A, Mgetc(W)); return 5;             /* SUBA $  */
  case 0xb1: EXT; Cmpc(&A, Mgetc(W)); return 5;             /* CMPA $  */
  case 0xb2: EXT; Sbc(&A, Mgetc(W)); return 5;              /* SBCA $  */
  case 0xb3: EXT; Subw(&D, Mgetw(W)); return 7;             /* SUBD $  */
  case 0xb4: EXT; Tstc(A &= Mgetc(W)); return 5;            /* ANDA $  */
  case 0xb5: EXT; Tstc(A & Mgetc(W)); return 5;             /* BITA $  */
  case 0xb6: EXT; Tstc(A = Mgetc(W)); return 5;             /* LDA  $  */
  case 0xb7: EXT; Mputc(W, A); Tstc(A); return 5;           /* STA  $  */
  case 0xb8: EXT; Tstc(A ^= Mgetc(W)); return 5;            /* EORA $  */
  case 0xb9: EXT; Adc(&A, Mgetc(W)); return 5;              /* ADCA $  */
  case 0xba: EXT; Tstc(A |= Mgetc(W)); return 5;            /* ORA  $  */
  case 0xbb: EXT; Addc(&A, Mgetc(W)); return 5;             /* ADDA $  */
  case 0xbc: EXT; Cmpw(&X, Mgetw(W)); return 7;             /* CMPX $  */
  case 0xbd: EXT; Pshs(0x80); PC = W; return 8;             /* JSR  $  */
  case 0xbe: EXT; Tstw(X = Mgetw(W)); return 6;             /* LDX  $  */
  case 0xbf: EXT; Mputw(W, X); Tstw(X); return 6;           /* STX  $  */

  case 0xc0: Subc(&B, Mgetc(PC++)); return 2;               /* SUBB #$ */
  case 0xc1: Cmpc(&B, Mgetc(PC++)); return 2;               /* CMPB #$ */
  case 0xc2: Sbc(&B, Mgetc(PC++)); return 2;                /* SBCB #$ */
  case 0xc3: EXT; Addw(&D, W); return 4;                    /* ADDD #$ */
  case 0xc4: Tstc(B &= Mgetc(PC++)); return 2;              /* ANDB #$ */
  case 0xc5: Tstc(B & Mgetc(PC++)); return 2;               /* BITB #$ */
  case 0xc6: Tstc(B = Mgetc(PC++)); return 2;               /* LDB  #$ */
  case 0xc8: Tstc(B ^= Mgetc(PC++)); return 2;              /* EORB #$ */
  case 0xc9: Adc(&B, Mgetc(PC++)); return 2;                /* ADCB #$ */
  case 0xca: Tstc(B |= Mgetc(PC++)); return 2;              /* ORB  #$ */
  case 0xcb: Addc(&B, Mgetc(PC++));return 2;                /* ADDB #$ */
  case 0xcc: EXT; Tstw(D = W); return 3;                    /* LDD  #$ */
  case 0xce: EXT; Tstw(U = W); return 3;                    /* LDU  #$ */

  case 0xd0: DIR; Subc(&B, Mgetc(DA)); return 4;            /* SUBB /$ */
  case 0xd1: DIR; Cmpc(&B, Mgetc(DA)); return 4;            /* CMPB /$ */
  case 0xd2: DIR; Sbc(&B, Mgetc(DA)); return 4;             /* SBCB /$ */
  case 0xd3: DIR; Addw(&D, Mgetw(DA)); return 6;            /* ADDD /$ */
  case 0xd4: DIR; Tstc(B &= Mgetc(DA)); return 4;           /* ANDB /$ */
  case 0xd5: DIR; Tstc(Mgetc(DA) & B); return 4;            /* BITB /$ */
  case 0xd6: DIR; Tstc(B = Mgetc(DA)); return 4;            /* LDB  /$ */
  case 0xd7: DIR; Mputc(DA,B); Tstc(B); return 4;           /* STB  /$ */
  case 0xd8: DIR; Tstc(B ^= Mgetc(DA)); return 4;           /* EORB /$ */
  case 0xd9: DIR; Adc(&B, Mgetc(DA)); return 4;             /* ADCB /$ */
  case 0xda: DIR; Tstc(B |= Mgetc(DA)); return 4;           /* ORB  /$ */
  case 0xdb: DIR; Addc(&B, Mgetc(DA)); return 4;            /* ADDB /$ */
  case 0xdc: DIR; Tstw(D = Mgetw(DA)); return 5;            /* LDD  /$ */
  case 0xdd: DIR; Mputw(DA, D); Tstw(D); return 5;          /* STD  /$ */
  case 0xde: DIR; Tstw(U = Mgetw(DA)); return 5;            /* LDU  /$ */
  case 0xdf: DIR; Mputw(DA, U); Tstw(U); return 5;          /* STU  /$ */

  case 0xe0: IND; Subc(&B, Mgetc(W)); return 4 + n;         /* SUBB IX */
  case 0xe1: IND; Cmpc(&B, Mgetc(W)); return 4 + n;         /* CMPB IX */
  case 0xe2: IND; Sbc(&B, Mgetc(W)); return 4 + n;          /* SBCB IX */
  case 0xe3: IND; Addw(&D, Mgetw(W)); return 6 + n;         /* ADDD IX */
  case 0xe4: IND; Tstc(B &= Mgetc(W)); return 4 + n;        /* ANDB IX */
  case 0xe5: IND; Tstc(Mgetc(W) & B); return 4 + n;         /* BITB IX */
  case 0xe6: IND; Tstc(B = Mgetc(W)); return 4 + n;         /* LDB  IX */
  case 0xe7: IND; Mputc(W, B); Tstc(B); return 4 + n;       /* STB  IX */
  case 0xe8: IND; Tstc(B ^= Mgetc(W)); return 4 + n;        /* EORB IX */
  case 0xe9: IND; Adc(&B, Mgetc(W)); return 4 + n;          /* ADCB IX */
  case 0xea: IND; Tstc(B |= Mgetc(W)); return 4 + n;        /* ORB  IX */
  case 0xeb: IND; Addc(&B, Mgetc(W)); return 4 + n;         /* ADDB IX */
  case 0xec: IND; Tstw(D = Mgetw(W)); return 5 + n;         /* LDD  IX */
  case 0xed: IND; Mputw(W, D); Tstw(D); return 5 + n;       /* STD  IX */
  case 0xee: IND; Tstw(U = Mgetw(W)); return 5 + n;         /* LDU  IX */
  case 0xef: IND; Mputw(W, U); Tstw(U); return 5 + n;       /* STU  IX */

  case 0xf0: EXT; Subc(&B, Mgetc(W)); return 5;             /* SUBB $  */
  case 0xf1: EXT; Cmpc(&B, Mgetc(W)); return 5;             /* CMPB $  */
  case 0xf2: EXT; Sbc(&B, Mgetc(W)); return 5;              /* SBCB $  */
  case 0xf3: EXT; Addw(&D, Mgetw(W)); return 7;             /* ADDD $  */
  case 0xf4: EXT; Tstc(B &= Mgetc(W)); return 5;            /* ANDB $  */
  case 0xf5: EXT; Tstc(B & Mgetc(W)); return 5;             /* BITB $  */
  case 0xf6: EXT; Tstc(B = Mgetc(W)); return 5;             /* LDB  $  */
  case 0xf7: EXT; Mputc(W, B); Tstc(B); return 5;           /* STB  $  */
  case 0xf8: EXT; Tstc(B ^= Mgetc(W)); return 5;            /* EORB $  */
  case 0xf9: EXT; Adc(&B, Mgetc(W)); return 5;              /* ADCB $  */
  case 0xfa: EXT; Tstc(B |= Mgetc(W)); return 5;            /* ORB  $  */
  case 0xfb: EXT; Addc(&B, Mgetc(W)); return 5;             /* ADDB $  */
  case 0xfc: EXT; Tstw(D = Mgetw(W)); return 6;             /* LDD  $  */
  case 0xfd: EXT; Mputw(W, D); Tstw(D); return 6;           /* STD  $  */
  case 0xfe: EXT; Tstw(U = Mgetw(W)); return 6;             /* LDU  $  */
  case 0xff: EXT; Mputw(W, U); Tstw(U); return 6;           /* STU  $  */

  case 0x1021: PC += 2; return 5;                           /* LBRN    */
  case 0x1022: if(BHI) LBRANCH; PC += 2; return 5 + n;      /* LBHI    */
  case 0x1023: if(BLS) LBRANCH; PC += 2; return 5 + n;      /* LBLS    */
  case 0x1024: if(BCC) LBRANCH; PC += 2; return 5 + n;      /* LBCC    */
  case 0x1025: if(BCS) LBRANCH; PC += 2; return 5 + n;      /* LBCS    */
  case 0x1026: if(BNE) LBRANCH; PC += 2; return 5 + n;      /* LBNE    */
  case 0x1027: if(BEQ) LBRANCH; PC += 2; return 5 + n;      /* LBEQ    */
  case 0x1028: if(BVC) LBRANCH; PC += 2; return 5 + n;      /* LBVC    */
  case 0x1029: if(BVS) LBRANCH; PC += 2; return 5 + n;      /* LBVS    */
  case 0x102a: if(BPL) LBRANCH; PC += 2; return 5 + n;      /* LBPL    */
  case 0x102b: if(BMI) LBRANCH; PC += 2; return 5 + n;      /* LBMI    */
  case 0x102c: if(BGE) LBRANCH; PC += 2; return 5 + n;      /* LBGE    */
  case 0x102d: if(BLT) LBRANCH; PC += 2; return 5 + n;      /* LBLT    */
  case 0x102e: if(BGT) LBRANCH; PC += 2; return 5 + n;      /* LBGT    */
  case 0x102f: if(BLE) LBRANCH; PC += 2; return 5 + n;      /* LBLE    */
  case 0x103f: Swi(2); return 20;                           /* SWI2    */

  case 0x1083: EXT; Cmpw(&D, W); return 5;                  /* CMPD #$ */
  case 0x108c: EXT; Cmpw(&Y, W); return 5;                  /* CMPY #$ */
  case 0x108e: EXT; Tstw(Y = W); return 4;                  /* LDY  #$ */
  case 0x1093: DIR; Cmpw(&D, Mgetw(DA)); return 7;          /* CMPD /$ */
  case 0x109c: DIR; Cmpw(&Y, Mgetw(DA)); return 7;          /* CMPY /$ */
  case 0x109e: DIR; Tstw(Y = Mgetw(DA)); return 6;          /* LDY  /$ */
  case 0x109f: DIR; Mputw(DA, Y); Tstw(Y); return 6;        /* STY  /$ */
  case 0x10a3: IND; Cmpw(&D, Mgetw(W)); return 7 + n;       /* CMPD IX */
  case 0x10ac: IND; Cmpw(&Y, Mgetw(W)); return 7 + n;       /* CMPY IX */
  case 0x10ae: IND; Tstw(Y = Mgetw(W)); return 6 + n;       /* LDY  IX */
  case 0x10af: IND; Mputw(W, Y); Tstw(Y); return 6 + n;     /* STY  IX */
  case 0x10b3: EXT; Cmpw(&D, Mgetw(W)); return 8;           /* CMPD $  */
  case 0x10bc: EXT; Cmpw(&Y, Mgetw(W)); return 8;           /* CMPY $  */
  case 0x10be: EXT; Tstw(Y = Mgetw(W)); return 7;           /* LDY  $  */
  case 0x10bf: EXT; Mputw(W, Y); Tstw(Y); return 7;         /* STY  $  */
  case 0x10ce: EXT; Tstw(S = W); return 4;                  /* LDS  #$ */
  case 0x10de: DIR; Tstw(S = Mgetw(DA)); return 6;          /* LDS  /$ */
  case 0x10df: DIR; Mputw(DA, S); Tstw(S); return 6;        /* STS  /$ */
  case 0x10ee: IND; Tstw(S = Mgetw(W)); return 6 + n;       /* LDS  IX */
  case 0x10ef: IND; Mputw(W, S); Tstw(S); return 6 + n;     /* STS  IX */
  case 0x10fe: EXT; Tstw(S = Mgetw(W)); return 7;           /* LDS  $  */
  case 0x10ff: EXT; Mputw(W, S); Tstw(S); return 7;         /* STS  $  */

  case 0x113f: Swi(3); return 20;                           /* SWI3    */
  case 0x1183: EXT; Cmpw(&U, W); return 5;                  /* CMPU #$ */
  case 0x118c: EXT; Cmpw(&S, W); return 5;                  /* CMPS #$ */
  case 0x1193: DIR; Cmpw(&U, Mgetw(DA)); return 7;          /* CMPU /$ */
  case 0x119c: DIR; Cmpw(&S, Mgetw(DA)); return 7;          /* CMPS /$ */
  case 0x11a3: IND; Cmpw(&U, Mgetw(W)); return 7 + n;       /* CMPU IX */
  case 0x11ac: IND; Cmpw(&S, Mgetw(W)); return 7 + n;       /* CMPS IX */
  case 0x11b3: EXT; Cmpw(&U, Mgetw(W)); return 8;           /* CMPU $  */
  case 0x11bc: EXT; Cmpw(&S, Mgetw(W)); return 8;           /* CMPS $  */

  default: return -code;                                    /* Illegal */
 } 
}
