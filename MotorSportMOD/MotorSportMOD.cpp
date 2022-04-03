// MotorSportMOD.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "csv.h"
#include "NamesGenerator.h"
#include "DriverGenerator.h"

#include <iomanip>
#include <iostream>
#include <set>

void
generateCreateDrivers() {

	const std::string dir = "C:\\Users\\syedr\\OneDrive\\Desktop\\MM_Hacks";

	// Read input files
	rapidcsv::Document input;
	rapidcsv::Document traits;
	{
		input.Load(dir + "\\create_team_drivers.txt");
		traits.Load(dir + "\\mm_personality_traits.txt");
	}

	// Create name generator 
	std::unique_ptr<NameGenerator> namesGenerator;
	{
		std::string namesLUT = dir + "\\names_lut.txt";
		std::string firstNames = dir + "\\first_names.txt";
		std::string lastNames = dir + "\\second_names.txt";
		namesGenerator = std::make_unique<NameGenerator>(namesLUT, firstNames, lastNames);
	}

	std::unique_ptr<DriverGenerator> driverGenerator;
	{
		driverGenerator = std::make_unique<DriverGenerator>(*namesGenerator.get(), traits);
	}

	// Write output file
	{
		rapidcsv::Document output = input;
		for (uint32_t i = 0; i < output.GetRowCount(); ++i) {
			Driver driver = driverGenerator->create(Division::eFreeAgent);

			output.SetCell<std::string>(input.GetColumnIdx("First Name"), i, driver.name.firstName);
			output.SetCell<std::string>(input.GetColumnIdx("Last Name"), i, driver.name.lastName);
			output.SetCell<std::string>(input.GetColumnIdx("Gender"), i, driver.name.gender);
			output.SetCell<std::string>(input.GetColumnIdx("Nationality"), i, driver.name.nationality);
			output.SetCell<std::string>(input.GetColumnIdx("DOB"), i, driver.dob);
			output.SetCell<int32_t>(input.GetColumnIdx("Weight"), i, driver.weight);
			output.SetCell<int32_t>(input.GetColumnIdx("Peak Duration"), i, driver.peakDuration);
			output.SetCell<int32_t>(input.GetColumnIdx("Peak Age"), i, driver.peakAge);
			output.SetCell<std::string>(input.GetColumnIdx("Traits"), i, driver.traits);

			// pay driver
			output.SetCell<int32_t>(input.GetColumnIdx("Pay Driver"), i, driver.payDriver);

			// driving ability + stars + potential
			for (const std::string& attribute : Driver::attributeKeys) {
				float attributeVal = driver.attributes[attribute];
				output.SetCell<float>(input.GetColumnIdx(attribute), i, attributeVal);
			}
			output.SetCell<float>(input.GetColumnIdx("Stars"), i, driver.starRating);
			output.SetCell<int32_t>(input.GetColumnIdx("Potential"), i, driver.potential);

			// optimal zone + style
			output.SetCell<float>(input.GetColumnIdx("Optimal Zone"), i, driver.optimalZone);
			output.SetCell<int32_t>(input.GetColumnIdx("Driving Style"), i, driver.drivingStyle);

			// clear history 
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

			// misc attributes 
			output.SetCell<int32_t>(input.GetColumnIdx("Experience"), i, driver.experience);
			output.SetCell<int32_t>(input.GetColumnIdx("Morale"), i, driver.morale);
			output.SetCell<int32_t>(input.GetColumnIdx("Obedience"), i, driver.obedience);
			output.SetCell<int32_t>(input.GetColumnIdx("Patience"), i, driver.patience);

			output.SetCell<int32_t>(input.GetColumnIdx("Chairman Popularity"), i, driver.chairmanPopularity);
			output.SetCell<int32_t>(input.GetColumnIdx("Fans Popularity"), i, driver.fanPopularity);
			output.SetCell<int32_t>(input.GetColumnIdx("Sponsor Popularity"), i, driver.sponsorPopularity);
			output.SetCell<int32_t>(input.GetColumnIdx("Marketability"), i, driver.marketability);

			output.SetCell<int32_t>(input.GetColumnIdx("Improvement Rate"), i, driver.improvementRate);
			output.SetCell<int32_t>(input.GetColumnIdx("Desired Championships"), i, driver.desiredChampionships);
			output.SetCell<int32_t>(input.GetColumnIdx("Desired Wins"), i, driver.desiredWins);

			output.SetCell<std::string>(input.GetColumnIdx("Series Preference"), i, "Any");
		}
		output.Save(dir + "\\create_team_drivers_OUTPUT.txt");
	}
}

