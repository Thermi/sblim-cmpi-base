# Makefile for testing the SBLIM Base Instrumentation using the 
# SBLIM Test Suite package

# change the following variables to fit your system

TESTSUITE=/home/heidineu/sblim/testsuite
TESTSUITECIM=$(TESTSUITE)/cim
TESTSUITESYSTEM=$(TESTSUITE)/system/linux


# in most cases it is not necessary to change the following stuff

all: 	test

install: all
	install test-cmpi-base.sh $(TESTSUITE)
	install cim/*.cim $(TESTSUITECIM)
	install system/linux/*.system $(TESTSUITESYSTEM)
	install system/linux/*.sh $(TESTSUITESYSTEM)
	install system/linux/*.pl $(TESTSUITESYSTEM)

test:
	@[ -d $(TESTSUITE) ] || ( echo $(TESTSUITE) does not exist && false)
	@[ -d $(TESTSUITECIM) ] || ( echo $(TESTSUITECIM) does not exist && false)
	@[ -d $(TESTSUITESYSTEM) ] || ( echo $(TESTSUITESYSTEM) does not exist && false)

uninstall: 
	$(RM) $(TESTSUITE)/test-cmpi-base.sh \
	$(RM) $(TESTSUITECIM)/Linux_ComputerSystem.cim \
	$(RM) $(TESTSUITECIM)/Linux_OperatingSystem.cim \
	$(RM) $(TESTSUITECIM)/Linux_UnixProcess.cim \
	$(RM) $(TESTSUITECIM)/Linux_Processor.cim \
	$(RM) $(TESTSUITECIM)/Linux_RunningOS.cim \
	$(RM) $(TESTSUITECIM)/Linux_OSProcess.cim \
	$(RM) $(TESTSUITECIM)/Linux_CSProcessor.cim \
	$(RM) $(TESTSUITESYSTEM)/Linux_ComputerSystem.system \
	$(RM) $(TESTSUITESYSTEM)/Linux_OperatingSystem.system \
	$(RM) $(TESTSUITESYSTEM)/Linux_UnixProcess.system \
	$(RM) $(TESTSUITESYSTEM)/Linux_UnixProcess.pl \
	$(RM) $(TESTSUITESYSTEM)/Linux_Processor.system \
	$(RM) $(TESTSUITESYSTEM)/Linux_Processor.pl \
	$(RM) $(TESTSUITESYSTEM)/createKeyFiles.sh \
	$(RM) $(TESTSUITESYSTEM)/Linux_RunningOS.system \
	$(RM) $(TESTSUITESYSTEM)/Linux_OSProcess.system \
	$(RM) $(TESTSUITESYSTEM)/Linux_CSProcessor.system

