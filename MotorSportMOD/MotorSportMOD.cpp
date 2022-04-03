// MotorSportMOD.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "csv.h"
#include "NamesGenerator.h"

#include <iomanip>
#include <iostream>
#include <random>
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

	std::random_device rd;
	std::mt19937 gen(rd());

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
				output.SetCell<std::string>(qualBonusPosition, i, "");
				output.SetCell<std::string>(raceBonusPosition, i, "");
			}

			// pay driver
			{
				ssize_t payDriverPosition = input.GetColumnIdx("Pay Driver");
				float r = float(rand()) / RAND_MAX;
				if (r < 0.1f) {
					output.SetCell<int32_t>(payDriverPosition, i, 30 * (rand() / RAND_MAX));
				}
				else {
					output.SetCell<int32_t>(payDriverPosition, i, 0);
				}
			}

			// Stars
			// Braking	Cornering	Smoothness	Overtaking	Consistency	Adaptability Fitness Feedback Focus	
			{
				static std::vector<std::string> attributes = {
				"Braking", "Cornering",
				"Smoothness", "Overtaking",
				"Consistency", "Adaptability",
				"Fitness", "Feedback",
				"Focus" };

				float mean;
				float sd;
				ssize_t teamIndex = input.GetColumnIdx("Team");
				if (!input.GetCell<std::string>(teamIndex, i).empty()) {
					int32_t teamId = input.GetCell<int32_t>(teamIndex, i);
					if (teamId <= 11) { // hack:f1
						mean = 12.0f + (5.0f * (float(rand()) / RAND_MAX));
						sd = 3.0f;
					}
					else if (teamId <= 22) { // hack:f2
						mean = 8.0f + (5.0f * (float(rand()) / RAND_MAX));
						sd = 4.0f;
					}
					else { // all other nonsense
						mean = 3.0f + (5.0f * (float(rand()) / RAND_MAX));
						sd = 5.0f;
					}
				}
				else { // no team
					mean = (20.0f * (float(rand()) / RAND_MAX));
					sd = (5.0f * (float(rand()) / RAND_MAX));
				}

				std::normal_distribution<float> attributeGen(mean, sd);
				float sum = 0.0f;
				for (const std::string& attribute : attributes) {
					float attr = std::min(20.0f, attributeGen(gen));
					float roundedAttr = int32_t(attr * 100.f) / 100.f;
					sum += roundedAttr;
					output.SetCell<int32_t>(input.GetColumnIdx(attribute), i, roundedAttr);
				}
				output.SetCell<int32_t>(input.GetColumnIdx(attribute), i, roundedAttr);

				// optimal zone
				{
					float zone = 0.5f + 0.5f * (float(rand()) / RAND_MAX);
					float roundedZone = int32_t(100.f * zone) / 100.0f;
					output.SetCell<float>(input.GetColumnIdx("Optimal Zone"), i, roundedZone);

					int32_t drivingStyle = 20 + int32_t(80.f * (float(rand()) / RAND_MAX));
					output.SetCell<int32_t>(input.GetColumnIdx("Driving Style"), i, drivingStyle);
				}

				// clear history 
				{
					output.SetCell<int32_t>(input.GetColumnIdx("Races"), i, 0);
					output.SetCell<int32_t>(input.GetColumnIdx("Wins"), i, 0);
					output.SetCell<int32_t>(input.GetColumnIdx("Podiums"), i, 0);
					output.SetCell<int32_t>(input.GetColumnIdx("Poles"), i, 0);
					output.SetCell<int32_t>(input.GetColumnIdx("DNFs"), i, 0);
					output.SetCell<int32_t>(input.GetColumnIdx("DNFs via error"), i, 0);
					output.SetCell<int32_t>(input.GetColumnIdx("DNS"), i, 0);
					output.SetCell<int32_t>(input.GetColumnIdx("Career Points"), i, 0);
					output.SetCell<int32_t>(input.GetColumnIdx("Championships"), i, 0);
					output.SetCell<std::string>(input.GetColumnIdx("Form"), i, "n/a");
				}
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
