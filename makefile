make: Program.cpp 
	g++ -Wall -o Clairvoyance Program.cpp  -lm

run: Program.cpp  Clairvoyance
	./Clairvoyance