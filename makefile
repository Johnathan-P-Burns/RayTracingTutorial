CC=g++

all: rt image convert

rt:
	$(CC) -o rt rt.cpp -O3

image:
	./rt > image.ppm

convert:
	convert image.ppm image.png

clean:
	rm -f rt image.ppm image.png
