//////////////////////////////////////////////////////////////////////////////
// DCMO5BOUTONS.H - definition des boutons
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcmo5.free.fr
// Creation : Novembre 2007
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
//////////////////////////////////////////////////////////////////////////////
 
button bouton[OTHERBUTTON_MAX] = {
{"[haut]",   0, 0,  22, 22, 58}, //0x00 
{"[gauche]", 0, 0,  22, 22, 59}, //0x01 
{"[droite]", 0, 0,  22, 22, 60}, //0x02 
{"[bas]",    0, 0,  22, 22, 61}, //0x03 
{"Action",   0, 0,  44, 18, 62}, //0x04 
{"[X]",      0, 0,  16, 15, 63}, //0x05
{"[prev]",   0, 0,  16,  8, 64}, //0x06
{"[next]",   0, 0,  16,  8, 65}, //0x07
{"Options",  0, 0,  50, 16, 66}, //0x08
{"?",        0, 0,  15, 16, 67}, //0x09
{" ",        0, 0, 214, 22, 68}, //0x0a
{"k7",       0, 0,  22, 16, 69}, //0x0b
{"fd",       0, 0,  22, 16, 70}, //0x0c
{"memo",     0, 0,  44, 16, 71}  //0x0d
};

int flechegauche[16] = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                        0x0000a000, 0x0000aa00, 0x00000aa0, 0x05555555,
                        0x00000aa0, 0x0000aa00, 0x0000a000, 0x00000000,
                        0x00000000, 0x00000000, 0x00000000, 0x00000000};
int flechedroite[16] = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                        0x00028000, 0x002a8000, 0x02a80000, 0x15555554,
                        0x02a80000, 0x002a8000, 0x00028000, 0x00000000,
                        0x00000000, 0x00000000, 0x00000000, 0x00000000};
int flechehaut[16]   = {0x00004000, 0x00004000, 0x00026000, 0x00026000,
                        0x000a6800, 0x000a6800, 0x00284a00, 0x00284a00,
                        0x00004000, 0x00004000, 0x00004000, 0x00004000,
                        0x00004000, 0x00004000, 0x00004000, 0x00000000};
int flechebas[16]    = {0x00000000, 0x00004000, 0x00004000, 0x00004000,
                        0x00004000, 0x00004000, 0x00004000, 0x00004000,
                        0x00284a00, 0x00284a00, 0x000a6800, 0x000a6800,
                        0x00026000, 0x00026000, 0x00004000, 0x00004000};
int flecheretour[16] = {0x00000000, 0x0000a000, 0x0000aa00, 0x00000aa0,
                        0x05555555, 0x04000aa0, 0x0400aa00, 0x0400a000,
                        0x04000000, 0x04000000, 0x04000000, 0x04000000,
                        0x05555554, 0x00000000, 0x00000000, 0x00000000};
int boutoncroix[8]   = {0x00000000, 0x00005005, 0x00001414, 0x00000550,
                        0x00000140, 0x00000550, 0x00001414, 0x00005005};
int boutonprev[2]    = {0x00000040, 0x00000150};
int boutonnext[2]    = {0x00000150, 0x00000040};
