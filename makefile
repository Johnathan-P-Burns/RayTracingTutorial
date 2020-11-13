CC=g++
FLAGS=-O3 -std=c++17

all: mt image convert

mt:
	$(CC) -o mt-raytracer mt.cpp -pthread $(FLAGS)

image:
	./mt-raytracer > image.ppm

convert:
	convert image.ppm image.png

clean:
	rm -f mt-raytracer
