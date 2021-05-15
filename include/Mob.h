#ifndef MOB_H
#define MOB_H

#include "utils.h"
#include "Entity.h"

class World;

// Classe définissant un mob

class Mob : public Entity
{
private:

	int				time_check;			// Temps avant de marcher
	int				tempo;				// Temps avant de resauter
	int				time_check_rand;	// Temps aléatoire
	float			animation_frame;	// Frame actuelle de l'animation
	bool			is_animated;		// Est animé
	bool			will_dispawn;		// Va disparaitre

public:

					Mob();
					Mob(const Mob& other);

	virtual Mob&	operator=(const Mob& other);

	void			behaviour();
	void			update(const World& world, const glm::vec3& player_pos) override;
	void			update_dispawn(const glm::vec3& player_pos);
	virtual void	animate(float time) = 0;

	friend			World;
};

#endif