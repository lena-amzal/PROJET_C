#pragma warning( disable : 4996 )
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
#include "glut.h"
#include <windows.h>

// map dimensions used in main
constexpr int MAP_COLS = 20;
constexpr int MAP_ROWS = 15;

// fullscreen toggle state
static int g_savedX = 200;
static int g_savedY = 200;
static int g_savedW = 800;
static int g_savedH = 600;
static bool g_isFullScreen = false;

// special key handler for toggling fullscreen (F11)
void OnSpecialKey(int key, int x, int y)
{
	if (key == GLUT_KEY_F11)
	{
		if (!g_isFullScreen)
		{
			// save current window geometry
			g_savedW = glutGet(GLUT_WINDOW_WIDTH);
			g_savedH = glutGet(GLUT_WINDOW_HEIGHT);
			g_savedX = glutGet(GLUT_WINDOW_X);
			g_savedY = glutGet(GLUT_WINDOW_Y);
			glutFullScreen();
			g_isFullScreen = true;
		}
		else
		{
			// restore
			glutReshapeWindow(g_savedW, g_savedH);
			glutPositionWindow(g_savedX, g_savedY);
			g_isFullScreen = false;
		}
	}
}

// simple keyboard handler (escape to quit)
void OnKeyboard(unsigned char key, int x, int y)
{
	if (key == 27) // ESC
		exit(0);
}

// one-shot timer to request fullscreen after main loop starts
void RequestFullScreenTimer(int value)
{
	// Try a robust fullscreen: resize/position window to the screen size.
	int sw = glutGet(GLUT_SCREEN_WIDTH);
	int sh = glutGet(GLUT_SCREEN_HEIGHT);
	if (sw > 0 && sh > 0)
	{
		// save current window geometry if not saved
		g_savedW = glutGet(GLUT_WINDOW_WIDTH);
		g_savedH = glutGet(GLUT_WINDOW_HEIGHT);
		g_savedX = glutGet(GLUT_WINDOW_X);
		g_savedY = glutGet(GLUT_WINDOW_Y);

		glutPositionWindow(0, 0);
		glutReshapeWindow(sw, sh);
		// fallback to GLUT fullscreen hint as well
		glutFullScreen();
		g_isFullScreen = true;
	}
	else
	{
		// fallback
		glutFullScreen();
		g_isFullScreen = true;
	}
}
 
 
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
    int IdTex = -1;
	Color Col = Color::White;
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

	bool IsFullScreen = false;

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
		 
    // Affichage des momies (fantomes) si positions valides
	// (ici on utilise Momie.PosPixels)
	if (G.Momie1.PosPixels.x >= 0)
		G2D::drawRectangle(G.Momie1.PosPixels, G.Momie1.Size, G.Momie1.Col, true);
	if (G.Momie2.PosPixels.x >= 0)
		G2D::drawRectangle(G.Momie2.PosPixels, G.Momie2.Size, G.Momie2.Col, true);
	if (G.Momie3.PosPixels.x >= 0)
		G2D::drawRectangle(G.Momie3.PosPixels, G.Momie3.Size, G.Momie3.Col, true);

	// affichage du héros avec boite englobante et texture
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

	// Momies: init texture and random color
	V2 momieSize;
	int momieTex = G2D::initTextureFromString(momieSize, G.Momie1.texture);
	Color possibleColors[6] = { Color::Red, Color::Green, Color::Blue, Color::Yellow, Color::Magenta, Color::Cyan };
	for (int i = 0; i < 3; i++)
	{
        // use same texture id for all (if textures used later)
		G.Momie1.IdTex = momieTex;
		G.Momie2.IdTex = momieTex;
		G.Momie3.IdTex = momieTex;
		// assign a random color per momie by selecting from possibleColors
		Color c = possibleColors[rand() % 6];
		if (i == 0) G.Momie1.Col = c;
		if (i == 1) G.Momie2.Col = c;
		if (i == 2) G.Momie3.Col = c;
		// set sizes
		if (i == 0) G.Momie1.Size = V2(G.Lpix * 0.8f, G.Lpix * 0.8f);
		if (i == 1) G.Momie2.Size = V2(G.Lpix * 0.8f, G.Lpix * 0.8f);
		if (i == 2) G.Momie3.Size = V2(G.Lpix * 0.8f, G.Lpix * 0.8f);
	}

	// set starting positions so they are visible
	G.Momie1.PosPixels = V2(3 * G.Lpix, 3 * G.Lpix);
	G.Momie2.PosPixels = V2(10 * G.Lpix, 5 * G.Lpix);
	G.Momie3.PosPixels = V2(5 * G.Lpix, 10 * G.Lpix);

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

    // set window size to match new map: MAP_COLS x MAP_ROWS
    G2D::initWindow(V2(G.Lpix * MAP_COLS, G.Lpix * MAP_ROWS), V2(200,200), string("Labyrinthe"));
	// request fullscreen (GLUT): must be called after window creation
    // some GLUT implementations require the full-screen request to happen after
	// the main loop starts; register a short timer to call it once.
	glutTimerFunc(10, RequestFullScreenTimer, 0);
	// register handlers to allow toggling fullscreen with F11 and quitting with ESC
	glutKeyboardFunc(OnKeyboard);
	glutSpecialFunc(OnSpecialKey);
	  
    srand((unsigned int)time(nullptr));
	AssetsInit(G);

	int nbCalltoLogicPerSec = 50;

	G2D::Run(Logic,Render,G, nbCalltoLogicPerSec,true);
 
}
  