//////////////////////////////////////////////////////////////////////////////
// DCMO5OPTIONS.C - Option setting, save & restore
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcmo5.free.fr
// Created  : November 2007
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
#include <stdio.h>
#include <string.h>
#include "dcmo5global.h"

FILE *fpi;        //fichier dcmo5.ini   
int language;     //0=francais 1=anglais
int frequency;    //frequence 6809 en kHz

button optionbutton[OPTIONBUTTON_MAX] = {
{"[prev]",  208, 32, 0, 0, 64}, //0x00 
{"[next]",  208, 40, 0, 0, 65}, //0x01 
{"[prev]",  208, 52, 0, 0, 64}, //0x02 
{"[next]",  208, 60, 0, 0, 65}, //0x03 
{"[prev]",  208, 72, 0, 0, 64}, //0x04 
{"[next]",  208, 80, 0, 0, 65}, //0x05
{"[prev]",  208, 92, 0, 0, 64}, //0x06 
{"[next]",  208, 100, 0, 0, 65}, //0x07
{"[prev]",  208, 112, 0, 0, 64}, //0x08 
{"[next]",  208, 120, 0, 0, 65}, //0x09
{"[prev]",  208, 132, 0, 0, 64}, //0x0a 
{"[next]",  208, 140, 0, 0, 65}, //0x0b
{" ",       10, 160, 0, 0, 68} //0x0c
};

extern SDL_Surface *dialogbox;
extern int xbitmap, ybitmap, xclient, yclient, ystatus, framedelay;
extern int k7protection; //protection k7 (0=lecture/ecriture 1=lecture seule)
extern int fdprotection; //protection fd (0=lecture/ecriture 1=lecture seule)
extern char *msg[LANG_MAX][MSG_MAX]; //messages en plusieurs langues
extern SDL_Surface *buttonsurface[][2];

extern void Drawtextbox(SDL_Surface *surf, char *string, SDL_Rect rect, int c);
extern void Buttontext(int i, int x, int y);

//Draw option box ////////////////////////////////////////////////////////////
void Drawoptionbox()
{
 SDL_Rect rect;
 int i;
 char string[50];
 extern int dialog;
 dialog = 2;     
 //titre
 rect.x = 10; rect.w = dialogbox->w - 32;
 rect.y = 5; rect.h = 15;
 Drawtextbox(dialogbox, msg[language][29], rect, 13);
 //options
 rect.x = 10; rect.w = 194;
 rect.y = 32; rect.h = 15;
 Drawtextbox(dialogbox, msg[language][27], rect, 1); 
 sprintf(string, "Zoom %1.2f", (float)xclient / xbitmap);
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 1); 
 sprintf(string, "%s %i%s", msg[language][28], frequency / 10, "%");
 rect.y += 20; 
 Drawtextbox(dialogbox, string, rect, 1); 
 sprintf(string, "%s %i%s", msg[language][26], 100 / framedelay, "%");
 rect.y += 20; 
 Drawtextbox(dialogbox, string, rect, 1); 
 sprintf(string, "k7 %s", msg[language][(k7protection) ? 32 : 33]);
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 1); 
 sprintf(string, "fd %s", msg[language][(fdprotection) ? 32 : 33]);
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 1); 
 //boutons
 for(i = 0; i < OPTIONBUTTON_MAX; i++)
 {
  rect.x = optionbutton[i].x; rect.y = optionbutton[i].y;
  SDL_BlitSurface(buttonsurface[optionbutton[i].n][0], NULL, dialogbox, &rect);
 }
 //texte
 Buttontext(2, rect.x, rect.y);
}

//Initialisation des valeurs par defaut //////////////////////////////////////
void Initdefault()
{
 language = 0;          //francais
 xclient = 2 * xbitmap; //zoomx 2
 yclient = 2 * ybitmap; //zoomy 2
 frequency = 1000;      //1000 kHz
 framedelay = 2;        //nombre de vbl entre deux affichages 
 k7protection = 1;      //protection cassette 
 fdprotection = 1;      //protection disquette
}

