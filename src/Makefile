# kernel main makefile
# (c) Gabor Nagy, 2006-2008

# do not export variables by default
unexport

# ===== config ===================================

# choose os (win/unix), architecture (eg. ia32) and shell
OS=win
ARCH=ia32
SH=clsh

# silent mode
MODE=silent

# ===== common variables =========================

export CC=gcc
export MAKE=make
export LD=ld
export LDFLAGS=--allow-multiple-definition

ifeq ($(MODE),silent)
  MAKE += -s
endif

# ===== os-dependent variables ===================

ifeq ($(OS), win)

  RM=cmd /C "del /S /Q >NUL 2>NUL"
  export AS=nasm

else

ifeq ($(OS), unix)

  export RM=rm
  export RMFLAGS=
  export AS=nasm

endif

endif

# ===== dependencies =============================

# -----------------------------------------------------------------------------------------
# ===> use 'make all' or 'make' if you want to rebuild everything
# -----------------------------------------------------------------------------------------

.PHONY : all
all: cleanall boot depend kernel

# -----------------------------------------------------------------------------------------
# ===> use 'make kernel' if you want to rebuild binaries only built from modified sources
#      1) Use when you make modifications to C sources but DON'T change any #include lines.
#      2) If you change any #include, type 'make dep' then 'make kernel'. It's a lot faster
#         than 'make all'
# -----------------------------------------------------------------------------------------

.PHONY : kernel
kernel:
	$(MAKE) -C arch/$(ARCH) -f make.d
	$(MAKE) -C lib -f make.d
	$(MAKE) -C shell/$(SH) -f make.d
	$(MAKE) -C sys -f make.d
	$(LD) $(LDFLAGS) -T arch/$(ARCH)/link.ld -o kernel arch/arch.o lib/lib.o shell/shell.o sys/sys.o

# -----------------------------------------------------------------------------------------
# ===> use 'make boot' if you want to rebuild only the boot loader
# -----------------------------------------------------------------------------------------

.PHONY : boot
boot:
	$(AS) -fbin -o nbl boot/nbl.s

# -----------------------------------------------------------------------------------------
# ===> use 'make dep' or 'make depend' if you want to rebuild only dependencies
# -----------------------------------------------------------------------------------------

.PHONY : dep depend
dep depend:
	$(MAKE) -C arch/$(ARCH) dep -f make.d
	$(MAKE) -C lib dep -f make.d
	$(MAKE) -C shell/$(SH) dep -f make.d
	$(MAKE) -C sys dep -f make.d

# -----------------------------------------------------------------------------------------
# ===> use 'make cleanall' if you want to remove everything but clean sources and makefiles
# -----------------------------------------------------------------------------------------

.PHONY : cleanall
cleanall: clean
	@$(RM) nbl
	@$(RM) kernel
	@$(RM) kernel.img

# -----------------------------------------------------------------------------------------
# ===> 'make clean' is the same as 'make cleanall' but preserves kernel image
# -----------------------------------------------------------------------------------------

.PHONY : clean
clean:

ifeq ($(OS), win)

	@cmd /C "del *.o /S /Q >NUL 2>NUL"
	@cmd /C "del depend.d /S /Q >NUL 2>NUL"

else

ifeq ($(OS), unix)

	@$(MAKE) -C arch/$(ARCH) clean -f make.d
	@$(MAKE) -C sys clean -f make.d

endif

endif

# -----------------------------------------------------------------------------------------
# ===> use 'make floppy' to create bootable floppy image
# -----------------------------------------------------------------------------------------

.PHONY : floppy
floppy:
ifeq ($(OS), win)

	copy /b nbl + map_sec + kernel kernel.img

else

ifeq ($(OS), unix)

	@echo FIXME!

endif

endif