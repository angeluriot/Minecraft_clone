#ifndef BLOCK_H
#define BLOCK_H

#include "utils.h"
#include "Mesh.h"

class Chunk;
class World;

typedef std::array<bool, Cube::nb_faces>	FacesShown;			// 0:Back, 1:Front, 2:Left, 3:Right, 4:Top, 5:Bottom
typedef std::array<uint8_t, Cube::nb_faces>	FacesOrientation;	// 0:Back, 1:Front, 2:Left, 3:Right, 4:Top, 5:Bottom

// Classe définissant un bloc

class Block
{
public:

	// Enum définissant les types de blocs

	enum class Type { Air, Grass, Dirt, Stone, Sand, Wood, Leaves, Snow, Weed, TallWeedBottom, TallWeebTop, Tulip, Orchid, Dandelion };

private:

	Chunk*					chunk;												// Pointeur vers le chunk du bloc
	FacesShown				faces_shown;										// Les faces affichées

	Type					type;												// Type du bloc
	glm::ivec3				position;											// Position
	FacesOrientation		faces_orientation;									// Orientation des faces

	static constexpr float	total_texture_size = 128.f;							// Taille totale de la texture des blocs
	static constexpr float	texture_size = 16.f;								// Taille de la texture d'un bloc
	static constexpr float	texture_ratio = texture_size / total_texture_size;	// Taille de la texture d'un bloc sur la taille totale

public:

	static Block			air;												// Bloc d'air

							Block();
							Block(const Block& other);
							Block(Type type, const glm::ivec3& position, Chunk* chunk);

	Block&					operator=(const Block& other);

	bool					is_plant() const;
	bool					is_transparent(const Block& block) const;
	bool					is_edge() const;
	void					set_type(Type type, bool update_block);
	void					update_face(uint8_t face_id);
	void					update(bool update_around, uint8_t direction_id = 6);
	void					add_texcoords(uint8_t face_id, uint8_t x, uint8_t y, std::vector<float>& texcoords) const;
	void					send_texcoords(uint8_t face_id, std::vector<float>& texcoords) const;
	void					draw(Mesh& mesh) const;
	void					draw_plant(Mesh& mesh) const;

	const Chunk&			get_chunk() const;
	Type					get_type() const;
	glm::ivec3				get_position() const;

	friend					Chunk;
};

#endif
