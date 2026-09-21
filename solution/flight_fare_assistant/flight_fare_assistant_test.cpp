#include "flight_fare_assistant.h"

#include "test/catch.hpp"

#include <stdexcept>
#include <tuple>

using namespace flight_fare_assistant;
using namespace Catch::Matchers;

// Prices are specified with cent‐level precision in USD
static constexpr double EPS = 1.5e-2;

TEST_CASE("direct flight window seat surcharge") {
  const auto result = calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Window, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(450.49, EPS));
}

TEST_CASE("direct flight round trip surcharge") {
  const auto result = calculateTotalFares("ATL", "LAX", true, TravelClass::Coach, SeatChoice::Unassigned, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(695.49, EPS));
}

TEST_CASE("direct flight round trip plus weekday booking surcharge") {
  const auto result = calculateTotalFares("ATL", "LAX", true, TravelClass::Coach, SeatChoice::Unassigned, "22:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(765.49, EPS));
}

TEST_CASE("one stop flight day surcharge") {
  const auto result = calculateTotalFares("JFK", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00",
                                     Day::Wednesday, Day::Sunday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"JFK", "MCO", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(576.76, EPS));
}

TEST_CASE("one stop weekday booking surcharge") {
  const auto result = calculateTotalFares("JFK", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "17:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"JFK", "MCO", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(554.75, EPS));
}

TEST_CASE("check all direct flights") {
  std::vector<std::tuple<std::string, std::string, double>> expected{
      {"ATL", "LAX", 350.00 + 29.99 + 35.50}, {"ATL", "LAS", 249.17 + 29.99 + 22.50},
      {"ATL", "DFW", 149.87 + 29.99 + 28.42}, {"ATL", "CLT", 200.00 + 29.99 + 17.75},
      {"ATL", "JFK", 219.53 + 29.99 + 40.00}, {"DFW", "ORD", 170.00 + 22.25 + 30.00},
      {"DFW", "MIA", 190.29 + 22.25 + 32.18}, {"DFW", "DEN", 160.00 + 22.25 + 20.00},
      {"JFK", "MCO", 240.00 + 35.00 + 18.37}, {"JFK", "CLT", 200.00 + 35.00 + 17.75},
      {"MCO", "LAX", 249.14 + 15.50 + 35.50}, {"LAS", "LAX", 130.00 + 20.00 + 35.50},
      {"LAS", "MIA", 259.87 + 20.00 + 32.18}, {"CLT", "MIA", 180.00 + 14.24 + 32.18},
      {"CLT", "LAX", 307.42 + 14.24 + 35.50}};
  for (const auto& [depart, arrive, price] : expected) {
    const auto result = calculateTotalFares(depart, arrive, false, TravelClass::Coach, SeatChoice::Unassigned, "08:00",
                                       Day::Wednesday, Day::Thursday);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector{depart, arrive});
    CHECK_THAT(result->fare, WithinAbs(price, EPS));
  }
}

TEST_CASE("one stop middle seat surcharge") {
  const auto result = calculateTotalFares("JFK", "LAX", false, TravelClass::Coach, SeatChoice::Middle, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"JFK", "MCO", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(532.74, EPS));
}

TEST_CASE("direct flight round trip plus flight day surcharge") {
  const auto result = calculateTotalFares("ATL", "LAX", true, TravelClass::Coach, SeatChoice::Unassigned, "08:00",
                                     Day::Wednesday, Day::Saturday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(800.49, EPS));
}

TEST_CASE("direct flight weekend flight surcharge") {
  for (const auto day : std::vector{Day::Saturday, Day::Sunday}) {
    const auto result =
        calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Wednesday, day);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
    CHECK_THAT(result->fare, WithinAbs(467.99, EPS));
  }
}

TEST_CASE("unknown destination") {
  CHECK_THROWS_AS(calculateTotalFares("ATL", "ZZZ", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);
}

TEST_CASE("one stop first class surcharge") {
  const auto result = calculateTotalFares("JFK", "LAX", false, TravelClass::FirstClass, SeatChoice::Unassigned, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"JFK", "MCO", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(730.84, EPS));
}

TEST_CASE("same departure and destination") {
  CHECK_THROWS_AS(calculateTotalFares("ATL", "ATL", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);
}

TEST_CASE("direct flight weekend booking surcharge") {
  for (const auto day : std::vector{Day::Saturday, Day::Sunday}) {
    // Booking before the surcharge window should incur no extra fee
    auto result =
        calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "09:59", day, Day::Thursday);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
    CHECK_THAT(result->fare, WithinAbs(415.49, EPS));

    // Booking after the surcharge window should incur no extra fee
    result =
        calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "22:01", day, Day::Thursday);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
    CHECK_THAT(result->fare, WithinAbs(415.49, EPS));

    // Booking during the surcharge window should apply the extra charge
    result =
        calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "10:00", day, Day::Thursday);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
    CHECK_THAT(result->fare, WithinAbs(467.99, EPS));

    result =
        calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "22:00", day, Day::Thursday);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
    CHECK_THAT(result->fare, WithinAbs(467.99, EPS));
  }
}

