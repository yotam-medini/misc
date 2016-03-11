# 

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

LIBDIR=/usr/lib/i386-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

wobj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

obj.d/ratgeo-test.o: ratgeo.cc
	@mkdir -p $(@D)
	g++ -c -DTEST=1 ${CFLAGS} -o $@ $<

${BINDIR}/ratgeo-test: obj.d/ratgeo-test.o
	@mkdir -p $(@D)
	g++ -g -o $@ $< ${LIBGMP}

run-ratgeo-test: ${BINDIR}/ratgeo-test
	$< intersection 1 0  2 0  0 1  0 2
	$< intersection 2 2  1 2  3 2  1 2  1 2  2 2  1 2  3 2
	$< intersection -1 -1  1 0  3 3  5 4

clean:
	rm -f ${OBJS}

-include ${DEPS}


