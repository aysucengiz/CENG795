TEST_FILE = src/main_for_test.cpp
MAIN_FILE = src/main.cpp
RAYTRACER_FILES = src/rayTracer/RayTracer.cpp src/rayTracer/raytracerThread.cpp
DATA_FILES = src/dataTypes/helpers.cpp src/dataTypes/Object.cpp
FILE_FILES = src/fileManagement/Parser.cpp src/fileManagement/PPM.cpp
all:
	g++ -Isrc $(MAIN_FILE) $(RAYTRACER_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer -std=c++20 -O3
test:
	g++ -Isrc $(TEST_FILE) $(RAYTRACER_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer_test -std=c++20 -O3
raytracer:
	g++ -Isrc $(MAIN_FILE) $(RAYTRACER_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer -std=c++20 -O3

clean:
	rm raytracer raytracer_test