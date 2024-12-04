#include "VisitorImpl.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

namespace bf2
{

VisitorImpl::VisitorImpl()
{
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("bf2d", *context);

    init_print_fn();
    init_getchar_fn();
}

std::any VisitorImpl::visitProgram(Brainfuck2DParser::ProgramContext* ctx)
{
    auto visit_res = visitChildren(ctx);
    return visit_res;
}

std::any VisitorImpl::visitMoveRight(Brainfuck2DParser::MoveRightContext* ctx)
{
    return visitChildren(ctx);
}

std::any VisitorImpl::visitMoveLeft(Brainfuck2DParser::MoveLeftContext* ctx)
{
    return visitChildren(ctx);
}

std::any VisitorImpl::visitIncrement(Brainfuck2DParser::IncrementContext* ctx)
{
    return visitChildren(ctx);
}

std::any VisitorImpl::visitDecrement(Brainfuck2DParser::DecrementContext* ctx)
{
    return visitChildren(ctx);
}

std::any VisitorImpl::visitOutput(Brainfuck2DParser::OutputContext* ctx)
{
    return visitChildren(ctx);
}

std::any VisitorImpl::visitInput(Brainfuck2DParser::InputContext* ctx)
{
    return visitChildren(ctx);
}

std::any VisitorImpl::visitLoopStart(Brainfuck2DParser::LoopStartContext* ctx)
{
    return visitChildren(ctx);
}

std::any VisitorImpl::visitLoopEnd(Brainfuck2DParser::LoopEndContext* ctx)
{
    return visitChildren(ctx);
}

std::any VisitorImpl::visitMoveUp(Brainfuck2DParser::MoveUpContext* ctx)
{
    return visitChildren(ctx);
}

std::any VisitorImpl::visitMoveDown(Brainfuck2DParser::MoveDownContext* ctx)
{
    return visitChildren(ctx);
}

void VisitorImpl::init_print_fn()
{
    auto i8p = builder->getPtrTy();
    auto printf_prototype = llvm::FunctionType::get(builder->getInt32Ty(), i8p, true);
    print_fn = llvm::Function::Create(printf_prototype, llvm::Function::ExternalLinkage, "printf", *module);
}

void VisitorImpl::init_getchar_fn()
{
    auto i32 = builder->getInt32Ty();
    auto getchar_prototype = llvm::FunctionType::get(i32, false);
    getchar_fn = llvm::Function::Create(getchar_prototype, llvm::Function::ExternalLinkage, "getchar", *module);
}
} // namespace bf2