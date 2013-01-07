
EXENAME		= svd

default: $(EXENAME)

DEBUG = 0

ifneq ($(DEBUG), 0)
  PARAM_PETSC_USE_DEBUG = -DPETSC_USE_DEBUG=$(DEBUG)
endif

PETSC_USE_COMPLEX	= 0
PETSC_USE_LOG		= 0

include ${SLEPC_DIR}/conf/slepc_common

OBJPATH	= objs
EXEPATH	= bin
EXEFILE	= $(EXEPATH)/$(EXENAME)

CC			= /usr/bin/mpicxx
CFLAGS		= -DCS_DEBUG=$(DEBUG) $(PARAM_PETSC_USE_DEBUG) -fPIC -Wall -pipe -fstack-protector
LDFLAGS		= -pipe
ifeq ($(DEBUG), 0)
  CFLAGS		+= -DNDEBUG -O3 -march=core2 -finline -finline-small-functions -fomit-frame-pointer -m64 -fno-threadsafe-statics -Wno-unused-function -D_FORTIFY_SOURCE=2 -mcx16 -msahf -maes -mpclmul -mpopcnt -mavx --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=6144 -mtune=generic
  LDFLAGS		+= -Wl,-O3
else
  CFLAGS		+= -O0 -g3 -fno-inline
  LDFLAGS		+= -Wl,-O0 -g3 -rdynamic
endif

CS_INCLUDES	= -I../../staging
CS_LIBS		= -lboost_system -lboost_program_options -lboost_filesystem -lglog
CLINKER		= $(CC)
CXXFLAGS	= $(CFLAGS)
CLEANFILES	= $(EXEFILE)

PCC					= $(CC)
#PCC_LINKER			= $(CLINKER)
#PCC_FLAGS			= $(CFLAGS)
#PCC_LINKER_FLAGS	= -Wall -fPIC -Wl,-O3
ifeq ($(DEBUG), 0)
  CC_LINKER_SLFLAG	= -Wall -fPIC -Wl,-O3 -Wl,-rpath,
else
  CC_LINKER_SLFLAG	= -Wall -fPIC -Wl,-O0 -g3 -Wl,-rpath,
endif
PETSC_CC_INCLUDES	+= $(CS_INCLUDES)
PETSC_WITH_EXTERNAL_LIB += $(CS_LIBS)

OBJS = $(EXENAME).o main.o config.o aside.o
OBJFILES = $(OBJPATH)/$(EXENAME).o $(OBJPATH)/main.o $(OBJPATH)/config.o $(OBJPATH)/aside.o

$(OBJFILES): $(OBJS)
	@test -n "$(find ./ -maxdepth 1 -name '*.o')" && mv *.o $(OBJPATH)/

$(EXENAME): $(OBJS) chkopts
	-${CLINKER} -o $(EXEFILE) $(OBJS) ${SLEPC_LIB}
