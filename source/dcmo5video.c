///////////////////////////////////////////////////////////////////////////////
// DCMO5VIDEO.C - Fonctions d'affichage pour dcmo5
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcmo5.free.fr
// Created  : July 2006
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
///////////////////////////////////////////////////////////////////////////////

#include <SDL.h>
#include <string.h>

#define VIDEO_MODE SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE
// | SDL_WINDOW_MINIMIZED 

// global variables //////////////////////////////////////////////////////////
SDL_Surface *screen = NULL;    //surface d'affichage de l'ecran
int xbitmap = 336;             //largeur ecran = 320 + 2 bordures de 8
int ybitmap = 216;             //hauteur ecran = 200 + 2 bordures de 8
int ystatus = 20;              //hauteur status bar
int xclient;                   //largeur fenetre utilisateur 
int yclient;                   //hauteur ecran MO5 dans fenetre utilisateur
int xmouse;                    //abscisse souris dans fenetre utilisateur
int ymouse;                    //ordonnée souris dans fenetre utilisateur
int vblcount, framecount;      //compteur d'affichage 
struct pix {char r, g, b, a;}; //structure pixel RGBA
struct pix pcolor[19][8];      //couleurs RGBA de la palette (pour 8 pixels)
int *p, *pmin, *pmax;          //pointeurs dans le bitmap
int screenlocked;              //indicateur de surface bloquee
int framedelay;                //nombre de VBL entre deux affichages de l'ecran

extern void Drawstatusbar();
extern void SDL_error(int n);

//Initialisation palette /////////////////////////////////////////////////////
void Initpalette()
{
 int i, j;
 //palette
 // 0 noir  1 rouge  2 vert   3 jaune   4 bleu   5 magenta   6 cyan   7 blanc
 // 8 gris  9 rose  10 vert  11 jaune  12 bleu  13 magenta  14 cyan  15 orange
 int r[19]={0,15,0 ,15,2 ,15,0 ,15,10,15,10,15,2 ,15,10,15,   11,14,2};
 int v[19]={0,2 ,15,15,2 ,0 ,15,15,10,10,15,15,10,10,15,10,   11,14,2};
 int b[19]={0,2 ,0 ,0 ,15,15,15,15,10,15,10,10,15,10,15,2 ,   11,14,2};
 //definition des intensites pour correction gamma
 int g[16]={0,  60, 90, 110,130,
	        148,165,180,193,205,
			215,225,230,235,240,
			255};
 //calcul de la palette 
 for(i = 0; i < 19; i++) for(j = 0; j < 8; j++) 
 {
  pcolor[i][j].r = g[b[i]];
  pcolor[i][j].g = g[v[i]];
  pcolor[i][j].b = g[r[i]];
  pcolor[i][j].a = 0xff;
 }
}

//Display screen /////////////////////////////////////////////////////////////
void Displayscreen()
{
 extern SDL_Surface *dialogbox;
 extern SDL_Surface *statusbar;
 extern SDL_Rect dialogrect;  
 extern int dialog;
 if(screenlocked) return;
 if(dialog > 0)
 if(SDL_BlitSurface(dialogbox, NULL, screen, &dialogrect) < 0) SDL_error(31);
 framecount += framedelay;
 if(framecount >= 10)  //10trames=0.2seconde
 {
  if(statusbar != NULL)
  if(SDL_BlitSurface(statusbar, NULL, screen, NULL) < 0) SDL_error(32);
  framecount = 0;
 }  
 SDL_Flip(screen);
}     

// Composition ligne ecran MO5 ///////////////////////////////////////////////
void ComposeMO5line(int a)
{
 int i, j, k, x, xmax, forme;
 void *c[2], *col;
 extern char ram[];
 x = 0; k = 0;
 for(i = 0; i < 40; i++)
 {
  c[0] = pcolor + (ram[a] & 0x0f);
  c[1] = pcolor + ((ram[a] >> 4) & 0x0f);
  forme = ram[0x2000 | a++]; 
  for(j = 7; j >=0; j--)
  {
   xmax = k++ * xclient / xbitmap;
   col = c[(forme >> j) & 1];
   while(x <= xmax) {memcpy(p++, col, 4); x++;}
  }
 }           
}

