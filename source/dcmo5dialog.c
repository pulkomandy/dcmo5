///////////////////////////////////////////////////////////////////////////////
// DCDIALOG.C - Fonctions d'interface utilisateur
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcmo5.free.fr
// Created  : March 2007
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

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h> 
#include <string.h>
#include <dirent.h>
#include <math.h>
#include "dcmo5global.h"

//#define FLAGS SDL_HWSURFACE | SDL_DOUBLEBUF
#define FLAGS SDL_SWSURFACE | SDL_DOUBLEBUF

#define DIRLIST_LENGTH 32
#define DIRLIST_NMAX 500
 
button closebutton = {"X", -20, 4, 0, 0, 63};

button statusbutton[STATUSBUTTON_MAX] = {
{"k7",       56,  2, 0, 0, 69}, //0x00 
{"fd",      186,  2, 0, 0, 70}, //0x01 
{"memo",    316,  2, 0, 0, 71}, //0x02 
{"Options", -69,  2, 0, 0, 66}, //0x03 
{"?",       -17,  2, 0, 0, 67}  //0x04 
};

//constantes
const SDL_Color textnoir = {0, 0, 0, 255};        //couleur d'ecriture noir  
const SDL_Color textblanc = {255, 255, 255, 255}; //couleur d'ecriture blanc   

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const int rmask = 0xff000000, gmask = 0x00ff0000;
const int bmask = 0x0000ff00, amask = 0x000000ff;
const int blanc = 0xffffffff, noir  = 0x000000ff;
const int gris  = 0xe0e0e077, bleu  = 0x20206077;
const int grisf = 0x80808077;
#else
const int rmask = 0x000000ff, gmask = 0x0000ff00;
const int bmask = 0x00ff0000, amask = 0xff000000;
const int blanc = 0xe4ffffff, noir  = 0xe4000000;
const int gris  = 0xe4e0e0e0, bleu  = 0xe4602020;
const int grisf = 0xe4808080;
#endif

//global variables ///////////////////////////////////////////////////////////
SDL_Surface *textbox = NULL;   //surface d'affichage de texte
SDL_Surface *dialogbox = NULL; //surface d'affichage dialogbox
SDL_Surface *statusbar = NULL; //surface de la barre de statut
TTF_Font *font11 = NULL;       //police d'ecriture taille 11
TTF_Font *font9 = NULL;        //police d'ecriture taille 9
SDL_Rect dialogrect;           //position dialogbox 
int dialog = 0;                //0=rien 1=message, 2=options 3=clavier 4=menu
int mouselastx, mouselasty;    //position souris dans dialogbox
int dircount;                  //nombre de fichiers dans le repertoire 
char dirlist[DIRLIST_NMAX][DIRLIST_LENGTH]; //liste des fichiers du repertoire
int dirmin, dirmax;            //plage de numeros de fichiers affiches 
void (*Load[3])(char *name);   //pointeur fonction de chargement de fichier

//external
extern SDL_Surface *buttonsurface[][2]; 
extern SDL_Surface *screen;    //surface d'affichage de l'ecran
extern int xbitmap, ybitmap;   //taille ecran MO5
extern int xclient, yclient;   //taille ecran affiche
extern int ystatus;            //hauteur barre d'etat
extern char *msg[LANG_MAX][MSG_MAX];  //messages en plusieurs langues
extern int language;

extern void SDL_error(int n);
extern void Info(int i);

//Tri du repertoire path avec filtre sur l'extension //////////////////////////
void Sortdirectory(char *path, char *extension)
{
 DIR *dir;
 int i, j;   
 struct dirent *entry;
 dircount = 0;
 if ((dir = opendir(path)) == NULL) return;
 while((entry = readdir(dir)))
 {
  strncpy(dirlist[dircount], entry->d_name, DIRLIST_LENGTH);
  dirlist[dircount][DIRLIST_LENGTH - 1] = 0;
  i = strlen(dirlist[dircount]); 
  j = strlen(extension); 
  if(i < j) continue;
  if(strcasecmp(dirlist[dircount] + i - j, extension)) continue; 
  if(++dircount >= DIRLIST_NMAX) break;
 }
 closedir(dir);
 //strcpy(dirlist[dircount++], msg[language][39]); // [décharger]
 qsort(dirlist, dircount, DIRLIST_LENGTH,
 (int(*)(const void*, const void*))strcmp);
}     

