CC=g++
FLAGS=-O3

all: mt image convert

rt:
	$(CC) -o rt rt.cpp $(FLAGS)

mt:
	$(CC) -o mt mt.cpp -pthread $(FLAGS)

image:
	./mt > image-mt.ppm
	#./rt > image-st.ppm

convert:
	convert image-mt.ppm image-mt.png
	#convert image-st.ppm image-st.png

clean:
	rm -f rt mt
