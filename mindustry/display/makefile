SRC=$(wildcard *.cpp)
OBJS=$(SRC:.cpp=.o)
BUILD_DIR=build
OPTS=-std=c++17 -MMD -g
BUILD_OBJS=$(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
LINKER=g++ $(OPTS)
LINK_POSTOPT=-pthread
COMPILER=g++
TARGET=display

all: build_dir $(TARGET)

run: $(TARGET)
	./$(TARGET)

-include $(BUILD_DIR)/*.d

.PHONY: build_dir
build_dir:
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(BUILD_OBJS)
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
	-rm -f display

mrproper: clean
	rm -f $(TARGET)
