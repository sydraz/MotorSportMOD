#include "NamesGenerator.h"
#include "Csv.h"

class NameGeneratorImpl {
public:
	NameGeneratorImpl(const std::string& lookupPath, const std::string& firstNamePath, const std::string& secondNamePath);
	NameGenerator::NameInfo generate();
private:
	rapidcsv::Document lookupDoc_;
	rapidcsv::Document firstNameDoc_;
	rapidcsv::Document secondNameDoc_;

	std::vector< std::string > weightCountry_;
	std::vector< float > weightStart_;
};

NameGeneratorImpl::NameGeneratorImpl(const std::string& lookupPath, const std::string& firstNamePath, const std::string& secondNamePath)
	: lookupDoc_(lookupPath)
	, firstNameDoc_(firstNamePath)
	, secondNameDoc_(secondNamePath)
{	
	fprintf(stderr, "NameGeneratorImpl: %s(%lu x %lu)\n", lookupPath.c_str(), lookupDoc_.GetRowCount(), lookupDoc_.GetColumnCount());
	fprintf(stderr, "NameGeneratorImpl: %s(%lu x %lu)\n", firstNamePath.c_str(), firstNameDoc_.GetRowCount(), firstNameDoc_.GetColumnCount());
	fprintf(stderr, "NameGeneratorImpl: %s(%lu x %lu)\n", secondNamePath.c_str(), secondNameDoc_.GetRowCount(), secondNameDoc_.GetColumnCount());

	weightCountry_.reserve(lookupDoc_.GetRowCount());
	weightStart_.reserve(lookupDoc_.GetRowCount());

	weightCountry_ = lookupDoc_.GetColumn<std::string>("Nation");
	weightStart_ = lookupDoc_.GetColumn<float>("Weighting Start");

	for (uint32_t i = 1; i < weightStart_.size(); ++i) {
		assert(weightStart_[i] > weightStart_[i - 1]);
	}
}

NameGenerator::NameInfo NameGeneratorImpl::generate()
{
	NameGenerator::NameInfo ret;

	fprintf(stderr, "Generating new name\n");

	// Step 1 : select country
	int32_t nationIndex = 0;
	{
		float r = (100.f * rand()) / RAND_MAX;
		auto it = std::lower_bound(weightStart_.begin(), weightStart_.end(), r);
		nationIndex = std::max<int32_t>(std::distance(weightStart_.begin(), it) - 1, 0);
		ret.nationality = weightCountry_[nationIndex];
		fprintf(stderr, "Picked nationality = %s\n", ret.nationality.c_str());
	}

	// Step 2: select gender, some countries do not have female names so we have to skip those
	bool isMale = false;
	{
		float r = float(rand()) / RAND_MAX;
		isMale = r < 0.5f;
		if (!isMale) {
			int32_t femaleFirstNameStart = lookupDoc_.GetColumn<int32_t>("Female First Name Start")[nationIndex];
			if (femaleFirstNameStart == -1) {
				isMale = true;
			}
		}
		ret.gender = isMale ? "M" : "F";
		fprintf(stderr, "Picked gender = %s\n", ret.gender.c_str());
	}

	// Step 3: select first name / last name
	{
		int32_t nameStart = isMale ? lookupDoc_.GetColumn<int32_t>("Male Start")[nationIndex] : lookupDoc_.GetColumn<int32_t>("Female Start")[nationIndex];
		int32_t nameEnd = isMale ? lookupDoc_.GetColumn<int32_t>("Male End")[nationIndex] : lookupDoc_.GetColumn<int32_t>("Female End")[nationIndex];

		fprintf(stderr,"For selection nation, names are in [%d,%d]\n", nameStart, nameEnd);

		int32_t firstNameStart;
		int32_t firstNameEnd;
		if (!isMale) {
			firstNameStart = lookupDoc_.GetColumn<int32_t>("Female First Name Start")[nationIndex];
			firstNameEnd = lookupDoc_.GetColumn<int32_t>("Female First Name End")[nationIndex];
		}
		else {
			int32_t femaleFirstNameEnd = lookupDoc_.GetColumn<int32_t>("Female First Name End")[nationIndex];
			firstNameStart = femaleFirstNameEnd == -1 ? lookupDoc_.GetColumn<int32_t>("First Name Start")[nationIndex] : femaleFirstNameEnd;
			firstNameEnd = lookupDoc_.GetColumn<int32_t>("First Name End")[nationIndex];
		}

		fprintf(stderr, "For selection nation, first names are in [%d,%d]\n", firstNameStart, firstNameEnd);

		fprintf(stderr, "First name dims: %lu x %lu\n", firstNameDoc_.GetRowCount(), firstNameDoc_.GetColumnCount());
		fprintf(stderr, "Second name dims: %lu x %lu\n", secondNameDoc_.GetRowCount(), secondNameDoc_.GetColumnCount());

		float r1 = float(rand()) / RAND_MAX;
		int32_t nameIndex = nameStart + int32_t((nameEnd - nameStart) * r1);
		std::vector<std::string> secondNameRow = secondNameDoc_.GetRow< std::string>(nameIndex);
		ret.lastName = secondNameRow[secondNameDoc_.GetColumnIdx("Name")];

		float r2 = float(rand()) / RAND_MAX;
		int32_t firstNameIndex = firstNameStart + int32_t((firstNameEnd - firstNameStart) * r2);
		std::vector<std::string> firstNameRow = firstNameDoc_.GetRow<std::string>(firstNameIndex);
		ret.firstName = firstNameRow[firstNameDoc_.GetColumnIdx("Name")];
		ret.ethnicity = firstNameRow[firstNameDoc_.GetColumnIdx("Ethnicity")];

		fprintf(stderr, "First: %s Last : %s Ethnicity:%s\n", ret.firstName.c_str(), ret.lastName.c_str(), ret.ethnicity.c_str());

		// VERIFICATION
		std::string femaleCol = firstNameRow[firstNameDoc_.GetColumnIdx("Female")];
		if (!isMale) {
			assert(femaleCol == "1");
		}
		else {
			assert(femaleCol == "0" || femaleCol == "-1");
		}
		assert(ret.nationality == firstNameRow[firstNameDoc_.GetColumnIdx("Nation")]);
		assert(ret.nationality == secondNameRow[secondNameDoc_.GetColumnIdx("Nation")]);
	}
	return ret;
}

NameGenerator::NameGenerator(const std::string& lookupPath, const std::string& firstNamePath, const std::string& secondNamePath)
	: impl_(std::make_unique<NameGeneratorImpl>(lookupPath, firstNamePath, secondNamePath)) {
}

NameGenerator::~NameGenerator() {
}
NameGenerator::NameInfo NameGenerator::generate()
{
	return impl_->generate();
}

