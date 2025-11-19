#include "LocationParserTest.hpp"
#include <vector>

void LocationParserTest::onRun()
{
	testHospitalParsing();
	testAbroadParsing();
	testTravelingParsing();
	testOkayParsing();
}	

void LocationParserTest::testHospitalParsing()
{
	OATPP_LOGD("LOCATIONPARSERTEST", "TestHospitalParsing: Start")
	auto dto = TornUserStatusDto::createShared();
	dto->state = TornUserStatusState::HOSPITAL;

	const std::vector<std::pair<const char*, TornLocation>> descriptions = {
		{"In a Mexican hospital for 18 mins ",              TornLocation::MEXICO},
		{"In a Caymanian hospital for 3 hrs 35 mins ",     TornLocation::CAYMAN_ISLANDS},
		{"In a Canadian hospital for 38 secs",             TornLocation::CANADA},
		{"In a Hawaiian hospital for 17 mins ",            TornLocation::HAWAII},
		{"In a British hospital for 1 hrs 48 mins ",       TornLocation::UNITED_KINGDOM},
		{"In an Argentinian hospital for 30 mins ",        TornLocation::ARGENTINA},
		{"In a Swiss hospital for 31 mins ",               TornLocation::SWITZERLAND},
		{"In a Japanese hospital for 11 mins ",            TornLocation::JAPAN},
		{"In a Chinese hospital for 29 mins ",             TornLocation::CHINA},
		{"In an Emirati hospital for 6 mins ",             TornLocation::UNITED_ARAB_EMIRATES},
		{"In a South African hospital for 26 mins ",       TornLocation::SOUTH_AFRICA},
		{"In hospital for 3 mins ",						   TornLocation::TORN}
	};

	for (const auto &p : descriptions) {
		dto->description = p.first;
		dto->parseLocation(); 
		OATPP_ASSERT(dto->location == p.second)
	}
	OATPP_LOGD("LOCATIONPARSERTEST", "TestHospitalParsing: Complete")
}

void LocationParserTest::testAbroadParsing()
{
	OATPP_LOGD("LOCATIONPARSERTEST", "TestAbroadParsing: Start")
	auto dto = TornUserStatusDto::createShared();
	dto->state = TornUserStatusState::ABROAD;

	const std::vector<std::pair<const char*, TornLocation>> descriptions = {
	{"In Mexico",              TornLocation::MEXICO},
	{"In Cayman Islands",     TornLocation::CAYMAN_ISLANDS},
	{"In Canada",             TornLocation::CANADA},
	{"In Hawaii",            TornLocation::HAWAII},
	{"In United Kingdom",       TornLocation::UNITED_KINGDOM},
	{"In Argentina",        TornLocation::ARGENTINA},
	{"In Switzerland",               TornLocation::SWITZERLAND},
	{"In Japan",            TornLocation::JAPAN},
	{"In China",             TornLocation::CHINA},
	{"In UAE",             TornLocation::UNITED_ARAB_EMIRATES},
	{"In South Africa ",       TornLocation::SOUTH_AFRICA}
	};

	for (const auto& p : descriptions) {
		dto->description = p.first;
		dto->parseLocation();
		OATPP_ASSERT(dto->location == p.second)
	}
	OATPP_LOGD("LOCATIONPARSERTEST", "TestAbroadParsing: Complete")
}

void LocationParserTest::testTravelingParsing()
{
	OATPP_LOGD("LOCATIONPARSERTEST", "TestTravelParsing: Start")
	auto dto = TornUserStatusDto::createShared();
	dto->state = TornUserStatusState::TRAVELING;

	const std::vector<std::pair<const char*, TornLocation>> descriptions = {
	{"Traveling to Mexico",              TornLocation::MEXICO},
	{"Traveling to Cayman Islands",     TornLocation::CAYMAN_ISLANDS},
	{"Traveling to Canada",             TornLocation::CANADA},
	{"Traveling to Hawaii",            TornLocation::HAWAII},
	{"Traveling to United Kingdom",       TornLocation::UNITED_KINGDOM},
	{"Traveling to Argentina",        TornLocation::ARGENTINA},
	{"Traveling to Switzerland",               TornLocation::SWITZERLAND},
	{"Traveling to Japan",            TornLocation::JAPAN},
	{"Traveling to China",             TornLocation::CHINA},
	{"Traveling to UAE",             TornLocation::UNITED_ARAB_EMIRATES},
	{"Traveling to South Africa",       TornLocation::SOUTH_AFRICA},
	{"Returning to Torn from UAE ",       TornLocation::TORN}
	};

	for (const auto& p : descriptions) {
		dto->description = p.first;
		dto->parseLocation();
		OATPP_ASSERT(dto->location == p.second)
	}
	OATPP_LOGD("LOCATIONPARSERTEST", "TestTravelParsing: Complete")
}

void LocationParserTest::testOkayParsing()
{
	OATPP_LOGD("LOCATIONPARSERTEST", "TestOkayParsing: Start")
	auto dto = TornUserStatusDto::createShared();
	dto->state = TornUserStatusState::OKAY;
	dto->parseLocation();
	OATPP_ASSERT(dto->location == TornLocation::TORN);
	OATPP_LOGD("LOCATIONPARSERTEST", "TestOkayParsing: Complete")
}
