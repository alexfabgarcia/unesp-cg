#ifndef _FUEL_H
#define _FUEL_H

#include "global.h"
#include "song.h"

#define FUEL_FULL 100
#define DECREASE_FACTOR_PER_SECOND 4
#define FUEL_CRITICAL_LEVEL 20
#define FUEL_CRITIVAL_SOUND "sound\\fuel-critical.wav"
#define PICK_UP_FUEL_SOUND "sound\\pick-up-fuel.wav"

int fuel = FUEL_FULL;
long time_fuel;
bool fuelAlertPlaying;
bool pickUpFuelPlayer;

void resetFuel() {
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
	if (!paused) {	
		if (fuel < FUEL_FULL) {
			if (!pickUpFuelPlayer) {
				pickUpFuelPlayer = true;
				playSoundStoppingOthers(PICK_UP_FUEL_SOUND);
			}
			time_fuel += 20;
		} else {
			pickUpFuelPlayer = false;
			time_fuel = time_counter;
		}
		if (!isFuelCritical()) {
			fuelAlertPlaying = false;
		}
		printf("Abastecendo Combustivel: %d\n", fuel);
	}
}

void decreaseFuel() {
	if (!paused) {
		fuel = FUEL_FULL - ((time_counter - time_fuel) / ONE_SECOND_IN_MILLI) * DECREASE_FACTOR_PER_SECOND;
	
		if (isFuelCritical() && !fuelAlertPlaying) {
			fuelAlertPlaying = true;
			pickUpFuelPlayer = false;
			playSoundStoppingOthers(FUEL_CRITIVAL_SOUND);
		}
	}
}

#endif
