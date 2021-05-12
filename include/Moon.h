#ifndef MOON_H
#define MOON_H

#include "utils.h"
#include "Luminary.h"

// Classe définissant la lune

class Moon : public Luminary
{
private:

	bool	inverted; // Sprite dans le sens inverse

public:

	void	init() override;
	void	update(const glm::vec3& player_pos) override;
};

#endif