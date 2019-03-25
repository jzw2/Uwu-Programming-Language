CXX = g++
LD = g++

CXXFLAGS = -std=c++17 -c -g
LDFLAGS = -std=c++17 -g -lpthread

test : test_pre clean
	

test_pre : test.o lexer.o parser.o lexerUtils.o
	$(LD) $(LDFLAGS) test.o lexer.o parser.o lexerUtils.o -o test

test.o :
	$(CXX) $(CXXFLAGS) test.cpp

lexerUtils.o : lexer.o
	$(CXX) $(CXXFLAGS) lexerUtils.cpp

lexer.o :
	$(CXX) $(CXXFLAGS) lexer.cpp

parser.o : 
	$(CXX) $(CXXFLAGS) parser.cpp

clean :
	rm *.o
