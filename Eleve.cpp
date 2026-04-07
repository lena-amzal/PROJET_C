 
#pragma warning( disable : 4996 ) 

 
#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
 
 
using namespace std;
 
struct _Heros
{
	string texture =
		"[RRR  ]"
		"[RRWR ]"
		"[RRR  ]"
		"[YY   ]"
		"[YYY  ]"
		"[YY YG]"
		"[GG   ]"
		"[CC   ]"
		"[CC   ]"
		"[C C  ]"
		"[C C  ]";
	V2 Size;
	int IdTex; 
	V2 Pos = V2(45,45);
	V2 PosPixels;
	V2 DirDeplacement;
};

struct Momie
{
	V2 Size;
	V2 PosPixels;
	V2 DirDeplacement;
	string texture =
		"[BBBB  ]"
		"[BBBB  ]"
		"[BBBB  ]"
		"[BBBB  ]";
};

struct _Key
{
	string texture =
		"[                      ]"
		"[     W W         WWWW ]"
		"[     W W        W    W]"
		"[   WWWWWWWWWWWWWW    W]"
  	    "[                W    W]"
		"[                 WWWW ]";


	V2 Size;
	int IdTex;
	V2 Pos = V2(440, 450);
};



struct GameData
{

    string Map =
				 "MMMMMMMMMMMMMMM"
				 "M M           M"
				 "M M M MMM MMM M"
				 "M   M       M M"
				 "MMM M M MMM M M"
				 "M   M M     M M"
				 "M MMM MMM MMMMM"
				 "M   M  M      M"
				 "M M M  M M MM M"
				 "M M M  M M M  M"
				 "M M M MM M MMMM"
				 "M M M    M    M"
				 "M M M MMMMMMM M"
				 "M M      M    M"
				 "MMMMMMMMMMMMMMM";

	// indique la présence d'un mur à la case (x,y)
	 bool Mur(int x, int y) const  { return Map[(15 - y - 1)*15+x] == 'M'; }  // les fonctions const ne modifient pas les données

	int Lpix = 40;  // largeur en pixels des cases du labyrinthe

	_Heros Heros;   // data du héros
	_Key   Key;
	Momie Momie1; // Momie numéro 1
	Momie Momie2; // Momie numéro 2
	Momie Momie3; // Momie numéro 3

	GameData() {}

};

bool collisionRectangles(V2 r1, int l1, int h1, V2 r2, int l2, int h2)
{
	if (r1.x + l1 < r2.x) return false;  // R1 à gauche de R2
	if (r1.x > r2.x + l2) return false;  // R1 à droite de R2

	if (r1.y + h1 < r2.y) return false; // R1 en dessous de R2
	if (r1.y > r2.y + h2) return false; // R1 au dessus de R2

	return true; // sinon il y a forcément une intersection entre les deux rectangles
}


bool DetectMur(const GameData & G, V2 pos, V2 size) {
	for (int x = 0; x < 15; x++) {
		for (int y = 0; y < 15; y++) {
			if (G.Mur(x, y)) {
				V2 posMur(x * G.Lpix, y * G.Lpix);
				if (collisionRectangles(pos, size.x, size.y, posMur, G.Lpix, G.Lpix)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool DetectCollisionMomies(const GameData& G)
{		
		if (collisionRectangles(G.Heros.Pos, G.Heros.Size.x, G.Heros.Size.y, G.Momie1.PosPixels, G.Momie1.Size.x, G.Momie1.Size.y))
				return true;
		if (collisionRectangles(G.Heros.Pos, G.Heros.Size.x, G.Heros.Size.y, G.Momie2.PosPixels, G.Momie2.Size.x, G.Momie2.Size.y))
			return true;
		if (collisionRectangles(G.Heros.Pos, G.Heros.Size.x, G.Heros.Size.y, G.Momie3.PosPixels, G.Momie3.Size.x, G.Momie3.Size.y))
			return true;
	return false;       // aucune collision trouvée

}


void GestionDeplacementHeros(GameData& G) {
	V2 anciennePos = G.Heros.Pos;
	if (G2D::isKeyPressed(Key::LEFT))  G.Heros.Pos.x--;
	if (G2D::isKeyPressed(Key::RIGHT)) G.Heros.Pos.x++;
	if (G2D::isKeyPressed(Key::UP))    G.Heros.Pos.y++;
	if (G2D::isKeyPressed(Key::DOWN))  G.Heros.Pos.y--;
	if (DetectMur(G, G.Heros.Pos, G.Heros.Size)) G.Heros.Pos = anciennePos; // on annule le déplacement
}

void Render(const GameData& G)  // const ref => garantit que l'on ne modifie pas les donnes de G
{
	G2D::clearScreen(Color::Black);
	 
	for (int x = 0; x < 15; x++)
		for (int y = 0; y < 15; y++)
		{
			int xx = x * G.Lpix;
			int yy = y * G.Lpix;
			if ( G.Mur(x,y))
			   G2D::drawRectangle(V2(xx, yy), V2(G.Lpix, G.Lpix), Color::Blue, true);
		}
		 
	// affichage du héro avec boite englobante et zoom x 2
	G2D::drawRectangle(G.Heros.Pos,   G.Heros.Size, Color::Red );
	G2D::drawRectWithTexture(G.Heros.IdTex, G.Heros.Pos,   G.Heros.Size);
	
	// affichage de la clef
	G2D::drawRectWithTexture(G.Key.IdTex, G.Key.Pos, G.Key.Size);
	 
	  
	G2D::Show();
}

 
	 void Logic(GameData& G)
	 {
		 GestionDeplacementHeros(G);
	 }
 

void AssetsInit(GameData & G)
{
   // Size passé en ref et texture en param
   G.Heros.IdTex = G2D::initTextureFromString(G.Heros.Size, G.Heros.texture);  
   G.Heros.Size  = G.Heros.Size  * 2; // on peut zoomer la taille du sprite

   G.Key.IdTex   = G2D::initTextureFromString(G.Key.Size, G.Key.texture);
   G.Key.Size    = G.Key.Size * 1.5; // on peut zoomer la taille du sprite

}

int gestion_jeu()
{	
	GameData G;
	// le héros
	GestionDeplacementHeros(G);    // lecture du clavier / test 
	return 0;
}

int main(int argc, char* argv[])
{ 
	GameData G;

	G2D::initWindow(V2(G.Lpix * 15, G.Lpix * 15), V2(200,200), string("Labyrinthe"));
	  
	AssetsInit(G);

	int nbCalltoLogicPerSec = 50;

	G2D::Run(Logic,Render,G, nbCalltoLogicPerSec,true);
 
}
 
  
 