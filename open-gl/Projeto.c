#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <time.h>

#define NUM_OF_SHIPS 14
#define NUM_OF_WALLS 36
#define NUM_OF_FLOOR_TILES 34
#define MAX_PROJECTILES_IN_GAME 20

#define ship_largura 3
#define ship_altura 1
#define ship_comprimento 1

#define projectile_largura 0.1
#define projectile_altura 0.1
#define projectile_comprimento 0.7

//Estruturas de dados que armazenam informaÃ§Ãµes dos objetos do jogo
struct t_ship{
	float x;
	float z;
	int direcao;
	bool inGame;
};

struct t_world_static_object{
	float x;
	float z;
};

struct t_projectile{
	float x;
	float y;
	float z;
	float lifetime;
	bool  inGame;
};

//Navios
t_ship ship[NUM_OF_SHIPS];
//Walls
t_world_static_object wall_left[NUM_OF_WALLS];
t_world_static_object wall_right[NUM_OF_WALLS];
//Floor
t_world_static_object floor[NUM_OF_FLOOR_TILES];
//Projectiles
t_projectile projectiles[MAX_PROJECTILES_IN_GAME];

//Ãndice dos objetos mais distÃ¢ntes (NecessÃ¡rias para o infinite runner)
int most_far_ship_index;
int most_far_wall_index;
int most_far_floor_tile_index;

//Camera
int cameraMode = 0;
int projecao = 0; //VariÃ¡vel LÃ³gica para Definir o Tipo de ProjeÃ§Ã£o (Perspectiva ou Ortogonal)
int posx = 0,	posy = 0,	posz = 10;		//VariÃ¡veis que definem a posiÃ§Ã£o da cÃ¢mera
int oy = -9,	ox=0,		oz = 0;         //VariÃ¡vesis que definem para onde a cÃ¢mera olha
int lx = 0,		ly=1,		lz = 0;         //VariÃ¡veis que definem qual eixo estarÃ¡ na vertical do monitor.

//Controles
float player_current_speed = 0.001;//0.001
bool paused = false;
bool debug_mode = false;

//Cores
int color_wall[3] = {0, 128, 0};
int color_water[3] = {0,0,128};

//Timer
long deltaTime, current_call_time, last_call_time;		//Calcular o tempo de cada chamada: inspirado no game loop de desenvolvimento de jogos
long time_counter = 0;									//Em milisegundos

//EspaÃ§o
float deltaPos = 0;

//ProtÃ³tipos das FunÃ§Ãµes
void Initializate();
void Display();
void keyboard (unsigned char key, int x, int y);
void DrawPlayer();
void DrawWall(float, float);
void DrawWater(float, float);
void DrawShip(float, float);
void DrawShoot(float, float, float);
void Pause();
bool AABB(float, float, float, float, float, float, float, float);
void Shoot();

