#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <time.h>

#define NUM_OF_SHIPS 2
#define NUM_OF_WALLS 36
#define NUM_OF_FLOOR_TILES 34

//Estruturas de dados que armazenam informações dos objetos do jogo
struct t_ship{
	float x;
	float z;
	int direcao;
};

struct t_world_static_object{
	float x;
	float z;
};

//Navios
t_ship ship[NUM_OF_SHIPS];
//Walls
t_world_static_object wall_left[NUM_OF_WALLS];
t_world_static_object wall_right[NUM_OF_WALLS];
//Floor
t_world_static_object floor[NUM_OF_FLOOR_TILES];

//Índice dos objetos mais distântes (Necessárias para o infinite runner)
int most_far_ship_index;
int most_far_wall_index;
int most_far_floor_tile_index;

//Camera
int cameraMode = 0;
int projecao = 0; //Variável Lógica para Definir o Tipo de Projeção (Perspectiva ou Ortogonal)
int posx = 0,	posy = 0,	posz = 10;		//Variáveis que definem a posição da câmera
int oy = -9,		ox=0,		oz = 0;         //Variávesis que definem para onde a câmera olha
int lx = 0,		ly=1,		lz = 0;         //Variáveis que definem qual eixo estará na vertical do monitor.

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

//Espaço
float deltaPos = 0;

//Protótipos das Funções
void Initializate();
void Display();
void keyboard (unsigned char key, int x, int y);
void DrawPlayer();
void DrawWall(float, float);
void DrawWater(float, float);
void DrawShip(float, float);
void Pause();
bool AABB(float, float, float, float, float, float, float, float);

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
	// Define a cor de fundo da janela de visualização como preta
	glClearColor(0.5, 0.8, 0.9, 0.0); 
   
	glMatrixMode(GL_PROJECTION);/*glMatrixMode()- define qual matriz será alterada. SEMPRE defina o tipo de apresentação 
	                          (Ortogonal ou Perspectiva) na matriz PROJECTION.*/
	glLoadIdentity();//"Limpa" ou "transforma" a matriz em identidade, reduzindo possíveis erros.

	if (projecao==1){
	glOrtho(-10, 10, -10, 10, -50, 50); //Define a projeção como ortogonal
	}
	else
	  gluPerspective(60,1,1,150); //Define a projeção como perspectiva
   
	glMatrixMode(GL_MODELVIEW);/*glMatrixMode()- define qual matriz será alterada. SEMPRE defina a câmera 
	                          (Ortogonal ou Perspectiva) na matriz MODELVIEW (onde o desenho ocorrerá).*/
	glLoadIdentity(); ////"Limpa" ou "transforma" a matriz em identidade, reduzindo possíveis erros.

	gluLookAt(posx,posy,posz,0,-4,0,lx,ly,lz); //Define a pos da câmera, para onde olha e qual eixo está na vertical.
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

   //Desenha Navios
   int i;
	for(i=0; i<NUM_OF_SHIPS; i++) {
		ship[i].z += player_current_speed * deltaTime;		//S = S0 + V * T
		DrawShip(ship[i].x, ship[i].z);
		
		ship[i].x += ship[i].direcao * 0.001 * deltaTime;
		if(ship[i].x < -4.5) {
			ship[i].direcao = 1;
		}else if(ship[i].x > 4.5) {
			ship[i].direcao = -1;
		}else if(paused) {
			ship[i].direcao = 0;
		}
		
		//Reseta posição do navio após sair da tela
		if(ship[i].z >= 12) {					
			ship[i].z = ship[most_far_ship_index].z - 10;
			most_far_ship_index = i;
		}
		
		//Colisão
		printf("Colisao: %d\n", AABB(ox-0.5, ox+0.5, ship[i].x-1.5, ship[i].x+1.5, oz-0.75, oz+0.75, ship[i].z-0.5, ship[i].z+0.5));
		//AABB_x(ox-0.5, ox+0.5, ship[i].x-1.5, ship[i].x+1.5)
		
		
	}

	//Desenha Paredes
	for(i=0; i<NUM_OF_WALLS; i++) {
		wall_right[i].z += player_current_speed * deltaTime;
		wall_left[i].z += player_current_speed * deltaTime;
		DrawWall(wall_right[i].x, wall_right[i].z);
		DrawWall(wall_left[i].x, wall_left[i].z);
		//Reseta posição da parede após sair da tela
		if(wall_right[i].z >= 16){
			wall_right[i].z = wall_right[most_far_wall_index].z-3.95;	//sobreposição de 0.05
			wall_left[i].z = wall_right[most_far_wall_index].z-3.95;	//sobreposição de 0.05
			most_far_wall_index = i;
		}
	}

	//Desenha a Água (Chão)
	for(i=0; i<NUM_OF_FLOOR_TILES; i++) {
		floor[i].z += player_current_speed * deltaTime;
		DrawWater(floor[i].x, floor[i].z);
		//Reseta posição do chão/água após sair da tela
		if(floor[i].z >= 14) {
			floor[i].z = floor[most_far_floor_tile_index].z-3.95; //sobreposição de 0.05
			most_far_floor_tile_index = i;
		}
	}

   //DrawWall(0, 0 + deltaPos);
   glutSwapBuffers(); //Executa a Cena. SwapBuffers dá suporte para mais de um buffer, permitindo execução de animações sem cintilações. 
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
		glScalef(3,1,1);
		glutSolidCube(1);
	glPopMatrix();
	//Debug
	if(debug_mode) {
		glPushMatrix();
		glColor3ub(255, 0, 0);
		glTranslated(x-1.5,-9.7+0.5,z+0.5);
		glScalef(0.2,0.2,0.2);
		glutSolidCube(1);
		glPopMatrix();
		
		glPushMatrix();
		glColor3ub(255, 0, 0);
		glTranslated(x+1.5,-9.7+0.5,z-0.5);
		glScalef(0.2,0.2,0.2);
		glutSolidCube(1);
		glPopMatrix();
	}
	glutPostRedisplay();	
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
	if(key==';')
		debug_mode = !debug_mode;
		
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

