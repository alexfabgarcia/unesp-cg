#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <time.h>
#include "global.h"
#include "color.h"
#include "collision.h"
#include "score.h"
#include "fuel.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SHOT_SOUND "sound\\shot.wav"
#define GAME_OVER_SOUND "sound\\game-over.wav"
#define EXPLOSION_SOUND "sound\\explosion.wav"

#define NUM_OF_SHIPS 14
#define NUM_OF_WALLS 36
#define NUM_OF_FUEL_STATIONS 4
#define NUM_OF_FLOOR_TILES 34
#define MAX_PROJECTILES_IN_GAME 20

#define ship_largura 3
#define ship_altura 1
#define ship_comprimento 1

#define fuel_station_largura 1
#define fuel_station_altura 1
#define fuel_station_comprimento 4

#define projectile_largura 0.1
#define projectile_altura 0.1
#define projectile_comprimento 0.7

// Variaveis globais
// Armazena identificador da janela para que seja possivel fecha-la posteriormente com a tecla 'ESC'
int windowId;
// Player
t_aabb_object player;
//Navios
t_aabb_object ship[NUM_OF_SHIPS];
//Walls
t_aabb_object wall_left[NUM_OF_WALLS];
t_aabb_object wall_right[NUM_OF_WALLS];
//Floor
t_aabb_object floor[NUM_OF_FLOOR_TILES];
//Projectiles
t_aabb_object projectiles[MAX_PROJECTILES_IN_GAME];
// Estacoes de abastecimento
t_aabb_object fuel_station[NUM_OF_FUEL_STATIONS];

//Índice dos objetos mais distântes (Necessárias para o infinite runner)
int most_far_ship_index;
int most_far_wall_index;
int most_far_floor_tile_index;
int most_far_fuel_station;

//Cores
int color_wall[3] = {159, 85, 41};
int color_grass[3] = {0, 128, 0};
int color_water[3] = {0,0,128};

//Prototipos das Funcoes
void Initializate();
void Display();
void keyboard (unsigned char key, int x, int y);
void DrawPlayer();
void DrawBackground();
void DrawWall(float, float, bool);
void DrawWater(float, float);
void DrawShip(float, float);
void DrawShoot(float, float, float);
void DrawFuelStation(float, float);
void DrawFuelGauge();
void DrawScore();
void Pause();
bool AABB(float, float, float, float, float, float, float, float);
void Shoot();
void Draw2DInfo();
void doBeforeGameOver();
void Sair();

// Desenha o texto na posicao 2D indicada
void DrawText(const char *text, int x, int y) {
	int length = strlen(text);
	
    glRasterPos2i(x,y);
    
    for ( int i = 0 ; i < length ; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)text[i]);
}

