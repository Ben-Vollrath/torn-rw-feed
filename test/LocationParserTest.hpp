#pragma once

#include <oatpp/core/macro/component.hpp>

#include "dto/Enums.hpp"
#include "dto/responses/WarStateResponseDto.hpp"
#include "oatpp-test/UnitTest.hpp"

class LocationParserTest : public oatpp::test::UnitTest {
  void testHospitalParsing();
  void testAbroadParsing();
  void testTravelingParsing();
  void testOkayParsing();

 public:
  LocationParserTest() : UnitTest("TEST[LocationParserTest]") {}

  void onRun() override;
};
