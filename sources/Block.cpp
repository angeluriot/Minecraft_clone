#include "Block.h"
#include "Chunk.h"
#include "World.h"

Block Block::air = Block(Block::Type::Air, glm::ivec3(), NULL);

// Crée un bloc

Block::Block()
{
	type = Type::Air;
	position = glm::ivec3(0, 0, 0);
	chunk = NULL;
	faces_shown.fill(false);
	faces_orientation.fill(0);
}

// Crée un bloc à partir d'un autre

Block::Block(const Block& other)
{
	*this = other;
}

// Crée un bloc à partir de ces propriétés

Block::Block(Type type, const glm::ivec3& position, Chunk* chunk)
{
	this->position = position;
	this->chunk = chunk;
	faces_shown.fill(false);
	faces_orientation.fill(0);

	set_type(type, false);
}

// Opérateur égal

Block& Block::operator=(const Block& other)
{
	type = other.type;
	position = other.position;
	chunk = other.chunk;
	faces_shown = other.faces_shown;
	faces_orientation = other.faces_orientation;

	return *this;
}

// Dit si le bloc est une plante

bool Block::is_plant() const
{
	return type == Type::Weed || type == Type::TallWeedBottom || type == Type::TallWeebTop || type == Type::Tulip || type == Type::Orchid || type == Type::Dandelion;
}

// Dit si le bloc en paramètre est transparent du point de vu du bloc courant

bool Block::is_transparent(const Block& block) const
{
	return block.type == Type::Air || block.is_plant() || (type != Type::Leaves && block.type == Type::Leaves);
}

// Dit si le bloc est au bord du chunk

bool Block::is_edge() const
{
	glm::ivec3 location = position - chunk->position;

	return (location.x == 0 || location.z == 0 || location.x == Chunk::size - 1 || location.z == Chunk::size - 1);
}

// Change le type du bloc

void Block::set_type(Type type, bool update_block)
{
	this->type = type;

	// Donne une orientation aléatoire de la face du dessus
	if (type == Type::Grass)
		faces_orientation[4] = random_int(0, 4);

	if (is_plant())
		faces_shown.fill(true);

	if (update_block)
	{
		if (position.y > chunk->layer_max)
			chunk->layer_max = std::min(uint16_t(position.y + 1), uint16_t(Chunk::height));

		if (position.y < chunk->layer_min)
			chunk->layer_min = std::max(uint16_t(position.y - 1), uint16_t(0));

		// Les plantes ne peuvent pas être en l'air
		if (type == Type::Air && (*chunk)[position + glm::ivec3(0, 1, 0)].is_plant())
			(*chunk)[position + glm::ivec3(0, 1, 0)].set_type(Type::Air, true);

		// Les grandes plantes se cassent en un coup
		if (type == Type::Air && (*chunk)[position + glm::ivec3(0, -1, 0)].get_type() == Type::TallWeedBottom)
			(*chunk)[position + glm::ivec3(0, -1, 0)].set_type(Type::Air, true);

		// Le sable tombe
		if (type == Type::Air && (*chunk)[position + glm::ivec3(0, 1, 0)].get_type() == Type::Sand)
		{
			set_type(Type::Sand, true);
			(*chunk)[position + glm::ivec3(0, 1, 0)].set_type(Type::Air, true);
		}

		update(true);
	}
}

// Met à jour une face d'un bloc

void Block::update_face(uint8_t face_id)
{
	if (chunk != NULL)
	{
		chunk->as_updated = false;
		chunk->as_sent = false;
		faces_shown[face_id] = false;

		if (type != Type::Air || is_plant())
			switch (face_id)
			{
			case 0:
				if (is_transparent((*chunk)[glm::ivec3(position.x, position.y, position.z - 1)]))
					faces_shown[0] = true;
				break;
			case 1:
				if (is_transparent((*chunk)[glm::ivec3(position.x, position.y, position.z + 1)]))
					faces_shown[1] = true;
				break;
			case 2:
				if (is_transparent((*chunk)[glm::ivec3(position.x - 1, position.y, position.z)]))
					faces_shown[2] = true;
				break;
			case 3:
				if (is_transparent((*chunk)[glm::ivec3(position.x + 1, position.y, position.z)]))
					faces_shown[3] = true;
				break;
			case 4:
				if (is_transparent((*chunk)[glm::ivec3(position.x, position.y + 1, position.z)]))
					faces_shown[4] = true;
				break;
			case 5:
				if (is_transparent((*chunk)[glm::ivec3(position.x, position.y - 1, position.z)]))
					faces_shown[5] = true;
				break;
			}
	}
}

