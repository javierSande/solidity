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

#include <libsolidity/codegen/ir/optimiser/ArrayLoopOptimizedAccessGenerator.h>

#include <libsolidity/codegen/ir/IRGenerationContext.h>
#include <libsolidity/codegen/ir/IRLValue.h>
#include <libsolidity/codegen/ir/IRVariable.h>
#include <libsolidity/codegen/YulUtilFunctions.h>
#include <libsolidity/codegen/ABIFunctions.h>
#include <libsolidity/codegen/CompilerUtils.h>
#include <libsolidity/codegen/ReturnInfo.h>
#include <libsolidity/ast/TypeProvider.h>
#include <libsolidity/ast/ASTUtils.h>

#include <liblangutil/Exceptions.h>

#include <libsolutil/StringUtils.h>
#include <libsolutil/Visitor.h>

#include <range/v3/view/transform.hpp>

using namespace std;
using namespace solidity;
using namespace solidity::frontend;
using namespace solidity::langutil;
using namespace solidity::util;

std::unordered_map<ASTString, ASTString> const& ArrayLoopOptimizedAccessGenerator::generateArrayLengths(
	IRGeneratorForStatements& _generator,
	IRGenerationContext& _context,
	YulUtilFunctions& _utils
)
{
	for (Expression const* arrayBase: m_accessedArrays)
	{
		ArrayType const& arrayType = dynamic_cast<ArrayType const&>(*arrayBase->annotation().type);
		string arrayLengthVar = _context.newYulVariable();

		arrayBase->accept(_generator);

		_generator.appendCode() <<
			"let " << arrayLengthVar << " := " <<
			_utils.arrayLengthFunction(arrayType) <<
			"(" << IRVariable(*arrayBase).commaSeparatedList() << ")" << "\n";

		if (arrayType.dataStoredIn(DataLocation::Storage))
			m_accessedArraySlotMap[arrayBase->nodeToString()] = IRVariable(*arrayBase).part("slot").name();
		else
			m_accessedArraySlotMap[arrayBase->nodeToString()] = IRVariable(*arrayBase).part("mpos").name();

		m_accessedArrayLengthMap[arrayBase->nodeToString()] = arrayLengthVar;
	}
	return m_accessedArrayLengthMap;
}
