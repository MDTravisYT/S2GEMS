.PHONY:	clean

%.obj:	%.s
	./asm68k.exe /g /o l+,v+ /k /l /w $<, $@
	
megadrive.bin: sonic.obj
	./TOOL/psylink.exe /m /p @linkmap.lnk,megadrive.gen,megadrive.sym,megadrive.map
	
sonic.obj: s2.asm
	./asm68k.exe /g /o l+,v+ /k /l /w $<, ./$@