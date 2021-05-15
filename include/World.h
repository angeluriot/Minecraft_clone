#ifndef WORLD_H
#define WORLD_H

#include "utils.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Shader.h"

constexpr uint8_t	average_stone_height	= 45;		// Hauteur moyenne de la pierre
constexpr uint8_t	above_stone_height		= 3;		// Hauteur moyenne de la terre
constexpr uint8_t	sand_dirt_limit			= 32;		// Limite entre le sable et l'herbe
constexpr uint8_t	dirt_stone_limit		= 60;		// Limite entre l'herbe et la montagne
constexpr uint8_t	stone_snow_limit		= 80;		// Limite entre la pierre et la neige
constexpr float		water_level				= 31.2f;	// Hauteur de l'eau
constexpr uint8_t	tree_height				= 7;		// Hauteur des arbres
constexpr float		gravity					= 30.f;		// Force de la gravité
constexpr uint16_t	nb_max_lights			= 10;		// Nombre maximum de lumières
constexpr uint16_t	nb_max_mobs				= 20;		// Nombre maximum de mobs

class Player;
class Block;
class Chunk;
class Mob;

// Classe représentant le monde

class World
{
private:

	std::list<Chunk*>		chunks;				// Liste des chunks chargés
	std::list<glm::ivec3>	future_chunks_pos;	// Liste des chunks autours des chunks chargés
	std::list<Mob*>			mobs;				// Liste des mobs chargées

public:

	uint16_t				seed;				// Seed de la map

							World();
							World(const World& other);
							~World();

	World&					operator=(const World& other);
	Block&					operator[](const glm::ivec3& block_pos) const;

	void					add_chunk(const glm::ivec3& chunk_pos);
	void					remove_chunk(Chunk* chunk);
	glm::ivec3				block_to_chunk(const glm::ivec3& block_pos) const;
	Chunk*					find_chunk(const glm::ivec3& chunk_pos) const;
	uint8_t					nb_chunks_around(const glm::ivec3& chunk_pos) const;
	float					get_distance(const glm::vec3& player_pos, const glm::ivec3& chunk_pos) const;
	Block*					get_selected_block(const Player& player);
	void					init(const int64_t& seed);
	void					generate(const glm::vec3& player_pos);
	void					generate_meshes();
	void					send_meshes();
	glm::vec3				get_spawn_position() const;
	void					update_mobs(const glm::vec3& player_pos);
	void					draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane = Plane(0.f, 1.f, 0.f, 10000)) const;
	void					draw_water(const Camera& camera, const std::vector<const Light*>& lights) const;
	void					draw_mobs(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane = Plane(0.f, 1.f, 0.f, 10000)) const;
	void					draw_debug(const Camera& camera) const;
};

#endif