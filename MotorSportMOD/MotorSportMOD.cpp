// MotorSportMOD.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "csv.h"
#include "NamesGenerator.h"

#include <iomanip>
#include <iostream>
#include <set>

void
generateCreateDrivers() {

	const std::string dir = "C:\\Users\\syedr\\OneDrive\\Desktop\\MM_Hacks";

	// Read input file
	rapidcsv::Document input;
	{
		std::string inputPath = dir + "\\create_team_drivers.txt";
		input.Load(inputPath);
	}

	// Create name generator 
	std::unique_ptr<NameGenerator> generator;
	{
		std::string namesLUT = dir + "\\names_lut.txt";
		std::string firstNames = dir + "\\first_names.txt";
		std::string lastNames = dir + "\\second_names.txt";

		generator = std::make_unique<NameGenerator>(namesLUT, firstNames, lastNames);
	}

	// Write output file
	{
		int32_t firstNameIndex = input.GetColumnIdx("First Name");
		int32_t lastNameIndex = input.GetColumnIdx("Last Name");
		int32_t genderIndex = input.GetColumnIdx("Gender");
		int32_t nationalityIndex = input.GetColumnIdx("Nationality");

		rapidcsv::Document output = input;
		for (int32_t i = 0; i < output.GetRowCount(); ++i) {
			NameGenerator::NameInfo name = generator->generate();
			output.SetCell<std::string>(firstNameIndex, i, name.firstName);
			output.SetCell<std::string>(lastNameIndex, i, name.lastName);
			output.SetCell<std::string>(genderIndex, i, name.gender);
			output.SetCell<std::string>(nationalityIndex, i, name.nationality);
		}
		output.Save(dir + "\\create_team_drivers_OUTPUT.txt");
	}
}


int main()
{

	generateCreateDrivers();

	/*
	std::string dir = "C:\\Users\\syedr\\OneDrive\\Desktop\\MM_Hacks";
	std::string namesLUT = dir + "\\names_lut.txt";
	std::string firstNames = dir + "\\first_names.txt";
	std::string lastNames = dir + "\\second_names.txt";

	NameGenerator gen(namesLUT, firstNames, lastNames);
	for (int32_t i = 0; i < 64; ++i) {
		NameGenerator::NameInfo name = gen.generate();
		std::cerr << " { Name: " << std::setw(20) << name.firstName << std::setw(20) << name.lastName
			<< ", Country: " << std::setw(20) << name.nationality
			<< ", Gender: " << name.gender
			<< ", Ethnicity: " << name.ethnicity
			<< "}" << std::endl;
	}
	*/

	/*
	std::string dir = "C:\\Users\\syedr\\OneDrive\\Desktop\\MM_Hacks";
	std::string path = dir + "\\mm_drivers.txt";
	rapidcsv::Document doc(path);

	constexpr int32_t width = 20;
	constexpr int32_t numCols = 15;
	constexpr int32_t numRows = 50;

	int32_t colIndex = 0;
	for (const std::string& col : doc.GetColumnNames()) {
		std::cerr << std::setw(width) << col << "\t";
		if (colIndex++ > numCols)
			break;
	}
	std::cerr << std::endl;

	for (uint32_t i = 0; i < std::min<int32_t>(numRows, doc.GetRowCount()); ++i) {
		std::vector<std::string> row = doc.GetRow<std::string>(i);
		colIndex = 0;
		for (const std::string& val : row) {
			std::cerr << std::setw(width) << val << "\t";
			if (colIndex++ > numCols)
				break;
		}
		std::cerr << std::endl;
	}
	*/
}
