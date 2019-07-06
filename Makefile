CC = g++
LD = g++

CCFLAGS = -g -Wall
LDFLAGS = -levent
SRC_FILE = $(wildcard *.cpp)
OBJ_FILE = $(patsubst %.cpp,%.o,$(SRC_FILE))

all:main

main:$(OBJ_FILE)
	$(LD) $^ -o $@ $(LDFLAGS)

%.d : %.cpp
	$(CC) $(CCFLAGS) -M $^ > $@

#include $(SRC_FILE:.cpp=.d)

print:
	@echo $(SRC_FILE)
	@echo $(OBJ_FILE)

clean:
	rm -f *.o
	rm -f *.d
	rm -f main 
.PHONY: print clean
