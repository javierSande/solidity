/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
// SPDX-License-Identifier: GPL-3.0
/**
 * Optimiser component that calculates hash values for blocks.
 */
#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <libsolidity/codegen/ir/IRLValue.h>
#include <libsolidity/codegen/ir/IRVariable.h>

#include <libsolidity/codegen/YulUtilFunctions.h>

#include <libsolidity/codegen/ir/IRGeneratorForStatements.h>

#include <string>
#include <unordered_map>

namespace solidity::frontend
{

class IRGenerationContext;
class IRGeneratorForStatements;

class ArrayLoopOptimizedAccessGenerator
{
public:
	ArrayLoopOptimizedAccessGenerator(std::vector<Expression  const*> _accessedArrays):
		m_accessedArrays(_accessedArrays),
		m_accessedArrayLengthMap(),
		m_accessedArraySlotMap()
	{}

	std::unordered_map<ASTString, ASTString> const& generateArrayLengths(
		IRGeneratorForStatements& _generator,
		IRGenerationContext& _context,
		YulUtilFunctions& _utils
	);

	std::unordered_map<ASTString, ASTString> const& getArraySlotMap() { return m_accessedArraySlotMap; }

	std::vector<Expression  const*> m_accessedArrays;
	std::unordered_map<ASTString, ASTString> m_accessedArrayLengthMap;
	std::unordered_map<ASTString, ASTString> m_accessedArraySlotMap;
};

}
