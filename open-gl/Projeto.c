#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <time.h>

#define NUM_OF_SHIPS 14

struct t_ship{
	float x;
	float z;
};

//Navios
t_ship ship[NUM_OF_SHIPS];

//Camera
int cameraMode = 0;
int projecao = 0; //Variável Lógica para Definir o Tipo de Projeção (Perspectiva ou Ortogonal)
int posx = 0,	posy = 0,	posz = 10; //Variáveis que definem a posição da câmera
int oy = 0,		ox=0,		oz = 0;         //Variávesis que definem para onde a câmera olha
int lx = 0,		ly=1,		lz = 0;         //Variáveis que definem qual eixo estará na vertical do monitor.

//Controles
float player_speed = 0.001;

//Colors
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

void Display()
{
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
   deltaPos += player_speed * deltaTime;
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
		ship[i].z += player_speed * deltaTime;		//S = S0 + V * T
		DrawShip(ship[i].x, ship[i].z);
		//Reseta posição do navio após sair da tela. Talvez ajustes serão necessários aqui
		if(ship[i].z >= 12) {					
			ship[i].z = -128;
		}
	}
   
   //Draw Walls
   for(i=-160; i<=12; i+=4) {
		DrawWall(8,i + deltaPos);
		DrawWall(-8,i + deltaPos);
		DrawWater(0,i + deltaPos);
   }
   glutSwapBuffers(); //Executa a Cena. SwapBuffers dá suporte para mais de um buffer, permitindo execução de animações sem cintilações. 
}

//Jogador
void DrawPlayer() {
  	glPushMatrix();
		glColor3ub(255,255,255);
		glTranslatef(ox,oy,0);
		glScalef(1,1,-1.4);
		glutSolidCone(.5,1,10,10);
	glPopMatrix();
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
	glutPostRedisplay();	
}

void keyboard (unsigned char key, int x, int y)
{
	if (key=='d')
		ox+=1;
	if (key=='a') 
		ox-=1;
	if (key=='w')
		oy+=1;
	if(key=='s')
		oy-=1;
	if(key=='c')
		cameraMode = (cameraMode + 1)%2 ;	//0 = normal		1 = top down
	if(key=='e')
		player_speed *=2;
	if(key=='q')
		player_speed /=2;
		
	printf("Camera mode: %d\n", cameraMode);
		
	if (cameraMode == 0) {
    	posx=0, posy=0, posz=10;
    	lx=0, ly=1,  lz=0;
    	
	} else if (cameraMode == 1) {
		posx=0, posy=10, posz=0;
    	lx=0, ly=0,  lz=-1;
	}

}

int main(int argc,char **argv)
{
	Initializate();
	
   glutInit(&argc, argv); // Initializes glut
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); /*Define as características do espaço vetorial. 
                                                                           //  Nesse caso, permite animações (sem cintilações), cores compostas por Verm. Verde e Azul,
                                                                           //  Buffer que permite trablhar com profundidade e elimina faces escondidas.*/           
  
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
	current_call_time = clock() / (CLOCKS_PER_SEC / 1000);
	int i;
	//Navios
	for(i=0; i<NUM_OF_SHIPS; i++) {
		ship[i].x = rand()% 10 - 5;
		ship[i].z = -i*10;
	}
}