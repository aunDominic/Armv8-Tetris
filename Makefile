# NEW FILE STUFF FOR TESTING

CC = gcc
CFLAGS = -Wall -O0 -ggdb

# Define the targets
TARGETS := assemble emulate

# Define the build directory and source directories
BUILD_DIR := ./armv8_testsuite/solution
SRC_DIR := ./src
ASSEMBLE_SRC_DIR := $(SRC_DIR)/assemble

# Find all the C and C++ files we want to compile
EMULATE_SRCS := $(shell find $(SRC_DIR) -maxdepth 1 -name '*.cpp' -or -name '*.c' -or -name '*.s')
ASSEMBLE_SRCS := $(shell find $(ASSEMBLE_SRC_DIR) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Prepends BUILD_DIR and appends .o to every src file
EMULATE_OBJS := $(EMULATE_SRCS:$(SRC_DIR)/%=$(BUILD_DIR)/%.o)
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

assemble: $(BUILD_DIR)/assemble
emulate: $(BUILD_DIR)/emulate

$(BUILD_DIR)/assemble: $(ASSEMBLE_OBJS)
	$(CXX) $(ASSEMBLE_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/emulate: $(EMULATE_OBJS)
	$(CXX) $(EMULATE_OBJS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/assemble/%.c.o: $(ASSEMBLE_SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/assemble/%.cpp.o: $(ASSEMBLE_SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles
-include $(EMULATE_DEPS)
-include $(ASSEMBLE_DEPS)