void Display() {
	//Encontrando dt
	current_call_time = clock()/ (CLOCKS_PER_SEC / 1000);
	deltaTime = current_call_time - last_call_time;
	last_call_time = current_call_time;
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH); 
	glEnable(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
	// Inicializa par�metros de rendering
	// Define a cor de fundo da janela de visualização como preta
	glClearColor(0.5, 0.8, 0.9, 0.0); 
   
	glMatrixMode(GL_PROJECTION);/*glMatrixMode()- define qual matriz será alterada. SEMPRE defina o tipo de apresentação 
	                          (Ortogonal ou Perspectiva) na matriz PROJECTION.*/
	glLoadIdentity();//"Limpa" ou "transforma" a matriz em identidade, reduzindo possíveis erros.

	if (projecao==1){
		// Proje��o ortogonal
		glOrtho(-10, 10, -5, 15, -50, 50);
	}
	else
	  gluPerspective(60,1,1,150); //Define a proje��o como perspectiva
   
	glMatrixMode(GL_MODELVIEW);/*glMatrixMode()- define qual matriz será alterada. SEMPRE defina a câmera 
	                          (Ortogonal ou Perspectiva) na matriz MODELVIEW (onde o desenho ocorrerá).*/
	glLoadIdentity(); ////"Limpa" ou "transforma" a matriz em identidade, reduzindo possíveis erros.

	gluLookAt(posx,posy,posz,0,-4,0,lx,ly,lz); //Define a pos da c�mera, para onde olha e qual eixo est� na vertical.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* "limpa" um buffer particular ou combinações de buffers, 
	                                                     onde buffer é uma área de armazenamento para informações da imagem. 
	                                                    Nesse caso, está "limpando os buffers para suportarem animações */

	//S = S0 + V * T
	deltaPos += player_current_speed * deltaTime;
	time_counter += deltaTime;
	//printf("%d \n\n", time_counter);

	//Contador de tempo (segundos)
	//if(time_counter/1000 >= 10)
	  //DrawShip( -5 ,-32 + deltaPos);

	//Draw Player
	DrawPlayer();
	//Draw Background
	DrawBackground();
	
   //Desenha Navios
   int i;
	for(i=0; i<NUM_OF_SHIPS; i++) {
		ship[i].z += player_current_speed * deltaTime;		//S = S0 + V * T	
		ship[i].x += ship[i].direcao * 0.001 * deltaTime;
		if(ship[i].x < -4.5) {
			ship[i].direcao = 1;
		}else if(ship[i].x > 4.5) {
			ship[i].direcao = -1;
		}else if(paused) {
			ship[i].direcao = 0;
		}
		
		//Reseta posi��es do navio ap�s sair da tela
		if(ship[i].z >= 12) {					
			ship[i].z = ship[most_far_ship_index].z - 10;
			ship[i].inGame = true;
			most_far_ship_index = i;
		}
		
		if(ship[i].inGame == true) {
			DrawShip(ship[i].x, ship[i].z);

			//Colis�o player com navio
			if (hasCollision(player, ship[i])) {
				doBeforeGameOver();
			}
		}
	}

	//Desenha Paredes
	for(i=0; i<NUM_OF_WALLS; i++) {
		wall_right[i].z += player_current_speed * deltaTime;
		wall_left[i].z += player_current_speed * deltaTime;
		DrawWall(wall_right[i].x, wall_right[i].z, false);
		DrawWall(wall_left[i].x, wall_left[i].z, true);
		//Reseta posi��es da parede ap�s sair da tela
		if(wall_right[i].z >= 16){
			wall_right[i].z = wall_right[most_far_wall_index].z-3.95;	//sobreposição de 0.05
			wall_left[i].z = wall_left[most_far_wall_index].z-3.95;	//sobreposição de 0.05
			most_far_wall_index = i;
		}
	}

	//Desenha a �gua (Ch�o)
	for(i=0; i<NUM_OF_FLOOR_TILES; i++) {
		floor[i].z += player_current_speed * deltaTime;
		DrawWater(floor[i].x, floor[i].z);
		//Reseta posi��es do ch�o/�gua ap�s sair da tela
		if(floor[i].z >= 14) {
			floor[i].z = floor[most_far_floor_tile_index].z-3.95; //sobreposição de 0.05
			most_far_floor_tile_index = i;
		}
	}
	
	// Desenha estacoes de abastecimento
	for(i=0; i<NUM_OF_FUEL_STATIONS; i++) {
		fuel_station[i].z += player_current_speed * deltaTime;		//S = S0 + V * T
		
		//Reseta posi��es do navio ap�s sair da tela
		if(fuel_station[i].z >= 12) {					
			fuel_station[i].z = ship[most_far_ship_index].z - 10;
			fuel_station[i].inGame = true;
			most_far_fuel_station = i;
		}
		
		if(fuel_station[i].inGame == true) {
			DrawFuelStation(fuel_station[i].x, fuel_station[i].z);
			
			// Colisao do player com a estacao de abastecimento
			if(hasCollision(player, fuel_station[i])) {
				increaseFuel();
			}
		}
	}

	for(i=0; i<MAX_PROJECTILES_IN_GAME; i++) {
		if(projectiles[i].inGame == true) {
			projectiles[i].z -= 0.04 * deltaTime;
			projectiles[i].lifetime += deltaTime;
			DrawShoot(projectiles[i].x, projectiles[i].y, projectiles[i].z);
			//Destr�i proj�til depois de 10 segundos
			if(projectiles[i].lifetime/1000 >= 10) {
				projectiles[i].inGame = false;
			}
			//Colisao de proj�til com os navios
			int j;
			for(j=0; j<NUM_OF_SHIPS; j++) {
				if(ship[j].inGame == true && hasCollision(projectiles[i], ship[j])) {
					//destroi proj�til
					projectiles[i].inGame = false;
					//destroi navio
					ship[j].inGame = false;
					
					// Abate o navio
					shootDownShip();
					playSoundStoppingOthers(EXPLOSION_SOUND);
					
					break;
				}
			}
		}
	}
	
	Draw2DInfo();
	decreaseFuel();
	
	if (!gameOver && isFuelEmpty()) {
		doBeforeGameOver();
	}
	
   glutSwapBuffers(); //Executa a Cena. SwapBuffers dá suporte para mais de um buffer, permitindo execução de animações sem cintilações. 
}

