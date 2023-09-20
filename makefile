make: Program.cpp 
	g++ -std=c++11 -Wall -o Clairvoyance Program.cpp  -lm

run: Program.cpp  Clairvoyance
	./Clairvoyance
