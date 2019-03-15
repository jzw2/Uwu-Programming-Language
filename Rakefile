
COMPILER = "g++"
EXEC = "tutorial"
FLAGS = "-Wall -Wextra -std=c++17"

# rules

task :default do
  sh "g++ -g -O3 helloworld.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o helloworld "
  #sh "g++ -g -O3 tutorial.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o tutorial "
end

