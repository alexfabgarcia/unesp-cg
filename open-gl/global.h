#ifndef _GLOBAL_H
#define _GLOBAL_H

#define ONE_SECOND_IN_MILLI 1000
#define MIN_SPEED 0.004
#define MAX_SPEED 0.064
#define SPEED_COEFFICIENT 2

// Variaveis globais
// Camera
int cameraMode = 0;
int projecao = 0; // Variavel logica para definir o tipo de projecao ((0) Perspectiva ou (1) Ortogonal)
int posx = 0, posy = 0,	posz = 10; // Variaveis que definem a posicao da camera
int lx = 0, ly=1, lz = 0; // Variaveis que definem qual eixo estara na vertical do monitor.

// Controles
float player_current_speed = MIN_SPEED;
bool gameOver = false;
bool paused = false;
bool debug_mode = false;

// Timer
long deltaTime, current_call_time, last_call_time;		//Calcular o tempo de cada chamada: inspirado no game loop de desenvolvimento de jogos
long time_counter = 0;									//Em milisegundos

// Espaco (delta S)
float deltaPos = 0;

// Obtem o momento da primeira chamada (para operar o deltaTime)
void resetTime() {
	current_call_time = clock() / (CLOCKS_PER_SEC / ONE_SECOND_IN_MILLI);
}

// Redefine a velocidade o jogador
void resetSpeed() {
	player_current_speed = MIN_SPEED;
}

// Para a velocidade do jogador
void stopSpeed() {
	player_current_speed = 0;
}

// Retorna o coeficiente de velocidade para tratamento diferenciado de pontuacao
int getSpeedCoefficient() {
	return player_current_speed / MIN_SPEED;
}

// Acelera
void speedUp() {
	if (player_current_speed < MAX_SPEED) {
		player_current_speed *= SPEED_COEFFICIENT;
	}
}

// Desacelera
void slowDown() {
	if (player_current_speed > MIN_SPEED) {
		player_current_speed /= SPEED_COEFFICIENT;
	}
}

// Funcao utilizada para desenhar um quadrado com o plano X fixado
void DrawSquareXPlane(float x, float y_min, float y_max, float z_min, float z_max) {
	glBegin(GL_QUADS);
		glVertex3f(x, y_min, z_min);
		glVertex3f(x, y_min, z_max);
		glVertex3f(x, y_max, z_max);
		glVertex3f(x, y_max, z_min);
	glEnd();
}

// Funcao utilizada para desenhar um quadrado com o plano Y fixado
void DrawSquareYPlane(float y, float x_min, float x_max, float z_min, float z_max) {
	glBegin(GL_QUADS);
		glVertex3f(x_min, y, z_min);
		glVertex3f(x_min, y, z_max);
		glVertex3f(x_max, y, z_max);
		glVertex3f(x_max, y, z_min);
	glEnd();
}

#endif
