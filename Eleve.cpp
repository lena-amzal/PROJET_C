#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <ctime>
#include "G2D.h"
 
	
using namespace std;


// ===================== CONSTANTES ECRANS =====================
const int ECRAN_ACCUEIL = 0;
const int INIT_PARTIE = 1;
const int ECRAN_JEU = 2;
const int ECRAN_GAMEOVER = 3;
const int ECRAN_WIN = 4;
 
struct _Heros
{
	string texture =
		"[     YYYYYYYYYYYY     ]"
		"[  YYYYYYYYYYYYYYYYYY  ]"
		"[ YYYYYYYYYYYYYYYYYYYY ]"
		"[YYYYYYYYYYYYYYYYYYYYYY]"
		"[YYYYYYYYYYYYYYYYYYYYYY]"
		"[YYYYYYYYYYYYYYYYYYYYYY]"
		"[YYYYYYYYYYYYYYYYYYYYYY]"
		"[YYYYYYYYYYYYYYYYYYYYYY]"
		"[ YYYYYYYYYYYYYYYYYYYY ]"
		"[   YYYYYYYYYYYYYYYYY  ]"
		"[     YYYYYYYYYYYY     ]";

	string texture2 =
		"[     YYYYYYYYYYYY     ]"
		"[  YYYYYYYYYYYYYYYYYY  ]"
		"[ YYYYYYYYYYYYYYYYYYYY ]"
		"[YYYYYYYYYYYYYYYYYY    ]"
		"[YYYYYYYYYYYYYYY       ]"
		"[YYYYYYYYYYYYY         ]"
		"[YYYYYYYYYYYYYYY       ]"
		"[YYYYYYYYYYYYYYYYYY    ]"
		"[ YYYYYYYYYYYYYYYYYYYY ]"
		"[   YYYYYYYYYYYYYYYYY  ]"
		"[     YYYYYYYYYYYY     ]";

	V2 Size;
	int IdTex;
	int IdTex2;
	V2 Pos = V2(45,45);
	int NbVies = 3;
	int PiecesOr = 0;
};


struct _Fantome
{
	string texture[3] = {
		// Rouge - Blinky
		"[    RRRRRRRRRRRRRR    ]"
		"[  RRRRRRRRRRRRRRRRRR  ]"
		"[ RRRRRRRRRRRRRRRRRRRR ]"
		"[RRRRRRRRRRRRRRRRRRRRRR]"
		"[RRR WW RRRRRR WW RRRRR]"
		"[RR WWKK RRRR WWKK RRRR]"
		"[RR WWKK RRRR WWKK RRRR]"
		"[RRR WW RRRRRR WW RRRRR]"
		"[RRRRRRRRRRRRRRRRRRRRRR]"
		"[RRRRRRRRRRRRRRRRRRRRRR]"
		"[RRRRRRRRRRRRRRRRRRRRRR]"
		"[RRR  RRRR  RRRR  RRRRR]",

		// Cyan - Inky
		"[    CCCCCCCCCCCCCC    ]"
		"[  CCCCCCCCCCCCCCCCCC  ]"
		"[ CCCCCCCCCCCCCCCCCCCC ]"
		"[CCCCCCCCCCCCCCCCCCCCCC]"
		"[CCC WW CCCCCC WW CCCCC]"
		"[CC WWKK CCCC WWKK CCCC]"
		"[CC WWKK CCCC WWKK CCCC]"
		"[CCC WW CCCCCC WW CCCCC]"
		"[CCCCCCCCCCCCCCCCCCCCCC]"
		"[CCCCCCCCCCCCCCCCCCCCCC]"
		"[CCCCCCCCCCCCCCCCCCCCCC]"
		"[CCC  CCCC  CCCC  CCCCC]",

		// Magenta - Pinky
		"[    MMMMMMMMMMMMMM    ]"
		"[  MMMMMMMMMMMMMMMMMM  ]"
		"[ MMMMMMMMMMMMMMMMMMMM ]"
		"[MMMMMMMMMMMMMMMMMMMMMM]"
		"[MMM WW MMMMMM WW MMMMM]"
		"[MM WWKK MMMM WWKK MMMM]"
		"[MM WWKK MMMM WWKK MMMM]"
		"[MMM WW MMMMMM WW MMMMM]"
		"[MMMMMMMMMMMMMMMMMMMMMM]"
		"[MMMMMMMMMMMMMMMMMMMMMM]"
		"[MMMMMMMMMMMMMMMMMMMMMM]"
		"[MMM  MMMM  MMMM  MMMMM]"
	};
	V2 Size;
	int IdTex;
	V2 Pos;
	V2 Dir;
	bool Vivante = true;
	bool Peur = false;
	double TimePeur = 0;
};



