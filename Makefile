NAME = stk-editor

SRC = $(wildcard *.cpp toolbox/*.cpp entities/*.cpp)
OBJ = $(SRC:.cpp=.o)
HDR = $(wildcard *.hpp)

# Irr paths will depend on your system.
IRR_INCLUDE ?= /opt/irr2/include/irrlicht
IRR_LIB ?= /opt/irr2/lib/libIrrlicht.a
CXXFLAGS += -isystem $(IRR_INCLUDE) -I.
LDFLAGS += $(IRR_LIB)

CXXFLAGS += -Wall -Wextra
LDFLAGS += -Wl,-O1
LDFLAGS += -lz -lpng -ljpeg -L /usr/X11R7/lib -lGL

.PHONY: all clean

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(CXXFLAGS) $(LDFLAGS)

$(OBJ): $(HDR) Makefile

clean:
	rm -f $(OBJ) $(NAME)
