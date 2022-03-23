#-*-Makefile-*-

CFLAGS  := -MMD
CC := gcc
SRC := c
SRCS := $(wildcard *.$(SRC))
OBJS := $(patsubst %.$(SRC),%.o,$(SRCS))
DEPS := %.h
LIBS := 
TARGET := CAN_convert_data

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

#Below is a general rule
%.o: %.$(SRC) $(DEPS)
	$(CC) -MMD -c $< -o $@


.PHONY: clean
clean:
	rm -rf *.o *.d $(TARGET)
	

