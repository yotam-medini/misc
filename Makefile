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

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

obj.d/ratgeo-test.o: ratgeo.cc
	@mkdir -p $(@D)
	g++ -c -DTEST=1 ${CFLAGS} -o $@ $<

${BINDIR}/ratgeo-test: obj.d/ratgeo-test.o
	@mkdir -p $(@D)
	g++ -g -o $@ $< ${LIBGMP}

ratgeo-test-run: ${BINDIR}/ratgeo-test
	$< ppdist2 4 0 0 3
	$< ppdist2 4 7  0 7   0 7  3 7
	$< lpdist2 0 0 1 0 3 1
	$< lpdist2 0 0 1 1 0 1
	$< lpside 0 0 1 1 0 1
	$< lpside 0 0 1 1 1 0
	$< lpside 0 0 1 1 2 2
	$< intersection 1 0  2 0  0 1  0 2
	$< intersection 2 2  1 2  3 2  1 2  1 2  2 2  1 2  3 2
	$< intersection -1 -1  1 0  3 3  5 4
	$< lsintersection 0 0  1 1  1 0  0 1
	$< lsintersection 0 0  1 1  -1 2  0 1
	$< ssintersection 0 0  1 1  1 0  0 1
	$< ssintersection 0 0  1 1  -1 2  0 1

clean:
	rm -f ${OBJS}

-include ${DEPS}


