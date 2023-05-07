// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/global.h"
#include "field.h"
#include "ientity.h"
#include "input.h"
#include <memory>
#include <map>
#include <string>

namespace libscratchcpp
{

/*! \brief The Block class represents a Scratch block. */
class LIBSCRATCHCPP_EXPORT Block : public IEntity
{
    public:
        Block(std::string id, std::string opcode);
        Block(const Block &) = delete;

        Value run(RunningScript *script = nullptr, const Value &defaultRetValue = Value());
        void compile(Compiler *compiler);

        std::string opcode() const;

        BlockImpl implementation() const;
        void setImplementation(BlockImpl impl);

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
        void updateInputMap();

        std::vector<std::shared_ptr<Field>> fields() const;
        int addField(std::shared_ptr<Field> field);
        std::shared_ptr<Field> fieldAt(int index) const;
        int findField(const std::string &fieldName) const;
        Field *findFieldById(int id) const;
        void updateFieldMap();

        bool shadow() const;
        void setShadow(bool newShadow);

        bool topLevel() const;
        void setTopLevel(bool newTopLevel);

        void setEngine(Engine *newEngine);

        void setTarget(Target *newTarget);

        BlockComp compileFunction() const;
        void setCompileFunction(BlockComp newCompileFunction);

    private:
        std::string m_opcode;
        BlockImpl m_implementation = nullptr;
        BlockComp m_compileFunction = nullptr;
        std::shared_ptr<Block> m_next = nullptr;
        std::string m_nextId;
        std::shared_ptr<Block> m_parent = nullptr;
        std::string m_parentId;
        std::vector<std::shared_ptr<Input>> m_inputs;
        std::unordered_map<int, Input *> m_inputMap;
        std::vector<std::shared_ptr<Field>> m_fields;
        std::unordered_map<int, Field *> m_fieldMap;
        bool m_shadow = false;
        bool m_topLevel = false;
        Engine *m_engine = nullptr;
        Target *m_target = nullptr;
};

} // namespace libscratchcpp
