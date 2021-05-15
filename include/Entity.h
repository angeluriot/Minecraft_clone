#ifndef ENTITY_H
#define ENTITY_H

#include "utils.h"
#include "Camera.h"
#include "World.h"
#include "Block.h"
#include "Mesh.h"
#include "VertexBuffer.h"
#include "Texture.h"

// Classe définissant une entité

class Entity
{
public:

	static constexpr float		jump_speed		= 9.f;	// Vitesse du saut
	static constexpr float		walk_speed		= 4.5f;	// Vitesse en marchant
	static constexpr float		texture_size	= 64.f;	// Taille de la texture du skin

	// Classe définissant une partie du model

	class Part
	{
	private:

		VertexBuffer				object;				// Vertex buffer OpenGL

	public:

		glm::mat4					propagated_matrix;	// Matrice de propagation
		glm::mat4					local_matrix;		// Matrice locale
		std::vector<Part*>			children;			// Parties reliées

									Part();
									Part(const Part& other);
									Part(Mesh mesh, const glm::vec2& texture_pos, const glm::vec3& size, const std::vector<Part*>& children, bool horizontal);

		Part&						operator=(const Part& other);

		void						draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane = Plane(0.f, 1.f, 0.f, 10000.f)) const;

		static std::vector<float>	init_texcoords(const glm::vec2& texture_pos, const glm::vec3& size, bool horizontal);
	};

protected:

	float						y_offset;				// Décallage en y de la hitbox par rapport au corps
	Part						body;					// Corps
	Part						neck;					// Cou
	std::array<glm::vec3, 12>	hitbox;					// Hitbox de l'entité
	glm::vec3					previous_pos;			// Position précédente
	glm::vec3					position;				// Position de l'entité
	glm::vec3					speed;					// Vitesse de l'entité
	bool						in_air;					// Est en l'air
	Chunk*						chunk;					// Le chunk dans lequel se trouve l'entité
	Texture*					texture;				// Texture
	bool						is_blocked;				// Est entré en collision
	bool						walking;				// Est en train de marcher
	glm::mat4					body_initial;			// Matrice initiale du corps
	glm::mat4					neck_initial;			// Matrice initiale du cou
	float						body_angle;				// Angle du corps
	float						head_angle;				// Angle de la tête

public:

								Entity();
								Entity(const Entity& other);
	virtual						~Entity() = 0;

	virtual Entity&				operator=(const Entity& other);

	void						move(const glm::vec3& position);
	void						rotate(float angle);
	glm::vec3					get_position() const;
	Chunk*						get_chunk() const;
	void						jump();
	void						update_position();
	void						update_rotation();
	void						find_entity_chunk(const World& world);
	void						check_hitbox(const World& world);
	virtual void				update(const World& world, const glm::vec3& player_pos);
	void						draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane = Plane(0.f, 1.f, 0.f, 10000.f)) const;
};

#endif