//Draw textbox ///////////////////////////////////////////////////////////////
void Drawtextbox(SDL_Surface *surf, char *string, SDL_Rect rect, int c)
{
 //c = couleur du fond et graisse
 //normal : O=gris, 1=blanc, 2=noir, 3=bleu, 4=transparent     
 //bold :  10=gris, 11=blanc, 12=noir, 13=bleu, 14=transparent      
 SDL_Rect textrect;    
 SDL_Color textcolor;
 int r, boxcolor;
 TTF_SetFontStyle(font11, TTF_STYLE_NORMAL);
 if(c > 9) {c -= 10; TTF_SetFontStyle(font11, TTF_STYLE_BOLD);}
 switch(c)
 {
  case 0: boxcolor = gris; textcolor = textnoir; break;
  case 1: boxcolor = blanc; textcolor = textnoir; break;
  case 2: boxcolor = noir; textcolor = textblanc; break;
  case 3: boxcolor = bleu; textcolor = textblanc; break;
  case 4: boxcolor = bleu; textcolor = textnoir; break;
  default: return;
 }         
 if(c < 4) SDL_FillRect(surf, &rect, boxcolor);
 if(string[0] == 0) return;
 SDL_FreeSurface(textbox);
 textbox = TTF_RenderText_Blended(font11, string, textcolor);
 if(textbox == NULL) {SDL_error(11); return;}
 textrect.x = -2; textrect.w = rect.w - 4;
 textrect.y = 0; textrect.h = rect.h;
 r = SDL_BlitSurface(textbox, &textrect, surf, &rect);
 if(r < 0) SDL_error(12);
}     

//Ajout du texte 36 + i sur un bouton /////////////////////////////////////////
void Buttontext(int i, int x, int y)
{
 SDL_Rect rect;
 rect.x = x + 5; rect.y = y + 4; rect.w = 209; rect.h = 15;
 Drawtextbox(dialogbox, msg[language][36 + i], rect, 4); 
}
	
//Draw message box ////////////////////////////////////////////////////////////
void Drawmessagebox(char *titre, char *text1[], char *text2[])
//text1 est affiché sur fond gris et text2 sur fond blanc
//le dernier des deux textes doit etre une chaine de longueur nulle
{
 SDL_Rect rect;
 int i;
 dialog = 1;  
 //titre
 rect.x = 10; rect.w = dialogbox->w - 32;
 rect.y = 5; rect.h = 15;
 Drawtextbox(dialogbox, titre, rect, 13);
 //texte sur fond gris 
 rect.x = 10; rect.w = dialogbox->w - 20;
 rect.y += 26;
 i = 0;
 while(text1[i][0] != 0)
 {
  Drawtextbox(dialogbox, text1[i], rect, 0);
  rect.y += 13; i++;
 }
 rect.y += 13;
 i = 0; 
 while(text2[i][0] != 0)
 {
  Drawtextbox(dialogbox, text2[i], rect, 1);                   
  rect.y += 13; i++;
 }
}

//Index cassette /////////////////////////////////////////////////////////////
void Drawk7index()
{
 SDL_Rect rect;    
 char index[10];
 extern FILE * fk7;
 extern int k7index, k7indexmax;
 index[0] = 0;
 if(fk7 != NULL) sprintf(index, "%03d/%03d", k7index, k7indexmax);
 rect.x = 2; rect.w = 52; rect.y = 2; rect.h = 15;
 Drawtextbox(statusbar, index, rect, 1);
}

//Draw status bar ////////////////////////////////////////////////////////////
void Drawstatusbar()
{
 SDL_Rect rect;    
 SDL_Surface *surf;    
 int i, r;
 extern char k7name[], fdname[], memoname[]; 
 
 if(statusbar == NULL) return;

 //fond
 rect.x = 0; rect.w = xclient;
 rect.y = 0; rect.h = ystatus - 1;
 SDL_FillRect(statusbar, &rect, gris);
 rect.y = ystatus - 1; rect.h = 1;
 SDL_FillRect(statusbar, &rect, blanc);

 //index cassette
 Drawk7index();

 //nom cassette, disquette, memo5
 rect.y = 2; rect.h = 15; rect.w = 100; 
 rect.x = 80;  Drawtextbox(statusbar, k7name, rect, 1); 
 rect.x = 210; Drawtextbox(statusbar, fdname, rect, 1); 
 rect.x = 362; Drawtextbox(statusbar, memoname, rect, 1); 

 //boutons
 for(i = 0; i < STATUSBUTTON_MAX; i++)
 {
  rect.x = statusbutton[i].x; if(rect.x < 0) rect.x += xclient;
  rect.y = statusbutton[i].y; if(rect.y < 0) rect.y += yclient;
  //Info(rect.x); SDL_Delay(1000);
  surf = buttonsurface[statusbutton[i].n][0];
  r = SDL_BlitSurface(surf, NULL, statusbar, &rect);
  if(r < 0) {SDL_error(13); return;}
 }
}     

//Init status bar ////////////////////////////////////////////////////////////
void Initstatusbar()
{
 statusbar = SDL_CreateRGBSurface(FLAGS, 2048, ystatus, 32, rmask, gmask, bmask, amask);
 if(statusbar == NULL) SDL_error(14);
 Drawstatusbar();
} 

