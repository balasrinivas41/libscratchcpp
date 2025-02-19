// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "entity.h"
#include "blockprototype.h"

namespace libscratchcpp
{

class IEngine;
class Target;
class Input;
class Field;
class InputValue;
class BlockPrivate;

/*! \brief The Block class represents a Scratch block. */
class LIBSCRATCHCPP_EXPORT Block : public Entity
{
    public:
        friend class Engine;

        Block(const std::string &id, const std::string &opcode);
        Block(const Block &) = delete;

        void compile(Compiler *compiler);

        std::string opcode() const;

        std::shared_ptr<Block> next() const;
        std::string nextId() const;
        void setNext(std::shared_ptr<Block> block);
        void setNextId(const std::string &nextId);

        std::shared_ptr<Block> parent() const;
        std::string parentId() const;
        void setParent(std::shared_ptr<Block> block);
        void setParentId(const std::string &id);

        std::vector<std::shared_ptr<Input>> inputs() const;
        int addInput(std::shared_ptr<Input> input);
        std::shared_ptr<Input> inputAt(int index) const;
        int findInput(const std::string &inputName) const;
        Input *findInputById(int id) const;

        std::vector<std::shared_ptr<Field>> fields() const;
        int addField(std::shared_ptr<Field> field);
        std::shared_ptr<Field> fieldAt(int index) const;
        int findField(const std::string &fieldName) const;
        Field *findFieldById(int id) const;

        bool shadow() const;
        void setShadow(bool newShadow);

        bool topLevel() const;

        void setEngine(IEngine *newEngine);
        IEngine *engine() const;

        void setTarget(Target *newTarget);
        Target *target() const;

        BlockComp compileFunction() const;
        void setCompileFunction(BlockComp newCompileFunction);

        bool mutationHasNext() const;
        void setMutationHasNext(bool newMutationHasNext);

        BlockPrototype *mutationPrototype();

        bool isTopLevelReporter() const;
        void setIsTopLevelReporter(bool isTopLevelReporter);

        InputValue *topLevelReporterInfo();

    private:
        void updateInputMap();
        void updateFieldMap();

        spimpl::unique_impl_ptr<BlockPrivate> impl;
};

} // namespace libscratchcpp
