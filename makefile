all: project2

project1: project2.c
	gcc project2.c -o project2 -Wall -pedantic
	
clean:
	rm project2
