#pragma once

#include <map>
#include <memory>
#include <string>

namespace bf2
{

class Compiler
{
  public:
    Compiler();
    Compiler(const Compiler&) = delete;
    Compiler& operator=(const Compiler&) = delete;

    void compile(std::string file_path);
};

} // namespace bf2