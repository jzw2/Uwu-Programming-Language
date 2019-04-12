CXX = g++
LD = g++

CXXFLAGS = `llvm-config --cxxflags` -c -g -O3 -std=c++17
LDFLAGS = `llvm-config --ldflags --system-libs --libs core` -std=c++17 


tutorial : tutorial
	$(LD) tutorial.o $(LDFLAGS) -o tutorial

tutorial : tutorial.cpp
	$(CXX) tutorial.cpp $(CXXFLAGS)

clean :
	rm *.o