void Display() {
	//Encontrando dt
	current_call_time = clock()/ (CLOCKS_PER_SEC / 1000);;
	deltaTime = current_call_time - last_call_time;
	last_call_time = current_call_time;
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH); 
	glEnable(GL_SMOOTH);
	glEnable(GL_BLEND);
   
	// Inicializa parâmetros de rendering
	// Define a cor de fundo da janela de visualizaÃ§Ã£o como preta
	glClearColor(0.5, 0.8, 0.9, 0.0); 
   
	glMatrixMode(GL_PROJECTION);/*glMatrixMode()- define qual matriz serÃ¡ alterada. SEMPRE defina o tipo de apresentaÃ§Ã£o 
	                          (Ortogonal ou Perspectiva) na matriz PROJECTION.*/
	glLoadIdentity();//"Limpa" ou "transforma" a matriz em identidade, reduzindo possÃ­veis erros.

	if (projecao==1){
		// Projeção ortogonal
		glOrtho(-10, 10, -10, 10, -50, 50);
	}
	else
	  gluPerspective(60,1,1,150); //Define a projeção como perspectiva
   
	glMatrixMode(GL_MODELVIEW);/*glMatrixMode()- define qual matriz serÃ¡ alterada. SEMPRE defina a cÃ¢mera 
	                          (Ortogonal ou Perspectiva) na matriz MODELVIEW (onde o desenho ocorrerÃ¡).*/
	glLoadIdentity(); ////"Limpa" ou "transforma" a matriz em identidade, reduzindo possÃ­veis erros.

	gluLookAt(posx,posy,posz,0,-4,0,lx,ly,lz); //Define a pos da câmera, para onde olha e qual eixo está¡ na vertical.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* "limpa" um buffer particular ou combinaÃ§Ãµes de buffers, 
	                                                     onde buffer Ã© uma Ã¡rea de armazenamento para informaÃ§Ãµes da imagem. 
	                                                    Nesse caso, estÃ¡ "limpando os buffers para suportarem animaÃ§Ãµes */

	//S = S0 + V * T
	deltaPos += player_current_speed * deltaTime;
	time_counter += deltaTime;
	//printf("%d \n\n", time_counter);

	//Contador de tempo (segundos)
	//if(time_counter/1000 >= 10)
	  //DrawShip( -5 ,-32 + deltaPos);

	//Draw Player
	DrawPlayer();

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
		
		//Reseta posições do navio após sair da tela
		if(ship[i].z >= 12) {					
			ship[i].z = ship[most_far_ship_index].z - 10;
			ship[i].inGame = true;
			most_far_ship_index = i;
		}
		
		if(ship[i].inGame == true) {
			DrawShip(ship[i].x, ship[i].z);
			//Colisão player com navio
			if(AABB(ox-0.5, ox+0.5, ship[i].x-1.5, ship[i].x+1.5, oz-0.75, oz+0.75, ship[i].z-0.5, ship[i].z+0.5)) {
				Initializate();
			}
		}
	}

	//Desenha Paredes
	for(i=0; i<NUM_OF_WALLS; i++) {
		wall_right[i].z += player_current_speed * deltaTime;
		wall_left[i].z += player_current_speed * deltaTime;
		DrawWall(wall_right[i].x, wall_right[i].z);
		DrawWall(wall_left[i].x, wall_left[i].z);
		//Reseta posições da parede após sair da tela
		if(wall_right[i].z >= 16){
			wall_right[i].z = wall_right[most_far_wall_index].z-3.95;	//sobreposiÃ§Ã£o de 0.05
			wall_left[i].z = wall_right[most_far_wall_index].z-3.95;	//sobreposiÃ§Ã£o de 0.05
			most_far_wall_index = i;
		}
	}

	//Desenha a Água (Chão)
	for(i=0; i<NUM_OF_FLOOR_TILES; i++) {
		floor[i].z += player_current_speed * deltaTime;
		DrawWater(floor[i].x, floor[i].z);
		//Reseta posições do chão/água após sair da tela
		if(floor[i].z >= 14) {
			floor[i].z = floor[most_far_floor_tile_index].z-3.95; //sobreposiÃ§Ã£o de 0.05
			most_far_floor_tile_index = i;
		}
	}

	for(i=0; i<MAX_PROJECTILES_IN_GAME; i++) {
		if(projectiles[i].inGame == true) {
			projectiles[i].z -= player_current_speed * 3 * deltaTime;
			projectiles[i].lifetime += deltaTime;
			DrawShoot(projectiles[i].x, projectiles[i].y, projectiles[i].z);
			//Destrói projétil depois de 10 segundos
			if(projectiles[i].lifetime/1000 >= 10) {
				projectiles[i].inGame = false;
			}
			//Colisao de projétil com os navios
			int j;
			for(j=0; j<NUM_OF_SHIPS; j++) {
				if(ship[j].inGame == true &&
					 AABB(projectiles[i].x-((float)projectile_largura/2), 			projectiles[i].x+((float)projectile_largura/2), 
						ship[j].x-((float)ship_largura/2), 							ship[j].x+((float)ship_largura/2),
						projectiles[i].z-((float)projectile_comprimento/2), 		projectiles[i].z+((float)projectile_comprimento/2),
						ship[j].z-((float)ship_comprimento/2), 						ship[j].z+((float)ship_comprimento/2))) {
					//destroi projétil
					projectiles[i].inGame = false;
					//destroi navio
					ship[j].inGame = false;
					break;
				}
			}
		}
	}

   glutSwapBuffers(); //Executa a Cena. SwapBuffers dÃ¡ suporte para mais de um buffer, permitindo execuÃ§Ã£o de animaÃ§Ãµes sem cintilaÃ§Ãµes. 
}

//Jogador
void DrawPlayer() {
  	glPushMatrix();
		glColor3ub(255,255,255);
		glTranslatef(ox,oy,+0.75);
		glScalef(1,1,-1.4);
		glutSolidCone(.5,1,10,10);
	glPopMatrix();
	//Debug
	if(debug_mode) {
		glPushMatrix();
			glColor3ub(255, 0, 0);
			glTranslated(ox-0.5,oy,0.75);
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
		
		glPushMatrix();
			glColor3ub(255, 0, 0);
			glTranslated(ox+0.5,oy,-0.75);
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
	}
  	glutPostRedisplay();
}

//Parede (Bloco)
void DrawWall(float x,float z) {
	glPushMatrix();
		glColor3ub(color_wall[0], color_wall[1], color_wall[2]);
		glTranslatef(x,-5,z);
		glScalef(4,10,4);
		glutSolidCube(1);
	glPopMatrix();
	glutPostRedisplay();
}

//Água
void DrawWater(float x,float z) {
	glPushMatrix();
		glColor3ub(color_water[0], color_water[1], color_water[2]);
		glTranslatef(x,-10,z);
		glScalef(12,.1,4);
		glutSolidCube(1);
	glPopMatrix();
	glutPostRedisplay();
}

