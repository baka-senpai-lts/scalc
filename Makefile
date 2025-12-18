##
# scalc
#
# @file
# @version 0.1

TARGET   := scalc

CC       ?= gcc
CFLAGS   ?= -Wall -Wextra -Werror -Wpedantic -Wshadow -Wstrict-overflow=5 \
-Wcast-align -Wconversion -Wformat=2 -Wuninitialized \
-Wnull-dereference -Wdouble-promotion -Wstack-protector \
-g -std=c99 -MMD -MP
LDFLAGS  ?=
LDLIBS   :=

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)

TIMER_START       := $(shell date +%s%N)
TIMER_END          = $(shell date +%s%N)
TIMER_NANOSECONDS  = $(shell expr $(TIMER_END) - $(TIMER_START))
TIMER_MILLISECONDS = $(shell expr $(TIMER_NANOSECONDS) / 1000000)

.PHONY: all clean

all: $(TARGET)
	@echo ""
	@echo "Welcome to the dark side, we have cookies!"
	@echo "Your functional calculator is ready to use:"
	@echo ""
	@echo "./scalc - run the interpreter"
	@echo ""
	@echo "Binary size:    " $(shell du -h $(TARGET) | cut -f1)
	@echo ""
	@echo "Compiler flags: " $(CFLAGS)
	@echo "Linker flags:   " $(LDFLAGS)
	@echo "Compiler:       " $(CC)
	@echo ""
	@echo "This build took $(TIMER_MILLISECONDS) milliseconds to finish"
	@echo ""

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# end
