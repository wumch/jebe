
EXENAME		= convert

default: $(EXENAME)

PETSC_USE_DEBUG		= 0
PETSC_USE_COMPLEX	= 0
PETSC_USE_LOG		= 0

include ${SLEPC_DIR}/conf/slepc_common

CC			= /usr/bin/mpicxx
CFLAGS		= -fPIC -Wall -O3

INCLUDES	= -I../../staging
CLINKER		= $(CC)
CXXFLAGS	= $(CFLAGS)
CLEANFILES	= $(EXENAME)

PCC					= $(CC)
#PCC_LINKER			= $(CLINKER)
#PCC_FLAGS			= $(CFLAGS)
#PCC_LINKER_FLAGS	= -Wall -fPIC -Wl,-O3
CC_LINKER_SLFLAG	= -Wall -fPIC -Wl,-O3 -Wl,-rpath,
PETSC_CC_INCLUDES	+= $(INCLUDES)

$(EXENAME): $(EXENAME).o main.o chkopts
	-${CLINKER} -o $(EXENAME) $(EXENAME).o main.o ${SLEPC_LIB}
	${RM} $(EXENAME).o main.o
	g++ -I$(INCLUDES) -g3 -O3 view.cpp -o view
