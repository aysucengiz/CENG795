TEST_FILE = main_for_test.cpp
MAIN_FILE = main.cpp
RAYTRACER_FILES = raytracer.cpp
DATA_FILES = dataTypes/helpers.cpp
FILE_FILES = fileManagement/Parser.cpp fileManagement/PPM.cpp
all:
	g++ $(MAIN_FILE) $(RAYTRACER_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer -std=c++17 -O3
test:
	g++ $(TEST_FILE) $(RAYTRACER_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer_test -std=c++17 -O3
raytracer:
	g++ $(MAIN_FILE) $(RAYTRACER_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer -std=c++17 -O3

clean:
	rm raytracer raytracer_test