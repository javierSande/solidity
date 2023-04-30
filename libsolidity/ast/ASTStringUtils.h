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
 * @author Javier <c@ethdev.com>
 * @date 2022
 * Solidity abstract syntax tree.
 */

#pragma once

#include <libsolidity/ast/AST.h>

#include <vector>

namespace solidity::frontend
{

ASTString const joinStrings(std::vector<ASTString> const& elements, char separator) {
	ASTString out = elements.empty() ? elements[0] : "";

	for (unsigned long i = 1; i < elements.size(); i++)
		out += separator + elements[i];

	return out;
}

ASTString const Conditional::nodeToString() const {
	std::vector<ASTString> components;
	components.push_back(condition().nodeToString());
	components.push_back(TokenTraits::toString(Token::Conditional));
	components.push_back(trueExpression().nodeToString());
	components.push_back(TokenTraits::toString(Token::Colon));
	components.push_back(falseExpression().nodeToString());
	return joinStrings(components, ' ');
}

ASTString const Assignment::nodeToString() const {
	std::vector<ASTString> components;
	components.push_back(leftHandSide().nodeToString());
	components.push_back(TokenTraits::toString(assignmentOperator()));
	components.push_back(rightHandSide().nodeToString());
	return joinStrings(components, ' ');
}

ASTString const TupleExpression::nodeToString() const {
	ASTString str = isInlineArray() ? TokenTraits::toString(Token::LBrack) : TokenTraits::toString(Token::LParen);
	std::vector<ASTString> tupleComponents;
	for (ASTPointer<Expression> expression: components())
		tupleComponents.push_back(expression->nodeToString());

	str += joinStrings(tupleComponents, ',');
	str += isInlineArray() ? TokenTraits::toString(Token::RBrack) : TokenTraits::toString(Token::RParen);
	return str;
}

ASTString const UnaryOperation::nodeToString() const {
	if (isPrefixOperation())
		return TokenTraits::toString(getOperator()) + subExpression().nodeToString();
	return subExpression().nodeToString() + TokenTraits::toString(getOperator());
}

ASTString const BinaryOperation::nodeToString() const {
	std::vector<ASTString> components;
	components.push_back(leftExpression().nodeToString());
	components.push_back(TokenTraits::toString(getOperator()));
	components.push_back(rightExpression().nodeToString());
	return joinStrings(components, ' ');
}

ASTString const FunctionCall::nodeToString() const {
	ASTString str = expression().nodeToString() + TokenTraits::toString(Token::LParen);
	TokenTraits::toString(Token::LBrace);

	std::vector<ASTPointer<Expression const>> argumentList = arguments();
	std::vector<ASTPointer<ASTString>> nameList = names();
	bool namedParameters = !nameList.empty();

	if (namedParameters)
		str += TokenTraits::toString(Token::LBrace);

	std::vector<ASTString> argumentStrings;
	for (unsigned long i = 0; i < argumentList.size(); i++) {
		ASTString parameter = argumentList[i]->nodeToString();
		if (namedParameters)
			parameter += TokenTraits::toString(Token::Colon) + *nameList[i];
		argumentStrings.push_back(parameter);
	}

	str += joinStrings(argumentStrings, ',');

	if (namedParameters)
		str += TokenTraits::toString(Token::RBrace);

	str += TokenTraits::toString(Token::RParen);
	return str;
}

ASTString const FunctionCallOptions::nodeToString() const {
	ASTString str = expression().nodeToString() + TokenTraits::toString(Token::LBrace);

	std::vector<ASTPointer<Expression const>> optionsList = options();
	std::vector<ASTPointer<ASTString>> nameList = names();

	std::vector<ASTString> optionStrings;
	for (unsigned long i = 0; i < optionsList.size(); i++) {
		optionStrings.push_back(optionsList[i]->nodeToString() + TokenTraits::toString(Token::Equal) + *nameList[i]);
	}
	str += joinStrings(optionStrings, ',');
	str += TokenTraits::toString(Token::RBrace);
	return str;
}

ASTString const NewExpression::nodeToString() const {
	std::vector<ASTString> components;
	components.push_back(TokenTraits::toString(Token::New));
	components.push_back(typeName().nodeToString());
	return joinStrings(components, ' ');
}

ASTString const MemberAccess::nodeToString() const {
	return expression().nodeToString() + TokenTraits::toString(Token::Period) + memberName();
}

ASTString const IndexRangeAccess::nodeToString() const {
	ASTString str = baseExpression().nodeToString();
	str += TokenTraits::toString(Token::LBrack) + startExpression()->nodeToString();
	str += TokenTraits::toString(Token::Colon);
	str += endExpression()->nodeToString() + TokenTraits::toString(Token::RBrack);
	return str;
}

ASTString const IndexAccess::nodeToString() const {
	return baseExpression().nodeToString() + TokenTraits::toString(Token::LBrack)  + indexExpression()->nodeToString() + TokenTraits::toString(Token::RBrack);
}

ASTString const Identifier::nodeToString() const {
	return name();
}

ASTString const ElementaryTypeNameExpression::nodeToString() const {
	return type().typeName().toString();
}

ASTString const Literal::nodeToString() const {
	return value();
}

ASTString const ElementaryTypeName::nodeToString() const {
	ASTString str = typeName().toString();
	if (stateMutability().has_value())
		str += " " + stateMutabilityToString(stateMutability().value());
	return str;
}

ASTString const UserDefinedTypeName::nodeToString() const {
	return joinStrings(namePath(), '.');
}

ASTString const FunctionTypeName::nodeToString() const {
	std::vector<ASTString> components;
	components.push_back(TokenTraits::toString(Token::Function));
	components.push_back(parameterTypeList()->nodeToString());
	components.push_back(visibilityToString(visibility()));
	components.push_back(TokenTraits::toString(Token::Returns));
	components.push_back(returnParameterTypeList()->nodeToString());
	return joinStrings(components, ' ');
}

ASTString const Mapping::nodeToString() const {
	std::vector<ASTString> components;
	components.push_back(TokenTraits::toString(Token::Mapping));
	components.push_back(TokenTraits::toString(Token::LParen));
	components.push_back(keyType().nodeToString());
	components.push_back(TokenTraits::toString(Token::DoubleArrow));
	components.push_back(valueType().nodeToString());
	components.push_back(TokenTraits::toString(Token::RParen));
	return joinStrings(components, ' ');
}

ASTString const ArrayTypeName::nodeToString() const {
	return baseType().nodeToString() + TokenTraits::toString(Token::LBrack) + length()->nodeToString() + TokenTraits::toString(Token::RBrack);
}

ASTString const ParameterList::nodeToString() const {
	ASTString str = TokenTraits::toString(Token::LParen);

	std::vector<ASTString> parameterStrings;
	for (ASTPointer<VariableDeclaration> parameter: parameters()) {
		parameterStrings.push_back(parameter->nodeToString());
	}
	str += joinStrings(parameterStrings, ',');

	str += TokenTraits::toString(Token::RParen);
	return str;
}

ASTString const VariableDeclaration::nodeToString() const {
	std::vector<ASTString> components;
	components.push_back(typeName().nodeToString());
	components.push_back(locationToString(referenceLocation()));
	components.push_back(name());

	components.push_back(TokenTraits::toString(Token::Conditional));
	return joinStrings(components, ' ');
}

}
