#include "flight_fare_assistant.h"
#include <algorithm> // bring in <algorithm> to use std::all_of
#include <limits>    // bring in <limits> for std::numeric_limits
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace flight_fare_assistant {

// mapping of routes to their base ticket prices
const std::unordered_map<std::string, std::unordered_map<std::string, double>> flightFares = {
    {"ATL", {{"LAX", 350.00}, {"LAS", 249.17}, {"DFW", 149.87}, {"CLT", 200.00}, {"JFK", 219.53}}},
    {"DFW", {{"ORD", 170.00}, {"MIA", 190.29}, {"DEN", 160.00}}},
    {"JFK", {{"MCO", 240.00}, {"CLT", 200.00}}},
    {"MCO", {{"LAX", 249.14}}},
    {"LAS", {{"LAX", 130.00}, {"MIA", 259.87}}},
    {"CLT", {{"MIA", 180.00}, {"LAX", 307.42}}}};

// mapping of airports to departure and arrival fee pairs
const std::unordered_map<std::string, std::pair<double, double>> airportFees = {
    {"ATL", {25.17, 29.99}}, {"LAX", {35.50, 40.00}}, {"DFW", {28.42, 22.25}}, {"DEN", {20.00, 18.75}},
    {"ORD", {30.00, 27.17}}, {"JFK", {40.00, 35.00}}, {"MCO", {18.37, 15.50}}, {"LAS", {22.50, 20.00}},
    {"CLT", {17.75, 14.24}}, {"MIA", {32.18, 27.99}}};

// checks whether an airport code is valid
bool isValidAirportCode(const std::string& code) {
  return code.length() == 3 && std::all_of(code.begin(), code.end(), [](char c) { return std::isupper(c); });
}

// verifies that time strings are correctly formatted
bool isValidTime(const std::string& time) {
  if (time.length() != 5 || time[2] != ':')
    return false;
  int hours = std::stoi(time.substr(0, 2));
  int minutes = std::stoi(time.substr(3, 2));
  return hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59;
}

// computes the route's base fare
double calculateBaseFare(const std::vector<std::string>& route) {
  double baseFare = 0.0;
  for (size_t i = 0; i < route.size() - 1; ++i) {
    const auto& fares = flightFares.at(route[i]);
    baseFare += fares.at(route[i + 1]);
  }
  if (route.size() == 3) { // applies when there's one layover
    baseFare *= 0.9;       // apply a 10% discount
  }
  return baseFare;
}

// computes the full fare including surcharges and fees
double calculateTotalFare(double baseFare, bool isRoundTrip, TravelClass travelClass, SeatChoice seatChoice,
                          const std::string& bookingTime, Day bookingDay, Day flightDay) {
  (void) baseFare;
  (void) isRoundTrip;
  (void) travelClass;
  (void) seatChoice;
  (void) bookingTime;
  (void) bookingDay;
  (void) flightDay;
  
  return -1.0;
}

std::optional<TripInfo> calculateTotalFares(const std::string& departure, const std::string& destination, bool isRoundTrip,
                                       TravelClass travelClass, SeatChoice seatChoice, const std::string& bookingTime,
                                       Day bookingDay, Day flightDay) {
  if (!isValidAirportCode(departure) || !isValidAirportCode(destination)) {
    throw std::invalid_argument("Invalid airport code");
  }

  if (airportFees.count(departure) == 0) {
    throw std::invalid_argument("Departure airport does not exist");
  }

  if (airportFees.count(destination) == 0) {
    throw std::invalid_argument("Destination airport does not exist");
  }

  if (departure == destination) {
    throw std::invalid_argument("Departure and destination codes are the same");
  }

  if (!isValidTime(bookingTime)) {
    throw std::invalid_argument("Invalid booking time");
  }

  // try direct route first
  if (flightFares.count(departure) && flightFares.at(departure).count(destination)) {
    std::vector<std::string> route = {departure, destination};
    double baseFare = flightFares.at(departure).at(destination);
    double totalFare =
        calculateTotalFare(baseFare, isRoundTrip, travelClass, seatChoice, bookingTime, bookingDay, flightDay);
    totalFare += airportFees.at(departure).second + airportFees.at(destination).first;
    return TripInfo{route, totalFare};
  }

  // search for routes with one layover
  double minFare = std::numeric_limits<double>::max();
  std::vector<std::string> bestRoute;
  for (const auto& intermediate : flightFares) {
    if (flightFares.count(departure) && flightFares.at(departure).count(intermediate.first) &&
        flightFares.count(intermediate.first) && flightFares.at(intermediate.first).count(destination)) {
      std::vector<std::string> route = {departure, intermediate.first, destination};
      double baseFare = calculateBaseFare(route);
      double totalFare =
          calculateTotalFare(baseFare, isRoundTrip, travelClass, seatChoice, bookingTime, bookingDay, flightDay);
      totalFare += airportFees.at(departure).second + airportFees.at(destination).first;

      if (totalFare < minFare) {
        minFare = totalFare;
        bestRoute = route;
      }
    }
  }

  if (!bestRoute.empty()) {
    return TripInfo{bestRoute, minFare};
  }

  return std::nullopt;
}

} // end of flight_fare_assistant namespace
