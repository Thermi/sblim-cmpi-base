# Makefile for SBLIM Base Instrumentation using the
# Common Manageability Programming Interface

include setting.cmpi

# define the Hardware Platform the compile will run on
HW=$(shell ./platform.sh)


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


# No changes necessary below this line

# support CIM Schema Version 2.7
CFLAGS=-Wall -I . -I $(CIMOMINC) -D_COMPILE_UNIX -D$(HW) -DDEBUG -DNOEVENTS $(CFLAG_SYSMAN)
# support CIM Schema Version 2.6
#CFLAGS=-Wall -I . -I $(CIMOMINC) -D_COMPILE_UNIX -DDEBUG -DNOEVENTS -DCIM26COMPAT $(CFLAG_SYSMAN)

LDFLAGS=-L . -L$(CIMOMLIB) -shared -lpthread -lOSBase_Common -lcmpiOSBase_Common


lib%.so: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $?


all: 	test \
	libOSBase_Common.so \
	libcmpiOSBase_Common.so \
	libOSBase_ComputerSystem.so \
	libcmpiOSBase_ComputerSystem.so \
	libcmpiOSBase_ComputerSystemProvider.so \
	libOSBase_OperatingSystem.so \
	libcmpiOSBase_OperatingSystem.so \
	libcmpiOSBase_OperatingSystemProvider.so \
	libOSBase_UnixProcess.so \
	libcmpiOSBase_UnixProcess.so \
	libcmpiOSBase_UnixProcessProvider.so \
	libOSBase_Processor.so \
	libcmpiOSBase_Processor.so \
	libcmpiOSBase_ProcessorProvider.so \
	libcmpiOSBase_RunningOSProvider.so \
	libcmpiOSBase_OSProcessProvider.so \
	libcmpiOSBase_CSProcessorProvider.so


libOSBase_Common.so: OSBase_Common.c
	$(CC) $(CFLAGS) -shared -o $@ $^

libcmpiOSBase_Common.so: cmpiOSBase_Common.c
	$(CC) $(CFLAGS) -L . -lOSBase_Common -shared -o $@ $^


libcmpiOSBase_ComputerSystem.so: cmpiOSBase_ComputerSystem.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lOSBase_ComputerSystem -o $@ $^

libcmpiOSBase_ComputerSystemProvider.so: cmpiOSBase_ComputerSystemProvider.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lcmpiOSBase_ComputerSystem -o $@ $^


libcmpiOSBase_OperatingSystem.so: cmpiOSBase_OperatingSystem.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lOSBase_OperatingSystem -o $@ $^

libcmpiOSBase_OperatingSystemProvider.so: cmpiOSBase_OperatingSystemProvider.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lcmpiOSBase_OperatingSystem -o $@ $^


libcmpiOSBase_UnixProcess.so: cmpiOSBase_UnixProcess.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lOSBase_UnixProcess -o $@ $^

libcmpiOSBase_UnixProcessProvider.so: cmpiOSBase_UnixProcessProvider.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lcmpiOSBase_UnixProcess -o $@ $^


libcmpiOSBase_Processor.so: cmpiOSBase_Processor.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lOSBase_Processor -o $@ $^

libcmpiOSBase_ProcessorProvider.so: cmpiOSBase_ProcessorProvider.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lcmpiOSBase_Processor -o $@ $^


#------------------------------------------------------------------------------#

sysman.o: sysman_pid.o
	$(LD) -r $^ -o $@
sysman_pid.o: sysman_pid.c
	$(CC) $(KFLAGS) $^ -o $@

#------------------------------------------------------------------------------#


install: all
	install libOSBase_*.so $(CIMOMLIB)
	install libcmpiOSBase_*.so $(CIMOMLIB)
	install -m 644 OSBase_Common.h $(CIMOMINC)
	install -m 644 cmpiOSBase_Common.h $(CIMOMINC)
ifdef SYSMAN
	install sysman.o $(CIMOMLIB)
endif
	$(MAKE) -C mof -f makefile.cmpi install
	$(MAKE) -C mof -f makefile.pegasus install

test:
	@[ -d $(CIMOMLIB) ] || ( echo $(CIMOMLIB) does not exist && false)
	@[ -d $(CIMOMINC) ] || ( echo $(CIMOMINC) does not exist && false)
	@[ -d $(CIMOMMOF) ] || ( echo $(CIMOMMOF) does not exist && false)

clean:
	$(RM)	*.so *.o *~

uninstall:
	$(MAKE) -C mof -f makefile.cmpi uninstall;
	$(RM) $(CIMOMINC)/OSBase_Common.h \
	$(RM) $(CIMOMINC)/cmpiOSBase_Common.h \
	$(RM) $(CIMOMLIB)/libOSBase_Common.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_Common.so \
	$(RM) $(CIMOMLIB)/libOSBase_ComputerSystem.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_ComputerSystem.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_ComputerSystemProvider.so \
	$(RM) $(CIMOMLIB)/libOSBase_OperatingSystem.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_OperatingSystem.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_OperatingSystemProvider.so \
	$(RM) $(CIMOMLIB)/libOSBase_UnixProcess.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_UnixProcess.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_UnixProcessProvider.so \
	$(RM) $(CIMOMLIB)/libOSBase_Processor.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_Processor.so \
	$(RM) $(CIMOMLIB)/libcmpiOSBase_ProcessorProvider.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_RunningOS.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_OSProcess.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_CSProcessor.so
