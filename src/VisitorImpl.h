#pragma once
#include "Brainfuck2DBaseVisitor.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <map>
#include <memory>
#include <string>
namespace bf2
{

constexpr uint16_t world_size = 100;

class VisitorImpl : public Brainfuck2DBaseVisitor
{
  public:
    VisitorImpl();
    ~VisitorImpl();

    std::any visitProgram(Brainfuck2DParser::ProgramContext* ctx) override;

    std::any visitMoveRight(Brainfuck2DParser::MoveRightContext* ctx) override;

    std::any visitMoveLeft(Brainfuck2DParser::MoveLeftContext* ctx) override;

    std::any visitIncrement(Brainfuck2DParser::IncrementContext* ctx) override;

    std::any visitDecrement(Brainfuck2DParser::DecrementContext* ctx) override;

    std::any visitOutput(Brainfuck2DParser::OutputContext* ctx) override;

    std::any visitInput(Brainfuck2DParser::InputContext* ctx) override;

    std::any visitLoop(Brainfuck2DParser::LoopContext* ctx) override;

    std::any visitMoveUp(Brainfuck2DParser::MoveUpContext* ctx) override;

    std::any visitMoveDown(Brainfuck2DParser::MoveDownContext* ctx) override;

  private:
    void init_print_fn();
    void init_getchar_fn();
    void init_world();
    void create_entry_point();

    llvm::Value* load_world_point_ptr(llvm::Value* x, llvm::Value* y);
    llvm::Value* load_x();
    llvm::Value* load_y();

    void create_increment_fn();
    void create_decrement_fn();
    void create_move_left_fn();
    void create_move_right_fn();
    void create_move_down_fn();
    void create_move_up_fn();

    llvm::Function* increment_fn;
    llvm::Function* decrement_fn;
    llvm::Function* move_left_fn;
    llvm::Function* move_right_fn;
    llvm::Function* move_down_fn;
    llvm::Function* move_up_fn;

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;

    llvm::Function* print_fn;
    llvm::Function* getchar_fn;

    llvm::ArrayType* inner_world_type;
    llvm::ArrayType* outer_world_type;

    llvm::GlobalVariable* world;
    llvm::GlobalVariable* x;
    llvm::GlobalVariable* y;
};

} // namespace bf2