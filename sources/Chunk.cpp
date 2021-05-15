#include "Chunk.h"
#include "Block.h"
#include "World.h"
#include "utils.h"
#include "Material.h"
#include "Game.h"

VertexBuffer Chunk::water_top;
VertexBuffer Chunk::water_bottom;
VertexBuffer Chunk::limits;

// Tentative de faire fonctionner les arbres en bordure de chunk :(

/*
std::vector<Chunk::LeavesToAdd>	Chunk::leaves_to_add = {};

// Crée une liste de feuilles à ajouter

Chunk::LeavesToAdd::LeavesToAdd()
{
	chunk = glm::ivec3();
	leaves.clear();
}

// Crée une liste de feuilles à ajouter à partir de la position du chunk

Chunk::LeavesToAdd::LeavesToAdd(const glm::ivec3& chunk_position)
{
	chunk = chunk_position;
	leaves.clear();
}

// Crée une liste de feuilles à ajouter à partir d'une autre

Chunk::LeavesToAdd::LeavesToAdd(const LeavesToAdd& other)
{
	*this = other;
}

// Opérateur égal

Chunk::LeavesToAdd& Chunk::LeavesToAdd::operator=(const LeavesToAdd& other)
{
	chunk = other.chunk;
	leaves = other.leaves;

	return *this;
}
*/

// Crée un chunk

Chunk::Chunk()
{
	position = glm::ivec3();
	world = NULL;
	local_layer_min = 0;
	local_layer_max = 0;
	layer_min = 0;
	layer_max = 0;
	as_updated = false;
	as_sent = false;
	chunks_around.clear();
	as_water = false;
}

// Crée un chunk à partir d'un autre

Chunk::Chunk(const Chunk& other)
{
	*this = other;
}

// Crée un chunk à partir de sa position

Chunk::Chunk(const glm::ivec3& position, World* world)
{
	object.set_shader(Shader::block);
	this->position = position;
	this->world = world;
	as_updated = false;
	as_sent = false;
	chunks_around.clear();
	as_water = false;

	local_layer_min = height;
	local_layer_max = 0;
	
	// Initie la position des blocs

	for (uint8_t x = 0; x < size; x++)
		for (uint8_t y = 0; y < height; y++)
			for (uint8_t z = 0; z < size; z++)
				blocks[x][y][z] = Block(Block::Type::Air, glm::ivec3(this->position.x + x, y, this->position.z + z), this);

	generate_blocks();
}

// Opérateur égal

Chunk& Chunk::operator=(const Chunk& other)
{
	blocks = other.blocks;
	position = other.position;
	world = other.world;
	local_layer_min = other.local_layer_min;
	local_layer_max = other.local_layer_max;
	layer_min = other.layer_min;
	layer_max = other.layer_max;
	as_updated = other.as_updated;
	as_sent = other.as_sent;
	chunks_around = other.chunks_around;
	mesh = other.mesh;
	object = other.object;
	as_water = other.as_water;

	return *this;
}

// Donne le bloc du chunk à une position donnée

Block& Chunk::operator[](const glm::ivec3& block_pos)
{
	Block* block = find_block(block_pos);

	if (block == NULL)
	{
		for (auto& chunk_around : chunks_around)
			if (chunk_around != NULL && chunk_around->find_block(block_pos) != NULL)
				return (*chunk_around)[block_pos];

		return Block::air;
	}

	return *block;
}

// Donne la valeur du bruit de Perlin en fonction de ces paramètres

float Chunk::noise(int8_t x, int8_t z, float size, float height, float shift)
{
	return glm::simplex(glm::vec2(float(world->seed + shift + position.x + x) / size, float(world->seed + shift + position.z + z) / size)) * height;
}

// Génération du paysage en utilisant le bruit de Perlin

