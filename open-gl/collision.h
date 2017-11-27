#ifndef _COLLISION_H
#define _COLLISION_H

// Estrutura para um objeto do jogo passivel de colisao AABB
struct t_aabb_object {
	float x;
	float y;
	float z;
	int direcao;
	float lifetime;
	bool inGame;
	float width;
	float length;
};

// Funcao utilizada para verificar se dois objetos se colidem utilizado a tecnica AABB
bool isAabbCollision(float A_xmin, float A_xmax, float B_xmin, float B_xmax, float A_zmin, float A_zmax, float B_zmin, float B_zmax) {
	return !(A_xmax < B_xmin || A_xmin > B_xmax || A_zmax < B_zmin || A_zmin > B_zmax);
}

// Funcao utilizada para verificar se dois objetos se colidem
bool hasCollision(t_aabb_object A, t_aabb_object B) {
	return !gameOver && isAabbCollision(A.x - A.width/2, A.x + A.width/2, B.x - B.width/2, B.x + B.width/2,
						   A.z - A.length/2, A.z + A.length/2, B.z - B.length/2, B.z + B.length/2);
}

#endif