//Jogador
void DrawPlayer() {
	
	//H�lice
	glPushMatrix();
		glColor3ub(140,140,140);
		glTranslated(ox,oy+.3,oz-0.8);
		glScalef(.2,.2,.1);
		glutSolidTorus(0.7,1,50,50);
	glPopMatrix();
	
	//Corpo
	glPushMatrix();
		glColor3ub(140, 140, 140);
		glTranslated(ox,oy+.1,oz);
		glScalef(.15,.15,1.7);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();	//Frente
		glColor3ub(140, 140, 140);
		glTranslated(ox,oy+.1,oz-0.2);
		glScalef(.2,.2,1);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();	//Frente
		glColor3ub(140, 140, 140);
		glTranslated(ox,oy+.1,oz-0.2);
		glScalef(.3,.3,.7);
		glutSolidCube(1);
	glPopMatrix();
	
	//Asas Dianteiras superiores
	glPushMatrix();
		glColor3ub(140, 0, 0);
		glTranslated(ox,oy+.4,oz-0.3);
		glScalef(2,.05,.7);
		glutSolidCube(1);
	glPopMatrix();
	
	//Asas Dianteiras inferiores
	glPushMatrix();
		glColor3ub(140, 0, 0);
		glTranslated(ox,oy-.2,oz-0.3);
		glScalef(2,.05,.7);
		glutSolidCube(1);
	glPopMatrix();
	
	//Hastes asas
	glPushMatrix();
		glColor3ub(140, 140, 140);	//frente esquerda
		glTranslated(ox-0.7,oy+0.1,oz-0.5);
		glScalef(0.05,0.6,0.05);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		glColor3ub(140, 140, 140);	//frente direita
		glTranslated(ox+0.7,oy+0.1,oz-0.5);
		glScalef(0.05,0.6,0.05);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		glColor3ub(140, 140, 140);	//tr�s direita
		glTranslated(ox+0.7,oy+0.1,oz);
		glScalef(0.05,0.6,0.05);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		glColor3ub(140, 140, 140);	//tr�s esquerda
		glTranslated(ox-0.7,oy+0.1,oz);
		glScalef(0.05,0.6,0.05);
		glutSolidCube(1);
	glPopMatrix();
	
	
	//Asas Traseiras
	glPushMatrix();
		glColor3ub(140, 0, 0);
		glTranslated(ox,oy+.3,oz+1);
		glScalef(0.5,.05,.3);
		glutSolidCube(1);
	glPopMatrix();
	
	//Debug
	if(debug_mode) {
		glPushMatrix();
			changeColorToRed();
			glTranslated(player.x - 0.5, player.y, 0.75);
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
		
		glPushMatrix();
			changeColorToRed();
			glTranslated(player.x + 0.5, player.y, -0.75);
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
	}
  	glutPostRedisplay();
}

void DrawBackground() {
	glPushMatrix();
		glColor3ub(color_water[0], color_water[1], color_water[2]);
		glTranslatef(0,-10,0);
		glScalef(12,.1,32);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		glColor3ub(color_wall[0], color_wall[1], color_wall[2]);
		glTranslatef(8,-10,0);
		glScalef(4,.1,32);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		glColor3ub(color_wall[0], color_wall[1], color_wall[2]);
		glTranslatef(-8,-10,0);
		glScalef(4,.1,32);
		glutSolidCube(1);
	glPopMatrix();
	glutPostRedisplay();
}