struct GameData
{

	string Map =
		"MMMMMMMMMMMMMMMMMMMM"
		"M M         M      M"
		"M M M MMM M M MM M M"
		"M   M     M M      M"
		"MMM M MMM M M MM M M"
		"M   M M   M M      M"
		"M MMM M MMM M MM M M"
		"M   M M   M M      M"
		"M M M M M M M MM M M"
		"M M   M M   M      M"
		"M M MMM M MMM MM M M"
		"M M     M     M    M"
		"M MMMMM M MMMM MM  M"
		"M       M          M"
		"MMMMMMMMMMMMMMMMMMMM";
	int	NbLignes = 15;
	int NbColonnes = 20;


	// indique la présence d'un mur à la case (x,y)
	 bool Mur(int x, int y) const  { return Map[(NbLignes - y - 1)*NbColonnes+x] == 'M'; }  // les fonctions const ne modifient pas les données

	int Lpix = 40;  // largeur en pixels des cases du labyrinthe


	_Heros Heros;   // data du héros
	_Fantome Fantome[3];
	

	int Ecran = ECRAN_ACCUEIL;
	double TimeEcran = 0.0;
	int FrameCount = 0;
	vector<vector<bool>> Pieces;
	vector<vector<bool>> Fruits;
	bool ModeFear = false;
	double TimeFear = 0;

	bool Invincible = false;
	double TimeInvincible = 0;


	GameData() {}

};


// indique collision et position valide pour le héros

bool CollisionRectRect(V2 r1, V2 s1, V2 r2, V2 s2)
{
	if (r1.x + s1.x < r2.x) return false;
	if (r1.x > r2.x + s2.x) return false;
	if (r1.y + s1.y < r2.y) return false;
	if (r1.y > r2.y + s2.y) return false;
	return true;
}


bool PositionValide(const GameData& G, V2 pos, V2 taille)
{
	for (int x = 0; x < G.NbColonnes; x++)
		for (int y = 0; y < G.NbLignes; y++)
			if (G.Mur(x, y))
			{
				V2 murPos = V2((float)(x * G.Lpix), (float)(y * G.Lpix));
				V2 murSize = V2((float)G.Lpix, (float)G.Lpix);
				if (CollisionRectRect(pos, taille, murPos, murSize))
					return false;
			}
	return true;
}

//deplacement du Heros



void GestionDeplacementHeros(GameData& G)
{
	V2 newPos = G.Heros.Pos;
	float speed = 2;
	bool enMouvement = false;

	if (G2D::isKeyPressed(Key::LEFT)) { newPos.x -= speed; enMouvement = true; }
	if (G2D::isKeyPressed(Key::RIGHT)) { newPos.x += speed; enMouvement = true; }
	if (G2D::isKeyPressed(Key::UP)) { newPos.y += speed; enMouvement = true; }
	if (G2D::isKeyPressed(Key::DOWN)) { newPos.y -= speed; enMouvement = true; }

	V2 taille = V2(G.Heros.Size.x - 4, G.Heros.Size.y - 4);

	if (PositionValide(G, newPos, taille))
		G.Heros.Pos = newPos;

	if (enMouvement)
		G.FrameCount++;
	else
		G.FrameCount = 0; 
}

//Mort du heros

void GestionMortHeros(GameData& G)
{
	G.Heros.Pos = V2(G.Lpix + 5, G.Lpix + 5);
	G.Heros.NbVies -= 1;
	// Repousse les fantômes à leur position de départ
	V2 startPos[3] = {
		V2(3 * G.Lpix,  13 * G.Lpix),
		V2(17 * G.Lpix, 13 * G.Lpix),
		V2(17 * G.Lpix,  1 * G.Lpix)
	};
	for (int i = 0; i < 3; i++)
		if (G.Fantome[i].Vivante)
			G.Fantome[i].Pos = startPos[i];
}



//collision Fantomes

void GestionCollisionFantomes(GameData& G)
{
	for (int i = 0; i < 3; i++)
	{
		if (!G.Fantome[i].Vivante) continue;

		if (CollisionRectRect(G.Heros.Pos, G.Heros.Size,
			G.Fantome[i].Pos, G.Fantome[i].Size))
		{
			if (G.Invincible)
			{
				V2 startPos[3] = {
					V2(3 * G.Lpix,  13 * G.Lpix),
					V2(17 * G.Lpix, 13 * G.Lpix),
					V2(17 * G.Lpix,  1 * G.Lpix)
				};

				G.Fantome[i].Pos = startPos[i];
				G.Fantome[i].Dir = V2(1, 0);
				G.Fantome[i].Peur = false;
			}
			else
			{
				GestionMortHeros(G);
				return;
			}
		}
	}
}

