CXX = g++
LD = g++

CXXFLAGS = -std=c++17 -c -g
LDFLAGS = -std=c++17 -g -lpthread

test : test_pre
	

test_pre : test.o lexer.o parser.o lexerUtils.o codegen.o
	$(LD) $(LDFLAGS) test.o lexer.o parser.o lexerUtils.o codegen.o `llvm-config --cxxflags --ldflags --system-libs --libs core` -o test

test.o : test.cpp
	$(CXX) $(CXXFLAGS) test.cpp

lexerUtils.o : lexer.o lexerUtils.cpp lexerUtils.h
	$(CXX) $(CXXFLAGS) lexerUtils.cpp

lexer.o : lexer.h lexer.cpp
	$(CXX) $(CXXFLAGS) lexer.cpp

parser.o : parser.cpp parser.h
	$(CXX) $(CXXFLAGS) parser.cpp

codegen.o : codegen.cpp
	$(CXX) $(CXXFLAGS) codegen.cpp

clean :
	rm *.o
