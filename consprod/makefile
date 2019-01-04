SRC=$(wildcard *.cpp)
OBJS=$(SRC:.cpp=.o)
BUILD_DIR=build
OPTS=-std=c++17 -MMD -Wall
BUILD_OBJS=$(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
LINKER=g++ $(OPTS)
LINK_POSTOPT=-pthread -lncurses
COMPILER=g++
TARGET=consprod

all: build_dir consprod

-include $(BUILD_DIR)/*.d

.PHONY: build_dir
build_dir:
	@mkdir -p $(BUILD_DIR)

consprod: $(BUILD_OBJS)
	@echo "Linking"
	$(LINKER) -o $@ $^ $(LINK_POSTOPT)
	@echo "Link done"

$(BUILD_DIR)/%.o: %.cpp makefile | build_dir
	$(COMPILER) $(OPTS) -c $< -o $@

clean:
	-rm -f $(BUILD_OBJS)
	-rm -f $(BUILD_DIR)/*.o
	-rm -f $(BUILD_DIR)/*.d
	-rmdir $(BUILD_DIR)

mrproper: clean
	rm -f $(TARGET)
