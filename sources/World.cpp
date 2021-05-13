#include "World.h"
#include "Chunk.h"
#include "Block.h"
#include "Texture.h"
#include "Game.h"

const int64_t World::seed = time(NULL);

// Crée le monde

World::World()
{
	chunks.clear();
	future_chunks_pos.clear();
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
}

// Opérateur égal

World& World::operator=(const World& other)
{
	chunks = other.chunks;
	future_chunks_pos = other.future_chunks_pos;

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

void World::init(const glm::vec3& player_pos)
{
	srand(seed);

	glm::mat4 water_scale = glm::scale(glm::mat4(1.f), glm::vec3(Chunk::size, Chunk::size, Chunk::size));
	glm::mat4 water_rotation = glm::rotate(glm::mat4(1.f), pi / 2.f, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 water_shift = glm::translate(glm::mat4(1.f), glm::vec3(Chunk::size / 2.f - 0.5f, water_level, Chunk::size / 2.f - 0.5f));

	glm::mat4 limits_scale = glm::scale(glm::mat4(1.f), glm::vec3(Chunk::size, 10000.f, Chunk::size));
	glm::mat4 limits_shift = glm::translate(glm::mat4(1.f), glm::vec3(Chunk::size / 2.f - 0.5f, 0.f, Chunk::size / 2.f - 0.5f));

	Chunk::water.send_data(Shader::water, (water_shift * water_rotation * water_scale) * Mesh::square);
	Chunk::limits.send_data(Shader::debug, (limits_shift * limits_scale) * Mesh::empty_cube, DataType::Positions);

	add_chunk(block_to_chunk(player_pos));
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
	Shader::water.bind();
	Texture::water_dudv.bind(2);
	Texture::water_normals.bind(3);
	Chunk::water.bind();

	Chunk::water.send_uniform("u_time", Game::time);
	Chunk::water.send_uniform("u_camera", camera.get_position());
	Chunk::water.send_uniform("u_light_direction", lights[0]->get_vector());
	Chunk::water.send_uniform("u_ambient_color", Material::water.get_color() * Material::water.get_ambient() * lights[0]->get_color());
	Chunk::water.send_uniform("u_diffuse_color", Material::water.get_color() * Material::water.get_diffuse() * lights[0]->get_color());
	Chunk::water.send_uniform("u_specular_color", Material::water.get_specular() * lights[0]->get_color());
	Chunk::water.send_uniform("u_shininess", Material::water.get_shininess());
	Chunk::water.send_texture("u_reflection", 0);
	Chunk::water.send_texture("u_refraction", 1);
	Chunk::water.send_texture("u_water_dudv", 2);
	Chunk::water.send_texture("u_water_normals", 3);

	for (auto& chunk : chunks)
		if (chunk->is_visible(camera))
			chunk->draw_water(camera, lights);

	VertexBuffer::unbind();
	Texture::unbind();
	Texture::unbind();
	Shader::unbind();
	glEnable(GL_CULL_FACE);
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
