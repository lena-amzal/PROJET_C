#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <ctime>
#include "G2D.h"


using namespace std;

// Enum & const
enum class Ecran { Accueil, InitPartie, Jeu, GameOver, Win };
enum class TypeFantome { Crimson, Berry, Azure, Total };
const int NB_FANTOMES = static_cast<int>(TypeFantome::Total);
const int NB_LIGNES = 15; 
const int NB_COLONNES = 20; 

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
	V2 Dir = V2(1, 0);
	int IdTex;
	int IdTex2;
	V2 Pos = V2(45, 45);
	int NbVies = 3;
	int PiecesOr = 0;
};


struct _Fantome
{
    string texture[3] = {
		// Crimson
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

        // Azure
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

        // Berry
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
		"M MMMMM M MMMMMMM  M"
		"M       M          M"
		"MMMMMMMMMMMMMMMMMMMM";

    // indique la présence d'un mur à la case (x,y)
	bool Mur(int x, int y) const { return Map[(NB_LIGNES - y - 1) * NB_COLONNES + x] == 'M'; }

	// largeur en pixels des cases du labyrinthe
	int Lpix = 40;  
	// données du héros et des fantômes
	_Heros Heros; 
	_Fantome Fantome[NB_FANTOMES]; 
	V2 FantomeStartPos[NB_FANTOMES];
	V2 FantomeStartDir[NB_FANTOMES];

    double TimeEcran = 0.0;
	int FrameCount = 0;
    Ecran EcranActuel = Ecran::Accueil;
    vector<vector<bool>> Pieces;
	vector<vector<bool>> Bonus;
	bool Invincible = false;
	double TimeInvincible = 0;


	GameData()
	{
		// initialise les positions de départ des fantômes
		FantomeStartPos[static_cast<int>(TypeFantome::Crimson)] = V2(3 * Lpix, 13 * Lpix);
		FantomeStartPos[static_cast<int>(TypeFantome::Berry)] = V2(17 * Lpix, 13 * Lpix);
		FantomeStartPos[static_cast<int>(TypeFantome::Azure)] = V2(17 * Lpix, 1 * Lpix);

		// Directions initiales
		FantomeStartDir[static_cast<int>(TypeFantome::Crimson)] = V2(1, 0);
		FantomeStartDir[static_cast<int>(TypeFantome::Berry)] = V2(0, 1);
		FantomeStartDir[static_cast<int>(TypeFantome::Azure)] = V2(-1, 0);
	}

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
    for (int x = 0; x < NB_COLONNES; x++)
		for (int y = 0; y < NB_LIGNES; y++)
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

	if (G2D::isKeyPressed(Key::RIGHT)) { newPos.x += speed; G.Heros.Dir = V2(1, 0);  enMouvement = true; }
	if (G2D::isKeyPressed(Key::LEFT)) { newPos.x -= speed; G.Heros.Dir = V2(-1, 0); enMouvement = true; }
	if (G2D::isKeyPressed(Key::UP)) { newPos.y += speed; G.Heros.Dir = V2(0, 1);  enMouvement = true; }
	if (G2D::isKeyPressed(Key::DOWN)) { newPos.y -= speed; G.Heros.Dir = V2(0, -1); enMouvement = true; }

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
    // Replace les fantômes à leurs positions de départ
	for (int i = 0; i < NB_FANTOMES; i++)
		G.Fantome[i].Pos = G.FantomeStartPos[i];
}



//collision Fantomes

void GestionCollisionFantomes(GameData& G)
{
	for (int i = 0; i < NB_FANTOMES; i++)
	{
		if (CollisionRectRect(G.Heros.Pos, G.Heros.Size, G.Fantome[i].Pos, G.Fantome[i].Size))
		{
            if (G.Invincible)
			{
				// teleport ghost back to its centralized start position
				G.Fantome[i].Pos = G.FantomeStartPos[i];
				G.Fantome[i].Dir = V2(1, 0);
			}
			else
			{
				GestionMortHeros(G);
				return;
			}
		}
	}
}

//IA des fantômes


void DeplacementFantomes(GameData& G, _Fantome& M, TypeFantome type)
{
	float speed = 2.0f;

	V2 cible = G.Heros.Pos;

	if (type == TypeFantome::Crimson)  // Crimson suit exactement le héros
		cible = G.Heros.Pos;

	if (type == TypeFantome::Berry)  // Berry arrive devant le héros
		cible = G.Heros.Pos + G.Heros.Dir * 4 * G.Lpix;

	if (type == TypeFantome::Azure)  // Azure arrive derrière le héros
		cible = G.Heros.Pos - G.Heros.Dir * 4 * G.Lpix;

	V2 dirs[4] =
	{
		V2(0,1),
		V2(1,0),
		V2(0,-1),
		V2(-1,0)
	};

	V2 bestDir = M.Dir;

	if (G.Invincible)
	{
		float bestDist = -1.0f;

		for (int i = 0; i < 4; i++)
		{
			V2 dir = dirs[i];

			V2 newPos = M.Pos + dir * speed;

			if (!PositionValide(G, newPos, M.Size))
				continue;

			float dx = cible.x - newPos.x;
			float dy = cible.y - newPos.y;

			float dist = dx * dx + dy * dy;

			if (dist > bestDist)
			{
				bestDist = dist;
				bestDir = dir;
			}
		}
	}
	else
	{

		float bestDist = 999999.f;

		for (int i = 0; i < 4; i++)
		{
			V2 dir = dirs[i];

			if (dir.x == -M.Dir.x && dir.y == -M.Dir.y)
				continue;

			V2 newPos = M.Pos + dir * speed;

			if (!PositionValide(G, newPos, M.Size))
				continue;

			float dx = cible.x - newPos.x;
			float dy = cible.y - newPos.y;

			float dist = dx * dx + dy * dy;

			if (dist < bestDist)
			{
				bestDist = dist;
				bestDir = dir;
			}
		}
	}

	M.Dir = bestDir;
	M.Pos = M.Pos + M.Dir * speed;
}

void UpdateFantomes(GameData& G)
{
    for (int i = 0; i < NB_FANTOMES; i++){
		DeplacementFantomes(G, G.Fantome[i], static_cast<TypeFantome>(i));
	}
}


//Pieces
bool PlusDePieces(const GameData& G)
{
  for (int x = 0; x < NB_COLONNES; x++)
	for (int y = 0; y < NB_LIGNES; y++)
			if (G.Pieces[x][y])
				return false;

	return true;
}

Ecran gestion_ecran_accueil(GameData& G)
{
    if (G2D::keyHasBeenHit(Key::ENTER))
		return Ecran::InitPartie;
	return Ecran::Accueil;
}

Ecran InitPartie(GameData& G)
{
	srand((unsigned int)time(nullptr)); 

	// Héros
	G.Heros.Pos = V2(45, 45);
	G.Heros.NbVies = 3;
	G.Heros.PiecesOr = 0;


	//Pieces
	G.Pieces.resize(NB_COLONNES, vector<bool>(NB_LIGNES, false));

    for (int x = 0; x < NB_COLONNES; x++)
	{
		for (int y = 0; y < NB_LIGNES; y++)
		{
			if (!G.Mur(x, y))
				G.Pieces[x][y] = true;
		}
	}


	// Bonus invincibilité
    G.Bonus.resize(NB_COLONNES, vector<bool>(NB_LIGNES, false));

	int placed = 0;

	while (placed < 4)
	{
        int x = rand() % NB_COLONNES;
		int y = rand() % NB_LIGNES;

		if (!G.Mur(x, y) && !G.Bonus[x][y])
		{
			G.Bonus[x][y] = true;
			placed++;
		}
	}

	G.Invincible = false;

	// Directions & positions initiales des fantômes
	for (int i = 0; i < NB_FANTOMES; i++)
	{
		G.Fantome[i].Pos = G.FantomeStartPos[i];
		G.Fantome[i].Dir = G.FantomeStartDir[i];
	}

	G.FrameCount = 0;
	G.TimeEcran = 0.0;

    return Ecran::Jeu;
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

void GestionBonus(GameData& G)
{
	int x = G.Heros.Pos.x / G.Lpix;
	int y = G.Heros.Pos.y / G.Lpix;

	if (G.Bonus[x][y])
	{
		G.Bonus[x][y] = false;

		G.Invincible = true;
		G.TimeInvincible = G2D::elapsedTimeFromStartSeconds();
	}
}

Ecran gestion_jeu(GameData& G)
{
	GestionDeplacementHeros(G);
	GestionPieces(G);
	GestionCollisionFantomes(G);
    GestionBonus(G);
	UpdateFantomes(G);



	G.FrameCount++;

	double now = G2D::elapsedTimeFromStartSeconds();

	if (G.Invincible && now - G.TimeInvincible > 10.0)
	{
		G.Invincible = false;
	}

	
	//Victoire
    if (PlusDePieces(G))
		return Ecran::Win;

	//Defaite
    if (G.Heros.NbVies <= 0)  return Ecran::GameOver;

	return Ecran::Jeu;
}

Ecran gestion_GameOver(GameData& G)
{
	double tempsActuel = G2D::elapsedTimeFromStartSeconds();
	if (G.TimeEcran == 0.0)
		G.TimeEcran = tempsActuel;

	if (tempsActuel - G.TimeEcran >= 3.0)
	{
		G.TimeEcran = 0.0;
        return Ecran::Accueil;
	}
    return Ecran::GameOver;
}

Ecran gestion_Win(GameData& G)
{
	double tempsActuel = G2D::elapsedTimeFromStartSeconds();
	if (G.TimeEcran == 0.0) G.TimeEcran = tempsActuel;

	if (tempsActuel - G.TimeEcran >= 3.0)
	{
		G.TimeEcran = 0.0;
        return Ecran::Accueil;
	}
    return Ecran::Win;
}


void Render(const GameData& G)
{
	G2D::clearScreen(Color::Black);
	int largeurFenetre = G.Lpix * NB_COLONNES;
	int hauteurFenetre = G.Lpix * NB_LIGNES;  

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
    if (G.EcranActuel == Ecran::Accueil)

	{	// Fonds des rectangles
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
  if (G.EcranActuel == Ecran::Jeu)
	{
		// Labyrinthe
    for (int x = 0; x < NB_COLONNES; x++)
	{
		for (int y = 0; y < NB_LIGNES; y++)
			{
				if (G.Mur(x, y))
				{
					G2D::drawRectangle(V2(x * G.Lpix, y * G.Lpix),V2(G.Lpix, G.Lpix), Color::Cyan);
				}

				// Pieces
				if (G.Pieces[x][y])
				{
					G2D::drawCircle(V2(x * G.Lpix + G.Lpix / 2, y * G.Lpix + G.Lpix / 2), 3,Color::Yellow, true);
				}

                // Bonus invincibilité
				if (G.Bonus[x][y])
				{
					G2D::drawCircle(V2(x * G.Lpix + G.Lpix / 2, y * G.Lpix + G.Lpix / 2), 6, Color::Green, true);
				}
			}
		}

        // Fantomes
		for (int i = 0; i < NB_FANTOMES; i++){
			G2D::drawRectWithTexture(G.Fantome[i].IdTex,G.Fantome[i].Pos, G.Fantome[i].Size);
		}

		// Héros - animation marche
		int texHeros = (G.FrameCount / 30) % 2 == 0
			? G.Heros.IdTex
			: G.Heros.IdTex2;
		G2D::drawRectWithTexture(texHeros, G.Heros.Pos, G.Heros.Size);

        // Points de vies
		G2D::drawStringFontMono(V2(10, G.Lpix * NB_LIGNES + 25), "HP :", 18, 2, Color::White);
		for (int i = 0; i < G.Heros.NbVies; i++)
			G2D::drawRectangle(V2(70 + i * (G.Lpix + 5), G.Lpix * NB_LIGNES + 20),
			V2(G.Lpix * 0.5f, G.Lpix * 0.5f), Color::Red, true);

		//Chrono invincibilité
		if (G.Invincible)
		{
			double now = G2D::elapsedTimeFromStartSeconds();
			double remaining = 10.0 - (now - G.TimeInvincible);

			if (remaining < 0) remaining = 0;
            string txt = "INVINCIBLE: " + to_string((int)remaining) + "s";
			G2D::drawStringFontMono(V2(200, G.Lpix * NB_LIGNES + 25), txt, 18, 2, Color:: Green);
		}

		G2D::Show();
		return;
	}

    // ECRAN GAME OVER
 if (G.EcranActuel == Ecran::GameOver)
	{
		G2D::drawRectangle(V2(x, y), V2(largeurRect, hauteurRect), Color::Red, true);
		G2D::drawStringFontMono(V2(x + largeurRect / 2 - 100, y + hauteurRect / 2 - 15), "GAME OVER", 30, 3, Color::White);
		G2D::Show();
		return;
	}

    // ECRAN WIN
  if (G.EcranActuel == Ecran::Win)
	{
		G2D::drawRectangle(V2(x, y), V2(largeurRect, hauteurRect), Color::Green, true);
		G2D::drawStringFontMono(V2(x + largeurRect / 2 - 100, y + hauteurRect / 2 - 15), "YOU WIN !", 30, 3, Color::White);
		G2D::Show();
		return;
	}
}


void Logic(GameData& G)
{
    if (G.EcranActuel == Ecran::Accueil)  G.EcranActuel = gestion_ecran_accueil(G);
	if (G.EcranActuel == Ecran::InitPartie)    G.EcranActuel = InitPartie(G);
	if (G.EcranActuel == Ecran::Jeu)      G.EcranActuel = gestion_jeu(G);
	if (G.EcranActuel == Ecran::GameOver) G.EcranActuel = gestion_GameOver(G);
	if (G.EcranActuel == Ecran::Win)      G.EcranActuel = gestion_Win(G);
}


void AssetsInit(GameData& G)
{
	// Size passé en ref et texture en param
	G.Heros.IdTex = G2D::initTextureFromString(G.Heros.Size, G.Heros.texture);
	G.Heros.Size = V2(G.Lpix * 0.8f, G.Lpix * 0.8f);


	V2 size2;
	G.Heros.IdTex2 = G2D::initTextureFromString(size2, G.Heros.texture2);


    for (int i = 0; i < NB_FANTOMES; i++)
	{
		G.Fantome[i].IdTex = G2D::initTextureFromString(G.Fantome[i].Size, G.Fantome[i].texture[i]);
		G.Fantome[i].Size = V2(G.Lpix * 0.8f, G.Lpix * 0.8f);
	}

}

int main(int argc, char* argv[])
{
	GameData G;

    G2D::initWindow(V2(G.Lpix * NB_COLONNES, G.Lpix * NB_LIGNES + 70), V2(200, 200), string("Labyrinthe"));

	AssetsInit(G);

	int nbCalltoLogicPerSec = 50;

	G2D::Run(Logic, Render, G, nbCalltoLogicPerSec, true);

}