//Parede (Bloco)
void DrawWall(float x,float z, bool left) {
	float x_pos = (left ? 1 : -1) * 0.5;
	glPushMatrix();
		glTranslatef(x,-5,z);
		glScalef(4,10,4);
		
		// Desenha parede marrom com topo verde para dar efeito de grama
		glColor3ub(color_wall[0], color_wall[1], color_wall[2]);
		DrawSquareXPlane(x_pos, -0.5, .45, 0, 1);
		glColor3ub(color_grass[0], color_grass[1], color_grass[2]);
		DrawSquareXPlane(x_pos, 0.44, .5, 0, 1);

		// Desenha grama
		DrawSquareYPlane(0.5, .5, -0.5, 0, 1);
	glPopMatrix();
	glutPostRedisplay();
}

//�gua
void DrawWater(float x,float z) {
	glPushMatrix();
		glColor3ub(color_water[0], color_water[1], color_water[2]);
		glTranslatef(x,-10,z);
		glScalef(12,.00001,4);
		glutSolidCube(1);
	glPopMatrix();
	glutPostRedisplay();
}

//Barco
void DrawShip(float x, float z) {
	glPushMatrix();
		changeColorToWhite();
		glTranslated(x,-9.7,z);
		glScalef(ship_largura, ship_altura, ship_comprimento);
		glutSolidCube(1);
	glPopMatrix();
	//Debug
	if(debug_mode) {
		glPushMatrix();
			changeColorToRed();
			glTranslated(x-((float)ship_largura/2), -9.7+((float)ship_altura/2), z+((float)ship_comprimento/2));
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
		
		glPushMatrix();
			changeColorToRed();
			glTranslated(x+((float)ship_largura/2), -9.7+((float)ship_altura/2), z-((float)ship_comprimento/2));
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
	}
	glutPostRedisplay();	
}

//DrawShoot
void DrawShoot(float x, float y, float z) {
	glPushMatrix();
		changeColorToYellow();
		glTranslated(x,y,z);
		glScalef(projectile_largura,projectile_altura,projectile_comprimento);
		glutSolidCube(1);
	glPopMatrix();
	//Debug
	if(debug_mode) {
		glPushMatrix();
			changeColorToRed();
			glTranslated(x-((float)projectile_largura/2),y,z+((float)projectile_comprimento/2));
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
		
		glPushMatrix();
			changeColorToRed();
			glTranslated(x+((float)projectile_largura/2),y,z-((float)projectile_comprimento/2));
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
	}
}
//Estacao de Combustivel
void DrawFuelStation(float x, float z) {
	glPushMatrix();
		changeColorToRed();
		glTranslated(x,-9.7,z);
		glScalef(fuel_station_largura, fuel_station_altura, fuel_station_comprimento);
		glutSolidCube(1);
	glPopMatrix();
	//Debug
	if(debug_mode) {
		glPushMatrix();
			changeColorToRed();
			glTranslated(x-((float)fuel_station_largura/2), -9.7+((float)fuel_station_altura/2), z+((float)fuel_station_comprimento/2));
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
		
		glPushMatrix();
			changeColorToRed();
			glTranslated(x+((float)fuel_station_largura/2), -9.7+((float)fuel_station_altura/2), z-((float)fuel_station_comprimento/2));
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
	}
	glutPostRedisplay();
}

