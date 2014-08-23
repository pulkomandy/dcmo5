//////////////////////////////////////////////////////////////////////////////
// DCMO5MAIN.C - dcmo5 v11 emulator main program
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
//////////////////////////////////////////////////////////////////////////////


#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h> 
#include "../include/policettf.h"
#include "../include/dcmo5icon.h"
#include "dcmo5global.h"
#include "dcmo5msg.h"

// global variables //////////////////////////////////////////////////////////
const SDL_VideoInfo *videoinfo;
SDL_AudioSpec audio;
int pause6809;        //processor pause state
int report;           //nombre de milliemes de cycle a reporter

unsigned char cursor[] = {
 128,0,192,0,160,0,144,0,136,0,132,0,130,0,129,0,128,128,128,64,128,32,129,240,
 137,0,152,128,164,128,196,64,130,64,2,32,1,32,1,16,0,176,0,192,0,0,0,0,128,0,
 192,0,224,0,240,0,248,0,252,0,254,0,255,0,255,128,255,192,255,224,255,240,255,
 0,231,128,199,128,131,192,3,192,3,224,1,224,1,240,0,240,0,192,0,0,0,0};

extern int language;

extern void Createdialogbox(int x, int y);
extern void Drawmessagebox(char *titre, char *text1[], char *text2[]);

// About message box /////////////////////////////////////////////////////////
void About()
{
 int i;    
 char *text1[9], *text2[6];     
 Createdialogbox(336, 231); 
 for(i = 0; i < 8; i++) text1[i] = msg[language][i + 2]; text1[i] = "";
 text1[i] = "";
 for(i = 0; i < 5; i++) text2[i] = msg[language][i + 20]; text2[i] = "";

 /*test police/////////////////////////////////////////////
 char string[8][33];
 for(i = 0; i < 8; i++)
 {
  int j;
  for(j = 0; j < 32; j++) string[i][j] = 32 + 32 * i + j;
  string[i][j] = 0;
  text2[i] = string[i];
 } 
 */

 /*version SDL/////////////////////////////////////////////
 const SDL_version *vdll = SDL_Linked_Version();
 SDL_version vsdl;
 SDL_VERSION(&vsdl);
 char versdl[50], verdll[50];
 sprintf(versdl, "Version SDL : %u.%u.%u",
         vsdl.major, vsdl.minor, vsdl.patch);
 sprintf(verdll, "Version DLL : %u.%u.%u",
         vdll->major, vdll->minor, vdll->patch);
 text2[0] = versdl;
 text2[1] = verdll;
 i = 2;
 */
 Drawmessagebox(msg[language][1], text1, text2);
}    

// Message d'erreur SDL //////////////////////////////////////////////////////
void SDL_error(int n)
{
 char string[256];    
 char *text1[3], *text2[1];     
 Createdialogbox(300, 90);    
 sprintf(string, "%i", n); 
 text1[0] = string;
 text1[1] = SDL_GetError();
 text1[2] = "";
 text2[0] = "";
 Drawmessagebox("SDL error", text1, text2);
 SDL_Delay(1000);
}

// Message d'erreur emulateur ////////////////////////////////////////////////
void Erreur(int n)
{
 char string[256];    
 char *text1[3], *text2[1];     
 Createdialogbox(300, 90);    
 sprintf(string, "%i", n); 
 text1[0] = string;
 text1[1] = msg[language][n];
 text1[2] = "";
 text2[0] = "";
 Drawmessagebox(msg[language][10], text1, text2);
}    

// Affichage de controle d'une valeur ///////////////////////////////////////
void Info(int n)
{
 char string[32];   
 char *text1[4], *text2[1];     
 sprintf(string, "0x%04x (%i)", n, n);
 Createdialogbox(120, 60);    
 text1[0] = string;
 text1[1] = "";
 text2[0] = "";
 Drawmessagebox("Info", text1, text2);
}    

// Joysticks initialization //////////////////////////////////////////////////
void Joyinit()
{ 
 //int i = 0;
 //if(joySetCapture(hwndmain, JOYSTICKID1, 0, TRUE)) i += 1; 
 //if(joySetCapture(hwndmain, JOYSTICKID2, 0, TRUE)) i += 2;
 //joySetThreshold(JOYSTICKID1, 2000); //10 trop peu pour Trust Predator
 //joySetThreshold(JOYSTICKID2, 2000); //2000 semble correct
}

// Play sound ////////////////////////////////////////////////////////////////
void Playsound(void *udata, Uint8 *stream, int bufferlength)
{
 int i;
 int mcycles; //nombre de milliemes de cycles entre deux echantillons
 int icycles; //nombre entier de cycles entre deux echantillons
 extern int sound, frequency;
 extern int Run(int n);
 extern void Testshiftkey();
 //45 cycles 6809 a 992250 Hz = un echantillon a 22050 Hz
 for(i = 0; i < bufferlength; i++)
 {
  if(pause6809) {stream[i] = 128; continue;}
  //calcul et execution du nombre de cycles entre deux echantillons
  //nbre de cycles theoriques pour la periode en cours =
  //nbre theorique + relicat arrondi precedent - cycles en trop precedent
  mcycles = frequency * 100000 / 2205;   //milliemes de cycles theoriques
  mcycles += report;                     //milliemes de cycles corriges
  icycles = mcycles / 1000;              //nbre entier de cycles a effectuer 
  report = mcycles - 1000 * icycles;     //relicat de l'arrondi a reporter
  report -= 1000 * Run(icycles);         //retrancher les milliemes en trop  
  stream[i] = sound + 128;
 }
 Testshiftkey(); //contournement bug SDL sur l'appui simultane SHIFT, ALT, CTRL
}  

