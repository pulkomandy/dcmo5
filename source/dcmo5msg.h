//////////////////////////////////////////////////////////////////////////////
// DCMO5MSG.C - message text used by dcmo5
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

//2 langues : 0=francais 1=anglais
//MSG_MAX messages par langue

char *msg[LANG_MAX][MSG_MAX]={
{
/*00*/ " DCMO5 v11 - Emulateur MO5 Thomson",
/*01*/ "A propos de DCMO5",
/*02*/ "Version : 11.0 - 12 décembre 2007",
/*03*/ "Auteur : Daniel Coulom",
/*04*/ "Site internet : http://dcmo5.free.fr",
/*05*/ " ",
/*06*/ "Ce programme est diffusé SANS AUCUNE GARANTIE.",
/*07*/ "C'est un logiciel libre, et vous êtes encouragés à le",
/*08*/ "redistribuer sous certaines conditions.",
/*09*/ "Lire les détails dans dcmo5v11-licence.txt",
/*10*/ "Erreur",
/*11*/ "Pas de cassette dans le lecteur",
/*12*/ "Fin de cassette, rembobinage automatique",
/*13*/ "Cassette protégée en écriture",
/*14*/ "Disposition du clavier",
/*15*/ "touche MO5 émulée",
/*16*/ "Appuyez sur une touche pour voir la ",
/*17*/ "Cliquez sur l'image pour changer la correspondance.",
/*18*/ "aucune",
/*19*/ "",
/*20*/ "Raccourcis clavier :",
/*21*/ "- ECHAP= initialisation programme",
/*22*/ "- PAUSE= arrêt processeur 6809",
/*23*/ " ",
/*24*/ "Lire la documentation pour d'autres informations...",
/*25*/ "",
/*26*/ "Vitesse d'affichage",
/*27*/ "Français",
/*28*/ "Vitesse processeur",
/*29*/ "Paramètres",
/*30*/ "Clavier",
/*31*/ "Manettes",
/*32*/ "lecture seule",
/*33*/ "lecture/écriture",
/*34*/ "Emulation des manettes",
/*35*/ "fonction émulée",
/*36*/ "Rétablir la disposition standard",
/*37*/ "Sauver la disposition personnalisée",
/*38*/ "Rétablir les valeurs par défaut",
/*39*/ "[décharger]",
/*40*/ "[suite...]",
/*41*/ "[retour au début...]"
},

{
/*00*/ " DCMO5 v11 - Thomson MO5 emulator",
/*01*/ "About DCMO5",
/*02*/ "Version : 11.0 - December 12th, 2007",
/*03*/ "Author : Daniel Coulom",
/*04*/ "Web site : http://dcmo5.free.fr",
/*05*/ " ",
/*06*/ "This program comes with ABSOLUTELY NO WARRANTY.",
/*07*/ "This is free software, and you are welcome",
/*08*/ "to redistribute it under certain conditions.",
/*09*/ "More details in dcmo5v11-licence.txt",
/*10*/ "Error",
/*11*/ "No tape currently loaded",
/*12*/ "End of tape, automatic rewind",
/*13*/ "Tape write error",
/*14*/ "Keyboard layout",
/*15*/ "emulated MO5 key",
/*16*/ "Press a key to show the ",
/*17*/ "Click a button in the picture to make a change.",
/*18*/ "none",
/*19*/ "",
/*20*/ "Keyboard shortcuts :",
/*21*/ "- ESC= reset MO5 program",
/*22*/ "- PAUSE= stop 6809 processor",
/*23*/ " ",
/*24*/ "Please check the documentation for more information.",
/*25*/ "",
/*26*/ "Display speed",
/*27*/ "English",
/*28*/ "Processor speed",
/*29*/ "Settings",
/*30*/ "Keyboard",
/*31*/ "Joysticks",
/*32*/ "read only",
/*33*/ "read/write",
/*34*/ "Joystick emulation",
/*35*/ "emulated function",
/*36*/ "Restore standard layout",
/*37*/ "Save customized layout",
/*38*/ "Restore default values",
/*39*/ "[unload]",
/*40*/ "[next...]",
/*41*/ "[back to first...]"
}};