//IA Fantomes


void DeplacementFantomes(GameData& G, _Fantome& M)
{
	float speed = 2.0f;

	// Snap sur la grille
	M.Pos.x = round(M.Pos.x / speed) * speed;
	M.Pos.y = round(M.Pos.y / speed) * speed;

	V2 dirs[4] = { V2(0,1), V2(1,0), V2(0,-1), V2(-1,0) };

	V2 bestDir = M.Dir;
	float bestScore = M.Peur ? -999999.f : 999999.f;
	bool found = false;

	for (int i = 0; i < 4; i++)
	{
		V2 dir = dirs[i];
		if (dir.x == -M.Dir.x && dir.y == -M.Dir.y) continue;

		V2 newPos = M.Pos + dir * speed;
		if (!PositionValide(G, newPos, M.Size)) continue;

		found = true;

		float dx = G.Heros.Pos.x - newPos.x;
		float dy = G.Heros.Pos.y - newPos.y;
		float dist = dx * dx + dy * dy;
		float score = M.Peur ? -dist : dist;

		if ((M.Peur && score > bestScore) || (!M.Peur && score < bestScore))
		{
			bestScore = score;
			bestDir = dir;
		}
	}

	if (!found)
	{
		for (int i = 0; i < 4; i++)
		{
			V2 newPos = M.Pos + dirs[i] * speed;
			if (PositionValide(G, newPos, M.Size))
			{
				M.Dir = dirs[i];
				M.Pos = newPos;
				return;
			}
		}
		return;
	}

	M.Dir = bestDir;
	M.Pos = M.Pos + M.Dir * speed;
}

void UpdateFantomes(GameData& G)
{
	for (int i = 0; i < 3; i++)
	{
		if (G.Fantome[i].Vivante)
			DeplacementFantomes(G, G.Fantome[i]);
	}
}


//Pieces
bool PlusDePieces(const GameData& G)
{
	for (int x = 0; x < G.NbColonnes; x++)
		for (int y = 0; y < G.NbLignes; y++)
			if (G.Pieces[x][y])
				return false;

	return true;
}

int gestion_ecran_accueil(GameData& G)
{
	if (G2D::keyHasBeenHit(Key::ENTER))
		return INIT_PARTIE;
	return ECRAN_ACCUEIL;
}

int InitPartie(GameData& G)
{
	srand((unsigned int)time(nullptr));

	// Héros
	G.Heros.Pos = V2(45, 45);
	G.Heros.NbVies = 3;
	G.Heros.PiecesOr = 0;


	//Pieces
	G.Pieces.resize(G.NbColonnes, vector<bool>(G.NbLignes, false));

	for (int x = 0; x < G.NbColonnes ; x++)
	{
		for (int y = 0; y < G.NbLignes; y++)
		{
			if (!G.Mur(x, y))
				G.Pieces[x][y] = true;
		}
	}


	// FRUITS
	G.Fruits.resize(G.NbColonnes, vector<bool>(G.NbLignes, false));

	int placed = 0;

	while (placed < 4)
	{
		int x = rand() % G.NbColonnes;
		int y = rand() % G.NbLignes;

		if (!G.Mur(x, y) && !G.Fruits[x][y])
		{
			G.Fruits[x][y] = true;
			placed++;
		}
	}

	G.Invincible = false;

	// Fantomes
	V2 startPos[3] = {
		V2(3 * G.Lpix,  13 * G.Lpix),  // haut gauche
		V2(17 * G.Lpix, 13 * G.Lpix),  // haut droite
		V2(17 * G.Lpix,  1 * G.Lpix)   // bas droite
	};
	V2 startDir[3] = { V2(1, 0), V2(0, 1), V2(-1, 0) };

	for (int i = 0; i < 3; i++)
	{
		G.Fantome[i].Pos = startPos[i];
		G.Fantome[i].Dir = startDir[i];
		G.Fantome[i].Vivante = true;
	}

	G.FrameCount = 0;
	G.TimeEcran = 0.0;

	return ECRAN_JEU;
	for(int i = 0; i < 3; i++)
	{
		int cx = G.Fantome[i].Pos.x / G.Lpix;
		int cy = G.Fantome[i].Pos.y / G.Lpix;
		cout << "Fantome " << i << " case (" << cx << "," << cy << ") mur=" << G.Mur(cx, cy) << endl;
	}
}

