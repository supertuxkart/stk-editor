NAME = stk-editor

SRC = $(wildcard src/*.cpp src/gui/*.cpp src/mesh/*.cpp src/spline/*.cpp src/commands/*.cpp src/viewport/*.cpp src/input/*.cpp)
OBJ = $(SRC:.cpp=.o)
HDR = $(wildcard *.hpp)

# Irr paths will depend on your system.
IRR_INCLUDE ?= /opt/irr2/include/irrlicht
IRR_LIB ?= /opt/irr2/lib/libIrrlicht.a

ifeq (,$(wildcard $(IRR_INCLUDE)))
	IRR_INCLUDE = /usr/local/include/irrlicht
endif
ifeq (,$(wildcard $(IRR_INCLUDE)))
	IRR_INCLUDE = /usr/include/irrlicht
endif
ifeq (,$(wildcard $(IRR_INCLUDE)))
$(error Irr includes not found. Set the IRR_INCLUDE env var correctly)
endif

ifeq (,$(wildcard $(IRR_LIB)))
	IRR_LIB = -lIrrlicht
$(warning Attempting to link irrlicht dynamically)
endif

CXXFLAGS += -isystem $(IRR_INCLUDE) -I src.
LDFLAGS += $(IRR_LIB)

CXXFLAGS += -Wall -Wextra -g
LDFLAGS += -Wl,-O1
LDFLAGS += -lz -lpng -ljpeg -L /usr/X11R7/lib -lGL -lX11 -lXxf86vm

.PHONY: all clean

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(CXXFLAGS) $(LDFLAGS)

$(OBJ): $(HDR) Makefile

clean:
	rm -f $(OBJ) $(NAME)
