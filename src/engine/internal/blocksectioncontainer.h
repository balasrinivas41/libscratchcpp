// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>
#include <string>
#include <scratchcpp/global.h>

namespace libscratchcpp
{

class BlockSectionContainer
{
    public:
        BlockSectionContainer() = default;
        BlockSectionContainer(const BlockSectionContainer &) = delete;

        void addCompileFunction(const std::string &opcode, BlockComp f);
        void addHatBlock(const std::string &opcode);
        void addInput(const std::string &name, int id);
        void addField(const std::string &name, int id);
        void addFieldValue(const std::string &value, int id);

        BlockComp resolveBlockCompileFunc(const std::string &opcode) const;
        int resolveInput(const std::string &name) const;
        int resolveField(const std::string &name) const;
        int resolveFieldValue(const std::string &value) const;

    private:
        std::unordered_map<std::string, BlockComp> m_compileFunctions;
        std::unordered_map<std::string, int> m_inputs;
        std::unordered_map<std::string, int> m_fields;
        std::unordered_map<std::string, int> m_fieldValues;
};

} // namespace libscratchcpp
