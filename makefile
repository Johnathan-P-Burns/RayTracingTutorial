CC=g++
FLAGS=-O3

all: rt mt image convert

rt:
	$(CC) -o rt rt.cpp $(FLAGS)

mt:
	$(CC) -o mt mt.cpp -pthread $(FLAGS)

image:
	./rt > image-st.ppm
	./mt > image-mt.ppm

convert:
	convert image-st.ppm image-st.png
	convert image-mt.ppm image-mt.png

clean:
	rm -f rt mt image-st.ppm image-st.png image-mt.ppm image-mt.png
