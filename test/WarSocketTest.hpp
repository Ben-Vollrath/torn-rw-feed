#pragma once


#include "oatpp-test/UnitTest.hpp"

class WarSocketTest : public oatpp::test::UnitTest
{
	void testSocketOk();
	void testMemberStatInsertOnlyOnce();

public:
	WarSocketTest() : UnitTest("TEST[WarSocketTest]")
	{
	}

	void onRun() override;
};
