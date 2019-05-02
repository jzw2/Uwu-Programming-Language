CXX = g++
LD = g++

CXXFLAGS = -std=c++17 -c -g
LDFLAGS = -std=c++17 -g -lpthread

uwu : uwu.o lexer.o parser.o lexerUtils.o codegen.o parser_dtors.o
	$(LD) $(LDFLAGS) uwu.o lexer.o parser.o lexerUtils.o codegen.o parser_dtors.o `llvm-config --cxxflags --ldflags --system-libs --libs core` -o uwu

test : test.o lexer.o parser.o lexerUtils.o codegen.o
	$(LD) $(LDFLAGS) test.o lexer.o parser.o lexerUtils.o codegen.o parser_dtors.o `llvm-config --cxxflags --ldflags --system-libs --libs core` -o test

test.o : test.cpp
	$(CXX) $(CXXFLAGS) test.cpp

uwu.o : uwu.cpp
	$(CXX) $(CXXFLAGS) uwu.cpp

lexerUtils.o : lexer.o lexerUtils.cpp lexerUtils.h
	$(CXX) $(CXXFLAGS) lexerUtils.cpp

lexer.o : lexer.h lexer.cpp
	$(CXX) $(CXXFLAGS) lexer.cpp

parser.o : parser.cpp parser.h
	$(CXX) $(CXXFLAGS) parser.cpp

parser_dtors.o : parser_dtors.cpp parser.h
	$(CXX) $(CXXFLAGS) parser_dtors.cpp

codegen.o : codegen.cpp parser.h
	$(CXX) $(CXXFLAGS) codegen.cpp

clean :
	rm *.o