void Chunk::generate_blocks()
{
	for (uint8_t x = 0; x < size; x++)
		for (uint8_t z = 0; z < size; z++)
		{
			// Fond marins

			float big_seabeds = pow(noise(x, z, 500.f, 1.f) / 2.f + 0.5f, 1.5f) * 35.f;
			float medium_seabeds = pow(noise(x, z, 200.f, 1.f) / 2.f + 0.5f, 1.3f) * 8.f;
			float small_seabeds = pow(noise(x, z, 70.f, 1.f) / 2.f + 0.5, 1.1f) * 4.f;

			float seabeds_noise = -(big_seabeds + medium_seabeds + small_seabeds);

			// Montagnes

			float mountains_mask = std::max(noise(x, z, 500.f, 1.f, -2000.f), 0.f);

			float big_mountains = pow(noise(x, z, 250.f, 1.f) / 2.f + 0.5f, 3.f) * 65.f;
			float medium_mountains = pow(noise(x, z, 70.f, 1.f) / 2.f + 0.5f, 2.f) * 25.f;
			float small_mountains = (noise(x, z, 40.f, 1.f) / 2.f + 0.5f) * 15.f;

			float mountains_noise = mountains_mask * (big_mountains + medium_mountains + small_mountains);

			// Détails

			float beaches_details_strength = std::max(smooth_clamp(1.f / abs((average_stone_height + seabeds_noise + above_stone_height) - water_level), -1.f, 1.f, 2.f), 0.f);
			float mountains_details_strength = std::max(smooth_clamp(average_stone_height + seabeds_noise + mountains_noise - water_level - 10.f, -1.f, 1.f, 10.f), 0.f);
			float details_strength = beaches_details_strength + mountains_details_strength;

			float big_details = noise(x, z, 50.f, 2.f);
			float medium_details = noise(x, z, 25.f, 1.5f);
			float small_details = noise(x, z, 10.f, 1.f);

			float details_noise = (0.5f + details_strength + mountains_details_strength * 5.f) * (big_details + medium_details + small_details);

			// Forêts

			float forest_noise = (noise(x, z, 500.f, 1.f, 2000.f) + noise(x, z, 150.f, 1.f, 2000.f)) / 40.f;

			// Limite sable - terre

			float big_sand_limit_noise = noise(x, z, 50.f, 1.f, 400.f);
			float small_sand_limit_noise = noise(x, z, 20.f, 0.5f, 400.f);
			float tiny_sand_limit_noise = noise(x, z, 10.f, 0.2f, 400.f);
			float sand_limit_noise = big_sand_limit_noise + small_sand_limit_noise + tiny_sand_limit_noise;

			// Limite terre - pierre

			float big_stone_limit_noise = noise(x, z, 50.f, 10.f, 800.f);
			float small_stone_limit_noise = noise(x, z, 20.f, 5.f, 800.f);
			float tiny_stone_limit_noise = noise(x, z, 10.f, 1.f, 800.f);
			float stone_limit_noise = big_stone_limit_noise + small_stone_limit_noise + tiny_stone_limit_noise;

			// Assemblage

			float stone_noise = seabeds_noise + mountains_noise + details_noise;

			uint8_t stone_height = std::clamp((int)average_stone_height + (int)round(stone_noise), 1, int(height - above_stone_height - 3));

			if (stone_height + above_stone_height < local_layer_min)
				local_layer_min = stone_height + above_stone_height;

			if (stone_height + above_stone_height > local_layer_max)
				local_layer_max = stone_height + above_stone_height;

			if (stone_height + above_stone_height < water_level)
				as_water = true;

			for (int8_t y = 0; y < stone_height; y++)
				blocks[x][y][z].set_type(Block::Type::Stone, false);

			// Ajout du sable, terre, neige, etc...

			for (uint8_t i = 0; i < above_stone_height; i++)
			{
				if (stone_height < sand_dirt_limit + round(sand_limit_noise))
					blocks[x][stone_height + i][z].set_type(Block::Type::Sand, false);

				else if (stone_height < dirt_stone_limit + round(stone_limit_noise))
					blocks[x][stone_height + i][z].set_type(i == above_stone_height - 1 ? Block::Type::Grass : Block::Type::Dirt, false);

				else if (stone_height < stone_snow_limit + round(stone_limit_noise))
					blocks[x][stone_height + i][z].set_type(Block::Type::Stone, false);

				else
					blocks[x][stone_height + i][z].set_type(Block::Type::Snow, false);
			}
			
			// Ajout des arbres

			if (blocks[x][stone_height][z].get_type() == Block::Type::Dirt && rand_probability(forest_noise) && add_tree(x, stone_height + above_stone_height, z))
			{			
				if (stone_height + above_stone_height + tree_height > local_layer_max)
					local_layer_max = stone_height + above_stone_height + tree_height;
			}

			// Ajout des plantes

			else if (blocks[x][stone_height][z].get_type() == Block::Type::Dirt && rand_probability(0.3f))
				add_plant(x, stone_height + above_stone_height, z);

			// Tentative de faire fonctionner les arbres en bordure de chunk :(

			/*
			// Ajout des feuilles des arbres des autres chunks
			
			auto leave_array = std::find_if(leaves_to_add.begin(), leaves_to_add.end(), [&](LeavesToAdd& i) -> bool { return i.chunk == position; });

			if (leave_array != leaves_to_add.end())
			{
				for (auto& leave : leave_array->leaves)
					add_leave(leave.x - position.x, leave.y - position.y, leave.z - position.z);

				leaves_to_add.erase(leave_array);
			}
			*/
		}

	local_layer_min = std::max(uint8_t(local_layer_min - 1), uint8_t(0));
	local_layer_max = std::min(uint8_t(local_layer_max + 1), height);

	layer_min = local_layer_min;
	layer_max = local_layer_max;
}

