# Makefile for SBLIM Base Instrumentation using the 
# Common Manageability Programming Interface

include setting.cmpi

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
CFLAGS=-Wall -I . -I $(ROOTDIRINC) -D_COMPILE_UNIX -DDEBUG -DNOEVENTS $(CFLAG_SYSMAN)
# support CIM Schema Version 2.6
#CFLAGS=-Wall -I . -I $(ROOTDIRINC) -D_COMPILE_UNIX -DDEBUG -DNOEVENTS -DCIM26COMPAT $(CFLAG_SYSMAN)
LDFLAGS=-L . -L$(ROOTDIRLIB) -shared -lpthread -lCimBase -lcmpiprovsup

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
	install libCimBase.so $(ROOTDIRLIB)
	install libcmpiprovsup.so $(ROOTDIRLIB)
	install libcmpiLinux_*.so $(ROOTDIRLIB)
ifdef SYSMAN
	install sysman.o $(ROOTDIRLIB)
endif
	install -m 644 mlogsup.h $(ROOTDIRINC)
	install -m 644 cimibase.h $(ROOTDIRINC)
	install -m 644 cmpiprovsup.h $(ROOTDIRINC)
	$(MAKE) -C mof -f makefile.cmpi install

test:
	@[ -d $(ROOTDIRLIB) ] || ( echo $(ROOTDIRLIB) does not exist && false)
	@[ -d $(ROOTDIRINC) ] || ( echo $(ROOTDIRINC) does not exist && false)
	@[ -d $(ROOTDIRMOF) ] || ( echo $(ROOTDIRMOF) does not exist && false)

clean:
	$(RM)	*.so *.o

uninstall: 
	$(MAKE) -C mof -f makefile.cmpi uninstall;
	$(RM) $(ROOTDIRINC)/cimibase.h \
	$(RM) $(ROOTDIRINC)/cmpiprovsup.h \
	$(RM) $(ROOTDIRINC)/mlogsup.h \
	$(RM) $(ROOTDIRLIB)/libCimBase.so \
	$(RM) $(ROOTDIRLIB)/libcmpiprovsup.so \
	$(RM) $(ROOTDIRLIB)/libcmpiLinux_ComputerSystem.so \
	$(RM) $(ROOTDIRLIB)/libcmpiLinux_OperatingSystem.so \
	$(RM) $(ROOTDIRLIB)/libcmpiLinux_UnixProcess.so \
	$(RM) $(ROOTDIRLIB)/libcmpiLinux_Processor.so \
	$(RM) $(ROOTDIRLIB)/libcmpiLinux_RunningOS.so \
	$(RM) $(ROOTDIRLIB)/libcmpiLinux_OSProcess.so \
	$(RM) $(ROOTDIRLIB)/libcmpiLinux_CSProcessor.so

