COMPILER=G++

# todo: object files into output path, processing c / c++ files in the same time (?), nested directories for source files (?)
C = cpp
OUTPUT_PATH = bin
SOURCE_PATH = .
OBJECT_PATH = objs
EXE_NAME	= rknn
EXE = $(OUTPUT_PATH)/$(EXE_NAME)
DEBUG = 1

ifeq ($(COMPILER), G++)
  ifeq ($(OS),Windows_NT)
    OBJ = obj
  else
    OBJ = o
  endif
  ifeq ($(DEBUG), 1)
  COPT = -DCS_DEBUG=1 -O0 -g3 -pipe -Wall -Wno-unused-function -fstack-protector
  LDOPT = -Wl,-O0 -g3 -rdynamic
  else
# -fno-rtti is no useable since boost::program_options...
  COPT = -DNDEBUG -march=core2 -O3 -fomit-frame-pointer -pipe -Wall -Wno-unused-function -finline -finline-small-functions -fno-strict-aliasing -m64 -fno-threadsafe-statics -D_FORTIFY_SOURCE=2 -mcx16 -msahf -maes -mpclmul -mpopcnt -mavx --param l1-cache-size=32 --param l1-cache-line-size=64 --param l2-cache-size=6144 -mtune=generic
  LDOPT = -Wl,-O3 $(COPT)
  endif
  CCMD = g++
  OBJFLAG = -o
  EXEFLAG = -o
  INCLUDES = 
  INCLUDES = -I../../staging
# LIBS = -lgc
#  LIBS = -lluaplus -lboost_system -lboost_thread
  LIBS = -lzmq -lmsgpack -lboost_system -lboost_thread -lboost_program_options -lboost_filesystem -lglog
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

OBJS = $(OBJECT_PATH)/aside.o $(OBJECT_PATH)/main.o $(OBJECT_PATH)/config.o \
$(OBJECT_PATH)/master.o $(OBJECT_PATH)/worker.o \
$(OBJECT_PATH)/handler.o $(OBJECT_PATH)/calculator.o \
$(OBJECT_PATH)/samples_fetcher.o $(OBJECT_PATH)/knn.o $(OBJECT_PATH)/vector.o $(OBJECT_PATH)/inputvector.o

all: $(OBJS)
	@echo Linking...
	$(CCMD) $(LDFLAGS) $^ $(EXEFLAG) $(EXE) $(LIBS)
	-cp $(EXE_NAME).sh $(OUTPUT_PATH)/

clean:
	-rm -f $(EXE)
	-find $(OBJECT_PATH) -name "*.$(OBJ)" | xargs rm -f

.PHONY: .preany

.preany:
	@echo preparing directories in $(OBJECT_PATH)
	@-find $(SOURCE_PATH) -type d -not -name '\.*' -and -not -samefile $(SOURCE_PATH) | awk '{sub("$(SOURCE_PATH)", "$(OBJECT_PATH)"); print;}' | xargs mkdir -p

GLOBAL_DEPEND = $(SOURCE_PATH)/predef.hpp

$(OBJECT_PATH)/main.o: $(SOURCE_PATH)/main.cpp $(GLOBAL_DEPEND)
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/master.o: $(SOURCE_PATH)/master.cpp $(SOURCE_PATH)/master.hpp $(GLOBAL_DEPEND)
	@echo compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/knn.o: $(SOURCE_PATH)/knn.cpp $(SOURCE_PATH)/knn.hpp  $(GLOBAL_DEPEND)
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/config.o: $(SOURCE_PATH)/config.cpp $(SOURCE_PATH)/config.hpp $(SOURCE_PATH)/predef.hpp
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/worker.o: $(SOURCE_PATH)/worker.cpp $(SOURCE_PATH)/worker.hpp
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/calculator.o: $(SOURCE_PATH)/calculator.cpp $(SOURCE_PATH)/calculator.hpp
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/handler.o: $(SOURCE_PATH)/handler.cpp $(SOURCE_PATH)/handler.hpp
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/samples_fetcher.o: $(SOURCE_PATH)/samples_fetcher.cpp $(SOURCE_PATH)/samples_fetcher.hpp
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/vector.o: $(SOURCE_PATH)/vector.cpp $(SOURCE_PATH)/vector.hpp
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/aside.o: $(SOURCE_PATH)/aside.cpp $(SOURCE_PATH)/aside.hpp
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/inputvector.o: $(SOURCE_PATH)/../inputvector.cpp $(SOURCE_PATH)/../inputvector.hpp
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) ../$(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

$(OBJECT_PATH)/{tpl}.o: $(SOURCE_PATH)/{tpl}.cpp $(SOURCE_PATH)/{tpl}.hpp
	@printf "\033[32;34;5m%s\033[0m \033[32;49;5m%s\033[0m\n" compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

#rebuild: clean all
#rebuild is not entirely correct

#OBJS := $(patsubst $(SOURCE_PATH)%.$(C),$(OBJECT_PATH)%.$(OBJ),$(wildcard $(SOURCE_PATH)*/*.$(C)) $(wildcard $(SOURCE_PATH)*.$(C)))

#%.$(OBJ):$(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@)
#	@echo Compiling $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) ...
#	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $(patsubst $(OBJECT_PATH)%.$(OBJ),$(SOURCE_PATH)%.$(C),$@) $(OBJFLAG) $@

#OBJS := $(OBJECT_PATH)/session.o $(OBJECT_PATH)/worker.o $(OBJECT_PATH)/master.o $(OBJECT_PATH)/portal.o $(OBJECT_PATH)/main.o

