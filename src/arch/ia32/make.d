# ia32

# init.o _MUST_ be the 1st in the following list, because it is the entry point

OBJS=init.o arch.o keyboard.o lowlevel.o memory.o prot.o strings.o video.o process.o lib.o
TARGET=../arch.o

# extra dependency
EXTRA_DEP=link.ld

include ../../common.d

