#pragma once


#include "oatpp-test/UnitTest.hpp"

class WarSocketTest : public oatpp::test::UnitTest
{
	void testSocketOk();

public:
	WarSocketTest() : UnitTest("TEST[WarSocketTest]")
	{
	}

	void onRun() override;
};
