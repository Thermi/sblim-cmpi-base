# Makefile for SBLIM Base Instrumentation using the
# Common Manageability Programming Interface

include setting.cmpi

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

ifdef PEGASUS
  ifndef PEGASUS_HOME
    ERROR = pegasus_home_undefined
pegasus_home_undefined:
	@ echo PEGASUS_HOME environment variable must be set to the Pegasus installation directory
	@ exit 1
  endif
endif

# define the Hardware Platform the compile will run on
HW=$(shell ./platform.sh)

# No changes necessary below this line

# support CIM Schema Version 2.7
CFLAGS=-Wall -g -I. -I$(CIMOMINC) -D_COMPILE_UNIX -D$(HW) -DDEBUG -DNOEVENTS $(CFLAG_SYSMAN)
# support CIM Schema Version 2.6
#CFLAGS=-Wall -g -I. -I$(CIMOMINC) -D_COMPILE_UNIX -DDEBUG -DNOEVENTS -DCIM26COMPAT $(CFLAG_SYSMAN)

LDFLAGS=-L. -L$(CIMOMLIB) -shared -lpthread -lcmpiOSBase_Common


lib%.so: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $?


all: 	test \
	libcmpiOSBase_Common.so \
	libcmpiOSBase_ComputerSystemProvider.so \
	libcmpiOSBase_OperatingSystemProvider.so \
	libcmpiOSBase_UnixProcessProvider.so \
	libcmpiOSBase_ProcessorProvider.so \
	libcmpiOSBase_RunningOSProvider.so \
	libcmpiOSBase_OSProcessProvider.so \
	libcmpiOSBase_CSProcessorProvider.so



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
	$(CC) $(CFLAGS) $(LDFLAGS) -nostartfiles -o $@ $^


libcmpiOSBase_UnixProcessProvider.so: cmpiOSBase_UnixProcessProvider.c \
				      cmpiOSBase_UnixProcess.c \
				      OSBase_UnixProcess.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


libcmpiOSBase_ProcessorProvider.so: cmpiOSBase_ProcessorProvider.c \
				    cmpiOSBase_Processor.c \
				    OSBase_Processor.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


#------------------------------------------------------------------------------#

sysman.o: sysman_pid.o
	$(LD) -r $^ -o $@
sysman_pid.o: sysman_pid.c
	$(CC) $(KFLAGS) $^ -o $@

#------------------------------------------------------------------------------#


install: all
	install libcmpiOSBase_*.so $(CIMOMLIB)
	install -m 644 cmpiOSBase_*.h $(CIMOMINC)
	install -m 644 OSBase_*.h $(CIMOMINC)
ifdef SYSMAN
	install sysman.o $(CIMOMLIB)
endif
	$(MAKE) -C mof -f makefile.cmpi install
ifdef PEGASUS
	$(MAKE) -C mof -f makefile.pegasus install
endif

test:
	@[ -d $(CIMOMLIB) ] || ( echo $(CIMOMLIB) does not exist && false)
	@[ -d $(CIMOMINC) ] || ( echo $(CIMOMINC) does not exist && false)
	@[ -d $(CIMOMMOF) ] || ( echo $(CIMOMMOF) does not exist && false)

clean:
	$(RM)	*.so *.o *~

uninstall:
	$(MAKE) -C mof -f makefile.cmpi uninstall;
	$(RM) $(CIMOMINC)/OSBase_Common.h \
	$(RM) $(CIMOMINC)/OSBase_ComputerSystem.h \
	$(RM) $(CIMOMINC)/OSBase_OperatingSystem.h \
	$(RM) $(CIMOMINC)/OSBase_UnixProcess.h \
	$(RM) $(CIMOMINC)/OSBase_Processor.h \
	$(RM) $(CIMOMINC)/cmpiOSBase_Util.h \
	$(RM) $(CIMOMINC)/cmpiOSBase_Common.h \
	$(RM) $(CIMOMINC)/cmpiOSBase_ComputerSystem.h \
	$(RM) $(CIMOMINC)/cmpiOSBase_OperatingSystem.h \
	$(RM) $(CIMOMINC)/cmpiOSBase_UnixProcess.h \
	$(RM) $(CIMOMINC)/cmpiOSBase_Processor.h \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_Common.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_ComputerSystemProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_OperatingSystemProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_UnixProcessProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_ProcessorProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_RunningOSProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_OSProcessProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_CSProcessorProvider.so
