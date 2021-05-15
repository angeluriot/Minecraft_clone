#include "Mob.h"
#include "Game.h"

// Crée un mob

Mob::Mob() : Entity()
{
	time_check = 0;
	tempo = 0;
	time_check_rand = random_int(0, 150) + 50;
	animation_frame = false;
	is_animated = false;
	will_dispawn = false;
}

// Crée un model 3D du joueur à partir d'un autre

Mob::Mob(const Mob& other)
{
	*this = other;
}

// Opérateur égal

Mob& Mob::operator=(const Mob& other)
{
	Entity::operator=(other);

	time_check = other.time_check;
	tempo = other.tempo;
	time_check_rand = other.time_check_rand;
	animation_frame = other.animation_frame;
	is_animated = other.is_animated;
	will_dispawn = other.will_dispawn;

	return *this;
}

// Gère le comportement du mob

void Mob::behaviour()
{
	time_check++;

	if (time_check > time_check_rand)
	{
		if (!walking && time_check < time_check_rand + 2)
			body_angle = random_float(-pi, pi);

		else
		{
			time_check = 0;
			time_check_rand = random_int(0, 80) + 40;
			walking = !walking;
		}
	}

	if (walking)
	{
		if (is_blocked && !in_air)
		{
			if (tempo == 0)
			{
				tempo += 10;
				jump();
			}

			else
				tempo--;
		}

		else
		{
			move(position - glm::vec3(sin(body_angle) * walk_speed * frame_duration, 0.f, cos(body_angle) * walk_speed * frame_duration));
			is_animated = true;
		}
	}

	if (is_animated)
	{
		animation_frame += pi / 32.f;
		animate(animation_frame);

		if (animation_frame >= pi)
		{
			is_animated = false;
			animation_frame = 0;
		}
	}
}

// Met à jour le mob

void Mob::update(const World& world, const glm::vec3& player_pos)
{
	check_hitbox(world);

	if (chunk == NULL)
	{
		will_dispawn = true;
		return;
	}

	behaviour();
	update_position();
	update_rotation();
	update_dispawn(player_pos);
}

// Fait dispawn l'entité si il faut

void Mob::update_dispawn(const glm::vec3& player_pos)
{
	if (glm::distance(glm::vec2(player_pos.x, player_pos.z), glm::vec2(position.x, position.z)) > Chunk::max_distance / 2.f)
		will_dispawn = true;
}

// Anime le mob (virtuelle pure)

void Mob::animate(float time)
{}