//Prepara as variáveis utilizadas na execução
void Initializate() {
	//Obtem o momento da primeira chamada (para operar o deltaTime)
	current_call_time = clock() / (CLOCKS_PER_SEC / 1000);
	int i;
	//Navios
	for(i=0; i<NUM_OF_SHIPS; i++) {
		//Inicializa posição dos navios
		ship[i].x = rand()% 10 - 5;
		ship[i].z = -i*10;
		ship[i].direcao = (rand() % 2 - 1 >= 0) ? 1:-1;
	}
	most_far_ship_index = NUM_OF_SHIPS-1;
	
	//Walls
	for(i=0; i<NUM_OF_WALLS; i++) {
		//Inicializa posição das paredes da direita
		wall_right[i].x = 8;
		wall_right[i].z = (-i*4)+16;
		//Inicializa posição das paredes da esquerda
		wall_left[i].x = -8;
		wall_left[i].z = (-i*4)+16;
	}
	most_far_wall_index = NUM_OF_WALLS-1;
	
	//Floor
	for(i=0; i<NUM_OF_FLOOR_TILES; i++) {
		//Inicializa posição dos tiles de água no chão
		floor[i].x = 0;
		floor[i].z = (-i*4)+14;
	}
	most_far_floor_tile_index = NUM_OF_FLOOR_TILES-1;
}

void Pause() {
	paused = !paused;
	if(paused) {
		player_current_speed = 0;
	}else {
		player_current_speed =  0.001;
	}
}

//Colisões em x e z (verifica a intersecção entre os pontos Axy e Bxy)
bool AABB(float A_xmin, float A_xmax, float B_xmin, float B_xmax, float A_zmin, float A_zmax, float B_zmin, float B_zmax) {
	return !(A_xmax < B_xmin || A_xmin > B_xmax || A_zmax < B_zmin || A_zmin > B_zmax);
}