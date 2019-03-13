CXX = g++
LD = g++

CXXFLAGS = -std=c++17 -c -g
LDFLAGS = -std=c++17 -lpthread

test : test.o lexer.o
	$(LD) $(LDFLAGS) test.o lexer.o -o test

test.o :
	$(CXX) $(CXXFLAGS) test.cpp

lexer.o : lexer.h
	$(CXX) $(CXXFLAGS) lexer.cpp

parser.o : 
	$(CXX) $(CXXFLAGS) parser.cpp

clean :
	rm *.o