void Draw2DInfo() {
	glMatrixMode(GL_PROJECTION);
    double *matrix = new double[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();
    
    // Colocar toda informacao 2D neste ponto (pontuacao, combustivel, etc.)
    DrawFuelGauge();
    DrawScore();
	
	if (gameOver) {
		changeColorToYellow();
		DrawText("GAME OVER!", 5, 100);
		if (isNewRecord()) {
			DrawText("Novo Recorde!", 5, 80);
		}
		DrawText("(R) Reiniciar", 5, 60);
	} else if (paused) {
		changeColorToYellow();
		DrawText("Jogo Pausado!", 5, 100);
		DrawText("(ESC) Sair", 5, 80);
		DrawText("(P) Continuar", 5, 60);
	}
	
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
}

// Draw Medidor Combustivel
void DrawFuelGauge() {
	// O angulo vai de -50 (100% de combust�vel) at� 50 (0% de combust�vel)
    double angle = -(fuel - 50);
    
    glPushMatrix(); /* save current transformation state on stack */
    
	    glTranslated(WINDOW_WIDTH - 95, 20, 1);
		glRotated(angle, 0, 0, 1);
		
		changeColorToRed();
		glBegin(GL_POLYGON);
			glVertex2i(-3, 0);
			glVertex2i(3, 0);
			glVertex2i(2, 80);
			glVertex2i(2, 80);
		glEnd();
		
		changeColorToBlack();
		glBegin(GL_POLYGON);
			glVertex2i(-15, 0);
			glVertex2i(0, 15);
			glVertex2i(15, 0);
			glVertex2i(0, -15);
		glEnd();	
	glPopMatrix(); /* reset to previous transformation state */
	
	changeColorToRed();
	glPushMatrix();
		glTranslated(WINDOW_WIDTH - 95, 20, 1);
		glRotated(50, 0, 0, 1);
		
		glBegin(GL_POLYGON);
			glVertex2i(-2, 25);
			glVertex2i(2, 25);
			glVertex2i(2, 40);
			glVertex2i(-2, 40);
		glEnd();
	glPopMatrix();
    DrawText("E", WINDOW_WIDTH - 150, 40); // Empty
	
	changeColorToWhite();
    glPushMatrix();
		glTranslated(WINDOW_WIDTH - 95, 20, 1);
		glRotated(-50, 0, 0, 1);
		
		glBegin(GL_POLYGON);
			glVertex2i(-2, 25);
			glVertex2i(0, 25);
			glVertex2i(0, 40);
			glVertex2i(-2, 40);
		glEnd();
	glPopMatrix();
    DrawText("F", WINDOW_WIDTH - 50, 40); // Full
    
    glPushMatrix();
		glTranslated(WINDOW_WIDTH - 95, 20, 1);
		
		glBegin(GL_POLYGON);
			glVertex2i(-2, 25);
			glVertex2i(0, 25);
			glVertex2i(0, 40);
			glVertex2i(-2, 40);
		glEnd();
	glPopMatrix();
    DrawText("1/2", WINDOW_WIDTH - 108, 80); // Metade do combustivel

    if (isFuelCritical()) {
    	changeColorToYellow();
    	DrawText("Critico!", WINDOW_WIDTH - 65, 10); // Full
	}
}

void DrawScore() {
    changeColorToWhite();
    
	DrawText("Score:", 5, 25);
	
	// snprintf tells you length if you call it with NULL, 0 as first parameters
	int length = snprintf(NULL, 0, "%ld", playerScore) + 1;
	char* score = (char*) malloc(length * sizeof(char));
	snprintf(score, length, "%ld", playerScore);
	
	DrawText(score, 5, 5);
	
	free(score);
}

void Shoot() {
	int i;
	//Encontra proj�til dispon�vel para entrar no jogo
	for(i=0; i<MAX_PROJECTILES_IN_GAME; i++) {
		if(projectiles[i].inGame == false) {
			projectiles[i].x = player.x;
			projectiles[i].y = -9;
			projectiles[i].z = player.z;
			projectiles[i].lifetime = 0;
			projectiles[i].inGame = true;
			playSoundStoppingOthers(SHOT_SOUND);
			break;
		}
	}
}

void keyboard (unsigned char key, int x, int y) {
	if (!paused) {
		//Movimento da nave
		if (key=='d') {
			player.x += 0.5;
		}
		if (key=='a') {
			player.x -= 0.5;
		}
		if (key=='w'){
			//player.y+=1;
		}
		if(key=='s'){
			//player.y-=1;
		}
		if(key=='e') {
			speedUp(); // Acelerar
		}
		if(key=='q') {
			slowDown(); // Desacelerar
		}
		if(key==' ') {
			Shoot(); // Atirar
		}
	}
	
	// Modo da camera
	if(key=='c') {
		cameraMode = (cameraMode + 1)%2 ;	//0 = normal		1 = top down
		if (cameraMode == 0) {
			projecao = 0;
	    	posx=0, posy=0, posz=10;
	    	lx=0, ly=1,  lz=0;
		} else if (cameraMode == 1) {
			projecao = 1;
			posx=0, posy=10, posz=0;
	    	lx=0, ly=0,  lz=-1;
		}
	}
	
	if(key==';') {
		debug_mode = !debug_mode; //Debug Mode
	}
	
	// Controle de pause, reinicio e saida do jogo
	if (paused && key==27) { // Escape key
		Sair();
	} else if (!gameOver && (key=='p' || key==27)) {
		Pause(); //Pause
	} else if (gameOver && (key=='r')) {
		Initializate();
	}
}

void doBeforeGameOver() {
	puts("Chamando doBeforeGameOver...");
	gameOver = true;
	playSoundStoppingOthers(GAME_OVER_SOUND);
	Pause();
	saveRecordScore();
}

void Pause() {
	paused = !paused;
	if (paused) {
		stopSpeed();
	} else {
		resetSpeed();
	}
}

void Sair() {
	glutDestroyWindow (windowId);
	exit(0);
}

// Prepara as variaveis utilizadas na execucao
void Initializate() {
	gameOver = false;
	paused = false;
	
	player.y = -9;
	player.x = 0;
	player.z = 0;
	player.width = 1;
	player.length = 1.5;

	int i;

	//Navios
	for(i=0; i<NUM_OF_SHIPS; i++) {
		//Inicializa posi��es dos navios
		ship[i].x = rand()% 10 - 5;
		ship[i].z = -i*10;
		ship[i].direcao = (rand() % 2 - 1 >= 0) ? 1:-1;
		ship[i].inGame = true;
		ship[i].width = ship_largura;
		ship[i].length = ship_comprimento;
	}
	most_far_ship_index = NUM_OF_SHIPS-1;
	
	//Walls
	for(i=0; i<NUM_OF_WALLS; i++) {
		//Inicializa posiçposi��es�o das paredes da direita
		wall_right[i].x = 8;
		wall_right[i].z = (-i*4)+16;
		//Inicializa posi��es�ão das paredes da esquerda
		wall_left[i].x = -8;
		wall_left[i].z = (-i*4)+16;
	}
	most_far_wall_index = NUM_OF_WALLS-1;
	
	//Floor
	for(i=0; i<NUM_OF_FLOOR_TILES; i++) {
		//Inicializa posi��es dos tiles de �gua no ch�o
		floor[i].x = 0;
		floor[i].z = (-i*4)+14;
	}
	most_far_floor_tile_index = NUM_OF_FLOOR_TILES-1;
	
	//Projectiles
	for(i=0; i<MAX_PROJECTILES_IN_GAME; i++) {
		projectiles[i].x = player.x;
		projectiles[i].y = player.y;
		projectiles[i].z = player.z;
		projectiles[i].inGame = false;
	}
	
	//Inicializa posicoes das estacoes de abastecimento
	for (i=0; i<NUM_OF_FUEL_STATIONS; i++) {	
		fuel_station[i].x = rand()%10 - 5;
		fuel_station[i].z = (-i-1)*25;
		fuel_station[i].inGame = true;
		fuel_station[i].width = fuel_station_largura;
		fuel_station[i].length = fuel_station_comprimento;
	}
	most_far_fuel_station = NUM_OF_FUEL_STATIONS - 1;
	
	resetTime();
	resetSpeed();
	resetPlayerScore();
	resetFuel();
}

int main(int argc,char **argv) {
	// Adiciona caracteristica de aleatoriedade atrelada com o tempo
	// srand(time(NULL));
	Initializate();
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	windowId = glutCreateWindow("CG - OPENGL - Projeto 1");
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0; 
}
