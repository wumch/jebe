COMPILER=G++

# todo: object files into output path, processing c / c++ files in the same time (?), nested directories for source files (?)
C = cpp
OUTPUT_PATH = bin
SOURCE_PATH = .
OBJECT_PATH = objs
EXE = $(OUTPUT_PATH)/fts
DEBUG = 0

ifeq ($(COMPILER), G++)
  ifeq ($(OS),Windows_NT)
    OBJ = obj
  else
    OBJ = o
  endif
  ifeq ($(DEBUG), 1)
  COPT = -DCS_DEBUG=1 -O0 -g3 -pipe -Wall -fstack-protector
  LDOPT = -g -rdynamic
  else
# -fno-rtti is no useable since boost::program_options...
  COPT = -DNDEBUG -march=core2 -O3 -fomit-frame-pointer -pipe -Wall -fno-strict-aliasing -m64 -fno-threadsafe-statics -D_FORTIFY_SOURCE=2 -mcx16 -msahf -maes -mpclmul -mpopcnt -mavx --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=6144 -mtune=generic
  LDOPT = $(COPT)
  endif
  CCMD = g++
  OBJFLAG = -o
  EXEFLAG = -o
 INCLUDES = 
  INCLUDES = -I../staging
# LIBS = -lgc
#  LIBS = -lluaplus -lboost_system -lboost_thread
  LIBS = -lboost_system -lboost_thread -lboost_program_options -lboost_filesystem -lzmq -lmsgpack -lleveldb
# LIBPATH = -L../gc/.libs
  LIBPATH =
  CPPFLAGS = $(COPT) $(INCLUDES)
  LDFLAGS = $(LDOPT) $(LIBPATH)
  DEP = dep
else
  OBJ = obj
  COPT = /O2
  CCMD = cl
  OBJFLAG = /Fo
  EXEFLAG = /Fe
# INCLUDES = /I..\\.includes
  INCLUDES =
# LIBS = ..\\.libs\\libgc.lib
  LIBS =
  CPPFLAGS = $(COPT) /DEBUG $(INCLUDES)
  LDFLAGS = /DEBUG
endif

OBJS = $(OBJECT_PATH)/index.o $(OBJECT_PATH)/handler.o $(OBJECT_PATH)/bus.o $(OBJECT_PATH)/worker.o $(OBJECT_PATH)/main.o $(OBJECT_PATH)/config.o

all: $(OBJS)
	@echo Linking...
	$(CCMD) $(LDFLAGS) $^ $(LIBS) $(EXEFLAG) $(EXE)
	-cp fts.sh $(OUTPUT_PATH)/

clean:
	-rm -f $(EXE)
	-find $(OBJECT_PATH) -name "*.$(OBJ)" | xargs rm -f

.PHONY: .preany

.preany:
	@echo preparing directories in $(OBJECT_PATH)
	@-find $(SOURCE_PATH) -type d -not -name '\.*' -and -not -samefile $(SOURCE_PATH) | awk '{sub("$(SOURCE_PATH)", "$(OBJECT_PATH)"); print;}' | xargs mkdir -p

GLOBAL_DEPEND = $(SOURCE_PATH)/predef.hpp

#$(SOURCE_PATH)/predef.hpp: $(SOURCE_PATH)/allocator.hpp

$(OBJECT_PATH)/main.o: $(SOURCE_PATH)/main.cpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/config.o: $(SOURCE_PATH)/config.cpp $(SOURCE_PATH)/config.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/worker.o: $(SOURCE_PATH)/worker.cpp $(SOURCE_PATH)/worker.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/bus.o: $(SOURCE_PATH)/bus.cpp $(SOURCE_PATH)/bus.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/handler.o: $(SOURCE_PATH)/handler.cpp $(SOURCE_PATH)/handler.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/index.o: $(SOURCE_PATH)/index.cpp $(SOURCE_PATH)/index.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/storage.o: $(SOURCE_PATH)/storage.cpp $(SOURCE_PATH)/storage.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

# this is template:
$(OBJECT_PATH)/{tpl}.o: $(SOURCE_PATH)/{tpl}.cpp $(SOURCE_PATH)/{tpl}.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

#rebuild: clean all
#rebuild is not entirely correct

#OBJS := $(patsubst $(SOURCE_PATH)%.$(C),$(OBJECT_PATH)%.$(OBJ),$(wildcard $(SOURCE_PATH)*/*.$(C)) $(wildcard $(SOURCE_PATH)*.$(C)))

#%.$(OBJ):$(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
#	@echo Compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) ...
#	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

#OBJS := $(OBJECT_PATH)/session.o $(OBJECT_PATH)/worker.o $(OBJECT_PATH)/master.o $(OBJECT_PATH)/portal.o $(OBJECT_PATH)/main.o

