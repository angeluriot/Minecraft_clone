#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"
#include "Camera.h"
#include "World.h"
#include "Chunk.h"
#include "Block.h"
#include "Skin.h"
#include "Material.h"
#include "Light.h"

// Classe définissant un joueur

class Player // : Entity :(
{
public:

	static constexpr float		jump_speed		= 9.f;	// Vitesse du saut
	static constexpr float		walk_speed		= 4.5f;	// Vitesse en marchant
	static constexpr float		run_speed		= 7.f;	// Vitesse en sautant
	static constexpr float		fly_speed		= 25.f;	// Vitesse en volant
	static constexpr float		break_distance	= 10.f;	// Distance maximum pour casser un bloc

private:

	Skin						skin;					// Model 3D du joueur
	glm::vec3					previous_pos;			// Position précédente
	glm::vec3					position;				// Position du joueur
	std::array<glm::vec3, 12>	hitbox;					// Hitbox du joueur
	glm::vec3					speed;					// Vitesse du joueur
	bool						in_air;					// Est en l'air
	Chunk*						chunk;					// Le chunk dans lequel se trouve le joueur

public:

	Camera						camera;					// Caméra
	bool						fly;					// Vol activé
	bool						first_person;			// Vu première personne

								Player();
								Player(const Player& other);
								Player(const glm::vec3& position);

	Player&						operator=(const Player& other);

	glm::vec3					get_position() const;
	void						look(const glm::ivec2& mouse_pos);
	void						update_head();
	void						move(const Uint8* keystates, World& world);
	void						jump();
	void						update_position();
	void						find_player_chunk(World& world);
	void						check_hitbox(World& world);
	void						break_block();
	void						draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane = Plane(0.f, 1.f, 0.f, 10000.f)) const;
};

#endif