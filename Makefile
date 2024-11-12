main: main.cpp gameutil.cpp
	g++ -std=c++11 $^ -o $@

clean: 
	rm main

.PHONY: all test clean

