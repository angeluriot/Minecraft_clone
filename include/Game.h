#ifndef GAME_H
#define GAME_H

#include "utils.h"
#include "Shader.h"
#include "Block.h"
#include "Chunk.h"
#include "utils.h"
#include "Camera.h"
#include "World.h"
#include "Player.h"
#include "Luminary.h"
#include "Sun.h"
#include "Moon.h"
#include "Sky.h"
#include "Texture.h"
#include "Shader.h"
#include "FrameBuffer.h"

constexpr float reflection_quality = 0.5f;	// Qualit� des reflets (de 0 � 1)

// Classe d�finissant le jeu

class Game
{
public:

	static World					world;			// Le monde
	static Sun						sun;			// Le soleil
	static Moon						moon;			// La lune
	static Sky						sky;			// Le ciel
	static std::stack<glm::mat4>	matrices;		// La pile de matrices
	static Player					player;			// Le joueur
	static Camera					fixed_cam;		// Cam�ra fix�e par le joueur
	static bool						fix_cam;		// La cam�ra est fix�e
	static bool						fake_cam;		// Une cam�ra virtuelle est utilis�e
	static bool						in_water;		// La cam�ra est sous l'eau
	static bool						stop_moving;	// Arr�te de bouger la cam�ra
	static bool						debug;			// Affiche les �lements de debug
	static float					time;			// Temps �coul�

	static void						init();
	static void						checks_events(const SDL_Event& my_event);
	static void						update(glm::ivec2& mouse_pos);
	static void						draw();
};

#endif