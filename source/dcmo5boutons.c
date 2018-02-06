//////////////////////////////////////////////////////////////////////////////
// DCMO5BOUTONS.C - dessin des boutons et des touches du clavier
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcmo5.free.fr
// Created  : Novembre 2006
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

#include <SDL.h>
#include <SDL_ttf.h> 
#include <stdio.h>
#include <string.h>
#include "dcmo5global.h"
#include "dcmo5boutons.h"

//surfaces des boutons ([0]=relache, [1]=enfonce)
SDL_Surface *buttonsurface[KEYBUTTON_MAX + JOYBUTTON_MAX + OTHERBUTTON_MAX][2];

//couleurs des pixels
char pnoir[] = {0x40, 0x40, 0x40};
char pgris[] = {0x80, 0x80, 0x80};
char pfond[] = {0xc8, 0xd0, 0xd4};
char pblanc[] = {0xff, 0xff, 0xff};

//variables externes
extern int rmask, gmask, bmask, amask;
extern SDL_Surface *screen;    //surface d'affichage de l'ecran
extern SDL_Surface *textbox;   //surface d'affichage de texte
extern SDL_Color textnoir;     //couleur d'ecriture noir  
extern TTF_Font *font11;       //police d'ecriture taille 11
extern TTF_Font *font9;        //police d'ecriture taille 9

extern button mo5key[], keyboardbutton[], joystickbutton[];
extern button statusbutton[], optionbutton[], closebutton;

extern void SDL_error(int n);                      

//Dessin d'un bitmap sur une surface //////////////////////////////////////////
void Drawbitmap(SDL_Surface *surf, int *bitmap, int offset, int imax)
{
 int i, j, b, n, w;
 void *origine, *pixel;     
 n = surf->format->BytesPerPixel;
 w = surf->w;
 origine = (void *)surf->pixels + n * (2 * w + offset);
 if(SDL_LockSurface(surf) < 0) {SDL_error(21); return;}
 for(i = 0; i < imax; i++)
 {
  b = bitmap[i];
  if(i < (surf->h - 3)) origine += n * w;
  pixel = origine;
  for(j = 0; j < 16; j++)
  {
   if(b & 1) memset(pixel, 0x00, 3);     
   if(b & 2) memset(pixel, 0x60, 3);
   pixel += n; b >>= 2;     
  }        
 }
 SDL_UnlockSurface(surf);
}

//Initialisation image de fond d'un bouton relache ////////////////////////////
void Initbuttonbackground0(void *bitmap, int w, int h)
{
 int i, min, max, wline;
 //initialisation couleur de fond
 wline = 3 * w;       
 for(i = 0; i < wline * h; i += 3) memcpy(bitmap + i, pfond, 3);
 //rangees verticales
 min = 2 * wline - 3; max = h * wline;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pnoir, 3);    
 min = 2 * wline - 6; max = (h - 1) * wline;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pgris, 3);    
 min = wline + 3; max = (h - 2) * wline;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pblanc, 3);    
 //rangees horizontales
 min = (h - 1) * wline + 3; max = h * wline - 3;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pnoir, 3);    
 min = (h - 2) * wline + 3; max = (h - 1) * wline - 6;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pgris, 3);
 min = wline + 3; max = 2 * wline - 6;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pblanc, 3);
}     

//Initialisation image de fond d'un bouton enfonce ////////////////////////////
void Initbuttonbackground1(void *bitmap, int w, int h)
{
 int i, min, max, wline;
 //initialisation couleur de fond
 wline = 3 * w;       
 for(i = 0; i < wline * h; i += 3) memcpy(bitmap + i, pfond, 3);
 //rangees verticales
 min = 0; max = (h - 1) * wline;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pnoir, 3);    
 min = wline + 3; max = (h - 1) * wline;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pgris, 3);    
 min = 2 * wline - 6; max = (h - 1) * wline;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pblanc, 3);    
 //rangees horizontales
 min = 3; max = wline - 3;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pnoir, 3);    
 min = wline + 6; max = 2 * wline - 3;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pgris, 3);
 min = (h - 2) * wline + 6; max = (h - 1) * wline - 6;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pblanc, 3);
}     

