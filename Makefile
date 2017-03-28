FILE=skeleton

########
#   Directories
S_DIR=Source
B_DIR=Build

########
#   Output
EXEC=$(B_DIR)/$(FILE)

# default build settings
CC_OPTS=-c -pipe -Wall -Wno-switch -ggdb -g3 -O3 
LIBS = 
CC=g++

OS := $(shell uname)
ifeq ($(OS),Darwin)
  # Run MacOS commands
  GLMDIR = 1
  LIBS += -lm -framework OpenCL
else
  # check for Linux and run other commands
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


########
#   Object list
#
OBJ = $(B_DIR)/$(FILE).o


########
#   Objects
$(B_DIR)/$(FILE).o : $(S_DIR)/$(FILE).cpp $(S_DIR)/SDLauxiliary.h $(S_DIR)/TestModel.h
	$(CC) $(CC_OPTS) -o $(B_DIR)/$(FILE).o $(S_DIR)/$(FILE).cpp $(SDL_CFLAGS) $(GLM_CFLAGS)


########
#   Main build rule     
Build : $(OBJ) Makefile
	$(CC) $(LIBS) -o $(EXEC) $(OBJ) $(SDL_LDFLAGS)


clean:
	rm -f $(B_DIR)/* 