// Met à jour les faces d'un bloc et possiblement les faces des cubes autours

void Block::update(bool update_around, uint8_t direction_id)
{
	if (chunk != NULL)
	{
		chunk->as_updated = false;
		chunk->as_sent = false;

		if (update_around)
		{
			// Met à jour toutes les faces autour

			if (direction_id > 5)
			{
				(*chunk)[glm::ivec3(position.x, position.y, position.z + 1)].update_face(0);
				(*chunk)[glm::ivec3(position.x, position.y, position.z - 1)].update_face(1);
				(*chunk)[glm::ivec3(position.x + 1, position.y, position.z)].update_face(2);
				(*chunk)[glm::ivec3(position.x - 1, position.y, position.z)].update_face(3);
				(*chunk)[glm::ivec3(position.x, position.y - 1, position.z)].update_face(4);
				(*chunk)[glm::ivec3(position.x, position.y + 1, position.z)].update_face(5);
			}

			// Met à jour la face autour choisie

			else
				switch (direction_id)
				{
				case 0:
					(*chunk)[glm::ivec3(position.x, position.y, position.z - 1)].update_face(1); break;
				case 1:
					(*chunk)[glm::ivec3(position.x, position.y, position.z + 1)].update_face(0); break;
				case 2:
					(*chunk)[glm::ivec3(position.x - 1, position.y, position.z)].update_face(3); break;
				case 3:
					(*chunk)[glm::ivec3(position.x + 1, position.y, position.z)].update_face(2); break;
				case 4:
					(*chunk)[glm::ivec3(position.x, position.y + 1, position.z)].update_face(5); break;
				case 5:
					(*chunk)[glm::ivec3(position.x, position.y - 1, position.z)].update_face(4); break;
				}
		}

		// Met à jour les faces du bloc

		faces_shown.fill(false);

		if (type != Type::Air || is_plant())
		{
			if (is_transparent((*chunk)[glm::ivec3(position.x, position.y, position.z - 1)]))
				faces_shown[0] = true;

			if (is_transparent((*chunk)[glm::ivec3(position.x, position.y, position.z + 1)]))
				faces_shown[1] = true;

			if (is_transparent((*chunk)[glm::ivec3(position.x - 1, position.y, position.z)]))
				faces_shown[2] = true;

			if (is_transparent((*chunk)[glm::ivec3(position.x + 1, position.y, position.z)]))
				faces_shown[3] = true;

			if (is_transparent((*chunk)[glm::ivec3(position.x, position.y + 1, position.z)]))
				faces_shown[4] = true;

			if (is_transparent((*chunk)[glm::ivec3(position.x, position.y - 1, position.z)]))
				faces_shown[5] = true;
		}
	}
}

// Ajoute les coordonnées de texture aux vertices du chunk

void Block::add_texcoords(uint8_t face_id, uint8_t x, uint8_t y, std::vector<float>& texcoords) const
{
	if (faces_orientation[face_id] == 0)
		for (uint8_t i = 0; i < 2 * Cube::Face::nb_points; i++)
			texcoords.push_back(Mesh::cube.get_texcoords()[face_id * 2 * Cube::Face::nb_points + i] * texture_ratio + (i % 2 == 0 ? x * texture_ratio : y * texture_ratio));

	else
		for (uint8_t i = 0; i < 2 * Cube::Face::nb_points; i++)
			texcoords.push_back(oriented_top_texcoord(faces_orientation[face_id])[i] * texture_ratio + (i % 2 == 0 ? x * texture_ratio : y * texture_ratio));
}

// Sélectionne les coordonnées de texture à envoyer en fonction du type de bloc

void Block::send_texcoords(uint8_t face_id, std::vector<float>& texcoords) const
{
	switch (type)
	{
	case Type::Grass:
		if (face_id == 4)
			add_texcoords(face_id, 0, 0, texcoords);

		else if (face_id == 5)
			add_texcoords(face_id, 2, 0, texcoords);

		else
			add_texcoords(face_id, 1, 0, texcoords);

		break;

	case Type::Dirt:
		add_texcoords(face_id, 2, 0, texcoords);
		break;

	case Type::Stone:
		add_texcoords(face_id, 3, 0, texcoords);
		break;

	case Type::Sand:
		add_texcoords(face_id, 4, 0, texcoords);
		break;

	case Type::Wood:
		if (face_id == 4 || face_id == 5)
			add_texcoords(face_id, 6, 0, texcoords);

		else
			add_texcoords(face_id, 5, 0, texcoords);
		break;

	case Type::Leaves:
		add_texcoords(face_id, 7, 0, texcoords);
		break;

	case Type::Snow:
		add_texcoords(face_id, 0, 1, texcoords);
		break;

	default:
		break;
	}
}

