#define FUEL_FULL 100
#define ONE_SECOND_IN_MILLI 1000
#define DECREASE_FACTOR_PER_SECOND 4

int fuel = FUEL_FULL;
long time_fuel;

void resetFuel(long time_counter) {
	fuel = FUEL_FULL;
	time_fuel = time_counter;
}

void decreaseFuel(long time_counter) {
	fuel = FUEL_FULL - ((time_counter - time_fuel) / ONE_SECOND_IN_MILLI) * DECREASE_FACTOR_PER_SECOND;
}

void increaseFuel() {
	fuel += 0.01;
}

bool isFuelEmpty() {
	return fuel <= 0;
}
