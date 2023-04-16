#include "DriverGenerator.h"

DriverGenerator::DriverGenerator(NameGenerator& names, rapidcsv::Document& traits)
	: names_(names)
	, gen_(rd_()) {

	std::vector<uint32_t> ids = traits.GetColumn<uint32_t>("ID");
	std::vector<std::string> type = traits.GetColumn<std::string>("Type");
	for (uint32_t i = 0; i < type.size(); ++i) {
		if (type[i] == "Permanent") {
			permTraitIDs_.push_back(ids[i]);
		}
	}
}

Driver DriverGenerator::create(Division division) {
	Driver driver;
	driver.name = names_.generate();

	// update DOB, pick randomly between 16-36
	int32_t day = rand() % 28 + 1;
	int32_t month = rand() % 12 + 1;
	int32_t year = 2016 - (16 + (rand() % 20));
	driver.dob = std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);

	std::normal_distribution<float> weight(60.0f, 10.0f);
	driver.weight = int32_t(std::min(90.0f, std::max(weight(gen_), 45.0f)));
	driver.weight = int32_t(std::min(90.0f, std::max(weight(gen_), 45.0f)));

	std::normal_distribution<float> peakDuration(1.0f, 2.0f);
	driver.peakDuration = int32_t(std::max(1.0f, peakDuration(gen_)));

	std::normal_distribution<float> peakAge(30.0f, 3.0f);
	driver.peakAge = int32_t(std::max(28.0f, peakAge(gen_)));

	// pick 1, 2 or 3 traits with equal probability
	int32_t numTraits = 1 + (rand() % 3);
	std::string driverTraits;
	for (int32_t j = 0; j < numTraits; ++j) {
		driverTraits += (j == 0 ? "" : ";");
		int32_t traitID = permTraitIDs_[(rand() % permTraitIDs_.size())];
		driverTraits += std::to_string(traitID);
	}
	driver.traits = driverTraits;

	if (division != Division::eFreeAgent) {
		float wages = 10.0f * (float(rand()) / RAND_MAX);
		driver.wages = int32_t(wages * 100) / 100.f;
	}

	// pay driver
	driver.payDriver = 0;
	float r = float(rand()) / RAND_MAX;
	if (r < 0.1f) {
		driver.payDriver = 30 * (rand() / RAND_MAX);
	}

	// optimal zone, style
	float zone = 0.5f + 0.5f * (float(rand()) / RAND_MAX);
	float roundedZone = int32_t(100.f * zone) / 100.0f;
	driver.optimalZone = roundedZone;

	int32_t drivingStyle = 20 + int32_t(80.f * (float(rand()) / RAND_MAX));
	driver.drivingStyle = drivingStyle;

	// attributes
	float mean;
	float sd;
	switch (division) {
	case Division::eFormula1:
		mean = 12.0f + (5.0f * (float(rand()) / RAND_MAX));
		sd = 3.0f;
		break;
	case Division::eFormula2:
		mean = 8.0f + (5.0f * (float(rand()) / RAND_MAX));
		sd = 4.0f;
		break;
	case Division::eOther:
		mean = 3.0f + (5.0f * (float(rand()) / RAND_MAX));
		sd = 5.0f;
		break;
	case Division::eFreeAgent:
		mean = (20.0f * (float(rand()) / RAND_MAX));
		sd = (5.0f * (float(rand()) / RAND_MAX));
		break;
	};

	std::normal_distribution<float> attributeGen(mean, sd);
	float sumAttr = 0.0f;
	for (const std::string& attribute : Driver::attributeKeys) {
		float attr = std::max(1.0f, std::min(20.0f, attributeGen(gen_)));
		float roundedAttr = int32_t(attr * 100.f) / 100.f;
		sumAttr += roundedAttr;
		driver.attributes[attribute] = roundedAttr;
	}

	// Star ratings are out of 5, attributes are out of 20
	float starRating = 0.25f * (sumAttr / Driver::attributeKeys.size());
	driver.starRating = starRating;

	float maxPotentialLeft = (Driver::attributeKeys.size() * 20.0f) - sumAttr;
	int32_t potential = int32_t(maxPotentialLeft * (float(rand()) / RAND_MAX));
	driver.potential = potential;

	// misc attributes 
	driver.experience = int32_t(100.0f * ((float(rand()) / RAND_MAX)));
	driver.morale = int32_t(100.0f * ((float(rand()) / RAND_MAX)));
	driver.obedience = int32_t(100.0f * ((float(rand()) / RAND_MAX)));
	driver.patience = int32_t(5.0f * ((float(rand()) / RAND_MAX)));

	auto generatePopularity = [](std::mt19937& gen_) {
		std::normal_distribution<float> popularityGen(50, 25);
		return int32_t(std::min(100.0f, std::max(0.0f, popularityGen(gen_))));
	};

	driver.chairmanPopularity = generatePopularity(gen_);
	driver.fanPopularity = generatePopularity(gen_);
	driver.sponsorPopularity = generatePopularity(gen_);
	driver.marketability = generatePopularity(gen_);

	driver.improvementRate = int32_t(200.0f * float(rand()) / RAND_MAX);

	std::normal_distribution<float> desiredChampionships(1.0f, 1.0f);
	driver.desiredChampionships = int32_t(std::min(10.0f, std::max(desiredChampionships(gen_), 1.0f)));

	std::normal_distribution<float> desiredWins(3.0f, 3.0f);
	driver.desiredWins = int32_t(std::min(10.0f, std::max(desiredChampionships(gen_), 1.0f)));
	return driver;
}