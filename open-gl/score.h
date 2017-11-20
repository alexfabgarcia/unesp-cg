#define HELICOPTER_SCORE_INCREMENT 60
#define SHIP_SCORE_INCREMENT 30
#define FUEL_SCORE_INCREMENT 10

long playerScore = 0;

long resetPlayerScore() {
	playerScore = 0;
}

void shootDownShip() {
	playerScore += SHIP_SCORE_INCREMENT;
}

void shootDownHelicopter() {
	playerScore += HELICOPTER_SCORE_INCREMENT;
}

void shootDownFuelStation() {
	playerScore += FUEL_SCORE_INCREMENT;
}
