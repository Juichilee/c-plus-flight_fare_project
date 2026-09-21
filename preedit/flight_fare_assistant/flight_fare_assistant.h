#ifndef FLIGHT_FARE_ASSISTANT_H
#define FLIGHT_FARE_ASSISTANT_H

#include <optional>
#include <string>
#include <vector>

namespace flight_fare_assistant {

enum class TravelClass { FirstClass, Coach };
enum class SeatChoice { Aisle, Middle, Window, Unassigned };
enum class Day { Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday };

struct TripInfo {
  std::vector<std::string> route;
  double fare;
};

std::optional<TripInfo> calculateTotalFares(const std::string& departure, const std::string& destination, bool isRoundTrip,
                                       TravelClass travelClass, SeatChoice seatChoice, const std::string& bookingTime,
                                       Day bookingDay, Day flightDay);

} // namespace flight_fare_calculator

#endif