// Donne un pointeur vers le chunk du bloc

const Chunk& Block::get_chunk() const
{
	return *chunk;
}

// Donne son type

Block::Type Block::get_type() const
{
	return type;
}

// Donne sa position

glm::ivec3 Block::get_position() const
{
	return position;
}

// Ajoute ses vertices dans les vertices du chunk

void Block::draw(Mesh& mesh) const
{
	if (is_plant())
	{
		draw_plant(mesh);
		return;
	}

	if (type != Type::Air)
		for (uint8_t i = 0; i < Cube::nb_faces; i++)
			if (faces_shown[i])
			{
				for (uint8_t j = 0; j < 3 * Cube::Face::nb_points; j++)
				{
					if (j % 3 == 0)
					{
						mesh.positions.push_back(position.x + Mesh::cube.get_positions()[i * 3 * Cube::Face::nb_points + j]);
						mesh.normals.push_back(Mesh::cube.get_normals()[i * 3 * Cube::Face::nb_points + j]);
					}
				
					else if (j % 3 == 1)
					{
						mesh.positions.push_back(position.y + Mesh::cube.get_positions()[i * 3 * Cube::Face::nb_points + j]);
						mesh.normals.push_back(Mesh::cube.get_normals()[i * 3 * Cube::Face::nb_points + j]);
					}

					else
					{
						mesh.positions.push_back(position.z + Mesh::cube.get_positions()[i * 3 * Cube::Face::nb_points + j]);
						mesh.normals.push_back(Mesh::cube.get_normals()[i * 3 * Cube::Face::nb_points + j]);
					}
				}

				send_texcoords(i, mesh.texcoords);
			}
}

// Affiche le bloc si c'est une plante

void Block::draw_plant(Mesh& mesh) const
{
	Mesh plant_1 = (glm::translate(glm::mat4(1.f), glm::vec3(position)) * glm::rotate(glm::mat4(1.f), pi / 4.f, glm::vec3(0.f, 1.f, 0.f))) * Mesh::square;
	Mesh plant_2 = (glm::translate(glm::mat4(1.f), glm::vec3(position)) * glm::rotate(glm::mat4(1.f), -pi / 4.f, glm::vec3(0.f, 1.f, 0.f))) * Mesh::square;
	Mesh plant_3 = (glm::translate(glm::mat4(1.f), glm::vec3(position)) * glm::rotate(glm::mat4(1.f), (3.f * pi) / 4.f, glm::vec3(0.f, 1.f, 0.f))) * Mesh::square;
	Mesh plant_4 = (glm::translate(glm::mat4(1.f), glm::vec3(position)) * glm::rotate(glm::mat4(1.f), (-3.f * pi) / 4.f, glm::vec3(0.f, 1.f, 0.f))) * Mesh::square;

	glm::ivec2 texture_position(0, 0);

	switch (type)
	{
	case Type::Weed:
		texture_position = glm::ivec2(1, 1);
		break;

	case Type::TallWeedBottom:
		texture_position = glm::ivec2(2, 1);
		break;

	case Type::TallWeebTop:
		texture_position = glm::ivec2(3, 1);
		break;

	case Type::Tulip:
		texture_position = glm::ivec2(4, 1);
		break;

	case Type::Orchid:
		texture_position = glm::ivec2(5, 1);
		break;

	case Type::Dandelion:
		texture_position = glm::ivec2(6, 1);
		break;

	default:
		break;
	}
	
	for (uint8_t i = 0; i < plant_1.texcoords.size(); i += 2)
	{
		plant_1.texcoords[i] = (plant_1.texcoords[i] + texture_position.x) * texture_ratio;
		plant_1.texcoords[i + 1] = (plant_1.texcoords[i + 1] + texture_position.y) * texture_ratio;
	}
	
	for (uint8_t i = 0; i < plant_1.normals.size(); i += 3)
	{
		plant_1.normals[i] = 0.f;
		plant_1.normals[i + 1] = 1.f;
		plant_1.normals[i + 2] = 0.f;
	}

	plant_2.texcoords = plant_1.texcoords;
	plant_3.texcoords = plant_1.texcoords;
	plant_4.texcoords = plant_1.texcoords;
	
	plant_2.normals = plant_1.normals;
	plant_3.normals = plant_1.normals;
	plant_4.normals = plant_1.normals;
	
	mesh += plant_1;
	mesh += plant_2;
	mesh += plant_3;
	mesh += plant_4;
}
