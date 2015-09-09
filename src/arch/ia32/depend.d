arch.o: arch.c arch.h ../../sys/config.h types.h lowlevel.h prot.h \
  ../../sys/macro.h video.h memory.h keyboard.h
keyboard.o: keyboard.c keyboard.h ../../sys/config.h lowlevel.h types.h \
  video.h memory.h prot.h ../../sys/macro.h
lib.o: lib.c lib.h ../../lib/errno.h
lowlevel.o: lowlevel.c lowlevel.h types.h
memory.o: memory.c memory.h prot.h types.h ../../sys/config.h \
  ../../sys/macro.h
process.o: process.c process.h ../../sys/macro.h ../../sys/types.h \
  types.h
prot.o: prot.c arch.h ../../sys/config.h types.h lowlevel.h prot.h \
  ../../sys/macro.h video.h memory.h keyboard.h
strings.o: strings.c strings.h lowlevel.h types.h
video.o: video.c video.h ../../sys/config.h lowlevel.h types.h memory.h \
  prot.h ../../sys/macro.h
