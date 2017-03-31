FILE=Raytracer

########
#   Directories
S_DIR=Source
B_DIR=Build

########
#   Output
EXEC=$(B_DIR)/$(FILE)

# default build settings
CC_OPTS=-std=c++11 -c -pipe -Wall -Wno-switch -ggdb -g3 -O3 
LIBS = 
CC=g++


OS := $(shell uname)
ifeq ($(OS),Darwin)
  # Run MacOS commands
  GLMDIR = 1
  LIBS += -lm -framework OpenCL -fopenmp -lglut 
  # LIBS += -lm -framework OpenCL 
	CC=clang-omp++

else
  # check for Linux and run other commands
  # LIBS += -lm -lOpenCL -fopenmp 
  LIBS += -lm -lOpenCL 
endif

########
#       SDL options
SDL_CFLAGS := $(shell sdl-config --cflags)
GLM_CFLAGS := -I$(GLMDIR)
# GLM_CFLAGS := -I1
SDL_LDFLAGS := $(shell sdl-config --libs)

########
#   This is the default action
all:Build

# export PATH=/install/prefix/bin:$PATH;
# export C_INCLUDE_PATH=/install/prefix/include:/usr/local/Cellar/clang-omp/2015-04-01/libexec/include:$C_INCLUDE_PATH;
# export CPLUS_INCLUDE_PATH=/install/prefix/include:/usr/local/Cellar/clang-omp/2015-04-01/libexec/include:$CPLUS_INCLUDE_PATH;
# export LIBRARY_PATH=/install/prefix/lib:/usr/local/Cellar/clang-omp/2015-04-01/libexec/lib:$LIBRARY_PATH;
# export DYLD_LIBRARY_PATH=/install/prefix/lib:/usr/local/Cellar/clang-omp/2015-04-01/libexec/lib:$DYLD_LIBRARY_PATH;


########
#   Object list
#
OBJ = $(B_DIR)/$(FILE).o


########
#   Objects
$(B_DIR)/$(FILE).o : $(S_DIR)/$(FILE).cpp $(S_DIR)/SDLauxiliary.h $(S_DIR)/TestModel.h $(S_DIR)/Raytracer.h
	$(CC) $(CC_OPTS) -o $(B_DIR)/$(FILE).o $(S_DIR)/$(FILE).cpp $(SDL_CFLAGS) $(GLM_CFLAGS)


########
#   Main build rule     
Build : $(OBJ) Makefile
	$(CC) $(LIBS) -o $(EXEC) $(OBJ) $(SDL_LDFLAGS)


clean:
	rm -f $(B_DIR)/* 
