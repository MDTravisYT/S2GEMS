.PHONY:	clean

%.obj:	%.s
	./TOOL/asm68k.exe /g /o l+,v+ /k /l /w $<, $@
	
megadrive.bin: SRC/TEMP/vectors.obj SRC/TEMP/interrupt.obj SRC/TEMP/main.obj SRC/TEMP/sound.obj
	./TOOL/psylink.exe /m /p @linkmap.lnk,OUT/megadrive.gen,OUT/megadrive.sym,OUT/megadrive.map
	
sonic.obj: s2.asm
	./TOOL/asm68k.exe /g /o l+,v+ /k /l /w $<, ./$@