void
generateTeamDrivers() {
	const std::string dir = "C:\\Users\\syedr\\OneDrive\\Desktop\\MM_Hacks";

	// Read input files
	rapidcsv::Document input;
	rapidcsv::Document traits;
	{
		input.Load(dir + "\\mm_drivers.txt");
		traits.Load(dir + "\\mm_personality_traits.txt");
	}

	// Create name generator 
	std::unique_ptr<NameGenerator> namesGenerator;
	{
		std::string namesLUT = dir + "\\names_lut.txt";
		std::string firstNames = dir + "\\first_names.txt";
		std::string lastNames = dir + "\\second_names.txt";
		namesGenerator = std::make_unique<NameGenerator>(namesLUT, firstNames, lastNames);
	}

	// Create driver generator
	std::unique_ptr<DriverGenerator> driverGenerator;
	{
		driverGenerator = std::make_unique<DriverGenerator>(*namesGenerator.get(), traits);
	}

	// Write output file
	{
		rapidcsv::Document output = input;
		for (uint32_t i = 0; i < output.GetRowCount(); ++i) {

			Division div = Division::eFreeAgent;
			// for contracted drivers, tweak stuff
			ssize_t teamIndex = input.GetColumnIdx("Team");
			if (!input.GetCell<std::string>(teamIndex, i).empty()) {
				int32_t teamId = input.GetCell<int32_t>(teamIndex, i);
				div = (teamId <= 11) ? Division::eFormula1
					: (teamId <= 22) ? Division::eFormula2
					: Division::eOther;
			}

			Driver driver = driverGenerator->create(div);

			output.SetCell<std::string>(input.GetColumnIdx("First Name"), i, driver.name.firstName);
			output.SetCell<std::string>(input.GetColumnIdx("Last Name"), i, driver.name.lastName);
			output.SetCell<std::string>(input.GetColumnIdx("Gender"), i, driver.name.gender);
			output.SetCell<std::string>(input.GetColumnIdx("Nationality"), i, driver.name.nationality);
			output.SetCell<std::string>(input.GetColumnIdx("DOB"), i, driver.dob);
			output.SetCell<int32_t>(input.GetColumnIdx("Weight"), i, driver.weight);
			output.SetCell<int32_t>(input.GetColumnIdx("Peak Duration"), i, driver.peakDuration);
			output.SetCell<int32_t>(input.GetColumnIdx("Peak Age"), i, driver.peakAge);
			output.SetCell<std::string>(input.GetColumnIdx("Traits"), i, driver.traits);

			// for contracted drivers, tweak stuff
			if (div != Division::eFreeAgent) {
				output.SetCell<int32_t>(input.GetColumnIdx("Fame"), i, 0);
				output.SetCell<int32_t>(input.GetColumnIdx("Contract Start"), i, 2016);
				output.SetCell<int32_t>(input.GetColumnIdx("Contract End"), i, 2016);
				output.SetCell<float>(input.GetColumnIdx("Wages"), i, driver.wages);

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
			output.SetCell<int32_t>(input.GetColumnIdx("Pay Driver"), i, driver.payDriver);

			// driving ability + stars + potential
			for (const std::string& attribute : Driver::attributeKeys) {
				float attributeVal = driver.attributes[attribute];
				output.SetCell<float>(input.GetColumnIdx(attribute), i, attributeVal);
			}
			output.SetCell<float>(input.GetColumnIdx("Stars"), i, driver.starRating);
			output.SetCell<int32_t>(input.GetColumnIdx("Potential"), i, driver.potential);

			// optimal zone + style
			output.SetCell<float>(input.GetColumnIdx("Optimal Zone"), i, driver.optimalZone);
			output.SetCell<int32_t>(input.GetColumnIdx("Driving Style"), i, driver.drivingStyle);

			// clear history 
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

			// misc attributes 
			output.SetCell<int32_t>(input.GetColumnIdx("Experience"), i, driver.experience);
			output.SetCell<int32_t>(input.GetColumnIdx("Morale"), i, driver.morale);
			output.SetCell<int32_t>(input.GetColumnIdx("Obedience"), i, driver.obedience);
			output.SetCell<int32_t>(input.GetColumnIdx("Patience"), i, driver.patience);

			output.SetCell<int32_t>(input.GetColumnIdx("Chairman Popularity"), i, driver.chairmanPopularity);
			output.SetCell<int32_t>(input.GetColumnIdx("Fans Popularity"), i, driver.fanPopularity);
			output.SetCell<int32_t>(input.GetColumnIdx("Sponsor Popularity"), i, driver.sponsorPopularity);
			output.SetCell<int32_t>(input.GetColumnIdx("Marketability"), i, driver.marketability);

			output.SetCell<int32_t>(input.GetColumnIdx("Improvement Rate"), i, driver.improvementRate);
			output.SetCell<int32_t>(input.GetColumnIdx("Desired Championships"), i, driver.desiredChampionships);
			output.SetCell<int32_t>(input.GetColumnIdx("Desired Wins"), i, driver.desiredWins);

			output.SetCell<std::string>(input.GetColumnIdx("Series Preference"), i, "Any");
		}

		output.Save(dir + "\\drivers_OUTPUT.txt");
	}
}

int main()
{
	srand(int32_t(time(nullptr)));
	generateCreateDrivers();
	generateTeamDrivers();
}
