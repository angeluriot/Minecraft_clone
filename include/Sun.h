#ifndef SUN_H
#define SUN_H

#include "utils.h"
#include "Luminary.h"

// Classe définissant le soleil

class Sun : public Luminary
{
public:

	void	init() override;
	void	update(const glm::vec3& player_pos) override;
};

#endif