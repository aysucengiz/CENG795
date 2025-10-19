MAIN_FILES = main.cpp raytracer.cpp
DATA_FILES = dataTypes/helpers.cpp
FILE_FILES = fileManagement/Parser.cpp fileManagement/PPM.cpp
all:
	g++ $(MAIN_FILES) $(DATA_FILES) $(FILE_FILES) -o raytracer -std=c++17 -O3

clean:
	rm main