TEST_CASE("invalid departure format") {
  CHECK_THROWS_AS(
      calculateTotalFares("", "LAS", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday, Day::Monday),
      std::invalid_argument);

  CHECK_THROWS_AS(calculateTotalFares("lax", "LAS", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);

  CHECK_THROWS_AS(
      calculateTotalFares("LA", "LAS", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday, Day::Monday),
      std::invalid_argument);

  CHECK_THROWS_AS(calculateTotalFares("LAXA", "LAS", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);
}

TEST_CASE("invalid destination format") {
  CHECK_THROWS_AS(
      calculateTotalFares("LAX", "", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday, Day::Monday),
      std::invalid_argument);

  CHECK_THROWS_AS(calculateTotalFares("LAX", "las", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);

  CHECK_THROWS_AS(
      calculateTotalFares("LAX", "LA", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday, Day::Monday),
      std::invalid_argument);

  CHECK_THROWS_AS(calculateTotalFares("LAX", "LASA", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);
}

TEST_CASE("direct flight weekday booking plus weekend flight surcharges") {
  const auto result = calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "17:00",
                                     Day::Monday, Day::Saturday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(502.99, EPS));
}

TEST_CASE("invalid booking time value") {
  CHECK_THROWS_AS(calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "24:00", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);

  CHECK_THROWS_AS(calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "08:60", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);
}

TEST_CASE("one stop cheapest route") {
  const auto result = calculateTotalFares("JFK", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"JFK", "MCO", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(510.73, EPS));
}

TEST_CASE("direct flight weekday booking surcharge") {
  for (const auto day : std::vector{Day::Monday, Day::Tuesday, Day::Wednesday, Day::Thursday, Day::Friday}) {
    // Booking before the surcharge window should incur no extra fee
    auto result =
        calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "15:59", day, Day::Thursday);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
    CHECK_THAT(result->fare, WithinAbs(415.49, EPS));

    // Booking after the surcharge window should incur no extra fee
    result =
        calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "22:01", day, Day::Thursday);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
    CHECK_THAT(result->fare, WithinAbs(415.49, EPS));

    // Booking during the surcharge window should apply the extra charge
    result =
        calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "16:00", day, Day::Thursday);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
    CHECK_THAT(result->fare, WithinAbs(450.49, EPS));

    result =
        calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "22:00", day, Day::Thursday);
    REQUIRE(result != std::nullopt);
    CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
    CHECK_THAT(result->fare, WithinAbs(450.49, EPS));
  }
}

TEST_CASE("one stop aisle seat surcharge") {
  const auto result = calculateTotalFares("JFK", "LAX", false, TravelClass::Coach, SeatChoice::Aisle, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"JFK", "MCO", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(554.75, EPS));
}

TEST_CASE("direct flight aisle seat surcharge") {
  const auto result = calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Aisle, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(450.49, EPS));
}

TEST_CASE("one stop atl to mia") {
  const auto result = calculateTotalFares("ATL", "MIA", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "DFW", "MIA"});
  CHECK_THAT(result->fare, WithinAbs(368.31, EPS));
}

TEST_CASE("direct flight all surcharges") {
  const auto result = calculateTotalFares("ATL", "LAX", true, TravelClass::FirstClass, SeatChoice::Aisle, "17:00",
                                     Day::Wednesday, Day::Sunday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(1290.49, EPS));
}

TEST_CASE("direct flight middle seat surcharge") {
  const auto result = calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Middle, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(432.99, EPS));
}

TEST_CASE("invalid booking time format") {
  CHECK_THROWS_AS(
      calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "", Day::Monday, Day::Monday),
      std::invalid_argument);

  CHECK_THROWS_AS(
      calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "0800", Day::Monday, Day::Monday),
      std::invalid_argument);

  CHECK_THROWS_AS(calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "08-00", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);

  CHECK_THROWS_AS(
      calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "8:00", Day::Monday, Day::Monday),
      std::invalid_argument);

  CHECK_THROWS_AS(
      calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "08:0", Day::Monday, Day::Monday),
      std::invalid_argument);

  CHECK_THROWS_AS(
      calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "8:0", Day::Monday, Day::Monday),
      std::invalid_argument);

  CHECK_THROWS_AS(calculateTotalFares("ATL", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00:00",
                                 Day::Monday, Day::Monday),
                  std::invalid_argument);
}

TEST_CASE("direct flight first class surcharge") {
  const auto result = calculateTotalFares("ATL", "LAX", false, TravelClass::FirstClass, SeatChoice::Unassigned, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"ATL", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(590.49, EPS));
}

TEST_CASE("no route exists") {
  const auto result = calculateTotalFares("DEN", "MIA", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00",
                                     Day::Wednesday, Day::Thursday);
  CHECK(result == std::nullopt);
}

TEST_CASE("unknown departure") {
  CHECK_THROWS_AS(calculateTotalFares("XXX", "LAX", false, TravelClass::Coach, SeatChoice::Unassigned, "08:00", Day::Monday,
                                 Day::Monday),
                  std::invalid_argument);
}

TEST_CASE("one stop round trip surcharge") {
  const auto result = calculateTotalFares("JFK", "LAX", true, TravelClass::Coach, SeatChoice::Unassigned, "08:00",
                                     Day::Wednesday, Day::Thursday);
  REQUIRE(result != std::nullopt);
  CHECK(result->route == std::vector<std::string>{"JFK", "MCO", "LAX"});
  CHECK_THAT(result->fare, WithinAbs(862.91, EPS));
}