// Ajoute un arbre à l'emplacement donné

bool Chunk::add_tree(int8_t x, uint8_t y, int8_t z)
{
	// Empêche qu'il soit en bord de chunk ou à côté d'un autre

	if (x <= 2 || x >= size - 3 || z <= 2 || z >= size - 3 || y >= height - tree_height - 1)
		return false;

	for (int8_t i = -1; i <= 1; i++)
		for (int8_t j = -1; j <= 1; j++)
			if ((*world)[position + glm::ivec3(x, y + 2, z)].get_type() == Block::Type::Wood)
				return false;

	// Feuilles

	for (int8_t i = -2; i <= 2; i++)
		for (uint8_t j = 3; j <= 4; j++)
			for (int8_t k = -2; k <= 2; k++)
				add_leave(x + i, y + j, z + k);

	for (int8_t i = -1; i <= 1; i++)
		for (int8_t k = -1; k <= 1; k++)
			add_leave(x + i, y + 5, z + k);

	add_leave(x, y + 6, z);
	add_leave(x, y + 6, z + 1);
	add_leave(x, y + 6, z - 1);
	add_leave(x + 1, y + 6, z);
	add_leave(x - 1, y + 6, z);

	// Tronc

	blocks[x][y - 1][z].set_type(Block::Type::Dirt, false);
	blocks[x][y][z].set_type(Block::Type::Wood, false);
	blocks[x][y + 1][z].set_type(Block::Type::Wood, false);
	blocks[x][y + 2][z].set_type(Block::Type::Wood, false);
	blocks[x][y + 3][z].set_type(Block::Type::Wood, false);
	blocks[x][y + 4][z].set_type(Block::Type::Wood, false);

	return true;
}

// Ajoute un bloc de feuilles d'arbre à un endroit donné

