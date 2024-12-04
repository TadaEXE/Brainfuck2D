#include <algorithm>
#include <iostream>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include "Compiler.h"

char* getCmdOption(char** begin, char** end, const std::string& option)
{
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[])
{
    if (!cmdOptionExists(argv, argv + argc, "-f"))
    {
        std::cout << "No file given." << std::endl;
        return 1;
    }

    char* file_path = getCmdOption(argv, argv + argc, "-f");

    if (file_path)
    {
        std::cout << "File: " << file_path << std::endl;

        bf2::Compiler compiler;
        compiler.compile(file_path);
    }

    return 0;
}