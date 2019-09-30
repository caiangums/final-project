# EPOS Network Makefile

include ../../makedefs

SUBDIRS := ipv4 tstp
OBJS := $(subst .cc,.o,$(shell find *.cc | grep -v _test | grep -v _init))
INITS := $(subst .cc,.o,$(shell find *.cc | grep _init))

all:		$(SUBDIRS) $(LIBSYS) $(LIBINIT)

$(SUBDIRS):	FORCE
		(cd $@ && $(MAKE))

#$(LIBSYS):	$(LIBSYS)($(OBJS))

$(LIBINIT):	$(LIBINIT)($(INITS))

clean:
		make MAKE:="$(MAKECLEAN)" $(SUBDIRS)
		$(CLEAN) *.o *_test

FORCE:
