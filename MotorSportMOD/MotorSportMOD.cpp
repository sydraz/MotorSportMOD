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
		ssize_t firstNameIndex = input.GetColumnIdx("First Name");
		ssize_t lastNameIndex = input.GetColumnIdx("Last Name");
		ssize_t genderIndex = input.GetColumnIdx("Gender");
		ssize_t nationalityIndex = input.GetColumnIdx("Nationality");

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

void
generateTeamDrivers() {
	const std::string dir = "C:\\Users\\syedr\\OneDrive\\Desktop\\MM_Hacks";

	// Read input file
	rapidcsv::Document input;
	{
		std::string inputPath = dir + "\\mm_drivers.txt";
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
		ssize_t firstNameIndex = input.GetColumnIdx("First Name");
		ssize_t lastNameIndex = input.GetColumnIdx("Last Name");
		ssize_t genderIndex = input.GetColumnIdx("Gender");
		ssize_t nationalityIndex = input.GetColumnIdx("Nationality");
		ssize_t dateOfBirthIndex = input.GetColumnIdx("DOB");
		ssize_t traitsIndex = input.GetColumnIdx("Traits");
		ssize_t fameIndex = input.GetColumnIdx("Fame");
		ssize_t contractStartIndex = input.GetColumnIdx("Contract Start");
		ssize_t contractEndIndex = input.GetColumnIdx("Contract End");
		ssize_t wagesIndex = input.GetColumnIdx("Wages");

		rapidcsv::Document output = input;
		for (int32_t i = 0; i < output.GetRowCount(); ++i) {
			NameGenerator::NameInfo name = generator->generate();

			// update name + nationality
			output.SetCell<std::string>(firstNameIndex, i, name.firstName);
			output.SetCell<std::string>(lastNameIndex, i, name.lastName);
			output.SetCell<std::string>(genderIndex, i, name.gender);
			output.SetCell<std::string>(nationalityIndex, i, name.nationality);

			// update DOB, pick randomly between 16-36
			int32_t day = rand() % 28 + 1;
			int32_t month = rand() % 12 + 1;
			int32_t year = 2016 - (16 + (rand() % 20));
			std::string dob = std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);
			output.SetCell<std::string>(dateOfBirthIndex, i, dob);

			// pick 1, 2 or 3 traits with equal probability
			std::vector<int32_t> permTraitIDs;
			rapidcsv::Document traits(dir + "\\mm_personality_traits.txt");
			std::vector<std::string> type = traits.GetColumn<std::string>("Type");
			for (int32_t i = 0; i < type.size(); ++i) {
				if (type[i] == "Permanent") {
					permTraitIDs.push_back(i);
				}
			}
			int32_t numTraits = 1 + (rand() % 3);
			std::string driverTraits;
			for (int32_t j = 0; j < numTraits; ++j) {
				driverTraits += (j == 0 ? "" : ";");
				int32_t traitID = permTraitIDs[(rand() % permTraitIDs.size())];
				driverTraits += std::to_string(traitID);
			}
			output.SetCell<std::string>(traitsIndex, i, driverTraits);

			// for contracted drivers, tweak stuff
			ssize_t teamIndex = input.GetColumnIdx("Team");
			if (!input.GetCell<std::string>(teamIndex, i).empty()) {

				output.SetCell<int32_t>(fameIndex, i, 0);
				output.SetCell<int32_t>(contractStartIndex, i, 2016);
				output.SetCell<int32_t>(contractEndIndex, i, 2016);

				float wages = 10.0f * (float(rand()) / RAND_MAX);
				float roundedWages = int32_t(wages * 100) / 100.f;
				output.SetCell<float>(wagesIndex, i, roundedWages);

				// no bonuses
				ssize_t qualBonusAmount = input.GetColumnIdx("Qualifying Bonus Amount");
				ssize_t qualBonusPosition = input.GetColumnIdx("Qualifying Bonus Position");
				ssize_t raceBonusAmount = input.GetColumnIdx("Race Bonus Amount");
				ssize_t raceBonusPosition = input.GetColumnIdx("Race Bonus Position");

				output.SetCell<float>(qualBonusAmount, i, 0.0f);
				output.SetCell<float>(raceBonusAmount, i, 0.0f);
				output.SetCell<std::string>(wagesIndex, i, roundedWages);
				output.SetCell<std::string>(wagesIndex, i, roundedWages);
			}
		}

		output.Save(dir + "\\drivers_OUTPUT.txt");
	}
}

int main()
{
	generateCreateDrivers();
	generateTeamDrivers();
}
