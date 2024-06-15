# NEW FILE STUFF FOR TESTING

CC = gcc
CFLAGS = -Wall -Wextra -g3

# Define the targets
TARGETS := assemble emulate

# Define the build directory and source directories
BUILD_DIR := ./build
SRC_DIR := ./src
ASSEMBLE_SRC_DIR := $(SRC_DIR)/assemble
EMULATE_SRC_DIR :=  $(SRC_DIR)/emulate

# Find all the C files we want to compile
EMULATE_SRCS := $(shell find $(EMULATE_SRC_DIR) -name '*.c')
ASSEMBLE_SRCS := $(shell find $(ASSEMBLE_SRC_DIR) -name '*.c')

# Prepends BUILD_DIR and appends .o to every src file
EMULATE_OBJS := $(EMULATE_SRCS:$(EMULATE_SRC_DIR)/%=$(BUILD_DIR)/emulate/%.o)
ASSEMBLE_OBJS := $(ASSEMBLE_SRCS:$(ASSEMBLE_SRC_DIR)/%=$(BUILD_DIR)/assemble/%.o)

# String substitution (suffix version without %)
EMULATE_DEPS := $(EMULATE_OBJS:.o=.d)
ASSEMBLE_DEPS := $(ASSEMBLE_OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
CPPFLAGS := $(INC_FLAGS) -MMD -MP

# Build rules for each target
all: $(TARGETS)

assemble: $(BUILD_DIR)/assemble_executable
emulate: $(BUILD_DIR)/emulate_executable

$(BUILD_DIR)/assemble_executable: $(ASSEMBLE_OBJS)
	$(CC) $(ASSEMBLE_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/emulate_executable: $(EMULATE_OBJS)
	$(CC) $(EMULATE_OBJS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/emulate/%.c.o: $(EMULATE_SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/assemble/%.c.o: $(ASSEMBLE_SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles
-include $(EMULATE_DEPS)
-include $(ASSEMBLE_DEPS)

# Cppcheck target
.PHONY: cppcheck
cppcheck:
	cppcheck --check-level=exhaustive --enable=all,style,performance,portability,information,unusedFunction --inconclusive --std=c99 --language=c --suppress=missingIncludeSystem $(SRC_DIR)