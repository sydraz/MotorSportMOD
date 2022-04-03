#pragma once
#include <memory>
#include <string>

class NameGeneratorImpl;

class NameGenerator {
public:
	NameGenerator(const std::string& lookupFile, const std::string& firstNameFile, const std::string& secondNameFile);
	~NameGenerator();

	struct NameInfo {
		std::string firstName;
		std::string lastName;
		std::string gender;
		std::string nationality;
		std::string ethnicity;
	};

	NameInfo generate();
private:
	std::unique_ptr<NameGeneratorImpl> impl_;
};