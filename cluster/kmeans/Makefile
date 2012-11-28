COMPILER=G++

# todo: object files into output path, processing c / c++ files in the same time (?), nested directories for source files (?)
C = cpp
PROGRAM_NAME = cluster
OUTPUT_PATH = bin
SOURCE_PATH = .
OBJECT_PATH = objs
EXE = $(OUTPUT_PATH)/$(PROGRAM_NAME)
DEBUG = 0

ifeq ($(COMPILER), G++)
  ifeq ($(OS),Windows_NT)
    OBJ = obj
  else
    OBJ = o
  endif
  ifeq ($(DEBUG), 1)
  COPT = -DCS_DEBUG=2 -O0 -g3 -pipe -Wall -fstack-protector
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
  INCLUDES = -I../../staging
# LIBS = -lgc
#  LIBS = -lluaplus -lboost_system -lboost_thread
  LIBS = -lboost_system -lboost_thread -lboost_program_options -lboost_filesystem -lzmq -lmsgpack -lglog
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

OBJS = $(OBJECT_PATH)/master.o $(OBJECT_PATH)/main.o $(OBJECT_PATH)/config.o ../objs/document.o $(OBJECT_PATH)/aside.o $(OBJECT_PATH)/input.o $(OBJECT_PATH)/netinput.o $(OBJECT_PATH)/calculater.o \
$(OBJECT_PATH)/centerpicker.o $(OBJECT_PATH)/vector.o $(OBJECT_PATH)/cluster.o

all: $(OBJS)
	@echo Linking...
	$(CCMD) $(LDFLAGS) $^ $(LIBS) $(EXEFLAG) $(EXE)
	-cp $(PROGRAM_NAME).sh $(OUTPUT_PATH)/

clean:
	-rm -f $(EXE)
	-rm -f $(OBJS)

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

$(OBJECT_PATH)/master.o: $(SOURCE_PATH)/master.cpp $(SOURCE_PATH)/master.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/aside.o: $(SOURCE_PATH)/aside.cpp $(SOURCE_PATH)/aside.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/netinput.o: $(SOURCE_PATH)/netinput.cpp $(SOURCE_PATH)/netinput.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/input.o: $(SOURCE_PATH)/input.cpp $(SOURCE_PATH)/input.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/calculater.o: $(SOURCE_PATH)/calculater.cpp $(SOURCE_PATH)/calculater.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) -Wno-unused-result $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/centerpicker.o: $(SOURCE_PATH)/centerpicker.cpp $(SOURCE_PATH)/centerpicker.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/cluster.o: $(SOURCE_PATH)/cluster.cpp $(SOURCE_PATH)/cluster.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/vector.o: $(SOURCE_PATH)/vector.cpp $(SOURCE_PATH)/vector.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

../objs/document.o: ../document.cpp ../document.hpp $(GLOBAL_DEPEND)
	@echo compiling ../document.cpp
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) ../document.cpp $(OBJFLAG) ../objs/document.o

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