void GestionPieces(GameData& G)
{
	int x = G.Heros.Pos.x / G.Lpix;
	int y = G.Heros.Pos.y / G.Lpix;

	if (G.Pieces[x][y])
	{
		G.Pieces[x][y] = false;
		G.Heros.PiecesOr++;
	}
}

void GestionFruits(GameData& G)
{
	int x = G.Heros.Pos.x / G.Lpix;
	int y = G.Heros.Pos.y / G.Lpix;

	if (G.Fruits[x][y])
	{
		G.Fruits[x][y] = false;

		G.Invincible = true;
		G.TimeInvincible = G2D::elapsedTimeFromStartSeconds();

		G.ModeFear = true;
		G.TimeFear = G2D::elapsedTimeFromStartSeconds();

		for (int i = 0; i < 3; i++)
		{
			if (G.Fantome[i].Vivante)
			{
				G.Fantome[i].Peur = true;
				G.Fantome[i].TimePeur = G.TimeFear;
			}
		}

	}
}

int gestion_jeu(GameData& G)
{
	GestionDeplacementHeros(G);
	GestionPieces(G);
	GestionCollisionFantomes(G);
	GestionFruits(G);
	UpdateFantomes(G);



	G.FrameCount++;

	double now = G2D::elapsedTimeFromStartSeconds();

	if (G.ModeFear && now - G.TimeFear > 7.0)
	{
		G.ModeFear = false;

		for (int i = 0; i < 3; i++)
			G.Fantome[i].Peur = false;
	}

	//Victoire
	if (PlusDePieces(G))
		return ECRAN_WIN;

	//Defaite
	if (G.Heros.NbVies <= 0)  return ECRAN_GAMEOVER;

	return ECRAN_JEU;
}

int gestion_GameOver(GameData& G)
{
	double tempsActuel = G2D::elapsedTimeFromStartSeconds();
	if (G.TimeEcran == 0.0)
		G.TimeEcran = tempsActuel;

	if (tempsActuel - G.TimeEcran >= 3.0)
	{
		G.TimeEcran = 0.0;
		return ECRAN_ACCUEIL;
	}
	return ECRAN_GAMEOVER;
}

int gestion_Win(GameData& G)
{
	double tempsActuel = G2D::elapsedTimeFromStartSeconds();
	if (G.TimeEcran == 0.0)
		G.TimeEcran = tempsActuel;

	if (tempsActuel - G.TimeEcran >= 3.0)
	{
		G.TimeEcran = 0.0;
		return ECRAN_ACCUEIL;
	}
	return ECRAN_WIN;
}


