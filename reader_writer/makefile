SRC=$(wildcard *.cpp)
OBJS=$(SRC:.cpp=.o)
BUILD_DIR=build
OPTS=-std=c++17
BUILD_OBJS=$(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
LINKER=g++ $(OPTS)
LINK_POSTOPT=-pthread
COMPILER=g++
TARGET=reader_writer

all: build_dir reader_writer

.PHONY: build_dir
build_dir:
	@mkdir -p $(BUILD_DIR)

reader_writer: $(BUILD_OBJS)
	@echo "Linking"
	$(LINKER) -o $@ $^ $(LINK_POSTOPT)
	@echo "Link done"

%.o: %.cpp makefile
	$(COMPILER) $(OPTS) -c $< -o $@


$(BUILD_DIR)/%.o: %.cpp | build_dir
	$(COMPILER) $(OPTS) -c $< -o $@

clean:
	-rm -f $(BUILD_OBJS)
	-rmdir $(BUILD_DIR)

mrproper: clean
	rm -f $(TARGET)