//Create box /////////////////////////////////////////////////////////////////
void Createbox()
{
 SDL_Rect rect;
 SDL_Delay(100);
 if(dialogbox != NULL) SDL_FreeSurface(dialogbox);
 dialogbox = SDL_CreateRGBSurface(FLAGS, dialogrect.w, dialogrect.h,
                                  32, rmask, gmask, bmask, amask);
 //fond gris
 rect.x = 0; rect.w = dialogbox->w;
 rect.y = 0; rect.h = dialogbox->h;
 SDL_FillRect(dialogbox, &rect, gris);
 //bordure haut blanche      
 rect.h = 1;
 SDL_FillRect(dialogbox, &rect, blanc);
 //bordure gauche blanche      
 rect.x = 0; rect.w = 1;
 rect.y = 0; rect.h = dialogbox->h - 1;
 SDL_FillRect(dialogbox, &rect, blanc);
 //bordure bas noire
 rect.x = 1; rect.w = dialogbox->w - 2;
 rect.y = dialogbox->h - 2; rect.h = 1; 
 SDL_FillRect(dialogbox, &rect, noir);
 //bordure bas gris fonce
 rect.x += 1; rect.y += 1; 
 SDL_FillRect(dialogbox, &rect, grisf);
 //bordure droite noire
 rect.x = dialogbox->w - 2; rect.w = 1;
 rect.y = 1; rect.h = dialogbox->h - 3;
 SDL_FillRect(dialogbox, &rect, noir);
 //bordure droite gris fonce
 rect.x += 1; rect.y += 1;
 SDL_FillRect(dialogbox, &rect, grisf);
}

//Create dialog box //////////////////////////////////////////////////////////
void Createdialogbox(int w, int h)
{
 SDL_Rect rect;
 int r;
 dialogrect.x = 10; dialogrect.w = w;
 dialogrect.y = ystatus + 10; dialogrect.h = h;
 Createbox(w, h);
 //barre de titre bleue
 rect.x = 4; rect.w = dialogbox->w - 8;
 rect.y = 3; rect.h = 18;
 SDL_FillRect(dialogbox, &rect, bleu);
 //bouton de fermeture
 rect.x = closebutton.x + dialogbox->w;
 rect.y = closebutton.y;
 r = SDL_BlitSurface(buttonsurface[closebutton.n][0], NULL, dialogbox, &rect);
 if(r < 0) SDL_error(15);
}

//Traitement des clics souris dans la barre de statut ////////////////////////
void Statusclick()
{
 SDL_Rect rect;
 int i, r, x, y;	
 extern int xmouse, ymouse;
 void Drawmenubox(), Drawdirlistbox(int);
 extern void Keyboard(), Nextmemo(), About();
 
 //recherche du bouton concerne par le clic
 for(i = 0; i < STATUSBUTTON_MAX; i++)
 {
  x = statusbutton[i].x; if(x < 0) x += xclient;     
  y = statusbutton[i].y; if(y < 0) y += yclient;     
  if(xmouse > x) if(xmouse < (x + statusbutton[i].w))
  if(ymouse > y) if(ymouse < (y + statusbutton[i].h))
  break;
 }
 if(i >= STATUSBUTTON_MAX) {dialog = 0; return;}

 //dessin du bouton enfonce
 rect.x = statusbutton[i].x; if(rect.x < 0) rect.x += xclient;
 rect.y = statusbutton[i].y; if(rect.y < 0) rect.y += yclient;
 r = SDL_BlitSurface(buttonsurface[statusbutton[i].n][1], NULL, screen, &rect);
 if(r < 0) {SDL_error(16); return;}
            
 //traitement           
 switch(i)
 {
  case 0: dirmin = 0; Drawdirlistbox(0); break;
  case 1: dirmin = 0; Drawdirlistbox(1); break;
  case 2: dirmin = 0; Drawdirlistbox(2); break;
  case 3: Drawmenubox(); break;
  case 4: About(); break;
 }
 Drawstatusbar();
}

