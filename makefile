# Makefile for SBLIM Base Instrumentation using the 
# Common Manageability Programming Interface

include setting.cmpi

# define the Hardware Platform the compile will run on
HW=$(shell ./platform.sh)

# insert only when using the sysman module
#SYSMAN = 1

ifdef SYSMAN
SYSMAN_O = sysman.o
CFLAG_SYSMAN = -DMODULE_SYSMAN

KFLAGS = -c -O -g -D__KERNEL__ -DMODULE -Wall -I/lib/modules/`uname -r`/include/build

# IMPORTANT: if you use an SMP kernel, uncomment the following line
KFLAGS += -D__SMP__ -DSMP

endif


# No changes necessary below this line
# support CIM Schema Version 2.7
CFLAGS=-Wall -I . -I $(CIMOMINC) -D_COMPILE_UNIX -D$(HW) -DDEBUG -DNOEVENTS $(CFLAG_SYSMAN)
# support CIM Schema Version 2.6
#CFLAGS=-Wall -I . -I $(CIMOMINC) -D_COMPILE_UNIX -DDEBUG -DNOEVENTS -DCIM26COMPAT $(CFLAG_SYSMAN)
LDFLAGS=-L . -L$(CIMOMLIB) -shared -lpthread -lCimBase -lcmpiprovsup

lib%.so: %.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $?

all: 	test \
	libCimBase.so \
	libcmpiprovsup.so \
	libcmpiLinux_ComputerSystem.so \
	libcmpiLinux_OperatingSystem.so \
	libcmpiLinux_UnixProcess.so \
	libcmpiLinux_Processor.so \
	libcmpiLinux_RunningOS.so \
	libcmpiLinux_OSProcess.so \
	libcmpiLinux_CSProcessor.so


sysman.o: sysman_pid.o
	$(LD) -r $^ -o $@

sysman_pid.o: sysman_pid.c
	$(CC) $(KFLAGS) $^ -o $@


libCimBase.so: CimBase.c mlogsup.c
	$(CC) $(CFLAGS) -shared -o $@ $^

libcmpiprovsup.so: cmpiprovsup.c
	$(CC) $(CFLAGS) -shared -o $@ $^

install: all
	install libCimBase.so $(CIMOMLIB)
	install libcmpiprovsup.so $(CIMOMLIB)
	install libcmpiLinux_*.so $(CIMOMLIB)
ifdef SYSMAN
	install sysman.o $(CIMOMLIB)
endif
	install -m 644 mlogsup.h $(CIMOMINC)
	install -m 644 cimibase.h $(CIMOMINC)
	install -m 644 cmpiprovsup.h $(CIMOMINC)
	$(MAKE) -C mof -f makefile.cmpi install

test:
	@[ -d $(CIMOMLIB) ] || ( echo $(CIMOMLIB) does not exist && false)
	@[ -d $(CIMOMINC) ] || ( echo $(CIMOMINC) does not exist && false)
	@[ -d $(CIMOMMOF) ] || ( echo $(CIMOMMOF) does not exist && false)

clean:
	$(RM)	*.so *.o

uninstall: 
	$(MAKE) -C mof -f makefile.cmpi uninstall;
	$(RM) $(CIMOMINC)/cimibase.h \
	$(RM) $(CIMOMINC)/cmpiprovsup.h \
	$(RM) $(CIMOMINC)/mlogsup.h \
	$(RM) $(CIMOMLIB)/libCimBase.so \
	$(RM) $(CIMOMLIB)/libcmpiprovsup.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_ComputerSystem.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_OperatingSystem.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_UnixProcess.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_Processor.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_RunningOS.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_OSProcess.so \
	$(RM) $(CIMOMLIB)/libcmpiLinux_CSProcessor.so

