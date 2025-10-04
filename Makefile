all:
	g++ *.cpp -o raytracer -std=c++17 -O3 -ljsoncpp

clean:
	rm main