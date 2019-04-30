CC=g++
FLAGS=-g -Og

all: mt image convert

rt:
	$(CC) -o rt rt.cpp $(FLAGS)

mt:
	$(CC) -o mt mt.cpp -pthread $(FLAGS)

image:
	./mt > image.ppm
#	./rt > image.ppm

convert:
	convert image.ppm image.png

clean:
	rm -f rt mt image.ppm image.png
