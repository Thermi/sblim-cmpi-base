# Makefile for SBLIM Base Instrumentation using the
# Common Manageability Programming Interface

include setting.cmpi

.PHONY: all testfiles install clean uninstall

#------------------------------------------------------------------------------#

# insert only when using the sysman module
#SYSMAN = 1

ifdef SYSMAN
SYSMAN_O = sysman.o
CFLAG_SYSMAN = -DMODULE_SYSMAN

KFLAGS = -c -O -g -D__KERNEL__ -DMODULE -Wall -I/lib/modules/`uname -r`/include/build

# IMPORTANT: if you use an SMP kernel, uncomment the following line
KFLAGS += -D__SMP__ -DSMP

endif

#------------------------------------------------------------------------------#

# define the Hardware Platform the compile will run on
HW=$(shell ./platform.sh)

# No changes necessary below this line

# support CIM Schema Version 2.7
CFLAGS=-Wall -g -I. -I$(CIMOMINC) -D_COMPILE_UNIX -D$(HW) -DDEBUG -DNOEVENTS $(CFLAG_SYSMAN) -fPIC
# support CIM Schema Version 2.6
#CFLAGS=-Wall -g -I. -I$(CIMOMINC) -D_COMPILE_UNIX -DDEBUG -DNOEVENTS -DCIM26COMPAT $(CFLAG_SYSMAN)

LDFLAGS=-L. -L$(CIMOMLIB) -shared -lpthread -lcmpiOSBase_Common


lib%.so: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $?


all: 	testfiles \
	libdmiinfo.so \
	dmitest \
	libcmpiOSBase_Common.so \
	libcmpiOSBase_ComputerSystemProvider.so \
	libcmpiOSBase_OperatingSystemProvider.so \
	libcmpiOSBase_UnixProcessProvider.so \
	libcmpiOSBase_ProcessorProvider.so \
	libcmpiOSBase_BaseBoardProvider.so \
	libcmpiOSBase_RunningOSProvider.so \
	libcmpiOSBase_OSProcessProvider.so \
	libcmpiOSBase_CSProcessorProvider.so \
	libcmpiOSBase_CSBaseBoardProvider.so



libdmiinfo.so: LDFLAGS=-shared
dmitest: LDFLAGS=-L. -ldmiinfo


libcmpiOSBase_Common.so: cmpiOSBase_Common.c \
			 OSBase_Common.c
	$(CC) $(CFLAGS) -shared -nostartfiles -o $@ $^


libcmpiOSBase_ComputerSystemProvider.so: cmpiOSBase_ComputerSystemProvider.c \
					 cmpiOSBase_ComputerSystem.c \
					 OSBase_ComputerSystem.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


libcmpiOSBase_OperatingSystemProvider.so: cmpiOSBase_OperatingSystemProvider.c \
					  cmpiOSBase_OperatingSystem.c \
					  OSBase_OperatingSystem.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lpthread -lind_helper -nostartfiles -o $@ $^


libcmpiOSBase_UnixProcessProvider.so: cmpiOSBase_UnixProcessProvider.c \
				      cmpiOSBase_UnixProcess.c \
				      OSBase_UnixProcess.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


libcmpiOSBase_ProcessorProvider.so: cmpiOSBase_ProcessorProvider.c \
				    cmpiOSBase_Processor.c \
				    OSBase_Processor.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


libcmpiOSBase_BaseBoardProvider.so: cmpiOSBase_BaseBoardProvider.c \
				    cmpiOSBase_BaseBoard.c \
				    OSBase_BaseBoard.c
	$(CC) $(CFLAGS) $(LDFLAGS) -ldmiinfo -o $@ $^


#------------------------------------------------------------------------------#

sysman.o: sysman_pid.o
	$(LD) -r $^ -o $@
sysman_pid.o: sysman_pid.c
	$(CC) $(KFLAGS) $^ -o $@

#------------------------------------------------------------------------------#


install: all
	install libdmiinfo.so $(COMMONLIB)
	install libcmpiOSBase_Common.so $(COMMONLIB)
	install libcmpiOSBase_*Provider.so $(CIMOMLIB)
	install -m 644 cmpiOSBase_*.h $(COMMONINC)
	install -m 644 OSBase_*.h $(COMMONINC)
	install -m 644 dmiinfo.h $(COMMONINC)
ifdef SYSMAN
	install sysman.o $(CIMOMLIB)
endif
	$(MAKE) -C mof -f $(MOFMAKEFILE) install


testfiles:	
	@[ -d $(CIMOMLIB) ] || ( echo directory $(CIMOMLIB) does not exist && false)
	@[ -d $(CIMOMINC) ] || ( echo directory $(CIMOMINC) does not exist - please create manually && false)
	@[ -d $(CIMOMMOF) ] || ( echo directory $(CIMOMMOF) does not exist - please create manually && false)
	@[ -d $(COMMONINC) ] || ( echo directory $(COMMONINC) does not exist - please create manually && false)
	@[ -d $(COMMONLIB) ] || ( echo directory $(COMMONLIB) does not exist - please create manually && false)

clean:
	$(RM) dmitest *.so *.o *~

uninstall:
	$(MAKE) -C mof -f $(MOFMAKEFILE) uninstall;
	$(RM) $(COMMONINC)/OSBase_Common.h \
	$(RM) $(COMMONINC)/OSBase_ComputerSystem.h \
	$(RM) $(COMMONINC)/OSBase_OperatingSystem.h \
	$(RM) $(COMMONINC)/OSBase_UnixProcess.h \
	$(RM) $(COMMONINC)/OSBase_Processor.h \
	$(RM) $(COMMONINC)/OSBase_BaseBoard.h \
	$(RM) $(COMMONINC)/cmpiOSBase_Util.h \
	$(RM) $(COMMONINC)/cmpiOSBase_Common.h \
	$(RM) $(COMMONINC)/cmpiOSBase_ComputerSystem.h \
	$(RM) $(COMMONINC)/cmpiOSBase_OperatingSystem.h \
	$(RM) $(COMMONINC)/cmpiOSBase_UnixProcess.h \
	$(RM) $(COMMONINC)/cmpiOSBase_Processor.h \
	$(RM) $(COMMONINC)/cmpiOSBase_BaseBoard.h \
	$(RM) $(COMMONLIB)/libcmpiOSBase_Common.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_ComputerSystemProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_OperatingSystemProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_UnixProcessProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_ProcessorProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_BaseBoardProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_RunningOSProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_OSProcessProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_CSProcessorProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_CSBaseBoardProvider.so
