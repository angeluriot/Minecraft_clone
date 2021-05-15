#ifndef WOLF_H
#define WOLF_H

#include "Mob.h"

class Wolf : public Mob
{
private:

	Part		muzzle;				// Museau
	Part		ear_1;				// Oreille gauche
	Part		ear_2;				// Oreille droite
	Part		head;				// Tête
	Part		topbody;			// Partie haute du corps
	Part		tail;				// Queue
	Part		front_leg_1;		// Jambe avant gauche
	Part		front_leg_2;		// Jambe avant droite
	Part		back_leg_1;			// Jambe arrière gauche
	Part		back_leg_2;			// Jambe arrière droite
	Part		tailbase;			// Base de la queue
	Part		shoulder_1;			// Epaule gauche
	Part		shoulder_2;			// Epaule droite
	Part		hip_1;				// Hanche gauche
	Part		hip_2;				// Hanche droite
	glm::mat4	shoulder_1_initial;	// Matrice initiale de l'épaule gauche
	glm::mat4	shoulder_2_initial;	// Matrice initiale de l'épaule droite
	glm::mat4	hip_1_initial;		// Matrice initiale de la hanche gauche
	glm::mat4	hip_2_initial;		// Matrice initiale de la hanche droite

public:

				Wolf();
				Wolf(const Wolf& other);
				Wolf(const glm::vec3& position);

	Wolf&		operator=(const Wolf& other);

	void		animate(float time);
};

#endif