//Traitement des clics souris dans la boite de dialogue //////////////////////
void Dialogclick()
{
 int x, y;    
 extern int xmouse, ymouse;
 extern void Optionclick(), Keyclick(), Joyclick();

 //fermeture dialogbox
 x = dialogrect.x + dialogrect.w + closebutton.x;     
 y = dialogrect.y + closebutton.y;     
 if(xmouse > x) if(xmouse < (x + closebutton.w))
 if(ymouse > y) if(ymouse < (y + closebutton.h))
 {dialog = 0; return;}

 //deplacement dialogbox
 if(ymouse < (dialogrect.y + 22))
 {
  int ytotal = yclient + ystatus;	  

  if(mouselastx > 0)
     dialogrect.x = xmouse - mouselastx;
  if(mouselasty > 0)
     dialogrect.y = ymouse - mouselasty;
  if(dialogrect.x > (xclient - dialogrect.w))
     dialogrect.x = xclient - dialogrect.w;
  if(dialogrect.y > (ytotal - dialogrect.h))
     dialogrect.y = ytotal - dialogrect.h;
  if(dialogrect.x < 0)
     dialogrect.x = 0;
  if(dialogrect.y < ystatus)
     dialogrect.y = ystatus;

  mouselastx = xmouse - dialogrect.x;
  mouselasty = ymouse - dialogrect.y;
  return;
 }

 //traitements particuliers
 if(dialog == 2) Optionclick();  //boite de dialogue Parametres
 if(dialog == 3) Keyclick();     //boite de dialogue Clavier
 if(dialog == 4) Joyclick();     //boite de dialogue Manettes
}

//Traitement des clics souris ////////////////////////////////////////////////
void Mouseclick()
{
 extern int xmouse, ymouse, pause6809;
 extern void Menuclick();
 pause6809 = 1;

 //clic dans la barre de statut
 if(ymouse < ystatus)
 {Statusclick(); pause6809 = 0; return;}
  
 //clic dans la boite de dialogue
 if(dialog > 0)
 if(xmouse > dialogrect.x) if(xmouse < (dialogrect.x + dialogrect.w))
 if(ymouse > dialogrect.y) if(ymouse < (dialogrect.y + dialogrect.h))
 {if(dialog > 999) Menuclick(); else Dialogclick();}

 //redemarrage de l'emulation, meme si elle etait arretee
 //sinon il n'y a pas d'affichage
 pause6809 = 0;
}

//Draw menu box ////////////////////////////////////////////////////////////
void Drawmenubox()
{
 SDL_Rect rect;
 int i;
 char string[50];
 dialogrect.x = xclient -102; dialogrect.w = 100;
 dialogrect.y = ystatus; dialogrect.h = 3 * 16 + 10;
 Createbox();
 rect.x = 10; rect.w = 80; rect.y = 4; rect.h = 14;
 for(i = 29; i < 32; i++)
 {
  sprintf(string, "%s...", msg[language][i]);   
  Drawtextbox(dialogbox, string, rect, 0);
  rect.y += 16;
 } 
 dialog = 1003; 
}

//Draw directory listbox /////////////////////////////////////////////////////
void Drawdirlistbox(int n)
{
 SDL_Rect rect;
 int i;
 void *string;
 switch(n)
 {
  case 0: Sortdirectory("software", ".k7"); break;
  case 1: Sortdirectory("software", ".fd"); break;
  case 2: Sortdirectory("software", ".rom"); break;
  default: return;    
 }
 if(dircount <= 0) return;
 dirmax = dirmin + 20;
 if(dirmax > dircount) dirmax = dircount;
 dialogrect.x = statusbutton[n].x; dialogrect.w = 200;
 dialogrect.y = ystatus; dialogrect.h = 16 * (dirmax - dirmin) + 26;
 if((dirmax < dircount) || (dirmin > 0)) dialogrect.h += 16; //[suite]/[debut]
 Createbox();
 rect.x = 10; rect.w = dialogrect.w - 12;
 rect.y = 4; rect.h = 14;
 Drawtextbox(dialogbox, msg[language][39], rect, 0); 
 for(i = dirmin; i < dirmax; i++)
 {
  rect.y += 16;
  Drawtextbox(dialogbox, dirlist[i], rect, 0); 
 }     
 if((dirmax < dircount) || (dirmin > 0))
 {
  rect.y += 16;
  string = msg[language][(dirmax == dircount) ? 41 : 40];
  Drawtextbox(dialogbox, string, rect, 0); 
 } 
 dialog = 1000 + n; 
}

//Traitement des clics dans un menu deroulant /////////////////////////////////
void Menuclick()
{
 int i, n, ymax;    
 extern int ymouse;
 extern void Options(), Clavier(), Manettes();

 n = dialog - 1000;
 if(n == 3) //menu options
 {
  if(ymouse < (ystatus + 20)) {Options(); return;}
  if(ymouse < (ystatus + 36)) {Clavier(); return;}
  if(ymouse < (ystatus + 52)) {Manettes(); return;}
  return;
 } 

 while(1) //dir listbox
 {
  ymax = ystatus + 20;
  if(ymouse < ymax) {Load[n](" "); break;} 
  for(i = dirmin; i < dirmax; i++)
  {ymax += 16; if(ymouse < ymax) {Load[n](dirlist[i]); break;}} 
  if(i < dirmax) break;
  ymax += 16; if(ymouse < ymax)
  {dirmin += 20; if(dirmin >= dircount) dirmin = 0; Drawdirlistbox(n);}
  return; 
 }
 Drawstatusbar();
 dialog = 0;         
}