//Traitement des clics des boutons d'options /////////////////////////////////
void Optionclick()
{
 SDL_Rect rect;
 int i, x, y, step;    
 extern int xmouse, ymouse;
 extern int dialog;
 extern SDL_Rect dialogrect; 
 extern void Displayscreen();
 extern void Resizescreen(int x, int y);

 //recherche du bouton du clic
 for(i = 0; i < OPTIONBUTTON_MAX; i++)
 {
  x = dialogrect.x + optionbutton[i].x;     
  y = dialogrect.y + optionbutton[i].y;     
  if(xmouse > x) if(xmouse < (x + optionbutton[i].w))
  if(ymouse > y) if(ymouse < (y + optionbutton[i].h))
  break;
 }
 if(i >= OPTIONBUTTON_MAX) return;
 //dessin du bouton enfonce
 rect.x = optionbutton[i].x;
 rect.y = optionbutton[i].y;
 SDL_BlitSurface(buttonsurface[optionbutton[i].n][1], NULL, dialogbox, &rect);
 if(i == (OPTIONBUTTON_MAX - 1)) Buttontext(2, rect.x, rect.y);
 Displayscreen(); SDL_Delay(200);
 //traitement en fonction du bouton           
 switch(i)
 {
  case  0: if(--language < 0) language++;              //previous language
           SDL_WM_SetCaption(msg[language][0], NULL);
           break;
  case  1: if(++language == LANG_MAX) language--;      //next language 
           SDL_WM_SetCaption(msg[language][0], NULL);
           break;
  case  2: x = xclient / xbitmap + 1; if(x > 4) x = 4; //zoom in
           Resizescreen(xbitmap * x, ystatus + ybitmap * x);
           break;
  case  3: x = xclient / xbitmap - 1; if(x < 1) x = 1; //zoom out
           Resizescreen(xbitmap * x, ystatus + ybitmap * x);
           break;
  case  4: step = 10; frequency = frequency * 10 / 10; //faster
           if(frequency < 900) step = 100; 
           if(frequency > 1099) step = 100; 
           if(frequency > 1999) step = 1000; 
           if(frequency < 9999 - step) frequency += step;
           break;
  case  5: step = 10; frequency = frequency * 10 / 10; //slower
           if(frequency < 901) step = 100; 
           if(frequency > 1100) step = 100; 
           if(frequency > 2000) step = 1000; 
           if(frequency > 9 + step) frequency -= step;
           break;
  case  6: framedelay /= 2; if(framedelay < 1) framedelay = 1;   //more frames
           break;
  case  7: framedelay *= 2; if(framedelay > 64) framedelay = 64; //less frames
           break;
  case  8: k7protection = 0; //unprotect k7
           break;
  case  9: k7protection = 1; //protect k7
           break;
  case 10: fdprotection = 0; //unprotect fd
           break;
  case 11: fdprotection = 1; //protect k7
           break;
  case 12: Initdefault();    //restore default values
           //Restorewindow(); //lire les commentaires de cette fonction
           Resizescreen(xclient, ystatus + yclient);
           break;
 } 
 if(dialog == 2) Drawoptionbox();
}

//Option setting /////////////////////////////////////////////////////////////
void Options()
{
 extern void Createdialogbox(int w, int h);
 Createdialogbox(236, 196);
 Drawoptionbox();
}     

//Option initialization //////////////////////////////////////////////////////
void Initoptions()
{
 char string[256];    
 Initdefault();
 //ouverture fichier dcmo5.ini
 fpi = fopen("dcmo5.ini", "rb+");                 //s'il existe ouverture
 if(fpi == NULL) fpi = fopen("dcmo5.ini", "wb+"); //s'il n'existe pas : creation
 fread(string, 12, 1, fpi);                       //lecture identifiant  
 if(strcmp("dcmo5ini-01", string) != 0) return;
 //initialisation des options
 fread(&language, 4, 1, fpi);  //langue 
 fread(&frequency, 4, 1, fpi); //frequence 6809
 fread(&xclient, 4, 1, fpi);   //largeur ecran
 fread(&yclient, 4, 1, fpi);   //hauteur ecran
 fread(&framedelay, 4, 1, fpi); 
 fread(&k7protection, 4, 1, fpi); 
 fread(&fdprotection, 4, 1, fpi); 
 //controle
 if(language < 0) language = 0;
 if(language > (LANG_MAX - 1)) language = LANG_MAX - 1;
 if(frequency < 100) frequency = 100;
 if(frequency > 9000) frequency = 9000;
 if(xclient < xbitmap) xclient = xbitmap; 
 if(xclient > (4 * xbitmap)) xclient = 4 * xbitmap; 
 if(yclient < ybitmap) yclient = ybitmap; 
 if(yclient > (4 * ybitmap)) yclient = 4 * ybitmap;
 if(framedelay < 1) framedelay = 1;
 if(framedelay > 64) framedelay = 64;
 if(k7protection) k7protection = 1; 
 if(fdprotection) fdprotection = 1; 
} 

//Option save ////////////////////////////////////////////////////////////////
void Saveoptions()
{
 int i = 0;    
 fseek(fpi, 0, SEEK_SET);
 fwrite("dcmo5ini-01", 12, 1, fpi);
 fwrite(&language, 4, 1, fpi);  //langue 
 fwrite(&frequency, 4, 1, fpi); //frequence 6809
 fwrite(&xclient, 4, 1, fpi);   //largeur ecran
 fwrite(&yclient, 4, 1, fpi);   //hauteur ecran
 fwrite(&framedelay, 4, 1, fpi); 
 fwrite(&k7protection, 4, 1, fpi); 
 fwrite(&fdprotection, 4, 1, fpi); 
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fclose(fpi); 
} 
