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

#pragma once

#include <libsolidity/ast/ASTEnums.h>
#include <libsolidity/ast/ASTForward.h>
#include <libsolidity/ast/ASTVisitor.h>

#include <map>
#include <memory>
#include <optional>

namespace solidity::frontend
{

class ViewArrayChecker: private ASTConstVisitor
{
public:
	ViewArrayChecker(ASTNode const* _ast):
		m_ast(_ast) {}

	bool check();

private:
	bool visit(InlineAssembly const& _inlineAssembly) override;
	bool visit(MemberAccess const& _memberAccess) override;
	void endVisit(MemberAccess const& _memberAccess) override;
	void endVisit(FunctionCall const& _memberAccess) override;
	void endVisit(Assignment const& _assignment) override;

	ASTNode const* m_ast;

	bool m_arrayView = true;
};

}