// Affichage d'une ligne de l'écran //////////////////////////////////////////
void Displayline(int n)
{
 int *p0, *p1;
 void *bcolor;
 extern int bordercolor;
 if(n == 0)
 {
  if(vblcount++ < framedelay) return;
  vblcount = 0;    
  Displayscreen();
  return;
 } 
 if(vblcount != 0) return;
 if((n > 263) || (n < 48)) return; //zone non affichable 0-47, 264-311 
 p0 = pmin + (n - 48) * yclient / ybitmap * xclient;
 if(p0 > (pmax - xclient)) p0 = pmax - xclient;
 p1 = pmin + (n - 47) * yclient / ybitmap * xclient;
 if(p1 > pmax) p1 = pmax;
 p = p0;
 bcolor = pcolor + bordercolor;

 if(SDL_MUSTLOCK(screen))
 {
  if(SDL_LockSurface(screen) < 0) {SDL_error(33); return;}
  screenlocked = 1;
 }

 if((n < 56) || (n > 255))         //bordure haute 48-55 et basse 256-263
  for(p = p0; p < p1; p++) memcpy(p, bcolor, 4);
 else
 {  
  for(p = p0; p < (p0 + 8 * xclient / xbitmap); p++) memcpy(p, bcolor, 4);
  ComposeMO5line((n - 56) * 40);   //partie centrale
  while(p < (p0 + xclient)) memcpy(p++, bcolor, 4);
 } 
 while(p < p1) {memcpy(p, p0, 4 * xclient); p += xclient;}

 SDL_UnlockSurface(screen);
 screenlocked = 0;
} 

// Resize screen //////////////////////////////////////////////////////////////
void Resizescreen(int x, int y)
{
 int i, savepause6809;    
 extern int pause6809;
 extern int dialog;
 extern void Drawoptionbox();
 savepause6809 = pause6809;
 pause6809 = 1; SDL_Delay(200);
 //effacement surface de l'ecran
 if(screen != NULL)
 {
  pmin = (int*)(screen->pixels);
  pmax = pmin + screen->w * screen->h;
  for(p = pmin; p < pmax; p++) memcpy(p, pcolor, 4);
  SDL_Flip(screen);
 } 
 //creation nouvelle surface
 SDL_FreeSurface(screen);
 y -= ystatus;	
 xclient = (x < 336) ? 336 : x;    
 yclient = (y < 216) ? 216 : y;
 framecount = 999;
 screen = SDL_SetVideoMode(xclient, yclient + ystatus, 32, VIDEO_MODE); 
 if(screen == NULL)
 {
  screen = SDL_SetVideoMode(336, 216, 8, 0); 
  SDL_WM_SetCaption(" Erreur fatale : Mode video non compatible", NULL);
  SDL_error(35);
 }
 pmin = (int*)(screen->pixels) + ystatus * xclient;
 pmax = pmin + yclient * xclient;
 //rafraichissement de l'écran
 vblcount = 0;
 Drawstatusbar();
 if(dialog == 2) Drawoptionbox();
 for(i = 48; i < 264; i++) Displayline(i);
 Displayscreen();
 pause6809 = savepause6809;
}     

//Set mouse position /////////////////////////////////////////////////////////
void Setmouseposition(int x, int y)
{
 extern int xpen, ypen;
 extern int dialog, penbutton;	
 extern void Mouseclick();	
 xmouse = x; xpen = x * xbitmap / xclient - 7;
 ymouse = y; ypen = (y - ystatus) * ybitmap / yclient - 8;
 if(dialog) if(penbutton) Mouseclick();	
}