//Event loop /////////////////////////////////////////////////////////////////
void Eventloop()
{
 SDL_Event event;
 extern int penbutton, mouselastx, mouselasty;
 extern void Resizescreen(int x, int y), Displayscreen();
 extern void Mouseclick(), Setmouseposition(int x, int y);
 extern void Keydown(int keysym, int scancode), Keyup(int keysym, int scancode);

 while(1)
 {
  while(SDL_PollEvent(&event))
  {
   switch(event.type)
   {
    case SDL_VIDEORESIZE:
         Resizescreen(event.resize.w, event.resize.h); break;
    case SDL_MOUSEBUTTONDOWN:
         penbutton = 1; Mouseclick(); break;
    case SDL_MOUSEBUTTONUP:
         penbutton = 0; mouselastx = 0; mouselasty = 0; break;
    case SDL_MOUSEMOTION:
         Setmouseposition(event.motion.x, event.motion.y); break;
    case SDL_KEYDOWN:
         Keydown(event.key.keysym.sym, event.key.keysym.scancode); break;
    case SDL_KEYUP:
         Keyup(event.key.keysym.sym, event.key.keysym.scancode); break;
    case SDL_QUIT: return;
   }
  }
  SDL_Delay(20);
 }
}     

/*
// Restaurer la fenetre ///////////////////////////////////////////////////////
void Restorewindow()
{
 //Avec SDL 1.2, il semble impossible
 //- de maximiser ou restaurer la fenetre
 //- de savoir si la fenetre est maximisee ou pas
 //La fenetre initiale de SDL n'etant pas maximisee, l'idee etait d'arreter
 //puis de redemarrer le sous-systeme video pour restaurer la fenetre.
 //Il y a eu un resultat (fenetre correctement restauree) mais trop
 //d'inconvenients : perte du focus, plantages et autres phenomenes inexpliques.
 //Je crois qu'il faut attendre la version 1.3 

 extern int penbutton, xclient, yclient, ystatus; 
 extern void Resizescreen(int x, int y);
  
 //SDL_PauseAudio(1);    //Arrete l'emulation
 //SDL_Delay(200);
 SDL_QuitSubSystem(SDL_INIT_VIDEO);
 SDL_InitSubSystem(SDL_INIT_VIDEO);
 atexit(SDL_Quit);
 SDL_WM_SetIcon(SDL_LoadBMP("dcmo5.bmp"), NULL);
 SDL_WM_SetCaption(msg[language][0], NULL);            //titre fenetre
 SDL_SetCursor(SDL_CreateCursor(cursor, cursor + 48, 16, 24, 0, 0));
 Resizescreen(xclient, ystatus + yclient);
 penbutton = 0;
 //SDL_PauseAudio(0);    //Lance l'emulation
} 
*/

//Main program ///////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
 SDL_RWops *rw;
 extern TTF_Font *font9;
 extern TTF_Font *font11;
 extern int xclient, yclient, ystatus;
 extern void Init6809registerpointers(), Initfilenames(), Hardreset();
 extern void Initoptions(), Saveoptions();
 extern void Initkeysurfaces(), Initbuttonsurfaces(), Initstatusbar();
 extern void Resizescreen(int x, int y), Keyboardinit();
 
 //initialisations
 Joyinit();                        //Joysticks initialization
 Initoptions();                    //Option initialization
 Initfilenames();
 Init6809registerpointers();
 Keyboardinit();                   //Keyboard initialization
 Hardreset();                      //MO5 initialization

 //initialize SDL ttf
 if(TTF_Init() == -1) SDL_error(1);
 rw = SDL_RWFromMem(verattf, sizeof(verattf));
 font11 = TTF_OpenFontRW(rw, 0, 11);
 rw = SDL_RWFromMem(verattf, sizeof(verattf));
 font9 = TTF_OpenFontRW(rw, 0, 9);
 
 //initialize SDL video
 if(SDL_Init(SDL_INIT_VIDEO) < 0) SDL_error(2);
 atexit(SDL_Quit);
 rw = SDL_RWFromMem(dcmo5icon, sizeof(dcmo5icon));
 SDL_WM_SetIcon(SDL_LoadBMP_RW(rw, 1), NULL);  //icone fenetre
 SDL_WM_SetCaption(msg[language][0], NULL);    //titre fenetre
 SDL_SetCursor(SDL_CreateCursor(cursor, cursor + 48, 16, 24, 0, 0)); //curseur
 Resizescreen(xclient, yclient + ystatus);
 Initbuttonsurfaces(); //Initialisation des surfaces des boutons
 Initstatusbar();      //Initialisation de la surface de la barre de statut  
 Initkeysurfaces();    //Initialisation des surfaces des touches  

 //initialize SDL audio
 if(SDL_Init(SDL_INIT_AUDIO) < 0) SDL_error(3);
 audio.freq = 22050;
 audio.format = AUDIO_U8;
 audio.channels = 1;
 audio.samples = 1024; 
 audio.callback = Playsound;
 audio.userdata = NULL;
 if(SDL_OpenAudio(&audio, NULL) < 0 ) SDL_error(4);
 SDL_PauseAudio(0);    //Lance l'emulation
 
 //test events
 Eventloop();
 
 //quit
 pause6809 = 1;
 SDL_PauseAudio(1);
 Saveoptions();
 SDL_Delay(100);
 TTF_Quit();
 SDL_Quit();
 return 0;
}
