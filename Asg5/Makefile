
WARN     = -Wall -Wextra -Wold-style-cast
GPP      = g++ -std=gnu++14 -g -O0 ${WARN}

DEPFILE  = Makefile.dep
HEADERS  = sockets.h protocol.h logstream.h
CPPLIBS  = sockets.cpp protocol.cpp 
CPPSRCS  = ${CPPLIBS} cix.cpp cixd.cpp
LIBOBJS  = ${CPPLIBS:.cpp=.o}
CIXOBJS  = cix.o ${LIBOBJS}
CIXDOBJS = cixd.o ${LIBOBJS}
OBJECTS  = ${CIXOBJS} ${CIXDOBJS}
EXECBINS = cix cixd
LISTING  = Listing.ps
SOURCES  = ${HEADERS} ${CPPSRCS} Makefile

all: ${DEPFILE} ${EXECBINS}

cix: ${CIXOBJS}
	${GPP} -o $@ ${CIXOBJS}

cixd: ${CIXDOBJS}
	${GPP} -o $@ ${CIXDOBJS}

%.o: %.cpp
	${GPP} -c $<
	- cpplint.py.perl $<

ci: ${SOURCES}
	- checksource ${SOURCES}
	- cpplint.py.perl ${CPPSRCS}
	- cid + ${SOURCES}

lis: all ${SOURCES} ${DEPFILE}
	mkpspdf ${LISTING} ${SOURCES} ${DEPFILE}

clean:
	- rm ${LISTING} ${LISTING:.ps=.pdf} ${OBJECTS} Makefile.dep

spotless: clean
	- rm ${EXECBINS}

submit :
	submit cmps109-wm.u17 asg5 ${SOURCES} README PARTNER

dep:
	- rm ${DEPFILE}
	make --no-print-directory ${DEPFILE}

${DEPFILE}:
	${GPP} -MM ${CPPSRCS} >${DEPFILE}

again: ${SOURCES}
	make --no-print-directory spotless ci all lis

include ${DEPFILE}

