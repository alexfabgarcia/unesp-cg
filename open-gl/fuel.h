#include "song.h"

#define FUEL_FULL 100
#define ONE_SECOND_IN_MILLI 1000
#define DECREASE_FACTOR_PER_SECOND 4
#define FUEL_CRITICAL_LEVEL 20
#define FUEL_CRITIVAL_SOUND "sound\\fuel-critical.wav"

int fuel = FUEL_FULL;
long time_fuel;
bool fuelAlertPlaying;

void resetFuel(long time_counter) {
	fuel = FUEL_FULL;
	time_fuel = time_counter;
	fuelAlertPlaying = false;
}

bool isFuelEmpty() {
	return fuel <= 0;
}

bool isFuelCritical() {
	return fuel <= FUEL_CRITICAL_LEVEL;
}

void increaseFuel() {
	fuel += 0.01;
	if (!isFuelCritical()) {
		fuelAlertPlaying = false;
	}
}

void decreaseFuel(long time_counter) {
	fuel = FUEL_FULL - ((time_counter - time_fuel) / ONE_SECOND_IN_MILLI) * DECREASE_FACTOR_PER_SECOND;

	if (isFuelCritical() && !fuelAlertPlaying) {
		fuelAlertPlaying = true;
		playSoundStoppingOthers(FUEL_CRITIVAL_SOUND);
	}
}
