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

#include <libsolidity/codegen/ir/optimiser/ViewArrayChecker.h>

#include <string>
#include <unordered_set>
#include <unordered_map>

namespace solidity::frontend
{

class ArrayLoopOptimiser: private ASTConstVisitor
{
public:
	ArrayLoopOptimiser():
		m_accessedArraysSet(),
		m_accessedArrayLengthMap(),
		m_accessedArraySlotMap(),
		m_scopes(),
		m_accessedArraysInScope(),
		m_loop(NULL)
	{}

	/// retrieves all the optimizable arrays accessed in the body of the loop
	/// and @returns true if any is found
	bool startOptimization(ForStatement const& _forStatement);
	bool startOptimization(WhileStatement const& _whileStatement);
	void endOptimization();

	std::vector<Expression const*> const& accessedArraysInScope() { return m_accessedArraysInScope.back(); }
	void setAccessedArrayLengths(std::unordered_map<ASTString, ASTString> _accessedArrayLengthMap) { m_accessedArrayLengthMap = std::move(_accessedArrayLengthMap); }
	void setAccessedArraySlots(std::unordered_map<ASTString, ASTString> _accessedArraySlotMap) { m_accessedArraySlotMap = std::move(_accessedArraySlotMap); }

	bool isOptimizable(Expression const& _arrayBase);
	ASTString getLengthVariable(Expression const& _arrayBase);
	ASTString getSlotVariable(Expression const& _arrayBase);

private:

	void addArrayBase(Expression const* _arrayBase);
	void removeArrayBase(Expression const* _arrayBase);

	// Visitor functions
	void endVisit(IndexAccess const& _indexAccess) override;
	void endVisit(Assignment const& _assignment) override;
	void endVisit(MemberAccess const& _memberAccess) override;

	std::unordered_set<ASTString> m_accessedArraysSet;
	std::unordered_map<ASTString, ASTString> m_accessedArrayLengthMap;
	std::unordered_map<ASTString, ASTString> m_accessedArraySlotMap;

	std::vector<std::unordered_set<ASTString>> m_scopes;
	std::vector<std::vector<Expression const*>> m_accessedArraysInScope;

	ASTNode const* m_loop;
};

}
