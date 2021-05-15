#include "World.h"
#include "Chunk.h"
#include "Block.h"
#include "Texture.h"
#include "Game.h"
#include "Chicken.h"
#include "Fox.h"
#include "Wolf.h"

// Crée le monde

World::World()
{
	chunks.clear();
	future_chunks_pos.clear();
	mobs.clear();
	seed = 0;
}

// Crée le monde à partir d'un autre

World::World(const World& other)
{
	*this = other;
}

// Détruit le monde

World::~World()
{
	for (auto& chunk : chunks)
		delete chunk;

	for (auto& mob : mobs)
		delete mob;
}

// Opérateur égal

World& World::operator=(const World& other)
{
	chunks = other.chunks;
	future_chunks_pos = other.future_chunks_pos;
	mobs = other.mobs;
	seed = other.seed;

	return *this;
}

// Trouve le bloc à une position donnée

Block& World::operator[](const glm::ivec3& block_pos) const
{
	// Regarde si la position est valide
	if (block_pos.y < 0 || block_pos.y >= Chunk::height)
		return Block::air;

	// Cherche le chunk contenant le bloc
	Chunk* chunk = find_chunk(block_to_chunk(block_pos));

	// Si aucun chunk ne correspond la position n'est pas valide
	if (chunk == NULL)
		return Block::air;

	// Demande le bloc au chunk trouvé
	return (*chunk)[block_pos];
}

// Charge un chunk

void World::add_chunk(const glm::ivec3& chunk_pos)
{
	// Crée et ajoute le chunk
	lock.lock();
	chunks.push_back(new Chunk(chunk_pos, this));
	lock.unlock();

	// Supprime le futur chunk à cet emplacement

	auto it = std::find(future_chunks_pos.begin(), future_chunks_pos.end(), chunk_pos);

	if (it != future_chunks_pos.end())
		future_chunks_pos.erase(it);

	// Met à jour le monde pour chaque chunk autour

	for (uint8_t i = 0; i < around_positions.size(); i++)
	{
		glm::ivec3 pos = chunks.back()->position + (around_positions[i] * (int)Chunk::size);
		Chunk* chunk = find_chunk(pos);

		// Ajoute des futures chunks autour du chunk créé
		if (chunk == NULL && std::count(future_chunks_pos.begin(), future_chunks_pos.end(), pos) == 0)
			future_chunks_pos.push_back(pos);

		if (chunk != NULL)
		{
			// Met à jours les pointeurs vers les chunks autours du chunk et des chunks autours
			chunks.back()->chunks_around.push_back(chunk);
			chunk->chunks_around.push_back(chunks.back());
			
			// Met à jour ses limites de hauteur et celles des chunks autours

			if (chunks.back()->layer_min > chunk->local_layer_min)
				chunks.back()->layer_min = chunk->local_layer_min;

			if (chunks.back()->layer_max < chunk->local_layer_max)
				chunks.back()->layer_max = chunk->local_layer_max;

			if (chunk->layer_min > chunks.back()->local_layer_min)
				chunk->layer_min = chunks.back()->local_layer_min;

			if (chunk->layer_max < chunks.back()->local_layer_max)
				chunk->layer_max = chunks.back()->local_layer_max;
		}
	}

	// Met à jour le chunk	
	chunks.back()->update_all();

	// Met à jour les bords des chunks autours
	for (auto chunk_around : chunks.back()->chunks_around)
		if (chunk_around != NULL)
			chunk_around->update_edges();
}

// Décharge un chunk

void World::remove_chunk(Chunk* chunk)
{
	lock.lock();
	auto chunk_pos = chunk->position;
	auto chunks_around = chunk->chunks_around;

	// Supprime les futures chunks autour

	for (auto around : around_positions)
	{
		glm::ivec3 pos = chunk_pos + (around * (int)Chunk::size);
		auto future_chunk_pos = std::find(future_chunks_pos.begin(), future_chunks_pos.end(), pos);

		if (future_chunk_pos != future_chunks_pos.end() && nb_chunks_around(*future_chunk_pos) <= 1)
			future_chunks_pos.erase(future_chunk_pos);
	}

	// Supprime les pointeurs vers le chunk supprimé

	for (auto c : chunks_around)
		if (c != NULL)
		{
			auto c_chunks_around = c->chunks_around;

			for (auto chunk_around : c_chunks_around)
				if (chunk_around == chunk || chunk_around == NULL)
					c->chunks_around.remove(chunk_around);
		}

	// Enlève le chunk et rajoute un future chunk à la place
	future_chunks_pos.push_back(chunk_pos);
	chunks.remove(chunk);
	delete chunk;

	lock.unlock();

	// Met à jour les bords des chunks autour
	for (auto chunk_around : chunks_around)
		if (chunk_around != NULL)
			chunk_around->update_edges();
}