//Barco
void DrawShip(float x, float z) {
	glPushMatrix();
		glColor3ub(255, 255, 255);
		glTranslated(x,-9.7,z);
		glScalef(ship_largura, ship_altura, ship_comprimento);
		glutSolidCube(1);
	glPopMatrix();
	//Debug
	if(debug_mode) {
		glPushMatrix();
			glColor3ub(255, 0, 0);
			glTranslated(x-((float)ship_largura/2), -9.7+((float)ship_altura/2), z+((float)ship_comprimento/2));
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
		
		glPushMatrix();
			glColor3ub(255, 0, 0);
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
		glColor3ub(77, 77, 77);
		glTranslated(x,y,z);
		glScalef(projectile_largura,projectile_altura,projectile_comprimento);
		glutSolidCube(1);
	glPopMatrix();
	//Debug
	if(debug_mode) {
		glPushMatrix();
			glColor3ub(255, 0, 0);
			glTranslated(x-((float)projectile_largura/2),y,z+((float)projectile_comprimento/2));
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
		
		glPushMatrix();
			glColor3ub(255, 0, 0);
			glTranslated(x+((float)projectile_largura/2),y,z-((float)projectile_comprimento/2));
			glScalef(0.2,0.2,0.2);
			glutSolidCube(1);
		glPopMatrix();
	}
}

void keyboard (unsigned char key, int x, int y) {
	//Movimento da nave
	if (key=='d')
		ox+=1;
	if (key=='a') 
		ox-=1;
	if (key=='w'){
		//oy+=1;
	}
	if(key=='s'){
		//oy-=1;
	}
	//Modo da camera
	if(key=='c')
		cameraMode = (cameraMode + 1)%2 ;	//0 = normal		1 = top down
	if(key=='e')
		player_current_speed *=2;
	if(key=='q')
		player_current_speed /=2;
	//Pause
	if(key=='p')
		Pause();
	//Debug Mode
	if(key==';')
		debug_mode = !debug_mode;
	//Atirar
	if(key==' ')
		Shoot();
		
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

int main(int argc,char **argv) {
	Initializate();
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("CG - OPENGL - Projeto 1");
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0; 
}

//Prepara as variÃ¡veis utilizadas na execuÃ§Ã£o
void Initializate() {
	//Obtem o momento da primeira chamada (para operar o deltaTime)
	current_call_time = clock() / (CLOCKS_PER_SEC / 1000);
	int i;
	//Navios
	for(i=0; i<NUM_OF_SHIPS; i++) {
		//Inicializa posições dos navios
		ship[i].x = rand()% 10 - 5;
		ship[i].z = -i*10;
		ship[i].direcao = (rand() % 2 - 1 >= 0) ? 1:-1;
		ship[i].inGame = true;
	}
	most_far_ship_index = NUM_OF_SHIPS-1;
	
	//Walls
	for(i=0; i<NUM_OF_WALLS; i++) {
		//Inicializa posiÃ§posições£o das paredes da direita
		wall_right[i].x = 8;
		wall_right[i].z = (-i*4)+16;
		//Inicializa posições§Ã£o das paredes da esquerda
		wall_left[i].x = -8;
		wall_left[i].z = (-i*4)+16;
	}
	most_far_wall_index = NUM_OF_WALLS-1;
	
	//Floor
	for(i=0; i<NUM_OF_FLOOR_TILES; i++) {
		//Inicializa posições dos tiles de Água no chão
		floor[i].x = 0;
		floor[i].z = (-i*4)+14;
	}
	most_far_floor_tile_index = NUM_OF_FLOOR_TILES-1;
	
	//Projectiles
	for(i=0; i<MAX_PROJECTILES_IN_GAME; i++) {
		projectiles[i].x = ox;
		projectiles[i].y = oy;
		projectiles[i].z = oz;
		projectiles[i].inGame = false;
	}
}

void Pause() {
	paused = !paused;
	if(paused) {
		player_current_speed = 0;
	}else {
		player_current_speed =  0.001;
	}
}

//Colisõees em x e z (verifica a intersecão entre os pontos Axy e Bxy)
bool AABB(float A_xmin, float A_xmax, float B_xmin, float B_xmax, float A_zmin, float A_zmax, float B_zmin, float B_zmax) {
	return !(A_xmax < B_xmin || A_xmin > B_xmax || A_zmax < B_zmin || A_zmin > B_zmax);
}

void Shoot() {
	int i;
	//Encontra projétil disponível para entrar no jogo
	for(i=0; i<MAX_PROJECTILES_IN_GAME; i++) {
		if(projectiles[i].inGame == false) {
			projectiles[i].x = ox;
			projectiles[i].y = -9;
			projectiles[i].z = oz;
			projectiles[i].lifetime = 0;
			projectiles[i].inGame = true;
			break;
		}
	}
}