void Render(const GameData& G)
{
	G2D::clearScreen(Color::Black);
	int largeurFenetre = G.Lpix * G.NbColonnes;  // 800
	int hauteurFenetre = G.Lpix * G.NbLignes;    // 600

	int largeurRect = 400;
	int hauteurRect = 80;
	int hauteurRect1 = 60;
	int hauteurRect2 = 40;

	// Centre horizontal 
	int x = (largeurFenetre - largeurRect) / 2;

	// Centre vertical
	int y = (hauteurFenetre - hauteurRect) / 2;
	int y1 = hauteurFenetre / 2;        // rectangle titre
	int y2 = hauteurFenetre / 2 - 80;  // rectangle bouton en dessous

	// ECRAN ACCUEIL
	if (G.Ecran == ECRAN_ACCUEIL)
	{
		G2D::drawRectangle(V2(x, y1 - hauteurRect1 / 2), V2(largeurRect, hauteurRect1), Color::White, true);
		G2D::drawRectangle(V2(x, y2 - hauteurRect2 / 2), V2(largeurRect, hauteurRect2), Color::Gray, true);
		// Texte centré dans le rectangle blanc
		G2D::drawStringFontMono(V2(x + largeurRect / 2 - 80, y1 - 10), "LABYRINTHE", 25, 3, Color::Black);
		// Texte centré dans le rectangle magenta  
		G2D::drawStringFontMono(V2(x + largeurRect / 2 - 80, y2 - 8), "ENTREE to play", 18, 2, Color::Black);
		G2D::Show();
		return;
	}

	// ECRAN JEU
	if (G.Ecran == ECRAN_JEU)
	{
		// Labyrinthe
		for (int x = 0; x < G.NbColonnes; x++)
		{
			for (int y = 0; y < G.NbLignes; y++)
			{
				if (G.Mur(x, y))
				{
					G2D::drawRectangle(
						V2(x * G.Lpix, y * G.Lpix),
						V2(G.Lpix, G.Lpix),
						Color::Cyan);
				}

				// PIECES
				if (G.Pieces[x][y])
				{
					G2D::drawCircle(
						V2(x * G.Lpix + G.Lpix / 2, y * G.Lpix + G.Lpix / 2),
						3,
						Color::Yellow,
						true
					);
				}

				// FRUITS
				if (G.Fruits[x][y])
				{
					G2D::drawCircle(
						V2(x * G.Lpix + G.Lpix / 2, y * G.Lpix + G.Lpix / 2),
						6,
						Color::Red,
						true
					);
				}

			}
		}




		// Fantome
		for (int i = 0; i < 3; i++)
			if (G.Fantome[i].Vivante)
				G2D::drawRectWithTexture(G.Fantome[i].IdTex,
					G.Fantome[i].Pos,
					G.Fantome[i].Size);	

		// Héros - animation marche
		int texHeros = (G.FrameCount / 30) % 2 == 0
			? G.Heros.IdTex
			: G.Heros.IdTex2;
		G2D::drawRectWithTexture(texHeros, G.Heros.Pos, G.Heros.Size);

		// Carrés de vies
		for (int i = 0; i < G.Heros.NbVies; i++)
			G2D::drawRectangle(
				V2(10 + i * (G.Lpix + 5), G.Lpix * G.NbLignes - G.Lpix * 0.7f),
				V2(G.Lpix * 0.5f, G.Lpix * 0.5f),
				Color::Red, true);

		//Chrono invincibilité
		if (G.Invincible)
		{
			double now = G2D::elapsedTimeFromStartSeconds();
			double remaining = 10.0 - (now - G.TimeInvincible);

			if (remaining < 0) remaining = 0;

			string txt = "INVINCIBLE: " + to_string((int)remaining) + "s";

			G2D::drawStringFontMono(
				V2(400, 570),
				txt,
				18,
				2,
				Color::Red
			);
		}

		G2D::Show();
		return;
	}

	// ECRAN GAME OVER
	if (G.Ecran == ECRAN_GAMEOVER)
	{
		G2D::drawRectangle(V2(x, y), V2(largeurRect, hauteurRect), Color::Red, true);
		G2D::drawStringFontMono(V2(x + largeurRect / 2 - 100, y + hauteurRect / 2 - 15), "GAME OVER", 30, 3, Color::White);
		G2D::Show();
		return;
	}

	// ECRAN WIN
	if (G.Ecran == ECRAN_WIN)
	{
		G2D::drawRectangle(V2(x, y), V2(largeurRect, hauteurRect), Color::Green, true);
		G2D::drawStringFontMono(V2(x + largeurRect / 2 - 100, y + hauteurRect / 2 - 15), "YOU WIN !", 30, 3, Color::White);
		G2D::Show();
		return;
	}
}




void Logic(GameData& G)
{
	
	if (G.Ecran == ECRAN_ACCUEIL)  G.Ecran = gestion_ecran_accueil(G);
	if (G.Ecran == INIT_PARTIE)    G.Ecran = InitPartie(G);
	if (G.Ecran == ECRAN_JEU)      G.Ecran = gestion_jeu(G);
	if (G.Ecran == ECRAN_GAMEOVER) G.Ecran = gestion_GameOver(G);
	if (G.Ecran == ECRAN_WIN)      G.Ecran = gestion_Win(G);	
}
 

void AssetsInit(GameData & G)
{
   // Size passé en ref et texture en param
   G.Heros.IdTex = G2D::initTextureFromString(G.Heros.Size, G.Heros.texture);  
   G.Heros.Size = V2(G.Lpix * 0.8f, G.Lpix * 0.8f);


   V2 size2;
   G.Heros.IdTex2 = G2D::initTextureFromString(size2, G.Heros.texture2);
 


   for (int i = 0; i < 3; i++)
   {
	   V2 momieSize;
	   G.Fantome[i].IdTex = G2D::initTextureFromString(momieSize, G.Fantome[i].texture[i]);
	   G.Fantome[i].Size = V2(G.Lpix * 0.8f, G.Lpix * 0.8f);
   }

}

int main(int argc, char* argv[])
{ 
	GameData G;

	G2D::initWindow(V2(G.Lpix * G.NbColonnes, G.Lpix * G.NbLignes), V2(200,200), string("Labyrinthe"));
	  
	AssetsInit(G);

	int nbCalltoLogicPerSec = 50;

	G2D::Run(Logic,Render,G, nbCalltoLogicPerSec,true);
 
}
 