#include "parser.h"
#include "lexer.h"
#include "lexerUtils.h"
#include "llvm/Support/raw_os_ostream.h"

#include <iostream>
#include <string>
#include <fstream>


int main(int argc, char **argv) {

  if (argc != 2) {
    std::cout << "Usage : uwu <filename>" << std::endl;
    return -1;
  }

  naruto::sModule = llvm::make_unique<llvm::Module>("module", naruto::sContext);
  std::vector<naruto::Lex> stream = naruto::naruto_lexize_file(argv[1]);

  auto f = llvm::make_unique<naruto::ASTRoot>();
  f->parse(stream, 0);
  // f.print();
  f->generate();

  std::string filename = argv[1];
  size_t begin_index;
  for (begin_index = filename.length() - 1; begin_index ; begin_index--) {
    if (filename[begin_index] == '/') {
      begin_index++;
      break;
    }
  }
  auto base = filename.substr(begin_index, filename.length() - 3);


  std::string llvm_file = base + "ll";

  std::ofstream std_file_stream(llvm_file);
  llvm::raw_os_ostream file_stream(std_file_stream);
  naruto::sModule->print(file_stream, nullptr);
}