void Chunk::add_leave(int8_t x, uint8_t y, int8_t z, bool update_block)
{
	if (blocks[x][y][z].get_type() == Block::Type::Air)
		blocks[x][y][z].set_type(Block::Type::Leaves, update_block);

	// Tentative de faire fonctionner les arbres en bordure de chunk :(

	/*
	else if (world->find_chunk(world->block_to_chunk(position + glm::ivec3(x, y, z))) != NULL)
	{
		glm::ivec3 chunk_pos = world->find_chunk(world->block_to_chunk(position + glm::ivec3(x, y, z)))->position;

		glm::ivec3 test = glm::ivec3(position.x + x - chunk_pos.x, position.y + y - chunk_pos.y, position.z + z - chunk_pos.z);

		(*world)[position + glm::ivec3(x, y, z)].chunk->add_leave(test.x, test.y, test.z, true);
	}

	else
	{
		glm::ivec3 chunk_pos = world->block_to_chunk(position + glm::ivec3(x, y, z));
		auto leave_array = std::find_if(leaves_to_add.begin(), leaves_to_add.end(), [&](LeavesToAdd& i) -> bool { return i.chunk == position; });

		if (leave_array == leaves_to_add.end())
		{
			leaves_to_add.push_back(LeavesToAdd(chunk_pos));
			leave_array = --leaves_to_add.end();
		}

		leave_array->leaves.push_back(glm::ivec3(position.x + x, position.y + y, position.z + z));
	}*/
}

// Ajoute une plente à un endroit donné

void Chunk::add_plant(int8_t x, uint8_t y, int8_t z)
{
	if (rand_probability(0.01f))
		blocks[x][y][z].set_type(Block::Type::Tulip, false);

	else if (rand_probability(0.01f))
		blocks[x][y][z].set_type(Block::Type::Orchid, false);

	else if (rand_probability(0.01f))
		blocks[x][y][z].set_type(Block::Type::Dandelion, false);

	else if (rand_probability(0.2f))
	{
		blocks[x][y][z].set_type(Block::Type::TallWeedBottom, false);
		blocks[x][y + 1][z].set_type(Block::Type::TallWeebTop, false);
	}

	else
		blocks[x][y][z].set_type(Block::Type::Weed, false);
}

// Cherche le bloc à une position donnée

Block* Chunk::find_block(const glm::ivec3& block_pos)
{
	auto location = block_pos - position;

	if (location.x >= 0 && location.y >= 0 && location.z >= 0 && location.x < size && location.y < height && location.z < size)
		return &(blocks[location.x][location.y][location.z]);

	return NULL;
}

// Met à jour tous les blocs du chunk et autour

void Chunk::update_all()
{
	// Centre

	for (uint8_t x = 1; x < size - 1; x++)
		for (uint8_t y = layer_min; y < layer_max; y++)
			for (uint8_t z = 1; z < size - 1; z++)
				blocks[x][y][z].update(false);

	// Côtés

	for (auto& rectangle : blocks)
		for (uint8_t y = layer_min; y < layer_max; y++)
		{
			rectangle[y].front().update(true, 0);
			rectangle[y].back().update(true, 1);
		}

	for (uint8_t y = layer_min; y < layer_max; y++)
		for (uint8_t z = 0; z < size; z++)
		{
			blocks.front()[y][z].update(true, 2);
			blocks.back()[y][z].update(true, 3);
		}
}

// Met à jour les bords du chunk

void Chunk::update_edges()
{
	for (auto& rectangle : blocks)
		for (uint8_t y = layer_min; y < layer_max; y++)
		{
			rectangle[y].front().update_face(0);
			rectangle[y].back().update_face(1);
		}

	for (uint8_t y = layer_min; y < layer_max; y++)
		for (uint8_t z = 0; z < size; z++)
		{
			blocks.front()[y][z].update_face(2);
			blocks.back()[y][z].update_face(3);
		}
}

// Met à jour un bord du chunk

