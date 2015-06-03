CC   = gcc
CXX  = g++

C_FLAGS = -W 
C_FLAGS += -O2 
C_FLAGS += -fno-strict-aliasing
C_FLAGS += -Wall -Wno-unused -Wno-missing-field-initializers -Wno-missing-braces
C_FLAGS += -g 
C_FLAGS += -z 

INC = -I../include/
LIB = -lz -lrt -lpthread 

SOURCE = $(wildcard *.cpp)
OBJS    = $(patsubst %.cpp,%.o,$(SOURCE))

OUTPUT  := ../lib/libmini_jsoncpp.a

all: $(OUTPUT)

.SUFFIXES: .o .cpp

$(OUTPUT):$(OBJS)
	 ar -rs $@ $(EX_LIB) $^

.cpp.o:
	g++ $(C_FLAGS) $(LIB) $(INC) -c  $^ -o $(patsubst %.cpp,%.o,$^)

clean:
	rm -f $(OBJS) 
