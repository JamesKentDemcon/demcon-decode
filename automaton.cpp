// cellular_automaton.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <map>

const std::map<uint8_t, bool> theARuleset = { {0b000, false}, {0b001, true}, {0b010, false}, {0b011, true}, {0b100, true}, {0b101, true}, {0b110, true}, {0b111, false} };
const std::map<uint8_t, bool> theBRuleset = { {0b000, false}, {0b001, true}, {0b010, true}, {0b011, false}, {0b100, true}, {0b101, false}, {0b110, true}, {0b111, false} };

struct Arguments {
	bool valid;
	char type;
	int numOfCells;
	int numOfGenerations;
	std::vector<uint8_t> initialConfig;
	std::vector<uint8_t> universalRuleset;
};

Arguments validInput(int n, char* args[]) {

	Arguments myArguments;
	myArguments.valid = true;
	
	if (n < 6) {
		myArguments.valid = false;
	}
	else {
		// Validate type.
		char type = *args[1];
		if (!(*args[1] != 'A' || *args[1] != 'B' || *args[1] != 'U')) {
			myArguments.valid = false;
			return myArguments;
		}
		else {
			myArguments.type = *args[1];
		}

		try {
			
			// Validate number of cells & generations.
			myArguments.numOfCells = std::stoi(args[2]);
			myArguments.numOfGenerations = std::stoi(args[3]);

			myArguments.initialConfig.resize(myArguments.numOfCells, 0);

			// Verify it contains init_start and init_end.
			int i = 5;
			if (!std::string("init_start").compare(args[4])) {
				
				while (std::string("init_end").compare(args[i])) {
					int pos = std::stoi(args[i]);

					if (pos < myArguments.numOfCells) {
						myArguments.initialConfig[pos - 1] = 0x1;
					}

					i++;
				}
			}

			if (std::string("init_end").compare(args[i])) {
				myArguments.valid = false;
				return myArguments;
			}
			else {
				// Increment past init_end.
				i++;
			}

			// If there are still remaining arguments, its the universal ruleset.
			while (i < n) {
				uint8_t value = std::stoi(args[i]);
				myArguments.universalRuleset.push_back(value);
				i++;
			}

			// In case of universal type, verify we have all the rules.
			if (myArguments.type == 'U' && myArguments.universalRuleset.size() != 8) {
				myArguments.valid = false;
				return myArguments;
			}

			myArguments.valid = true;
		}
		catch (std::exception& e) {
			myArguments.valid = false;
			return myArguments;
		}
	}

	return myArguments;
}


void printUsage() {
	std::cout << "Error: unrecognized or incomplete command line.\n";
	std::cout << "USAGE:\n";
	std::cout << "\tautomaton type numberofcells numberofgenerations init_start <1, 2, n, ...> init_end <b0, ..., b7>\n";
	std::cout << "\n";
	std::cout << "\t\ttype \t\t\t Type of automaton, A B or U.\n";
	std::cout << "\t\tnumberofcells \t\t Number of cells.\n";
	std::cout << "\t\tnumberofgenerations \t Number of generations.\n";
	std::cout << "\t\tinit_start \t\t Start indication of occupied cells.\n";
	std::cout << "\t\t<1, 2, n, ...> \t\t Positions of occupied cells. Integers higher than numberofcells are ignored.\n";
	std::cout << "\t\tinit_end \t\t End indication of occupied cells.\n";
	std::cout << "\t\t<b0, ..., b7> \t\t Ruleset required if type is U.\n";
}

std::map<uint8_t, bool> generateUniversalRuleset(const std::vector<uint8_t>& rules) {

	std::map<uint8_t, bool> myRuleset;

	for (int i = 0; i < 8; i++) {
		myRuleset[i] = rules[i];
	}

	return myRuleset;
}

uint8_t cellsToKey(const std::vector<uint8_t>& row, const int& pos) {
	uint8_t key = 0x0;

	if (pos == 0) {
		key = (row[pos] << 1) | row[pos + 1];
	}
	else if (pos == row.size() - 1) {
		key = (row[pos - 1] << 2) | (row[pos] << 1) | (0x0);
	}
	else {
		key = (row[pos - 1] << 2) | (row[pos] << 1) | row[pos + 1];
	}

	return key;
}

int main(int argc, char* argv[])
{
	auto myValidatedInput = validInput(argc, argv);
	
	if (false == myValidatedInput.valid) {
		printUsage();
	}
	else {	
		std::vector<std::vector<uint8_t>> occupiedCells(myValidatedInput.numOfGenerations, std::vector<uint8_t>(myValidatedInput.numOfCells));

		occupiedCells[0] = myValidatedInput.initialConfig;

		std::map<uint8_t, bool> myRuleset;

		// Determine which ruleset to use.
		switch (myValidatedInput.type) {
			case 'A': 
			{
				myRuleset = theARuleset;
				break;
			}
			case 'B':
			{
				myRuleset = theBRuleset;
				break;
			}
			case 'U':
			{
				// In the event that there are still arguments left, it's type U, generate our ruleset.
				myRuleset = generateUniversalRuleset(myValidatedInput.universalRuleset);
				
				break;
			}
		}

		// Generate new generations based off of our ruleset.
		for (int i = 1; i < myValidatedInput.numOfGenerations; i++) {
			for (int j = 0; j < myValidatedInput.numOfCells; j++) {
				uint8_t key = cellsToKey(occupiedCells[i - 1], j);

				occupiedCells[i][j] = myRuleset[key];
			}
		}

		// Print out the automaton.
		for (int i = 0; i < myValidatedInput.numOfGenerations; i++) {
			for (int j = 0; j < myValidatedInput.numOfCells; j++) {
				if (occupiedCells[i][j])
					std::cout << "*";
				else
					std::cout << " ";
			}
			std::cout << "\n";
		}
	}
}
