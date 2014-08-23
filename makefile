# Project: dcmo5

objects = object/dcmo5main.o object/dc6809emul.o object/dcmo5boutons.o \
	object/dcmo5dialog.o object/dcmo5devices.o object/dcmo5emulation.o \
	object/dcmo5keyb.o object/dcmo5options.o object/dcmo5video.o

dcmo5 : $(objects)
	cc -o dcmo5 -lSDL -lSDL_ttf $(objects)

object/dcmo5main.o : source/dcmo5main.c source/dcmo5msg.h include/policettf.h include/dcmo5icon.h
	cc -c source/dcmo5main.c -o object/dcmo5main.o -O2

object/dc6809emul.o : source/dc6809emul.c
	cc -c source/dc6809emul.c -o object/dc6809emul.o -O2

object/dcmo5boutons.o : source/dcmo5boutons.c source/dcmo5boutons.h
	cc -c source/dcmo5boutons.c -o object/dcmo5boutons.o -O2

object/dcmo5dialog.o : source/dcmo5dialog.c
	cc -c source/dcmo5dialog.c -o object/dcmo5dialog.o -O2

object/dcmo5devices.o : source/dcmo5devices.c
	cc -c source/dcmo5devices.c -o object/dcmo5devices.o -O2

object/dcmo5emulation.o : source/dcmo5emulation.c include/dcmo5rom.h
	cc -c source/dcmo5emulation.c -o object/dcmo5emulation.o -O2

object/dcmo5keyb.o : source/dcmo5keyb.c source/dcmo5keyb.h
	cc -c source/dcmo5keyb.c -o object/dcmo5keyb.o -O2

object/dcmo5options.o : source/dcmo5options.c
	cc -c source/dcmo5options.c -o object/dcmo5options.o -O2

object/dcmo5video.o : source/dcmo5video.c
	cc -c source/dcmo5video.c -o object/dcmo5video.o -O2

clean :
	rm dcmo5 $(objects)