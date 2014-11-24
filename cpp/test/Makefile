all: test_1-o test_1-d

clean:
	rm test_1-o test_1-d -v

test_1-o: ../main/* test_1.C
	g++ -I../main test_1.C -o test_1-o -O3 -Wall

test_1-d: ../main/* test_1.C
	g++ -I../main test_1.C -o test_1-d -g -Wall
