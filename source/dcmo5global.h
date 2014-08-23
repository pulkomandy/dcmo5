//////////////////////////////////////////////////////////////////////////////
// DCMO5GLOBAL.H - declarations globales
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

#define MO5KEY_MAX 58         //nombre de touches du clavier MO5
#define KEYBOARDBUTTON_MAX 2  //nombre de boutons boite de dialogue clavier 
#define JOYKEY_MAX 10         //nombre total de contacts des deux manettes 
#define JOYSTICKBUTTON_MAX 2  //nombre de boutons boite de dialogue manettes 
#define STATUSBUTTON_MAX 5    //nombre de boutons de la barre de statut 
#define OPTIONBUTTON_MAX 13   //nombre de boutons boite de dialogue options 
#define LANG_MAX 2            //nombre de langues differentes 
#define MSG_MAX 42            //nombre de textes dans chaque langue 

#define KEYBUTTON_MAX 58      //nombre de boutons differents clavier
#define JOYBUTTON_MAX 5       //nombre de boutons differents joysticks
#define OTHERBUTTON_MAX 14    //nombre de boutons differents autres

typedef struct
{
 char name[12]; //nom du bouton
 int x;         //ordonnee du coin haut-gauche
 int y;         //abscisse du coin haut-gauche
 int w;         //largeur en pixels
 int h;         //hauteur en pixels
 int n;         //numero de la surface SDL
} button;

