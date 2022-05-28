#-*-Makefile-*-

CFLAGS  := -MMD
CC := g++
SRC := cpp
SRCS := $(wildcard *.$(SRC))
OBJS := $(patsubst %.$(SRC),%.o,$(SRCS))
DEPS := %.h
LIBS := -lpaho-mqtt3cs -lpq -ljson-c -lrt
TARGET := Pro_subscriber

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

#Below is a general rule
%.o: %.$(SRC) $(DEPS)
	$(CC) -MMD -c $< -o $@


.PHONY: clean
clean:
	rm -rf *.o *.d $(TARGET)
	

