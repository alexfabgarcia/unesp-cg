#define HELICOPTER_SCORE_INCREMENT 60
#define SHIP_SCORE_INCREMENT 30
#define FUEL_SCORE_INCREMENT 10
#define RECORD_FILE "recorde.txt"

long playerScore = 0;
long record = 0;
FILE *recordFile;

void readRecordFile() {
	recordFile = fopen(RECORD_FILE, "r");
	
	if (recordFile) {
		if (fscanf(recordFile, "%ld", &record) == 0) {
			record = 0;
		}
		fclose(recordFile);
	}
	printf("Recorde atual: %ld.\n", record);
}

long resetPlayerScore() {
	playerScore = 0;
	readRecordFile();
}

long getScoreBySpeed(long score) {
	return score * getSpeedCoefficient();
}

void shootDownShip() {
	playerScore += getScoreBySpeed(SHIP_SCORE_INCREMENT);
}

void shootDownHelicopter() {
	playerScore += getScoreBySpeed(HELICOPTER_SCORE_INCREMENT);
}

void shootDownFuelStation() {
	playerScore -= FUEL_SCORE_INCREMENT;
}

bool isNewRecord() {
	return playerScore > record;
}

void saveRecordScore() {
	if (isNewRecord()) {
		recordFile = fopen(RECORD_FILE, "w+");

		if (recordFile) {
			printf("Salvando recorde %ld no arquivo.\n", playerScore);
			fprintf(recordFile, "%ld", playerScore);
			fclose(recordFile);
		}
	}
}
