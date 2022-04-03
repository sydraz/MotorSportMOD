#pragma once

#include "Csv.h"
#include "NamesGenerator.h"

#include <array>
#include <map>
#include <random>
#include <vector>

enum class Division {
	eFormula1,
	eFormula2,
	eOther,
	eFreeAgent
};

struct Driver {
	NameGenerator::NameInfo name;
	std::string dob;
	int32_t weight;

	int32_t peakDuration;
	int32_t peakAge;

	std::string traits;

	float wages;
	int32_t payDriver;

	float optimalZone;
	int32_t drivingStyle;

	constexpr static std::array< const char*, 9> attributeKeys{
		"Braking", "Cornering",
		"Smoothness", "Overtaking",
		"Consistency", "Adaptability",
		"Fitness", "Feedback",
		"Focus" };

	// Star ratings are out of 5, attributes are out of 20
	std::map<std::string, float> attributes;
	float starRating;
	int32_t potential;

	int32_t experience;
	int32_t morale;
	int32_t obedience;
	int32_t patience;

	int32_t chairmanPopularity;
	int32_t fanPopularity;
	int32_t sponsorPopularity;
	int32_t marketability;

	int32_t improvementRate;
	int32_t desiredChampionships;
	int32_t desiredWins;
};

class DriverGenerator {
public:
	DriverGenerator(NameGenerator& names, rapidcsv::Document & traits);
	Driver create(Division division);
private:
	NameGenerator& names_;
	std::random_device rd_;
	std::mt19937 gen_; 
	std::vector<int32_t> permTraitIDs_;
};