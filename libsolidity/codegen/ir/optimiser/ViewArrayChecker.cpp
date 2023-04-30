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

#include <libsolidity/codegen/ir/optimiser/ViewArrayChecker.h>
#include <libyul/AST.h>

#include <functional>
#include <utility>
#include <variant>

using namespace std;
using namespace solidity;
using namespace solidity::langutil;
using namespace solidity::frontend;

bool ViewArrayChecker::check()
{
	m_ast->accept(*this);
	return m_arrayView;
}

bool ViewArrayChecker::visit(InlineAssembly const&)
{
	m_arrayView = false;
	return false;
}

void ViewArrayChecker::endVisit(FunctionCall const& _functionCall)
{
	if (*_functionCall.annotation().kind != FunctionCallKind::FunctionCall)
		return;

	StateMutability state = dynamic_cast<FunctionType const&>(*_functionCall.expression().annotation().type).stateMutability();
	if (state != StateMutability::View && state != StateMutability::Pure)
		m_arrayView = false;
}

bool ViewArrayChecker::visit(MemberAccess const& _memberAccess)
{
	// Catch the case case of `f.push` or `f.pop` which is an expression that modifies the size of an array.
	ASTString const& member = _memberAccess.memberName();
	if (_memberAccess.expression().annotation().type->category() == Type::Category::Array)
	{
		auto const& type = dynamic_cast<ArrayType const&>(*_memberAccess.expression().annotation().type);
		if ((member == "push" || member == "pop") && type.isDynamicallySized() && type.dataStoredIn(DataLocation::Storage))
		{
			m_arrayView = false;
			return false;
		}
	}

	return true;
}


void ViewArrayChecker::endVisit(MemberAccess const& _memberAccess)
{
	ASTString const& member = _memberAccess.memberName();
	switch (_memberAccess.expression().annotation().type->category())
	{
	case Type::Category::Magic:
	{
		using MagicMember = pair<MagicType::Kind, string>;
		set<MagicMember> static const payableMembers{
			{MagicType::Kind::Message, "value"}
		};

		auto const& type = dynamic_cast<MagicType const&>(*_memberAccess.expression().annotation().type);
		MagicMember magicMember(type.kind(), member);

		if (payableMembers.count(magicMember))
			m_arrayView = false;
		break;
	}
	default:
		break;
	}
}

void ViewArrayChecker::endVisit(Assignment const& _assignment)
{
	Type const* type = _assignment.leftHandSide().annotation().type;
	if (type->category()  == Type::Category::Array &&
		dynamic_cast<ArrayType const&>(*type).dataStoredIn(DataLocation::Storage)
	)
		m_arrayView = false;
}
