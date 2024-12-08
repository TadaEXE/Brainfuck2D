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
    init_world();

    create_decrement_fn();
    create_increment_fn();
    create_move_down_fn();
    create_move_left_fn();
    create_move_right_fn();
    create_move_up_fn();
}

VisitorImpl::~VisitorImpl()
{
}

std::any VisitorImpl::visitProgram(Brainfuck2DParser::ProgramContext* ctx)
{
    create_entry_point();
    auto visit_res = visitChildren(ctx);

    builder->CreateRet(builder->getInt32(0));
    std::error_code ec;
    llvm::raw_fd_ostream llvm_out("out.ll", ec);

    if (!ec)
    {
        module->print(llvm_out, nullptr);
    }

    return visit_res;
}

std::any VisitorImpl::visitMoveRight(Brainfuck2DParser::MoveRightContext* ctx)
{
    (void)builder->CreateCall(move_right_fn);
    return visitChildren(ctx);
}

std::any VisitorImpl::visitMoveLeft(Brainfuck2DParser::MoveLeftContext* ctx)
{
    (void)builder->CreateCall(move_left_fn);
    return visitChildren(ctx);
}

std::any VisitorImpl::visitIncrement(Brainfuck2DParser::IncrementContext* ctx)
{
    (void)builder->CreateCall(increment_fn);
    return visitChildren(ctx);
}

std::any VisitorImpl::visitDecrement(Brainfuck2DParser::DecrementContext* ctx)
{
    (void)builder->CreateCall(decrement_fn);
    return visitChildren(ctx);
}

std::any VisitorImpl::visitOutput(Brainfuck2DParser::OutputContext* ctx)
{
    auto wp = load_world_point_ptr(load_x(), load_y());
    (void)builder->CreateCall(print_fn, {wp});
    return visitChildren(ctx);
}

std::any VisitorImpl::visitInput(Brainfuck2DParser::InputContext* ctx)
{
    auto wp = load_world_point_ptr(load_x(), load_y());
    auto input = builder->CreateCall(getchar_fn);
    (void)builder->CreateStore(input, wp);
    return visitChildren(ctx);
}

std::any VisitorImpl::visitLoop(Brainfuck2DParser::LoopContext* ctx)
{
    auto loop_cmp = llvm::BasicBlock::Create(*context, "loop_cmp", module->getFunction("main"));
    auto loop_body = llvm::BasicBlock::Create(*context, "loop_body", module->getFunction("main"));
    auto loop_end = llvm::BasicBlock::Create(*context, "loop_end", module->getFunction("main"));

    (void)builder->CreateBr(loop_cmp);

    builder->SetInsertPoint(loop_cmp);
    auto wp_ptr = load_world_point_ptr(load_x(), load_y());
    auto wp = builder->CreateLoad(builder->getInt32Ty(), wp_ptr, "wp");
    auto cmp = builder->CreateICmpNE(wp, builder->getInt32(0));
    (void)builder->CreateCondBr(cmp, loop_body, loop_end);

    builder->SetInsertPoint(loop_body);
    auto visit = visitChildren(ctx);
    (void)builder->CreateBr(loop_cmp);

    builder->SetInsertPoint(loop_end);

    return visit;
}

std::any VisitorImpl::visitMoveUp(Brainfuck2DParser::MoveUpContext* ctx)
{
    (void)builder->CreateCall(move_up_fn);
    return visitChildren(ctx);
}

std::any VisitorImpl::visitMoveDown(Brainfuck2DParser::MoveDownContext* ctx)
{
    (void)builder->CreateCall(move_down_fn);
    return visitChildren(ctx);
}

void VisitorImpl::create_entry_point()
{
    auto i32 = builder->getInt32Ty();
    auto prototype = llvm::FunctionType::get(i32, false);
    auto main_fn = llvm::Function::Create(prototype, llvm::Function::ExternalLinkage, "main", module.get());
    auto body = llvm::BasicBlock::Create(*context, "entry", main_fn);
    builder->SetInsertPoint(body);
}

