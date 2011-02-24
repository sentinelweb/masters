# wxWindows Template App
# Lukasz Zalewski 2002/2006
# ------------------

###################### linux #################################
INCS =
LIBP = 

ARCH_CFLAGS = -D__3dNOW_A__ -D__SSE__
#The code below should be used rather than the
#specific 2.4 version one. With this one we ensure
#that we run the latest release of wxGTK rather
#than a specific one:

WX_LIBS = `wx-config --libs --gl-libs`
WX_FLAGS = `wx-config --cxxflags`

LIBS = $(WX_LIBS)
ARCH_CFLAGS = -D__3dNOW_A__ -D__SSE__
EXES = window

CFLAGS	= $(ARCH_CFLAGS) $(WX_FLAGS) -Wall -Wno-unused -Wno-reorder \
	-O3 -mtune=i686 -march=i686 -fomit-frame-pointer -fforce-addr 


# ------------------

all : clean $(EXES)

clean :
	find -name "*.o" -exec rm {} \;
	rm -f ${EXES} -R

# ------------------

window : window.o Histogram.o
	g++ window.o Histogram.o -o window $(ARCH_CFLAGS) $(LIBS) $(CFLAGS)
	
:
	g++ Histogram.o Histogram.h -o window $(ARCH_CFLAGS) $(LIBS) $(CFLAGS)
	
window.o : window.cpp window.h masks.h
	g++ -c window.cpp $(ARCH_CFLAGS) $(INCS) $(WX_FLAGS)

# ------------------