void Chunk::update_face(uint8_t face_id)
{
	if (face_id == 0)
		for (uint8_t y = layer_min; y < layer_max; y++)
			for (uint8_t z = 0; z < size; z++)
				blocks.front()[y][z].update_face(2);

	else if (face_id == 1)
		for (uint8_t y = layer_min; y < layer_max; y++)
			for (uint8_t z = 0; z < size; z++)
				blocks.back()[y][z].update_face(3);

	else if (face_id == 2)
		for (auto& rectangle : blocks)
			for (uint8_t y = layer_min; y < layer_max; y++)
				rectangle[y].front().update_face(0);

	else
		for (auto& rectangle : blocks)
			for (uint8_t y = layer_min; y < layer_max; y++)
				rectangle[y].back().update_face(1);
}

// Génère les vertices OpenGL

void Chunk::generate_mesh()
{
	mesh.clear();

	for (auto& rectangle : blocks)
		for (uint8_t y = layer_min; y < layer_max; y++)
			for (auto& block : rectangle[y])
				block.draw(mesh);

	as_updated = true;
}

// Envoie les vertices au shader

void Chunk::send_mesh()
{
	object.send_data(mesh);
	as_sent = true;
}

// Dit si le chunk est dans le champ de vision

bool Chunk::is_visible(const Camera& camera) const
{
	glm::vec2 cam_pos = glm::vec2(camera.get_position().x - camera.get_direction().x * size, camera.get_position().z - camera.get_direction().z * size);
	glm::vec2 chunk_dir = glm::vec2(position.x + size / 2.f, position.z + size / 2.f) - cam_pos;
	glm::vec2 cam_dir = glm::vec2(camera.get_direction().x, camera.get_direction().z);

	return cos(glm::dot(glm::normalize(chunk_dir), glm::normalize(cam_dir))) < pi / 3.5f || glm::length(chunk_dir) < camera.get_position().y;
}

// Affiche le chunk

void Chunk::draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane) const
{
	object.bind();

	ColorRGB water_color = ColorRGB(0.f, 0.f, 0.f);

	for (uint16_t i = 0; i < std::min((int)lights.size(), (int)nb_max_lights); i++)
		water_color += ColorRGB(Material::water.get_color()) * ColorRGB(lights[i]->get_color()) * lights[i]->get_intensity();

	object.send_uniform("u_mvp", camera.get_matrix());
	object.send_uniform("u_camera", camera.get_position());
	object.send_uniform("u_water_level", water_level);
	object.send_uniform("u_water_color", water_color);
	object.send_uniform("u_ambient", Material::block.get_ambient());
	object.send_uniform("u_diffuse", Material::block.get_diffuse());
	object.send_uniform("u_fake_cam", (int)Game::fake_cam);
	object.send_uniform("u_clipping_plane", clipping_plane);
	object.send_texture("u_texture", 0);

	std::vector<int> light_types;
	std::vector<glm::vec3> light_vectors;
	std::vector<ColorRGB> light_colors;
	std::vector<float> light_intensities;

	for (uint16_t i = 0; i < std::min((int)lights.size(), (int)nb_max_lights); i++)
	{
		light_types.push_back((int)lights[i]->get_type());
		light_vectors.push_back(lights[i]->get_vector());
		light_colors.push_back(ColorRGB(lights[i]->get_color()));
		light_intensities.push_back(lights[i]->get_intensity());
	}

	object.send_uniform("u_light_types", light_types);
	object.send_uniform("u_light_vectors", light_vectors);
	object.send_uniform("u_light_colors", light_colors);
	object.send_uniform("u_light_intensities", light_intensities);
	object.send_uniform("u_nb_lights", std::min((int)lights.size(), (int)nb_max_lights));

	object.draw();
	
	VertexBuffer::unbind();
}

// Affiche l'eau du chunk

void Chunk::draw_water(const Camera& camera, const std::vector<const Light*>& lights) const
{
	if (as_water)
	{
		VertexBuffer& water = Game::in_water ? water_bottom : water_top;

		glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(position));
		water.send_uniform("u_mvp", camera.get_matrix() * model);
		water.send_uniform("u_model", model);

		water.draw();
	}
}