// Donne les coordonnées du chunk où se trouve le bloc

glm::ivec3 World::block_to_chunk(const glm::ivec3& block_pos) const
{
	glm::ivec3 result = glm::ivec3(0, 0, 0);

	if (block_pos.x >= 0)
		result.x = block_pos.x - (block_pos.x % Chunk::size);

	else
	{
		result.x = -((-block_pos.x) - ((-block_pos.x) % Chunk::size));

		if ((-block_pos.x) % Chunk::size != 0)
			result.x -= Chunk::size;
	}

	if (block_pos.z >= 0)
		result.z = block_pos.z - (block_pos.z % Chunk::size);

	else
	{
		result.z = -((-block_pos.z) - ((-block_pos.z) % Chunk::size));

		if ((-block_pos.z) % Chunk::size != 0)
			result.z -= Chunk::size;
	}

	return result;
}

// Trouve le chunk à la position donnée

Chunk* World::find_chunk(const glm::ivec3& chunk_pos) const
{
	for (auto& chunk : chunks)
		if (chunk->position == chunk_pos)
			return chunk;

	return NULL;
}

// Donne le nombre de chunks autour d'une position

uint8_t World::nb_chunks_around(const glm::ivec3& chunk_pos) const
{
	uint8_t i = 0;

	for (auto& chunk : chunks)
	{
		for (auto around : around_positions)
			if (chunk->position == chunk_pos + (around * (int)Chunk::size))
				i++;

		if (i == 4)
			break;
	}

	return i;
}

// Donner la distance entre le joueur et un chunk

float World::get_distance(const glm::vec3& player_pos, const glm::ivec3& chunk_pos) const
{
	return sqrt(pow(player_pos.x - chunk_pos.x, 2) + pow(player_pos.z - chunk_pos.z, 2));
}

// Donne le bloc fixé par le joueur

Block* World::get_selected_block(const Player& player)
{
	return NULL;
}

// Ajoute un chunk à l'emplacement du joueur

void World::init(const int64_t& seed)
{
	srand(seed);
	this->seed = rand();

	glm::mat4 water_scale = glm::scale(glm::mat4(1.f), glm::vec3(Chunk::size, Chunk::size, Chunk::size));
	glm::mat4 water_rotation = glm::rotate(glm::mat4(1.f), pi / 2.f, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 water_shift = glm::translate(glm::mat4(1.f), glm::vec3(Chunk::size / 2.f - 0.5f, water_level, Chunk::size / 2.f - 0.5f));

	glm::mat4 limits_scale = glm::scale(glm::mat4(1.f), glm::vec3(Chunk::size, 10000.f, Chunk::size));
	glm::mat4 limits_shift = glm::translate(glm::mat4(1.f), glm::vec3(Chunk::size / 2.f - 0.5f, 0.f, Chunk::size / 2.f - 0.5f));

	Chunk::water_top.send_data(Shader::water, (water_shift * water_rotation * water_scale) * Mesh::square);
	Chunk::water_bottom.send_data(Shader::in_water, (water_shift * water_rotation * water_scale) * Mesh::square);
	Chunk::limits.send_data(Shader::debug, (limits_shift * limits_scale) * Mesh::empty_cube, DataType::Positions);

	add_chunk(glm::ivec3(0, 0, 0));
}

// Génère les chunks autour des chunks chargés

void World::generate(const glm::vec3& player_pos)
{
	bool finish = false;

	// Décharge les chunks trop éloignés du joueur

	for (auto& chunk : chunks)
		if (chunk != NULL && get_distance(player_pos, chunk->position) > Chunk::max_distance * 1.3f)
		{
			remove_chunk(chunk);
			finish = true;
			break;
		}

	// Charge les chunks trop proches du joueur

	if (!finish)
	{
		auto temp = future_chunks_pos;

		for (auto future_chunk_pos : temp)
			if (get_distance(player_pos, future_chunk_pos) < Chunk::max_distance)
				add_chunk(block_to_chunk(future_chunk_pos));
	}

	generate_meshes();
}

// Génère les vertices OpenGL

void World::generate_meshes()
{
	for (auto& chunk : chunks)
		if (!chunk->as_updated)
			chunk->generate_mesh();
}

// Envoie les vertices au shader

void World::send_meshes()
{
	for (auto& chunk : chunks)
		if (!chunk->as_sent && chunk->as_updated)
			chunk->send_mesh();
}

// Donne la position où apparait le joueur

glm::vec3 World::get_spawn_position() const
{
	if (chunks.size() == 0)
		return glm::vec3(Chunk::size / 2.f, Chunk::height + 1, Chunk::size / 2.f);

	return glm::vec3(chunks.front()->position.x + Chunk::size / 2.f, chunks.front()->local_layer_max + 1, chunks.front()->position.z + Chunk::size / 2.f);
}

// Gère les mobs du monde

void World::update_mobs(const glm::vec3& player_pos)
{
	// Ajout des mobs
	
	if (mobs.size() < nb_max_mobs && rand_probability(0.1f) && chunks.back()->local_layer_min > water_level)
	{
		glm::vec3 mob_pos = glm::vec3(random_float(player_pos.x - Chunk::max_distance / 2.f, player_pos.x + Chunk::max_distance / 2.f),
			0.f, random_float(player_pos.z - Chunk::max_distance / 2.f, player_pos.z + Chunk::max_distance / 2.f));

		Chunk* chunk = find_chunk(block_to_chunk(glm::ivec3(mob_pos)));

		if (chunk != NULL && glm::distance(glm::vec2(player_pos.x, player_pos.z), glm::vec2(mob_pos.x, mob_pos.z)) < Chunk::max_distance / 2.f
			&& chunk->local_layer_min > water_level)
		{
			if (rand_probability(1.f / 3.f))
				mobs.push_back(new Chicken(glm::vec3(mob_pos.x, chunk->local_layer_max + 1, mob_pos.z)));

			else if (rand_probability(1.f / 3.f))
				mobs.push_back(new Fox(glm::vec3(mob_pos.x, chunk->local_layer_max + 1, mob_pos.z)));

			else
				mobs.push_back(new Wolf(glm::vec3(mob_pos.x, chunk->local_layer_max + 1, mob_pos.z)));
		}
	}

	// Mise à jour des mobs
	
	for (auto& mob : mobs)
		mob->update(*this, player_pos);

	// Supprime les mobs trop loin

	for (auto it = mobs.begin(); it != mobs.end(); it++)
		if ((*it)->will_dispawn)
		{
			delete* it;
			mobs.erase(it);
		}
}

// Affiche les blocs

void World::draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane) const
{
	Shader::block.bind();
	Texture::blocks.bind(0);

	for (auto& chunk : chunks)
		if (chunk->is_visible(camera))
			chunk->draw(camera, lights, clipping_plane);

	Texture::unbind();
	Shader::unbind();
}

