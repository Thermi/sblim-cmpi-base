# Makefile for testing the SBLIM Base Instrumentation using the 
# SBLIM Test Suite package

# change the following variables to fit your system

TESTSUITE=$(HOME)/sblim/testsuite
TESTSUITECIM=$(TESTSUITE)/cim
TESTSUITESYSTEM=$(TESTSUITE)/system/linux

all:

clean:

install:
	@[ -d $(TESTSUITE) ] || ( echo $(TESTSUITE) does not exist && false)
	@[ -d $(TESTSUITECIM) ] || ( echo $(TESTSUITECIM) does not exist && false)
	@[ -d $(TESTSUITESYSTEM) ] || ( echo $(TESTSUITESYSTEM) does not exist && false)
	$(foreach FILE, $(shell ls -1 *.sh), install -m 755 $(FILE) $(TESTSUITE); )
	$(foreach FILE, $(shell ls -1 cim/*.cim), install -m 644 $(FILE) $(TESTSUITECIM); )
	$(foreach FILE, $(shell ls -1 system/linux/*.system), install -m 644 $(FILE) $(TESTSUITESYSTEM); )
	$(foreach FILE, $(shell ls -1 system/linux/*.pl), install -m 755 $(FILE) $(TESTSUITESYSTEM); )
	$(foreach FILE, $(shell ls -1 system/linux/*.sh), install -m 755 $(FILE) $(TESTSUITESYSTEM); )

uninstall:
	$(foreach FILE, $(shell find *.sh -printf '%f\n'), $(RM) $(TESTSUITE)/$(FILE); )
	$(foreach FILE, $(shell find cim/*.cim -printf '%f\n'), $(RM) $(TESTSUITECIM)/$(FILE); )
	$(foreach FILE, $(shell find system/linux/*.system -printf '%f\n'), $(RM) $(TESTSUITESYSTEM)/$(FILE); )
	$(foreach FILE, $(shell find system/linux/*.pl -printf '%f\n'), $(RM) $(TESTSUITESYSTEM)/$(FILE); )
	$(foreach FILE, $(shell find system/linux/*.sh -printf '%f\n'), $(RM) $(TESTSUITESYSTEM)/$(FILE); )

test:
	@[ -d $(TESTSUITE) ] || ( echo $(TESTSUITE) does not exist && false)
	$(foreach FILE, $(shell ls -1 *.sh), cd $(TESTSUITE); ./$(FILE); cd -; )
