COMPILER=G++

# todo: object files into output path, processing c / c++ files in the same time (?), nested directories for source files (?)
C = cpp
OUTPUT_PATH = bin
SOURCE_PATH = .
OBJECT_PATH = objs
EXE_NAME = ets
EXE = $(OUTPUT_PATH)/$(EXE_NAME)
DEBUG = 0

ifeq ($(COMPILER), G++)
  ifeq ($(OS),Windows_NT)
    OBJ = obj
  else
    OBJ = o
  endif
  ifeq ($(DEBUG), 1)
  COPT = -DCS_DEBUG=2 -O0 -g3 -pipe -Wall -fstack-protector
  LDOPT = -Wl,-O0 -g3 -rdynamic
  else
# -fno-rtti is not useable since boost::program_options...
  COPT = -DNDEBUG -march=core2 -O3 -fomit-frame-pointer -pipe -Wall -fno-strict-aliasing -m64 -fno-threadsafe-statics -D_FORTIFY_SOURCE=2 -mcx16 -msahf -maes -mpclmul -mpopcnt -mavx --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=6144 -mtune=generic
  LDOPT = -Wl,-O3 $(COPT)
  endif
  CCMD = g++
  OBJFLAG = -o
  EXEFLAG = -o
 INCLUDES = 
  INCLUDES = -I../../staging
# LIBS = -lgc
#  LIBS = -lluaplus -lboost_system -lboost_thread
  LIBS = -lzmq -lmsgpack -lboost_system -lboost_thread -lboost_program_options -lboost_filesystem -lmongoclient -lglog
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

OBJS = $(OBJECT_PATH)/filter.o $(OBJECT_PATH)/master.o $(OBJECT_PATH)/main.o $(OBJECT_PATH)/config.o \
$(OBJECT_PATH)/aside.o $(OBJECT_PATH)/collector.o $(OBJECT_PATH)/transfer.o $(OBJECT_PATH)/mongoinput.o \
$(OBJECT_PATH)/input_document.o $(OBJECT_PATH)/wordmap.o ../objs/document.o

all: $(OBJS)
	@echo Linking...
	$(CCMD) $(LDFLAGS) $^ $(EXEFLAG) $(EXE) $(LIBS)
	-cp $(EXE_NAME).sh $(OUTPUT_PATH)/

clean:
	-rm -f $(EXE)
	-rm -f $(OBJS)

.PHONY: .preany

.preany:
	@echo preparing directories in $(OBJECT_PATH)
	@-find $(SOURCE_PATH) -type d -not -name '\.*' -and -not -samefile $(SOURCE_PATH) | awk '{sub("$(SOURCE_PATH)", "$(OBJECT_PATH)"); print;}' | xargs mkdir -p

GLOBAL_DEPEND = $(SOURCE_PATH)/predef.hpp

$(OBJECT_PATH)/main.o: $(SOURCE_PATH)/main.cpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/master.o: $(SOURCE_PATH)/master.cpp $(SOURCE_PATH)/master.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/filter.o: $(SOURCE_PATH)/filter.cpp $(SOURCE_PATH)/filter.hpp  $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/config.o: $(SOURCE_PATH)/config.cpp $(SOURCE_PATH)/config.hpp $(SOURCE_PATH)/predef.hpp
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/collector.o: $(SOURCE_PATH)/collector.cpp $(SOURCE_PATH)/collector.hpp
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/transfer.o: $(SOURCE_PATH)/transfer.cpp $(SOURCE_PATH)/transfer.hpp
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/mongoinput.o: $(SOURCE_PATH)/mongoinput.cpp $(SOURCE_PATH)/mongoinput.hpp
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/aside.o: $(SOURCE_PATH)/aside.cpp $(SOURCE_PATH)/aside.hpp
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

../objs/document.o: ../document.cpp ../document.hpp $(GLOBAL_DEPEND)
	@echo compiling ../document.cpp
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) ../document.cpp $(OBJFLAG) ../objs/document.o

$(OBJECT_PATH)/input_document.o: $(SOURCE_PATH)/input_document.cpp $(SOURCE_PATH)/input_document.hpp
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/wordmap.o: $(SOURCE_PATH)/wordmap.cpp $(SOURCE_PATH)/wordmap.hpp
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/{tpl}.o: $(SOURCE_PATH)/{tpl}.cpp $(SOURCE_PATH)/{tpl}.hpp
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

#rebuild: clean all
#rebuild is not entirely correct

#OBJS := $(patsubst $(SOURCE_PATH)%.$(C),$(OBJECT_PATH)%.$(OBJ),$(wildcard $(SOURCE_PATH)*/*.$(C)) $(wildcard $(SOURCE_PATH)*.$(C)))

#%.$(OBJ):$(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
#	@echo Compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) ...
#	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

#OBJS := $(OBJECT_PATH)/session.o $(OBJECT_PATH)/worker.o $(OBJECT_PATH)/master.o $(OBJECT_PATH)/portal.o $(OBJECT_PATH)/main.o

