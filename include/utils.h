#ifndef UTILS_H
#define UTILS_H

#include "libraries.h"

constexpr float						pi					= 3.14159265359f;			// Pi
constexpr float						framerate			= 60.f;						// Nombre max d'images par seconde
constexpr float						frame_duration		= 1.f / framerate;			// Dur�e min d'une frame
constexpr float						frame_duration_ms	= 1.f / framerate * 1e3;	// Dur�e min d'une frame en seconde
constexpr float						epsilon				= 0.01f;					// Petite valeur
constexpr float						epsilon_2			= 0.005f;					// Toute petite valeur

constexpr std::array<glm::ivec3, 4>	around_positions	= { glm::ivec3(-1, 0, 0), glm::ivec3(1, 0, 0), glm::ivec3(0, 0, -1), glm::ivec3(0, 0, 1) }; // Positions autour d'une position

typedef glm::vec4 Color;	// Type d�finissant une couleur
typedef glm::vec3 ColorRGB;	// Type d�finissant une couleur sans l'opacit�
typedef glm::vec4 Plane;	// Type d�finissant un plan

extern std::mutex lock;

struct GameWindow
{
	static SDL_Window*		window;		// Fen�tre
	static SDL_GLContext	context;	// Contexte
	static glm::ivec2		size;		// Taille de la fen�tre
	static glm::ivec2		center;		// Centre de la fen�tre

	static bool				init();
	static void				clear();
};

int32_t		random_int(int32_t min, int32_t max);
bool		rand_probability(float probability);
float		get_horizontal_norm(const glm::vec3& vector);
void		set_horizontal_norm(glm::vec3& vector, float value);
glm::vec3	operator*(const glm::mat4& matrix, const glm::vec3& vector);
float		normalize(float number);
float		ratio(float number, float min, float max);
float		smooth_clamp(float value, float min, float max, float smoothness);

#endif