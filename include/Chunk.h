#ifndef CHUNK_H
#define CHUNK_H

#include "utils.h"
#include "Block.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Mesh.h"

class Player;
class World;
class Game;
class Entity;

// Classe définissant un chunk

class Chunk
{
public:

	static constexpr uint8_t	size = 16;					// Largeur d'un chunk
	static constexpr uint8_t	height = 128;				// Hauteur d'un chunk
	static constexpr uint16_t	max_distance = 20 * size;	// Distance maximum des chunks chargés

	typedef std::array<std::array<std::array<Block, size>, height>, size> BlockArray;

	// Tentative de faire fonctionner les arbres en bordure de chunk :(

	/*
	// Structure représentant une liste de feuilles à ajouter

	struct LeavesToAdd
	{
		glm::ivec3					chunk;	// Chunk où se trouve les feuilles
		std::vector<glm::ivec3>		leaves;	// Feuilles à ajouter

									LeavesToAdd();
									LeavesToAdd(const glm::ivec3& chunk_position);
									LeavesToAdd(const LeavesToAdd& other);

		LeavesToAdd&				operator=(const LeavesToAdd& other);
	};
	*/

private:

	World*						world;						// Pointeur vers le monde
	glm::ivec3					position;					// Position du chunk
	BlockArray					blocks;						// Liste des blocs du chunk
	uint8_t						local_layer_min;			// Couche la plus basse
	uint8_t						local_layer_max;			// Couche la plus haute
	uint8_t						layer_min;					// Couche la plus basse
	uint8_t						layer_max;					// Couche la plus haute
	bool						as_updated;					// A mis à jour ses vertices
	bool						as_sent;					// A envoyé ses vertices au shader
	std::list<Chunk*>			chunks_around;				// Chunks autour
	Mesh						mesh;						// Géométrie
	VertexBuffer				object;						// Vertex Buffer OpenGl
	bool						as_water;					// Comporte de l'eau

	static VertexBuffer			water_top;					// Surface de l'eau vu d'au dessus
	static VertexBuffer			water_bottom;				// Surface de l'eau vu d'en dessous
	static VertexBuffer			limits;						// Limites des chunks
	//static std::vector<LeavesToAdd>	leaves_to_add;

public:

								Chunk();
								Chunk(const Chunk& other);
								Chunk(const glm::ivec3& position, World* world);

	Chunk&						operator=(const Chunk& other);
	Block&						operator[](const glm::ivec3& block_pos);

	float						noise(int8_t x, int8_t z, float size, float height, float shift = 0.f);
	void						generate_blocks();
	bool						add_tree(int8_t x, uint8_t y, int8_t z);
	void						add_leave(int8_t x, uint8_t y, int8_t z, bool update_block = false);
	void						add_plant(int8_t x, uint8_t y, int8_t z);
	Block*						find_block(const glm::ivec3& block_pos);
	void						update_all();
	void						update_edges();
	void						update_face(uint8_t face_id);
	void						generate_mesh();
	void						send_mesh();
	bool						is_visible(const Camera& camera) const;
	void						draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane = Plane(0.f, 1.f, 0.f, 10000.f)) const;
	void						draw_water(const Camera& camera, const std::vector<const Light*>& lights) const;

	friend						World;
	friend						Block;
	friend						Player;
	friend						Entity;
};

#endif