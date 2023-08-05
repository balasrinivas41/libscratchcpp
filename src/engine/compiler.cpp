// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compiler.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <iostream>

#include "compiler_p.h"

// TODO: Remove this
#include "engine.h"

using namespace libscratchcpp;
using namespace vm;

/*! Constructs Compiler. */
Compiler::Compiler(IEngine *engine) :
    impl(spimpl::make_unique_impl<CompilerPrivate>(engine))
{
}

/*! Compiles the script. Use bytecode() to read the generated bytecode. */
void Compiler::compile(std::shared_ptr<Block> topLevelBlock)
{
    impl->bytecode.clear();
    impl->procedurePrototype = nullptr;
    impl->atomic = true;
    impl->warp = false;

    // Add start instruction
    addInstruction(OP_START);

    impl->block = topLevelBlock;
    while (impl->block) {
        size_t substacks = impl->substackTree.size();

        if (impl->block->compileFunction())
            impl->block->compile(this);
        else
            std::cout << "warning: unsupported block: " << impl->block->opcode() << std::endl;

        if (substacks != impl->substackTree.size())
            continue;

        if (impl->block)
            impl->block = impl->block->next();

        if (!impl->block && !impl->substackTree.empty())
            impl->substackEnd();
    }

    // Add end instruction (halt)
    addInstruction(OP_HALT);
}

/*! Returns the generated bytecode. */
const std::vector<unsigned int> &Compiler::bytecode() const
{
    return impl->bytecode;
}

/*! Returns the Engine. */
IEngine *Compiler::engine() const
{
    return impl->engine;
}

/*! Returns the list of constant input values. */
const std::vector<InputValue *> &Compiler::constInputValues() const
{
    return impl->constValues;
}

/*! Returns the list of constant values. */
std::vector<Value> Compiler::constValues() const
{
    std::vector<Value> ret;
    for (auto value : impl->constValues)
        ret.push_back(value->value());
    return ret;
}

/*! Returns the list of variables. */
const std::vector<Variable *> &Compiler::variables() const
{
    return impl->variables;
}

/*! Returns the list of pointers to variable values. */
std::vector<Value *> Compiler::variablePtrs() const
{
    std::vector<Value *> ret;
    for (auto var : impl->variables)
        ret.push_back(var->valuePtr());
    return ret;
}

/*! Returns the list of lists. */
const std::vector<List *> &Compiler::lists() const
{
    return impl->lists;
}

/*! Adds an instruction to the bytecode. */
void Compiler::addInstruction(Opcode opcode, std::initializer_list<unsigned int> args)
{
    impl->addInstruction(opcode, args);
}

/*! Compiles the given input and adds it to the bytecode. */
void Compiler::addInput(Input *input)
{
    if (!input) {
        addInstruction(OP_NULL);
        return;
    }
    switch (input->type()) {
        case Input::Type::Shadow:
            addInstruction(OP_CONST, { constIndex(input->primaryValue()) });
            break;

        case Input::Type::NoShadow: {
            auto previousBlock = impl->block;
            impl->block = input->valueBlock();
            assert(impl->block);
            if (impl->block->compileFunction())
                impl->block->compile(this);
            else {
                std::cout << "warning: unsupported reporter block: " << impl->block->opcode() << std::endl;
                addInstruction(OP_NULL);
            }
            impl->block = previousBlock;
            break;
        }

        case Input::Type::ObscuredShadow: {
            auto previousBlock = impl->block;
            impl->block = input->valueBlock();
            if (impl->block) {
                if (impl->block->compileFunction())
                    impl->block->compile(this);
                else {
                    std::cout << "warning: unsupported reporter block: " << impl->block->opcode() << std::endl;
                    addInstruction(OP_NULL);
                }
            } else
                input->primaryValue()->compile(this);
            impl->block = previousBlock;
            break;
        }
    }
}

/*! Compiles the given input (resolved by ID) and adds it to the bytecode. */
void Compiler::addInput(int id)
{
    addInput(input(id));
}

/*! Adds a function call to the bytecode (the OP_EXEC instruction). */
void Compiler::addFunctionCall(BlockFunc f)
{
    // TODO: Remove the cast;
    addInstruction(OP_EXEC, { dynamic_cast<Engine *>(impl->engine)->functionIndex(f) });
}

