TEST_FILE = src/main_for_test.cpp
MAIN_FILE = src/main.cpp
RAYTRACER_FILES = src/rayTracer/RayTracer.cpp src/rayTracer/raytracerThread.cpp
DATA_FILES = src/dataTypes/helpers.cpp src/dataTypes/Object.cpp
FILE_FILES = src/fileManagement/Parser.cpp src/fileManagement/PPM.cpp
FLAGS = -Isrc -fopenmp -O3 -std=c++20
all:
	g++ $(FLAGS) $(MAIN_FILE) $(RAYTRACER_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer
test:
	g++ $(FLAGS) $(TEST_FILE) $(RAYTRACER_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer_test
raytracer:
	g++ $(FLAGS) $(MAIN_FILE) $(RAYTRACER_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer

clean:
	rm raytracer raytracer_test