void VisitorImpl::init_world()
{
    inner_world_type = llvm::ArrayType::get(builder->getInt32Ty(), world_size);
    outer_world_type = llvm::ArrayType::get(inner_world_type, world_size);

    world = new llvm::GlobalVariable(*module, outer_world_type, false, llvm::GlobalValue::LinkageTypes::ExternalLinkage,
                                     llvm::Constant::getNullValue(outer_world_type), "world");

    x = new llvm::GlobalVariable(*module, builder->getInt32Ty(), false,
                                 llvm::GlobalValue::LinkageTypes::ExternalLinkage,
                                 llvm::ConstantInt::get(builder->getInt32Ty(), 0), "x");

    y = new llvm::GlobalVariable(*module, builder->getInt32Ty(), false,
                                 llvm::GlobalValue::LinkageTypes::ExternalLinkage,
                                 llvm::ConstantInt::get(builder->getInt32Ty(), 0), "y");
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

llvm::Value* VisitorImpl::load_world_point_ptr(llvm::Value* x, llvm::Value* y)
{
    auto world = module->getNamedGlobal("world");
    return builder->CreateInBoundsGEP(outer_world_type, world, {builder->getInt32(0), x, y}, "wp_ptr");
}

llvm::Value* VisitorImpl::load_x()
{
    auto x = module->getNamedGlobal("x");
    return builder->CreateLoad(builder->getInt32Ty(), x, "x");
}

llvm::Value* VisitorImpl::load_y()
{
    auto y = module->getNamedGlobal("y");
    return builder->CreateLoad(builder->getInt32Ty(), y, "y");
}

void VisitorImpl::create_increment_fn()
{
    increment_fn = llvm::Function::Create(llvm::FunctionType::get(builder->getVoidTy(), false),
                                          llvm::GlobalValue::LinkageTypes::ExternalLinkage, "increment", *module);
    auto body = llvm::BasicBlock::Create(*context, "entry", increment_fn);
    builder->SetInsertPoint(body);

    auto wp_ptr = load_world_point_ptr(load_x(), load_y());
    auto wp = builder->CreateLoad(builder->getInt32Ty(), wp_ptr, "wp");
    auto add = builder->CreateAdd(wp, builder->getInt32(1));
    (void)builder->CreateStore(add, wp_ptr);
    (void)builder->CreateRetVoid();
}

void VisitorImpl::create_decrement_fn()
{
    decrement_fn = llvm::Function::Create(llvm::FunctionType::get(builder->getVoidTy(), false),
                                          llvm::GlobalValue::LinkageTypes::ExternalLinkage, "decrement", *module);
    auto body = llvm::BasicBlock::Create(*context, "entry", decrement_fn);
    builder->SetInsertPoint(body);

    auto wp_ptr = load_world_point_ptr(load_x(), load_y());
    auto wp = builder->CreateLoad(builder->getInt32Ty(), wp_ptr, "wp");
    auto sub = builder->CreateSub(wp, builder->getInt32(1));
    (void)builder->CreateStore(sub, wp_ptr);
    (void)builder->CreateRetVoid();
}

void VisitorImpl::create_move_left_fn()
{
    move_left_fn = llvm::Function::Create(llvm::FunctionType::get(builder->getVoidTy(), false),
                                          llvm::GlobalValue::LinkageTypes::ExternalLinkage, "move_left", *module);
    auto body = llvm::BasicBlock::Create(*context, "entry", move_left_fn);
    builder->SetInsertPoint(body);

    auto sub = builder->CreateSub(load_x(), builder->getInt32(1));
    (void)builder->CreateStore(sub, module->getNamedGlobal("x"));
    (void)builder->CreateRetVoid();
}

void VisitorImpl::create_move_right_fn()
{
    move_right_fn = llvm::Function::Create(llvm::FunctionType::get(builder->getVoidTy(), false),
                                           llvm::GlobalVariable::LinkageTypes::ExternalLinkage, "move_right", *module);
    auto body = llvm::BasicBlock::Create(*context, "entry", move_right_fn);
    builder->SetInsertPoint(body);

    auto add = builder->CreateAdd(load_x(), builder->getInt32(1));
    (void)builder->CreateStore(add, module->getNamedGlobal("x"));
    (void)builder->CreateRetVoid();
}

void VisitorImpl::create_move_down_fn()
{
    move_down_fn = llvm::Function::Create(llvm::FunctionType::get(builder->getVoidTy(), false),
                                          llvm::GlobalVariable::LinkageTypes::ExternalLinkage, "move_down", *module);
    auto body = llvm::BasicBlock::Create(*context, "enty", move_down_fn);
    builder->SetInsertPoint(body);

    auto sub = builder->CreateSub(load_y(), builder->getInt32(1));
    (void)builder->CreateStore(sub, module->getNamedGlobal("y"));
    (void)builder->CreateRetVoid();
}

void VisitorImpl::create_move_up_fn()
{
    move_up_fn = llvm::Function::Create(llvm::FunctionType::get(builder->getVoidTy(), false),
                                        llvm::GlobalVariable::LinkageTypes::ExternalLinkage, "move_up", *module);
    auto body = llvm::BasicBlock::Create(*context, "entry", move_up_fn);
    builder->SetInsertPoint(body);

    auto add = builder->CreateAdd(load_y(), builder->getInt32(1));
    (void)builder->CreateStore(add, module->getNamedGlobal("y"));
    (void)builder->CreateRetVoid();
}

} // namespace bf2