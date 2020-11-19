
prog1:
	g++ program1.cpp -o program1 -pthread -std=c++11


build:
	g++ main.cpp -o typroc


test:
	./typroc node1