# 

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

LIBDIR=/usr/lib/i386-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -std=c++20 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

BOOST_PO_LIB = -lboost_program_options
PNGXX_LIB = -lpng

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

${BINDIR}/sqrmagic: obj.d/sqrmagic.o
	@mkdir -p $(@D)
	g++ -g -o $@ $<

sqrmagic-test: ${BINDIR}/sqrmagic
	$< 1
	$< 2
	$< 3

bin/tictac: tictac.cc
	g++ ${CFLAGS} -o $@ tictac.cc

q${BINDIR}/longaltseq: obj.d/longaltseq.o
	@mkdir -p $(@D)
	g++ -g -o $@ $<

longaltseq-test: ${BINDIR}/longaltseq
	$< rand 3 3

${BINDIR}/permdup: obj.d/permdup.o
	@mkdir -p $(@D)
	g++ -g -o $@ $<

permdup-test: ${BINDIR}/permdup
	$< a
	$< ab
	$< aba
	$< ababc

${BINDIR}/nqueens: obj.d/nqueens.o
	@mkdir -p $(@D)
	g++ -g -o $@ $<

nqueens-test: ${BINDIR}/nqueens
	for n in 1 2 3 4 5 6 7 8; do \
	  $< $${n}; \
	done

${BINDIR}/minpali: obj.d/minpali.o
	@mkdir -p $(@D)
	g++ -g -o $@ $<

minpali-test: ${BINDIR}/minpali
	  $< anoxinixon

${BINDIR}/nparen: obj.d/nparen.o
	@mkdir -p $(@D)
	g++ -g -o $@ $<

nparen-test: ${BINDIR}/nparen
	for n in 0 2 3 4 5 6 7 8; do \
	  $< $${n}; \
	done

MIDIFILE_DIR = ${HOME}/pub/midifile
MIDIFILE_INC = ${MIDIFILE_DIR}/include
MIDIFILE_LIB = ${MIDIFILE_DIR}/lib
obj.d/midiconv.o: midiconv.cc
	@mkdir -p $(@D)
	g++ -c -g -I${MIDIFILE_INC} -o $@ $<

${BINDIR}/midiconv: obj.d/midiconv.o
	@mkdir -p $(@D)
	g++ -g -L${MIDIFILE_LIB} -o $@  $< -lmidifile
	ls -lG $@

03.mid:
	wget http://parischoralsociety.free.fr/stjean/03.mid

midiconv-test: ${BINDIR}/midiconv 03.mid
	${BINDIR}/midiconv -tv 1 1.5 20 -tv -1 0.6 -10 /tmp/esperanza.midi /tmp/ts.midi
	ls -l /tmp/ts.midi
	${BINDIR}/midiconv -tv 1 1.5 20 -tv -1 0.6 -10 03.mid 03-sop.midi
	ls -lGt 03-sop.midi

obj.d/pngcomb.o: pngcomb.cc
	@mkdir -p $(@D)
	g++ -c -g -I${MIDIFILE_INC} -o $@ $<

${BINDIR}/pngcomb: obj.d/pngcomb.o
	@mkdir -p $(@D)
	g++ -g  -o $@ $< ${BOOST_PO_LIB} ${PNGXX_LIB}
	ls -lG $@

pngcomb-test: ${BINDIR}/pngcomb Makefile
	true

obj.d/nofear2tex.o: nofear2tex.cc
	@mkdir -p $(@D)
	g++ -c -I/usr/include/libxml2 ${CFLAGS} -o $@ $<

LXMLDIR=/home/yotam/pub/libxml2/.libs
#	g++ -g -Wl,-L${LXMLDIR} -Wl,-R${LXMLDIR} -o $@  $< -lxml2
${BINDIR}/nofear2tex: obj.d/nofear2tex.o Makefile
	@mkdir -p $(@D)
	g++ -g -o $@  $< -lxml2
	ldd $@; ls -lG $@

obj.d/litcharts2tex.o: litcharts2tex.cc
	@mkdir -p $(@D)
	g++ -g -c -I/usr/include/libxml2 ${CFLAGS} -o $@ $<

${BINDIR}/litcharts2tex: obj.d/litcharts2tex.o Makefile
	@mkdir -p $(@D)
	g++ -g -o $@  $< -lxml2
	ls -lG $@

clean:
	rm -f ${OBJS}

-include ${DEPS}
