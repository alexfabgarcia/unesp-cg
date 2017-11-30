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

#define Z_POSITION_OUT_OF_SCREEN 16

#define NUM_OF_SHIPS 14
#define NUM_OF_WALLS 36
#define NUM_OF_FUEL_STATIONS 4
#define NUM_OF_FLOOR_TILES 36
#define MAX_PROJECTILES_IN_GAME 20

#define WALL_AND_WATER_LENGTH 4
#define REPLACEABLE_WALL_INDEX_BY_BRIDGE 30

#define BRIDGE_WIDTH 12
 
#define ship_largura 3
#define ship_altura 1
#define ship_comprimento 2

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
// Ponte
t_aabb_object bridge;

//√çndice dos objetos mais dist√¢ntes (Necess√°rias para o infinite runner)
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
void DrawWall(float, float, bool, bool);
void DrawBridge(t_aabb_object);
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

int randInRange(int min, int max)
{
  return min + (int) (rand() / (double) (RAND_MAX + 1) * (max - min + 1));
}

long time_explosion_bridge = 0;

//Display
void Display() {	
	//Encontrando dt
	current_call_time = clock()/ (CLOCKS_PER_SEC / ONE_SECOND_IN_MILLI);
	deltaTime = current_call_time - last_call_time;
	last_call_time = current_call_time;
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH); 
	glEnable(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
	// Inicializa par‚metros de rendering
	// Define a cor de fundo da janela de visualiza√ß√£o como preta
	glClearColor(0.5, 0.8, 0.9, 0.0); 
   
	glMatrixMode(GL_PROJECTION);/*glMatrixMode()- define qual matriz ser√° alterada. SEMPRE defina o tipo de apresenta√ß√£o 
	                          (Ortogonal ou Perspectiva) na matriz PROJECTION.*/
	glLoadIdentity();//"Limpa" ou "transforma" a matriz em identidade, reduzindo poss√≠veis erros.

	if (projecao==1){
		// ProjeÁ„o ortogonal
		glOrtho(-10, 10, -5, 15, -50, 50);
	}
	else
	  gluPerspective(60,1,1,150); //Define a projeÁ„o como perspectiva
   
	glMatrixMode(GL_MODELVIEW);/*glMatrixMode()- define qual matriz ser√° alterada. SEMPRE defina a c√¢mera 
	                          (Ortogonal ou Perspectiva) na matriz MODELVIEW (onde o desenho ocorrer√°).*/
	glLoadIdentity(); ////"Limpa" ou "transforma" a matriz em identidade, reduzindo poss√≠veis erros.

	gluLookAt(posx,posy,posz,0,-4,0,lx,ly,lz); //Define a pos da c‚mera, para onde olha e qual eixo est·° na vertical.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* "limpa" um buffer particular ou combina√ß√µes de buffers, 
	                                                     onde buffer √© uma √°rea de armazenamento para informa√ß√µes da imagem. 
	                                                    Nesse caso, est√° "limpando os buffers para suportarem anima√ß√µes */

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
	//DrawBackground();
	
   //Desenha Navios
   int i;
	for(i=0; i<NUM_OF_SHIPS; i++) {
		ship[i].z += player_current_speed * deltaTime;		//S = S0 + V * T	
		ship[i].x += ship[i].direcao * 0.001 * deltaTime;
		if(ship[i].x < -3.5) {
			ship[i].direcao = 1;
		}else if(ship[i].x > 3.5) {
			ship[i].direcao = -1;
		}else if(paused) {
			ship[i].direcao = 0;
		}
		
		//Reseta posiÁıes do navio apÛs sair da tela
		if(ship[i].z >= 12) {					
			ship[i].z = ship[most_far_ship_index].z - 10;
			ship[i].inGame = true;
			most_far_ship_index = i;
		}
		
		if(ship[i].inGame) {
			if (bridge.inGame && hasCollision(ship[i], bridge)) {
				ship[i].inGame = false;
			} else {
				DrawShip(ship[i].x, ship[i].z);
			
				//Colis„o player com navio
				if (hasCollision(player, ship[i])) {
					doBeforeGameOver();
				}	
			}
		}
	}

	//Desenha Paredes
	for(i=0; i<NUM_OF_WALLS; i++) {
		wall_right[i].z += player_current_speed * deltaTime;
		wall_left[i].z += player_current_speed * deltaTime;

		// Desenha parece convencional ou "modo ponte", quando a parede nao esta em jogo
		DrawWall(wall_right[i].x, wall_right[i].z, false, !(wall_right[i].inGame));
		DrawWall(wall_left[i].x, wall_left[i].z, true, !(wall_right[i].inGame));
		
		if (i == REPLACEABLE_WALL_INDEX_BY_BRIDGE && bridge.inGame) {
			bridge.x = 0;
			bridge.z = wall_right[i].z + WALL_AND_WATER_LENGTH/2;

			// Colisao do jogador com a ponte
			if (bridge.inGame && hasCollision(player, bridge)) {
				doBeforeGameOver();
			}

			DrawBridge(bridge);
		}

		//Reseta posiÁıes da parede apÛs sair da tela
		if(wall_right[i].z >= Z_POSITION_OUT_OF_SCREEN){
			wall_right[i].z = wall_right[most_far_wall_index].z-WALL_AND_WATER_LENGTH;
			wall_left[i].z = wall_left[most_far_wall_index].z-WALL_AND_WATER_LENGTH;

			if (i == REPLACEABLE_WALL_INDEX_BY_BRIDGE) {
				if (randInRange(0, 1) == 1) { // 50% de chance de parede dar lugar a uma ponte
					wall_right[i].inGame = false; 
					wall_left[i].inGame = wall_right[i].inGame;
					bridge.inGame = !(wall_right[i].inGame);
					printf("Parede %d sera substituida por ponte: %s\n", i, bridge.inGame ? "Sim" : "Nao");
				} else {
					
				}
			}

			most_far_wall_index = i;
		}
	}

	//Desenha a ¡gua (Ch„o)
	for(i=0; i<NUM_OF_FLOOR_TILES; i++) {
		floor[i].z += player_current_speed * deltaTime;
		DrawWater(floor[i].x, floor[i].z);
		//Reseta posiÁıes do ch„o/·gua apÛs sair da tela
		if(floor[i].z >= Z_POSITION_OUT_OF_SCREEN) {
			floor[i].z = floor[most_far_floor_tile_index].z-WALL_AND_WATER_LENGTH;
			most_far_floor_tile_index = i;
		}
	}
	
	// Desenha estacoes de abastecimento
	for(i=0; i<NUM_OF_FUEL_STATIONS; i++) {
		fuel_station[i].z += player_current_speed * deltaTime;		//S = S0 + V * T
		
		//Reseta posiÁıes do navio apÛs sair da tela
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

			//DestrÛi projÈtil depois de 10 segundos
			if(projectiles[i].lifetime / ONE_SECOND_IN_MILLI >= 10) {
				projectiles[i].inGame = false;
			} else {
				//Colisao de projÈtil com a ponte
				if (bridge.inGame && hasCollision(projectiles[i], bridge)) {
					bridge.inGame = false;
					projectiles[i].inGame = false;
					puts("Projetil explodiu ponte.");
					time_explosion_bridge = time_counter;
					shootDownBridge();
					playSoundStoppingOthers(EXPLOSION_SOUND);
					continue;
				}

				//Colisao de projÈtil com os navios
				int j;
				for(j=0; j<NUM_OF_SHIPS; j++) {
					if(ship[j].inGame && hasCollision(projectiles[i], ship[j])) {
						//destroi projÈtil
						projectiles[i].inGame = false;
						//destroi navio
						ship[j].inGame = false;
						
						// Abate o navio
						shootDownShip();
						playSoundStoppingOthers(EXPLOSION_SOUND);
						
						break;
					}
				}

				//Colisao de projÈtil com os estaÁ„o de abastecimento
				for(j = 0; j < NUM_OF_FUEL_STATIONS; j++) {
					if (fuel_station[j].inGame && hasCollision(projectiles[i], fuel_station[j])) {
						//destroi projÈtil
						projectiles[i].inGame = false;
						//destroi estaÁ„o de abastecimento
						fuel_station[j].inGame = false;
						
						// Abate a estaÁ„o de abastecimento
						shootDownFuelStation();
						playSoundStoppingOthers(EXPLOSION_SOUND);
						
						break;
					}
				}
			}
		}
	}
	
	Draw2DInfo();
	decreaseFuel();
	
	if (!gameOver && isFuelEmpty()) {
		doBeforeGameOver();
	}
	
   glutSwapBuffers(); //Executa a Cena. SwapBuffers d√° suporte para mais de um buffer, permitindo execu√ß√£o de anima√ß√µes sem cintila√ß√µes. 
}

