# Makefile for "Always on top" program, for Watcom
# By Martin Panter

CXX = wpp386
CXXFLAGS = -wx -6r -fp6 -fpi87 -zv -oikl+nr -zq
LINK = wlink
LINKFLAGS =

EXENAME = aot
RESNAME = resource
#~ LIBS =
OBJS = main.obj

!ifdef __LOADDLL__
!  loaddll wpp386 wppd386.dll
!  loaddll wlink wlink.dll
!endif
 
all: .SYMBOLIC $(EXENAME).exe
exec: .SYMBOLIC $(EXENAME).exe
	$(EXENAME).exe

$(EXENAME).exe: $(OBJS) $(RESNAME).res
	$(LINK) SYS NT_WIN F {$(OBJS)} $(LINKFLAGS) OP RES=$(RESNAME).res N $@
# L {$(LIBS)}

CXX_CMD = $(CXX) -bg -bt=NT $[@ $(CXXFLAGS) -fo=$@
main.obj: main.cpp $(RESNAME).h
	$(CXX_CMD)

$(RESNAME).res: $(RESNAME).rc $(RESNAME).h main.ico
	wrc -r -bt=nt $[@ -fo=$@

clean: .PROCEDURE
	FOR %o IN ($(OBJS)) DO @del %o
	del $(RESNAME).res
	del $(EXENAME).exe