//Initialisation image de fond d'une touche relachee //////////////////////////
void Initkeybackground0(void *bitmap, int w, int h)
{
 int i, min, max, wline;
 //initialisation couleur de fond
 wline = 3 * w;       
 for(i = 0; i < wline * h; i += 3) memcpy(bitmap + i, pfond, 3);
 //rangees verticales
 min = 4 * wline - 3; max = (h - 2) * wline - 3;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pnoir, 3);    
 min = 3 * wline - 6; max = (h - 3) * wline - 6;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pgris, 3);    
 min = 2 * wline + 3; max = (h - 4) * wline + 3;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pblanc, 3);    
 //rangees horizontales
 min = (h - 3) * wline + 9; max = (h - 2) * wline - 3;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pnoir, 3);    
 min = (h - 4) * wline + 6; max = (h - 3) * wline - 6;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pgris, 3);
 min = wline + 6; max = 2 * wline - 6;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pblanc, 3);
 //points isoles
 memcpy(bitmap + 2 * wline + 6, pblanc, 3);
 memcpy(bitmap + 3 * wline - 9, pgris, 3);    
 memcpy(bitmap + (h - 5) * wline + 6, pgris, 3);    
 memcpy(bitmap + (h - 4) * wline - 9, pgris, 3);    
 memcpy(bitmap + (h - 3) * wline - 6, pnoir, 3);    
}     

//Initialisation image de fond d'une touche enfoncee //////////////////////////
void Initkeybackground1(void *bitmap, int w, int h)
{
 int i, min, max, wline;
 //initialisation couleur de fond
 wline = 3 * w;       
 for(i = 0; i < wline * h; i += 3) memcpy(bitmap + i, pfond, 3);
 //rangees verticales
 min = 1 * wline; max = (h - 5) * wline;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pnoir, 3);    
 min = 2 * wline + 3; max = (h - 4) * wline + 3;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pgris, 3);    
 min = 3 * wline - 6; max = (h - 3) * wline - 6;
 for(i = min; i < max; i += wline) memcpy(bitmap + i, pblanc, 3);    
 //rangees horizontales
 min = 3; max = wline - 9;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pnoir, 3);    
 min = wline + 6; max = 2 * wline - 6;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pgris, 3);
 min = (h - 4) * wline + 6; max = (h - 3) * wline - 6;
 for(i = min; i < max; i += 3) memcpy(bitmap + i, pblanc, 3);
 //points isoles
 memcpy(bitmap + wline + 3, pnoir, 3);
 memcpy(bitmap + 2 * wline + 6, pgris, 3);
 memcpy(bitmap + 3 * wline - 9, pgris, 3);
 memcpy(bitmap + (h - 5) * wline + 6, pgris, 3);
 memcpy(bitmap + (h - 4) * wline - 9, pblanc, 3);
}     

//Initialisation des surfaces des touches /////////////////////////////////////
void Initkeysurfaces()
{
 SDL_Rect rect;
 int i, w, h;
 char bitmap[3 * 24 * 108]; //taille maxi= touche espace

 for(i = 0; i < MO5KEY_MAX; i++)
 {
  w = mo5key[i].w;
  h = mo5key[i].h;
  //surface de fond de la touche relachee
  Initkeybackground0(bitmap, w, h);
  buttonsurface[i][0] = SDL_ConvertSurface(SDL_CreateRGBSurfaceFrom
  (bitmap, w, h, 24, w * 3, rmask, gmask, bmask, amask), screen->format, 0); 
  if(buttonsurface[i][0] == NULL) {SDL_error(22); return;}
  //surface de fond de la touche enfoncee
  Initkeybackground1(bitmap, w, h);
  buttonsurface[i][1] = SDL_ConvertSurface(SDL_CreateRGBSurfaceFrom
  (bitmap, w, h, 24, w * 3, rmask, gmask, bmask, amask), screen->format, 0); 
  if(buttonsurface[i][1] == NULL) {SDL_error(23); return;}
  //creation du bitmap sur la surface de la touche
  if(mo5key[i].name[0] == '[') //dessin des bitmaps
  {
   void *picture;                    
   switch(mo5key[i].name[1])
   {
    case 'h': picture = flechehaut; break;                                           
    case 'b': picture = flechebas; break;                                           
    case 'g': picture = flechegauche; break;                                           
    case 'd': picture = flechedroite; break;                                           
    default : picture = flecheretour; break;
   }                                            
   Drawbitmap(buttonsurface[i][0], picture, 5, 16);
   Drawbitmap(buttonsurface[i][1], picture, 5, 16);
   continue;                     
  }                     
  //creation de l'image du texte sur la surface de la touche
  SDL_FreeSurface(textbox); rect.x = 3; rect.y = 2;
  textbox = TTF_RenderText_Blended(font9, mo5key[i].name, textnoir);
  if(textbox == NULL) {SDL_error(24); return;}
  //application du texte sur la surface de la touche relachee et enfoncee 
  SDL_BlitSurface(textbox, NULL, buttonsurface[i][0], &rect);
  SDL_BlitSurface(textbox, NULL, buttonsurface[i][1], &rect);
 }
}     

