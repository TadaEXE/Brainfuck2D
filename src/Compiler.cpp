#include "Compiler.h"

#include "Brainfuck2DLexer.h"
#include "Brainfuck2DParser.h"
#include "VisitorImpl.h"
#include "antlr4-runtime.h"
#include <istream>

namespace bf2
{
Compiler::Compiler()
{
}

void Compiler::compile(std::string file_path)
{
    std::ifstream stream;
    stream.open(file_path);

    antlr4::ANTLRInputStream input(stream);
    Brainfuck2DLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    Brainfuck2DParser parser(&tokens);

    auto tree = parser.program();

    VisitorImpl visitor;
    visitor.visitProgram(tree);
}
} // namespace bf2