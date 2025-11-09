CXX = g++

TEST_FILE = src/main_for_test.cpp
MAIN_FILE = src/main.cpp
ACC_FILES = $(wildcard src/acceleration/*.cpp)
RAYTRACER_FILES = $(wildcard src/rayTracer/*.cpp)
MATRIX_FILES    = $(wildcard src/dataTypes/matrix/*.cpp)
BASE_FILES      = $(wildcard src/dataTypes/base/*.cpp)
OBJECT_FILES = $(wildcard src/dataTypes/object/*.cpp)
FUNCTION_FILES = $(wildcard src/dataTypes/functions/*.cpp)
DATA_FILES = $(MATRIX_FILES) $(BASE_FILES) $(OBJECT_FILES) $(FUNCTION_FILES)
FILE_FILES = $(wildcard src/fileManagement/*.cpp)
PARALLEL = -fopenmp
FLAGS = -Isrc  -O3 -std=c++20 $(PARALLEL)

SRC_FILES = $(DATA_FILES) $(FILE_FILES) $(RAYTRACER_FILES) $(ACC_FILES)
TEST_FILES = $(TEST_FILE) $(SRC_FILES)
RT_FILES = $(MAIN_FILE) $(SRC_FILES)

OBJ_DIR = obj
RT_OBJ = $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(RT_FILES))
TEST_OBJ = $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(TEST_FILES))

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) -c $< -o $@

all: raytracer

raytracer_test: $(TEST_OBJ)
	$(CXX) $(FLAGS) $^ -o $@
raytracer: $(RT_OBJ)
	$(CXX) $(FLAGS) $^ -o $@


clean:
	rm -rf $(OBJ_DIR) raytracer raytracer_test