/*Desenho do navio */

#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <time.h>
float multiplicador; //Para aumentar ou diminuir o tamanho dos objetos


/*Todos os vértices são desenhados no sentido horário, começando sempre pelo superior esquerdo, e terminando
no inferior esquerdo de cada quadrilátero.
Seguindo o blend, os vetores do eixo y devem ser multiplicados por dois, devido a distorção da altura do blend.
Dessa mesma forma, o eixo Z deve ser dividido por 2, para evitar as distorções.
O desenho da figura original está no discord, com suas respectivas medidas.
No navio, o eixo Z nao faz sentido ser negativo, pois com ele igual a zero é o mínimo valor que pode assumir, já que 0 é o chão (água)
O eixo Z não inverte sinal pois não está espelhado em ambos os lados do plano, somente z>0.
*/
void desenhar_navio(float posX, float posY, float posZ){
	glEnableClientState(GL_VERTEX_ARRAY);
	multiplicador = 1;  //Para ajustar o tamanho do objeto, basta definir um valor para o multiplicador
	GLfloat vertices_cubo1[] = {
	//Cubo da base (6 faces, 4 vértices por face)
		//Face Frontal (y, z, x), y=horizontal, z=vertical, x=profundidade, visão lateral do navio, virado pra direita
				posY-(5.6 * multiplicador), posZ+(2*multiplicador), 	posX-(2*multiplicador),//v1
				posY+(5.6 * multiplicador), posZ+(2*multiplicador), 	posX-(2*multiplicador),//v2 
				posY+(5.6 * multiplicador), posZ, 						posX-(2*multiplicador),//v3
				posY+(5.6 * multiplicador), posZ, 						posX-(2*multiplicador),//v4
		//Face lateral esquerda
				posY-(5.6 * multiplicador), posZ+(2*multiplicador), 	posX+(2*multiplicador),//v1
				posY-(5.6 * multiplicador), posZ+(2*multiplicador), 	posX-(2*multiplicador),//v2 
				posY-(5.6 * multiplicador),	posZ,						posX-(2*multiplicador),//v3 
				posY-(5.6 * multiplicador), posZ,						posX+(2*multiplicador),//v4
		//Face lateral direita (somente inverte o sinal do eixo y)
				posY+(5.6 * multiplicador), posZ+(2*multiplicador), 	posX+(2*multiplicador),//v1
				posY+(5.6 * multiplicador), posZ+(2*multiplicador), 	posX-(2*multiplicador),//v2 
				posY+(5.6 * multiplicador),	posZ, 						posX-(2*multiplicador),//v3 
				posY+(5.6 * multiplicador), posZ,						posX+(2*multiplicador),//v4	
		//Face de trás (somente inverte o sinal eixo x, da profundidade)
				posY-(5.6 * multiplicador), posZ+(2*multiplicador),	 	posX+(2*multiplicador),//v1
				posY+(5.6 * multiplicador), posZ+(2*multiplicador),		posX+(2*multiplicador),//v2 
				posY+(5.6 * multiplicador), posZ,						posX+(2*multiplicador),//v3
				posY+(5.6 * multiplicador), posZ,						posX+(2*multiplicador),//v4
		//Face de cima
				posY-(5.6 * multiplicador), posZ+(2*multiplicador),		posX-(2*multiplicador), //(menor ponto do navio).
				posY+(5.6 * multiplicador), posZ+(2*multiplicador),		posX-(2*multiplicador),	
				posY+(5.6 * multiplicador), posZ+(2*multiplicador),		posX+(2*multiplicador),
				posY-(5.6 * multiplicador), posZ+(2*multiplicador),		posX+(2*multiplicador),
		//Face de baixo (somente inverte o sinal do eixo z, da altura).
				posY-(5.6 * multiplicador), posZ,						posX-(2*multiplicador), //(menor ponto do navio).
				posY+(5.6 * multiplicador), posZ,						posX-(2*multiplicador),	
				posY+(5.6 * multiplicador), posZ,						posX+(2*multiplicador),
				posY-(5.6 * multiplicador), posZ,						posX+(2*multiplicador),		
	};
	//Desenha o cubo definido acima
	glVertexPointer(3,GL_FLOAT,0,vertices_cubo1);
	glDrawArrays(GL_QUADS, 0, 24);
	
	GLfloat vertices_cubo2[] = {
	//Cubo do meio
		//Face da frente
				posY-(7 * multiplicador),	posZ+(3.5*multiplicador),	posX-(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(3.5*multiplicador), 	posX-(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(2*multiplicador), 	posX-(2.5*multiplicador),
				posY-(7 * multiplicador),	posZ+(2*multiplicador),		posX-(2.5*multiplicador),
		//Face lateral esquerda
				posY-(7 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.5*multiplicador),
				posY-(7 * multiplicador),	posZ+(3.5*multiplicador), 	posX-(2.5*multiplicador),
				posY-(7 * multiplicador),	posZ+(2*multiplicador),		posX-(2.5*multiplicador),
				posY-(7 * multiplicador),	posZ+(2*multiplicador),		posX+(2.5*multiplicador),
		//Face lateral direita (somente inverte o sinal do eixo y)
				posY+(7 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(3.5*multiplicador), 	posX-(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(2*multiplicador),		posX-(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(2*multiplicador),		posX+(2.5*multiplicador),
		//Face de trás (somente inverte o sinal eixo x, da profundidade)
				posY-(7 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(3.5*multiplicador), 	posX+(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(2*multiplicador), 	posX+(2.5*multiplicador),
				posY-(7 * multiplicador),	posZ+(2*multiplicador),		posX+(2.5*multiplicador),
		//Face de cima (muda apenas a posição do eixo Z)	
				posY-(7 * multiplicador),	posZ+(3.5*multiplicador),	posX-(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(3.5*multiplicador),	posX-(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.5*multiplicador),
				posY-(7 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.5*multiplicador),
		//Face de baixo	
				posY-(7 * multiplicador),	posZ+(2*multiplicador),		posX-(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(2*multiplicador),		posX-(2.5*multiplicador),
				posY+(7 * multiplicador),	posZ+(2*multiplicador),		posX+(2.5*multiplicador),
				posY-(7 * multiplicador),	posZ+(2*multiplicador),		posX+(2.5*multiplicador),
	};
		//Desenha o cubo definido acima
	glVertexPointer(3,GL_FLOAT,0,vertices_cubo2);
	glDrawArrays(GL_QUADS, 0, 24);
	
	GLfloat vertices_cubo3[] = {
	//Cubo de cima (chão do navio)
		//Face da frente
				posY-(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX-(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX-(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX-(2.75*multiplicador),
				posY-(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX-(2.75*multiplicador),
		//Face lateral esquerda
				posY-(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX-(2.75*multiplicador),
				posY-(7.5 * multiplicador), posZ+(4.0*multiplicador),	posX+(2.75*multiplicador),
				posY-(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.75*multiplicador),
				posY-(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX-(2.75*multiplicador),
		//Face lateral direita (somente inverte o sinal do eixo y)
				posY+(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX-(2.75*multiplicador),
				posY+(7.5 * multiplicador), posZ+(4.0*multiplicador),	posX+(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX-(2.75*multiplicador),
		//Face de trás (somente inverte o sinal eixo x, da profundidade)
				posY-(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX+(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX+(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.75*multiplicador),
				posY-(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.75*multiplicador),
		//Face de cima
				posY-(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX+(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX+(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX-(2.75*multiplicador),
				posY-(7.5 * multiplicador),	posZ+(4.0*multiplicador),	posX-(2.75*multiplicador),
		//Face de baixo		
				posY-(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX+(2.75*multiplicador),
				posY+(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX-(2.75*multiplicador),
				posY-(7.5 * multiplicador),	posZ+(3.5*multiplicador),	posX-(2.75*multiplicador),
	};
	//Desenha o cubo definido acima
	glVertexPointer(3,GL_FLOAT,0,vertices_cubo3);
	glDrawArrays(GL_QUADS, 0, 24);
	
	
	
	GLfloat vertices_cubo4[] = {
	//Cubo do meio, que segura o canhão
		//Face frontal
				posY-(4 * multiplicador),	posZ+(5.5*multiplicador),	posX-(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(5.5*multiplicador),	posX-(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(4*multiplicador),		posX-(2*multiplicador),
				posY-(4 * multiplicador),	posZ+(4*multiplicador),		posX-(2*multiplicador),	
	
		//Face lateral esquerda
				posY-(4 * multiplicador),	posZ+(5.5*multiplicador),	posX-(2*multiplicador),
				posY-(4 * multiplicador), 	posZ+(5.5*multiplicador),	posX+(2*multiplicador),
				posY-(4 * multiplicador),	posZ+(4*multiplicador),		posX+(2*multiplicador),
				posY-(4 * multiplicador),	posZ+(4*multiplicador),		posX-(2*multiplicador),
		//Face lateral direita (somente inverte o sinal do eixo y)
				posY+(4 * multiplicador),	posZ+(5.5*multiplicador),	posX-(2*multiplicador),
				posY+(4 * multiplicador), posZ+(5.5*multiplicador),		posX+(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(4*multiplicador),		posX+(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(4*multiplicador),		posX-(2*multiplicador),		
		//Face de trás (somente inverte o sinal eixo x, da profundidade)
				posY-(4 * multiplicador),	posZ+(5.5*multiplicador),	posX+(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(5.5*multiplicador),	posX+(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(4*multiplicador),		posX+(2*multiplicador),
				posY-(4 * multiplicador),	posZ+(4*multiplicador),		posX+(2*multiplicador),		
		//Face de cima
				posY-(4 * multiplicador),	posZ+(5.5*multiplicador),	posX+(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(5.5*multiplicador),	posX+(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(5.5*multiplicador),	posX-(2*multiplicador),
				posY-(4 * multiplicador),	posZ+(5.5*multiplicador),	posX-(2*multiplicador),		
		//Face de baixo	
				posY-(4 * multiplicador),	posZ+(4*multiplicador),		posX+(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(4*multiplicador),		posX+(2*multiplicador),
				posY+(4 * multiplicador),	posZ+(4*multiplicador),		posX-(2*multiplicador),
				posY-(4 * multiplicador),	posZ+(4*multiplicador),		posX-(2*multiplicador),		
	};
	//Desenha o cubo definido acima
	glVertexPointer(3,GL_FLOAT,0,vertices_cubo4);
	glDrawArrays(GL_QUADS, 0, 24);
	
	
	GLfloat vertices_cubo5[] = {
	//Canhão
		//Face frontal
				posY-(6 * multiplicador), 	posZ+(5.3*multiplicador),	posX-(0.3*multiplicador),
				posY-(4 * multiplicador),	posZ+(5.3*multiplicador),	posX-(0.3*multiplicador),
				posY-(4 * multiplicador),	posZ+(5.0*multiplicador),	posX-(0.3*multiplicador),
				posY-(6 * multiplicador),	posZ+(5.0*multiplicador),	posX-(0.3*multiplicador),
		//Face lateral esquerda
				posY-(6 * multiplicador),	posZ+(5.3*multiplicador),	posX-(0.3*multiplicador),
				posY-(6 * multiplicador), 	posZ+(5.3*multiplicador),	posX+(0.3*multiplicador),
				posY-(6 * multiplicador),	posZ+(5.0*multiplicador),	posX+(0.3*multiplicador),
				posY-(6 * multiplicador),	posZ+(5.0*multiplicador),	posX-(0.3*multiplicador),
		//Face lateral direita (somente inverte o sinal do eixo y)
				posY+(6 * multiplicador),	posZ+(5.3*multiplicador),	posX-(0.3*multiplicador),
				posY+(6 * multiplicador), 	posZ+(5.3*multiplicador),	posX+(0.3*multiplicador),
				posY+(6 * multiplicador),	posZ+(5.0*multiplicador),	posX+(0.3*multiplicador),
				posY+(6 * multiplicador),	posZ+(5.0*multiplicador),	posX-(0.3*multiplicador),	
		//Face de trás (somente inverte o sinal eixo x, da profundidade)
				posY-(6 * multiplicador),	posZ+(5.3*multiplicador),	posX+(0.3*multiplicador),
				posY+(6 * multiplicador),	posZ+(5.3*multiplicador),	posX+(0.3*multiplicador),
				posY+(6 * multiplicador),	posZ+(5.0*multiplicador),	posX+(0.3*multiplicador),
				posY-(6 * multiplicador),	posZ+(5.0*multiplicador),	posX+(0.3*multiplicador),	
		//Face de cima
				posY-(6 * multiplicador),	posZ+(5.3*multiplicador),	posX+(0.3*multiplicador),
				posY+(6 * multiplicador),	posZ+(5.3*multiplicador),	posX+(0.3*multiplicador),
				posY+(6 * multiplicador),	posZ+(5.3*multiplicador),	posX-(0.3*multiplicador),
				posY-(6 * multiplicador),	posZ+(5.3*multiplicador),	posX-(0.3*multiplicador),								
		//Face de baixo
				posY-(6 * multiplicador),	posZ-(5.3*multiplicador),	posX+(0.3*multiplicador),
				posY+(6 * multiplicador),	posZ-(5.3*multiplicador),	posX+(0.3*multiplicador),
				posY+(6 * multiplicador),	posZ-(5.3*multiplicador),	posX-(0.3*multiplicador),
				posY-(6 * multiplicador),	posZ-(5.3*multiplicador),	posX-(0.3*multiplicador),
	};
	//Desenha o cubo definido acima
	glVertexPointer(3,GL_FLOAT,0,vertices_cubo5);
	glDrawArrays(GL_QUADS, 0, 24);
	glDisableClientState(GL_VERTEX_ARRAY);
	
}


