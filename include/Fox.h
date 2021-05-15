#ifndef FOX_H
#define FOX_H

#include "Mob.h"

// Classe définissant un renard

class Fox : public Mob
{
	private:

		Part		muzzle;				// Museau
		Part		ear_1;				// Oreille gauche
		Part		ear_2;				// Oreille droite
		Part		head;				// Tête
		Part		tail;				// Queue
		Part		leg;				// Jambe
		Part		tailbase;			// Base de la queue
		Part		shoulder_1;			// Epaule gauche
		Part		shoulder_2;			// Epaule droite
		Part		hips_1;				// Hanche gauche
		Part		hips_2;				// Hanche droite
		glm::mat4	shoulder_1_initial;	// Matrice initiale de l'épaule gauche
		glm::mat4	shoulder_2_initial;	// Matrice initiale de l'épaule droite
		glm::mat4	hips_1_initial;		// Matrice initiale de la hanche gauche
		glm::mat4	hips_2_initial;		// Matrice initiale de la hanche droite
		glm::mat4	head_initial;		// Matrice initiale de la tête
		glm::mat4	tail_initial;		// Matrice initiale de la queue


	public:

					Fox();
					Fox(const Fox& other);
					Fox(const glm::vec3& position);

		Fox&		operator=(const Fox& other);

		void		animate(float time);
};

#endif