// Affiche l'eau

void World::draw_water(const Camera& camera, const std::vector<const Light*>& lights) const
{
	glDisable(GL_CULL_FACE);

	if (Game::in_water)
		Shader::in_water.bind();

	else
		Shader::water.bind();

	VertexBuffer& water = Game::in_water ? Chunk::water_bottom : Chunk::water_top;

	Texture::water_dudv.bind(2);
	Texture::water_normals.bind(3);
	water.bind();

	water.send_uniform("u_time", Game::time);
	water.send_uniform("u_camera", camera.get_position());
	water.send_uniform("u_water_level", water_level);
	water.send_uniform("u_color", Material::water.get_color());
	water.send_uniform("u_ambient", Material::water.get_ambient());
	water.send_uniform("u_diffuse", Material::water.get_diffuse());
	water.send_uniform("u_specular", Material::water.get_specular());
	water.send_uniform("u_shininess", Material::water.get_shininess());

	if (!Game::in_water)
		water.send_texture("u_reflection", 0);

	water.send_texture("u_refraction", 1);
	water.send_texture("u_water_dudv", 2);
	water.send_texture("u_water_normals", 3);

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

	water.send_uniform("u_light_types", light_types);
	water.send_uniform("u_light_vectors", light_vectors);
	water.send_uniform("u_light_colors", light_colors);
	water.send_uniform("u_light_intensities", light_intensities);
	water.send_uniform("u_nb_lights", std::min((int)lights.size(), (int)nb_max_lights));

	for (auto& chunk : chunks)
		if (chunk->is_visible(camera))
			chunk->draw_water(camera, lights);

	VertexBuffer::unbind();
	Texture::unbind();
	Texture::unbind();
	Shader::unbind();
	glEnable(GL_CULL_FACE);
}

// Affiche les mobs

void World::draw_mobs(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane) const
{
	for (auto& mob : mobs)
		mob->draw(camera, lights, clipping_plane);
}

// Affiche les élements de debug

void World::draw_debug(const Camera& camera) const
{
	glDisable(GL_CULL_FACE);
	Shader::debug.bind();
	Chunk::limits.bind();

	for (auto chunk : chunks)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(chunk->position));
		Chunk::limits.send_uniform("u_mvp", camera.get_matrix() * model);

		Chunk::limits.draw(DrawType::Lines);
	}

	VertexBuffer::unbind();
	Shader::unbind();
	glEnable(GL_CULL_FACE);
}
