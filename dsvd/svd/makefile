
EXENAME		= svd

default: $(EXENAME)

DEBUG = 0

ifneq ($(DEBUG), 0)
PETSC_USE_DEBUG		= 2
endif

PETSC_USE_COMPLEX	= 0
PETSC_USE_LOG		= 0

include ${SLEPC_DIR}/conf/slepc_common

CC			= /usr/bin/mpicxx
CFLAGS		= -DCS_DEBUG=$(DEBUG) -DPETSC_USE_DEBUG=$(PETSC_USE_DEBUG) -fPIC -Wall
ifeq ($(DEBUG), 0)
CFLAGS		+= -DNDEBUG -fno-inline -O3
else
CFLAGS		+= -O0 -g3
endif

CS_INCLUDES	= -I/data/fsuggest/staging/ccpp
CS_LIBS		= -lboost_system -lboost_program_options -lboost_filesystem -lglog
CLINKER		= $(CC)
CXXFLAGS	= $(CFLAGS)
CLEANFILES	= $(EXENAME)

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

$(EXENAME): $(EXENAME).o main.o config.o aside.o chkopts
	-${CLINKER} -o $(EXENAME) $(EXENAME).o aside.o main.o config.o ${SLEPC_LIB}
