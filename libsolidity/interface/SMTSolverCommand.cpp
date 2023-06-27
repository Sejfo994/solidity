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
#include <libsolidity/interface/SMTSolverCommand.h>

#include <liblangutil/Exceptions.h>

#include <libsolutil/CommonIO.h>
#include <libsolutil/Exceptions.h>
#include <libsolutil/Keccak256.h>
#include <libsolutil/TemporaryDirectory.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/process.hpp>

using solidity::langutil::InternalCompilerError;
using solidity::util::errinfo_comment;


namespace solidity::frontend
{

ReadCallback::Result SMTSolverCommand::solve(std::string const& _kind, std::string const& _query)
{
	try
	{
		auto pos = _kind.find(' ');
		auto kind = _kind.substr(0, pos);
		auto solverCommand = _kind.substr(pos + 1);
		if (kind != ReadCallback::kindString(ReadCallback::Kind::SMTQuery))
			solAssert(false, "SMTQuery callback used as callback kind " + kind);

		auto tempDir = solidity::util::TemporaryDirectory("smt");
		util::h256 queryHash = util::keccak256(_query);
		auto queryFileName = tempDir.path() / ("query_" + queryHash.hex() + ".smt2");

		auto queryFile = boost::filesystem::ofstream(queryFileName);

		queryFile << _query;

		std::vector<std::string> commandArgs;
		boost::split(commandArgs, solverCommand, boost::is_any_of(" "));
		solAssert(commandArgs.size() > 0, "SMT command was empty");
		auto const& solverBinary = commandArgs[0];
		auto pathToBinary = boost::process::search_path(solverBinary);

		if (pathToBinary.empty())
			return ReadCallback::Result{false, solverBinary + " binary not found."};

		commandArgs.erase(commandArgs.begin());
		commandArgs.push_back(queryFileName.string());
		boost::process::ipstream pipe;
		boost::process::child solver(
			pathToBinary,
			boost::process::args(commandArgs),
			boost::process::std_out > pipe
		);

		std::vector<std::string> data;
		std::string line;
		while (solver.running() && std::getline(pipe, line))
			if (!line.empty())
				data.push_back(line);

		solver.wait();

		return ReadCallback::Result{true, boost::join(data, "\n")};
	}
	catch (...)
	{
		return ReadCallback::Result{false, "Unknown exception in SMTQuery callback: " + boost::current_exception_diagnostic_information()};
	}
}

}
