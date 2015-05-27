CC   = gcc
CXX  = g++

C_FLAGS = -W 
C_FLAGS += -O2 
C_FLAGS += -fno-strict-aliasing
C_FLAGS += -Wall
C_FLAGS += -Wno-unused
C_FLAGS += -Wno-missing-field-initializers 
C_FLAGS += -Wno-missing-braces
C_FLAGS += -g 
C_FLAGS += -z 
C_FLAGS += -lz  
C_FLAGS += -lrt 
C_FLAGS += -lpthread   

INC = -I../include/
LIB = -L../lib/ -static -lmini_jsoncpp

OBJS = demo


all: $(OBJS)

.SUFFIXES: .cpp
.cpp:
	$(CXX) $(C_FLAGS) -o  $* $*.cpp $(INC) $(LIB);

clean:
	rm -f $(OBJS)