//Jogador
void DrawPlayer() {
	
	//HÈlice
	glPushMatrix();
		changeColorToLightGrey();
		glTranslated(player.x,player.y+.3,player.z-0.8);
		glScalef(.2,.2,.1);
		glutSolidTorus(0.7,1,50,50);
	glPopMatrix();
	
	//Corpo
	glPushMatrix();
		changeColorToLightGrey();
		glTranslated(player.x,player.y+.1,player.z);
		glScalef(.15,.15,1.7);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();	//Frente
		changeColorToLightGrey();
		glTranslated(player.x,player.y+.1,player.z-0.2);
		glScalef(.2,.2,1);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();	//Frente
		changeColorToLightGrey();
		glTranslated(player.x,player.y+.1,player.z-0.2);
		glScalef(.3,.3,.7);
		glutSolidCube(1);
	glPopMatrix();
	
	//Asas Dianteiras superiores
	glPushMatrix();
		glColor3ub(140, 0, 0);
		glTranslated(player.x,player.y+.4,player.z-0.3);
		glScalef(2,.05,.7);
		glutSolidCube(1);
	glPopMatrix();
	
	//Asas Dianteiras inferiores
	glPushMatrix();
		glColor3ub(140, 0, 0);
		glTranslated(player.x,player.y-.2,player.z-0.3);
		glScalef(2,.05,.7);
		glutSolidCube(1);
	glPopMatrix();
	
	//Hastes asas
	glPushMatrix();
		changeColorToLightGrey();	//frente esquerda
		glTranslated(player.x-0.7,player.y+0.1,player.z-0.5);
		glScalef(0.05,0.6,0.05);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		changeColorToLightGrey();	//frente direita
		glTranslated(player.x+0.7,player.y+0.1,player.z-0.5);
		glScalef(0.05,0.6,0.05);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		changeColorToLightGrey();	//tr·s direita
		glTranslated(player.x+0.7,player.y+0.1,player.z);
		glScalef(0.05,0.6,0.05);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		changeColorToLightGrey();	//tr·s esquerda
		glTranslated(player.x-0.7,player.y+0.1,player.z);
		glScalef(0.05,0.6,0.05);
		glutSolidCube(1);
	glPopMatrix();
	
	//Asas Traseiras
	glPushMatrix();
		glColor3ub(140, 0, 0);
		glTranslated(player.x,player.y+.3,player.z+1);
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

//Background em Top View
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
void DrawWall(float x,float z, bool left, bool bridgeWall) {
	float x_pos = (left ? 1 : -1) * 0.5;
	glPushMatrix();
		glTranslatef(x,-5,z);
		glScalef(4, 10, WALL_AND_WATER_LENGTH);
		
		if (bridgeWall) {
			changeColorToLightGrey();
		} else {
			// Desenha parede marrom com topo verde para dar efeito de grama
			glColor3ub(color_wall[0], color_wall[1], color_wall[2]);
		}
		DrawSquareXPlane(x_pos, -0.5, .45, 0, 1.05); // Offset para garantia de sensacao de continuidade
		
		if (!bridgeWall) {
			glColor3ub(color_grass[0], color_grass[1], color_grass[2]);
		}
		DrawSquareXPlane(x_pos, 0.44, .5, 0, 1.05); // Offset para garantia de sensacao de continuidade

		// Desenha grama
		DrawSquareYPlane(0.5, .5, -0.5, 0, 1.05);
	glPopMatrix();
	glutPostRedisplay();
}

// Ponte
void DrawBridge(t_aabb_object bridge) {
	glPushMatrix();
		glTranslatef(bridge.x, -5, bridge.z);
		glScalef(BRIDGE_WIDTH, 10, WALL_AND_WATER_LENGTH);
		changeColorToLightGrey();
		glutSolidCube(1);
	glPopMatrix();

	//Debug
	if(debug_mode) {
		glPushMatrix();
			changeColorToRed();
			glTranslatef(bridge.x - BRIDGE_WIDTH/2, -10, bridge.z + WALL_AND_WATER_LENGTH/2);
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
		
		glPushMatrix();
			changeColorToRed();
			glTranslatef(bridge.x + BRIDGE_WIDTH/2, 0, bridge.z - WALL_AND_WATER_LENGTH/2);
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
	}

	glutPostRedisplay();
}

//¡gua
void DrawWater(float x,float z) {
	glPushMatrix();
		if (time_explosion_bridge != 0 && (time_counter - time_explosion_bridge) < 1500) {
			changeColorToDarkPinkRed();
		} else {
			glColor3ub(color_water[0], color_water[1], color_water[2]);
		}
		glTranslatef(x,-10,z);
		glScalef(12,.00001,WALL_AND_WATER_LENGTH);
		glutSolidCube(1.05); // Offset para garantia de sensacao de 
	glPopMatrix();
	glutPostRedisplay();
}

//Barco
void DrawShip(float x, float z) {
	glPushMatrix();
		changeColorToLightGrey();
		glTranslated(x,-9.7,z);
		glScalef(ship_largura, ship_altura, ship_comprimento);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		changeColorToLightGrey();
		glTranslated(x,-9.7+0.2,z);
		glScalef(ship_largura+1.4, ship_altura-0.4, ship_comprimento-0.4);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		changeColorToLightGrey();
		glTranslated(x,-9.7+0.4,z);
		glScalef(ship_largura+1.8, ship_altura-0.6, ship_comprimento-0.8);
		glutSolidCube(1);
	glPopMatrix();
	//Cabine
	glPushMatrix();
		changeColorToDarkGrey();
		glTranslated(x,-9.7,z);
		glScalef(ship_largura-0.4, ship_altura+1, ship_comprimento-0.4);
		glutSolidCube(1);
	glPopMatrix();
	//Vidros
	float aux;
	for(aux = -1; aux<1; aux+=0.4) {
		glPushMatrix();
			changeColorToGlassBlue();
			glTranslated(x-aux,-9.7+0.7,z);
			glScalef(0.2, 0.2, ship_comprimento-0.37);
			glutSolidCube(1);
		glPopMatrix();
	}
	for(aux = -1.4; aux<1.4; aux+=0.4) {
		glPushMatrix();
			changeColorToGlassBlue();
			glTranslated(x-aux,-9.7+0.2,z);
			glScalef(0.2, 0.2, ship_comprimento+0.1);
			glutSolidCube(1);
		glPopMatrix();
	}
	glPushMatrix();
		changeColorToGlassBlue();
		glTranslated(x-1.8,-9.7+0.2,z);
		glScalef(0.2, 0.2, ship_comprimento-0.2);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		changeColorToGlassBlue();
		glTranslated(x+1.8,-9.7+0.2,z);
		glScalef(0.2, 0.2, ship_comprimento-0.2);
		glutSolidCube(1);
	glPopMatrix();
	//Canhıes
	for(aux = -1; aux<=1; aux++) {
		glPushMatrix();	//Frente
			changeColorToDarkGrey();
			glTranslated(x+aux*1.2,-9.7-0.2,z+1.1);
			glRotatef(25,-1,10*aux,0);
			glScalef(0.2, 0.2, ship_comprimento/2);
			glutSolidCube(1);
		glPopMatrix();	
		glPushMatrix();	//Tr·s
			changeColorToDarkGrey();
			glTranslated(x+aux*1.2,-9.7-0.2,z-1.1);
			glRotatef(25,1,-10*aux,0);
			glScalef(0.2, 0.2, ship_comprimento/2);
			glutSolidCube(1);
		glPopMatrix();
	}
	
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
	// Base
	glPushMatrix();
		changeColorToWhite();
		glTranslated(x,-9.7,z);
		glScalef(fuel_station_largura, fuel_station_altura, fuel_station_comprimento);
		glutSolidCube(1);
	glPopMatrix();
	
	// Efeito borda
	glPushMatrix();
		changeColorToWhite();
		glTranslated(x,-9.7,z-0.2);
		glScalef(fuel_station_largura*0.8, fuel_station_altura, fuel_station_comprimento);
		glutSolidCube(1);
	glPopMatrix();
	
	// Efeito base
	glPushMatrix();
		changeColorToWhite();
		glTranslated(x, -9.7, z+fuel_station_comprimento/2);
		glScalef(fuel_station_largura*1.2, fuel_station_altura, .2);
		glutSolidCube(1);
	glPopMatrix();
	
	// "Vidro" da estaÁ„o de combustÌvel
	glPushMatrix();
		changeColorToGlassBlue();
		glTranslated(x, -9.6, z-1);
		glScalef(0.8, 1, 0.8);
		glutSolidCube(1);
	glPopMatrix();
	
	// Desenha "F" de "Fuel"
	glPushMatrix();
		changeColorToRed();
		glTranslated(x, -9.6, z);
		glScalef(0.8, 1, 0.8);
        DrawSquareYPlane(.6, -.3, -.15, .8, 1.8);
        DrawSquareYPlane(.6, -.15, .3, .8, 1);
        DrawSquareYPlane(.6, -.15, .3, 1.2, 1.4);
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
	// O angulo vai de -50 (100% de combustÌvel) atÈ 50 (0% de combustÌvel)
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
	//Encontra projÈtil disponÌvel para entrar no jogo
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
			if(player.x < 5) {
				player.x += 0.5;
			}
		}
		if (key=='a') {
			if(player.x > -5) {
				player.x -= 0.5;
			}
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
		restoreFuelCounter();
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
		//Inicializa posiÁıes dos navios
		ship[i].x = rand()% 10 - 5;
		ship[i].z = (-i-1)*10;
		ship[i].direcao = (rand() % 2 - 1 >= 0) ? 1:-1;
		ship[i].inGame = true;
		ship[i].width = ship_largura;
		ship[i].length = ship_comprimento;
	}
	most_far_ship_index = NUM_OF_SHIPS-1;
	
	//Walls
	for(i=0; i<NUM_OF_WALLS; i++) {
		//Inicializa posi√ßposiÁıes£o das paredes da direita
		wall_right[i].x = 8;
		wall_right[i].z = (-i*WALL_AND_WATER_LENGTH)+Z_POSITION_OUT_OF_SCREEN;
		//Inicializa posiÁıesß√£o das paredes da esquerda
		wall_left[i].x = -8;
		wall_left[i].z = (-i*WALL_AND_WATER_LENGTH)+Z_POSITION_OUT_OF_SCREEN;
		wall_right[i].inGame = true;
		wall_left[i].inGame = true;
	}
	most_far_wall_index = NUM_OF_WALLS-1;

	// Ponte
	bridge.x = 0;
	bridge.z = 0;
	bridge.inGame = false;
	bridge.width = BRIDGE_WIDTH;
	bridge.length = WALL_AND_WATER_LENGTH;
	
	//Floor
	for(i=0; i<NUM_OF_FLOOR_TILES; i++) {
		//Inicializa posiÁıes dos tiles de ¡gua no ch„o
		floor[i].x = 0;
		floor[i].z = (-i*WALL_AND_WATER_LENGTH)+Z_POSITION_OUT_OF_SCREEN;
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
		fuel_station[i].z = (-i-2) * 25; // Atribui uma dist‚ncia inicial dada a aletoriedade por tempo "srand"
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
	// Adiciona caracteristica de aleatoriedade atrelada com o tempo (semente)
	srand(time(NULL));
	
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
