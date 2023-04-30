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
 * Optimiser array iterations in for loops
 */

#include <libsolidity/codegen/ir/optimiser/ArrayLoopOptimiser.h>

#include <libyul/AST.h>
#include <libsolutil/StringUtils.h>

using namespace std;
using namespace solidity;
using namespace solidity::frontend;
using namespace solidity::langutil;
using namespace solidity::util;


bool ArrayLoopOptimiser::startOptimization(ForStatement const& _forStatement)
{
	m_loop = &_forStatement;
	m_scopes.push_back(unordered_set<ASTString>());
	m_accessedArraysInScope.push_back(vector<Expression const*>());

	ViewArrayChecker arrayChecker(&_forStatement);
	if (!arrayChecker.check())
		return false;

	_forStatement.accept(*this);

	return !m_accessedArraysInScope.back().empty();
}

bool ArrayLoopOptimiser::startOptimization(WhileStatement const& _whileStatement)
{
	m_loop = &_whileStatement;
	m_scopes.push_back(unordered_set<ASTString>());
	m_accessedArraysInScope.push_back(vector<Expression const*>());

	ViewArrayChecker arrayChecker(&_whileStatement);
	if (!arrayChecker.check())
		return false;

	m_scopes.push_back(unordered_set<ASTString>());
	_whileStatement.accept(*this);

	return !m_accessedArraysInScope.back().empty();
}

void ArrayLoopOptimiser::endOptimization()
{
	unordered_set<ASTString> scope = m_scopes.back();
	for (ASTString _arrayBase: scope)
	{
		m_accessedArraysSet.erase(_arrayBase);
		m_accessedArrayLengthMap.erase(_arrayBase);
		m_accessedArraySlotMap.erase(_arrayBase);
	}

	m_accessedArraysInScope.pop_back();
	m_scopes.pop_back();
}

void ArrayLoopOptimiser::addArrayBase(Expression const* _arrayBase)
{
	if (!isOptimizable(*_arrayBase))
	{
		m_scopes.back().insert(_arrayBase->nodeToString());
		m_accessedArraysInScope.back().push_back(_arrayBase);
		m_accessedArraysSet.insert(_arrayBase->nodeToString());
	}
}

void ArrayLoopOptimiser::removeArrayBase(Expression const* _arrayBase)
{
	unordered_set<ASTString> scope = m_scopes.back();
	scope.erase(_arrayBase->nodeToString());
	vector<Expression const*> accessedArraysInScope = m_accessedArraysInScope.back();
	accessedArraysInScope.erase(
		std::remove_if(
			accessedArraysInScope.begin(),
			accessedArraysInScope.end(),
			[_arrayBase](Expression  const* expr) { return expr->nodeToString() == _arrayBase->nodeToString(); }
		),
		accessedArraysInScope.end()
	);

}

bool ArrayLoopOptimiser::isOptimizable(Expression const& _arrayBase)
{
	return m_accessedArraysSet.find(_arrayBase.nodeToString()) != m_accessedArraysSet.end();
}

ASTString ArrayLoopOptimiser::getLengthVariable(Expression const& _arrayBase)
{
	solAssert(isOptimizable(_arrayBase), "array length was not previous generated");
	return m_accessedArrayLengthMap[_arrayBase.nodeToString()];
}

ASTString ArrayLoopOptimiser::getSlotVariable(Expression const& _arrayBase)
{
	solAssert(isOptimizable(_arrayBase), "array length was not previous generated");
	return m_accessedArraySlotMap[_arrayBase.nodeToString()];
}

void ArrayLoopOptimiser::endVisit(IndexAccess const& _indexAccess)
{
	Expression const& _arrayBase = _indexAccess.baseExpression();
	Type const *type = _arrayBase.annotation().type;

	if (!type->dataStoredIn(DataLocation::Storage))
		return;

	if (!dynamic_cast<ArrayType const*>(type) || isOptimizable(_indexAccess.baseExpression()))
		return;

	if (!dynamic_cast<Identifier const*>(&_indexAccess.baseExpression()))
		return;

	if (dynamic_cast<ReferenceType const*>(type) && dynamic_cast<ReferenceType const*>(type)->isPointer())
	{
		Identifier const* identifier = dynamic_cast<Identifier const*>(&_indexAccess.baseExpression());

		if (m_loop->location() < identifier->annotation().referencedDeclaration->location())
			return;
	}


	addArrayBase(&_arrayBase);
}

void ArrayLoopOptimiser::endVisit(MemberAccess const& _memberAccess)
{
	if (_memberAccess.memberName() != "length"
	)
		return;

	Expression const& _arrayBase = _memberAccess.expression();
	Type const *type = _arrayBase.annotation().type;
	
	if (!dynamic_cast<Identifier const*>(&_arrayBase))
		return;

	if (dynamic_cast<ReferenceType const*>(type)->isPointer())
 		return;

	if (!type->dataStoredIn(DataLocation::Storage))
		return;

	addArrayBase(&_arrayBase);
}


void ArrayLoopOptimiser::endVisit(Assignment const& _assignment)
{
	if (isOptimizable(_assignment.leftHandSide()) &&
		_assignment.assignmentOperator() == Token::Assign &&
		_assignment.leftHandSide().nodeToString() != _assignment.rightHandSide().nodeToString() )
	{
		removeArrayBase(&_assignment.leftHandSide());
	}
}
