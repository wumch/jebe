COMPILER=G++

# todo: object files into output path, processing c / c++ files in the same time (?), nested directories for source files (?)
C = cpp
OUTPUT_PATH = bin
SOURCE_PATH = .
OBJECT_PATH = objs
EXE = $(OUTPUT_PATH)/extractor
PREPROCESS_EXE = $(OUTPUT_PATH)/preprocess
DEBUG = 1

ifeq ($(COMPILER), G++)
  ifeq ($(OS),Windows_NT)
    OBJ = obj
  else
    OBJ = o
  endif
  ifeq ($(DEBUG), 1)
  COPT = -trigraphs -O0 -g3 -rdynamic -pipe -Wall -fno-strict-aliasing -fstack-protector
  LDOPT = -g -rdynamic
  else
  COPT = -DNDEBUG -trigraphs -march=core2 -O3 -fomit-frame-pointer -pipe -Wall -fno-strict-aliasing -m64 -fno-threadsafe-statics -D_FORTIFY_SOURCE=2 -mcx16 -msahf -maes -mpclmul -mpopcnt -mavx --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=6144 -mtune=generic
  LDOPT = $(COPT)
  endif
  CCMD = g++
  OBJFLAG = -o
  EXEFLAG = -o
 INCLUDES = 
  INCLUDES = -I../staging
# LIBS = -lgc
#  LIBS = -lluaplus -lboost_system -lboost_thread
  LIBS = -lboost_system -lboost_filesystem -lboost_thread
  PREPROCESS_LIBS = -lboost_system -lboost_filesystem -lboost_program_options
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

OBJS = $(OBJECT_PATH)/extractor.o $(OBJECT_PATH)/analyzer.o $(OBJECT_PATH)/phrase.o $(OBJECT_PATH)/main.o
PREPROCESS_OBJS = $(OBJECT_PATH)/preprocess.o $(OBJECT_PATH)/config.o $(OBJECT_PATH)/filter.o

all: extractor preprocess

extractor: $(OBJS)
	@echo Linking...
	$(CCMD) $(LDFLAGS) $^ $(LIBS) $(EXEFLAG) $(EXE)

preprocess: $(PREPROCESS_OBJS)
	@echo Linking...
	$(CCMD) $(LDFLAGS) $^ $(PREPROCESS_LIBS) $(EXEFLAG) $(PREPROCESS_EXE)

clean:
	-rm -f $(EXE)
	-find $(OBJECT_PATH) -name "*.$(OBJ)" -type f | xargs rm -f

.PHONY: .preany

.preany:
	@echo preparing directories in $(OBJECT_PATH)
	@-find $(SOURCE_PATH) -type d -not -name '\.*' -and -not -samefile $(SOURCE_PATH) | awk '{sub("$(SOURCE_PATH)", "$(OBJECT_PATH)"); print;}' | xargs mkdir -p

GLOBAL_DEPEND = $(SOURCE_PATH)/../staging/staging.hpp

$(OBJECT_PATH)/main.o: $(SOURCE_PATH)/main.cpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/extractor.o: $(SOURCE_PATH)/extractor.cpp $(SOURCE_PATH)/extractor.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/analyzer.o: $(SOURCE_PATH)/analyzer.cpp $(SOURCE_PATH)/analyzer.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/phrase.o: $(SOURCE_PATH)/phrase.cpp $(SOURCE_PATH)/phrase.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/preprocess.o: $(SOURCE_PATH)/preprocess.cpp $(SOURCE_PATH)/preprocess.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/filter.o: $(SOURCE_PATH)/filter.cpp $(SOURCE_PATH)/filter.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/config.o: $(SOURCE_PATH)/config.cpp $(SOURCE_PATH)/config.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