//Initialisation des surfaces des boutons ////////////////////////////////////
void Initbuttonsurfaces()
{
 SDL_Rect rect;
 int i, j, n, w, h;
 char bitmap[3 * 22 * 214]; //taille maxi= bouton vide
 for(i = 0; i < OTHERBUTTON_MAX; i++)
 {
  n = bouton[i].n;
  w = bouton[i].w;
  h = bouton[i].h;
  //initialisation w et h pour tous les boutons identiques
  for(j = 0; j < KEYBOARDBUTTON_MAX; j++)
  if(keyboardbutton[j].n == n) {keyboardbutton[j].w = w; keyboardbutton[j].h = h;} 
  for(j = 0; j < JOYSTICKBUTTON_MAX; j++)
  if(joystickbutton[j].n == n) {joystickbutton[j].w = w; joystickbutton[j].h = h;} 
  for(j = 0; j < STATUSBUTTON_MAX; j++)
  if(statusbutton[j].n == n) {statusbutton[j].w = w; statusbutton[j].h = h;} 
  for(j = 0; j < OPTIONBUTTON_MAX; j++)
  if(optionbutton[j].n == n) {optionbutton[j].w = w; optionbutton[j].h = h;} 
  if(closebutton.n == n) {closebutton.w = w; closebutton.h = h;} 
  //surface du bouton relache
  Initbuttonbackground0(bitmap, w, h);
  buttonsurface[n][0] = SDL_ConvertSurface(SDL_CreateRGBSurfaceFrom
  (bitmap, w, h, 24, w * 3, rmask, gmask, bmask, amask), screen->format, 0); 
  //surface du bouton enfonce
  Initbuttonbackground1(bitmap, w, h);
  buttonsurface[n][1] = SDL_ConvertSurface(SDL_CreateRGBSurfaceFrom
  (bitmap, w, h, 24, w * 3, rmask, gmask, bmask, amask), screen->format, 0); 
  //creation du bitmap sur la surface du bouton
  if(bouton[i].name[0] == '[') //dessin des bitmaps
  {
   void *picture; 
   int imax;                   
   switch(bouton[i].name[1])
   {
    case 'h': picture = flechehaut; imax = sizeof(flechehaut) / 4; break;                                           
    case 'b': picture = flechebas; imax = sizeof(flechebas) / 4; break;                                           
    case 'g': picture = flechegauche; imax = sizeof(flechegauche) / 4; break;                                           
    case 'd': picture = flechedroite; imax = sizeof(flechedroite) / 4; break;                                           
    case 'p': picture = boutonprev; imax = sizeof(boutonprev) / 4; break;                                           
    case 'n': picture = boutonnext; imax = sizeof(boutonnext) / 4; break;                                           
    default : picture = boutoncroix; imax = sizeof(boutoncroix) / 4; break;
   }                                            
   Drawbitmap(buttonsurface[n][0], picture, 4, imax);
   Drawbitmap(buttonsurface[n][1], picture, 4, imax);
   continue;                     
  } 
  //creation de l'image du texte sur la surface du bouton
  SDL_FreeSurface(textbox); rect.x = 4; rect.y = 1;
  textbox = TTF_RenderText_Blended(font11, bouton[i].name, textnoir);
  if(textbox == NULL) {SDL_error(25); return;}
  //application du texte sur la surface de la touche relachee et enfoncee 
  //SDL_BlitSurface(textbox, NULL, buttonsurface[n][0], &rect);
  if(SDL_BlitSurface(textbox, NULL, buttonsurface[n][0], &rect) < 0) SDL_error(26);
  //SDL_BlitSurface(textbox, NULL, buttonsurface[n][1], &rect);
  if(SDL_BlitSurface(textbox, NULL, buttonsurface[n][1], &rect) < 0) SDL_error(27);
 }
}     