/*! Adds an argument to a procedure (custom block). */
void Compiler::addProcedureArg(std::string procCode, std::string argName)
{
    impl->procedureArgs[procCode].push_back(argName);
}

/*! Jumps to the given substack. The second substack is used for example for the if/else block. */
void Compiler::moveToSubstack(std::shared_ptr<Block> substack1, std::shared_ptr<Block> substack2, SubstackType type)
{
    impl->substackTree.push_back({ { impl->block, substack2 }, type });
    impl->block = substack1;
    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given substack. */
void Compiler::moveToSubstack(std::shared_ptr<Block> substack, SubstackType type)
{
    moveToSubstack(substack, nullptr, type);
}

/*!
 * Adds the vm::OP_BREAK_ATOMIC instruction at the end of the current loop.
 * This can be used for example in motion blocks.
 * \note Nothing will happen if the script is set to run without screen refresh.
 */
void Compiler::breakAtomicScript()
{
    impl->atomic = false;
}

/*! Makes current script run without screen refresh. */
void Compiler::warp()
{
    impl->warp = true;
    addInstruction(vm::OP_WARP);
}

/*! Returns the input with the given ID. */
Input *Compiler::input(int id) const
{
    return impl->block->findInputById(id);
}

/*! Returns the field with the given ID. */
Field *Compiler::field(int id) const
{
    return impl->block->findFieldById(id);
}

/*! Returns the block in the given input. Same as input(id)->valueBlock(), but with a null pointer check. */
std::shared_ptr<Block> Compiler::inputBlock(int id) const
{
    auto in = input(id);
    return in ? in->valueBlock() : nullptr;
}

/*! Returns the index of the given variable. */
unsigned int Compiler::variableIndex(std::shared_ptr<Entity> varEntity)
{
    auto var = dynamic_cast<Variable *>(varEntity.get());
    auto it = std::find(impl->variables.begin(), impl->variables.end(), var);
    if (it != impl->variables.end())
        return it - impl->variables.begin();
    impl->variables.push_back(var);
    return impl->variables.size() - 1;
}

/*! Returns the index of the given list. */
unsigned int Compiler::listIndex(std::shared_ptr<Entity> listEntity)
{
    auto list = dynamic_cast<List *>(listEntity.get());
    auto it = std::find(impl->lists.begin(), impl->lists.end(), list);
    if (it != impl->lists.end())
        return it - impl->lists.begin();
    impl->lists.push_back(list);
    return impl->lists.size() - 1;
}

/*! Returns the index of the given constant input value. */
unsigned int Compiler::constIndex(InputValue *value)
{
    auto it = std::find(impl->constValues.begin(), impl->constValues.end(), value);
    if (it != impl->constValues.end())
        return it - impl->constValues.begin();
    impl->constValues.push_back(value);
    return impl->constValues.size() - 1;
}

/*! Returns the index of the procedure code of the given block. */
unsigned int Compiler::procedureIndex(std::string proc)
{
    auto it = std::find(impl->procedures.begin(), impl->procedures.end(), proc);
    if (it != impl->procedures.end())
        return it - impl->procedures.begin();
    impl->procedures.push_back(proc);
    return impl->procedures.size() - 1;
}

/*! Returns the index of the argument of the given procedure (custom block). */
long Compiler::procedureArgIndex(std::string procCode, std::string argName)
{
    if (impl->procedureArgs.count(procCode) == 0) {
        std::cout << "warning: could not find custom block '" << procCode << "'" << std::endl;
        return -1;
    }
    const std::vector<std::string> args = impl->procedureArgs[procCode];
    auto it = std::find(args.begin(), args.end(), argName);
    if (it != args.end())
        return it - args.begin();
    std::cout << "warning: could not find argument '" << argName << "' in custom block '" << procCode << "'" << std::endl;
    return -1;
}

/*! Returns the prototype of the current custom block. */
BlockPrototype *Compiler::procedurePrototype() const
{
    return impl->procedurePrototype;
}

/*! Sets the prototype of the current custom block. */
void Compiler::setProcedurePrototype(BlockPrototype *prototype)
{
    impl->procedurePrototype = prototype;
}

/*! Returns the list of custom block prototypes. */
const std::vector<std::string> &Compiler::procedures() const
{
    return impl->procedures;
}

/*! Returns the current block. */
const std::shared_ptr<Block> &Compiler::block() const
{
    return impl->block;
}
