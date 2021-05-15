#ifndef CHICKEN_H
#define CHICKEN_H

#include "Mob.h"

// Classe d�finissant une poule

class Chicken : public Mob
{
	private:
		
		Part		head;					// T�te
		Part		shoulder_1;				// �paule gauche
		Part		shoulder_2;				// �paule droite
		Part		hips_1;					// Hanche gauche
		Part		hips_2;					// Hanche droite
		Part		leg;					// Jambe
		Part		palm;					// Palme
		Part		wing_1;					// Aile gauche
		Part		wing_2;					// Aile droite
		Part		beak;					// Bec
		Part		beak_bottom;			// Partie sous le bec
		Part		wattle;					// Barbillon
		glm::mat4	beak_bottom_initial;	// Matrice initiale du dessous du bec
		glm::mat4	shoulder_1_initial;		// Matrice initiale de l'�paule gauche
		glm::mat4	shoulder_2_initial;		// Matrice initiale de l'�paule droite
		glm::mat4	hips_1_initial;			// Matrice initiale de la hanche gauche
		glm::mat4	hips_2_initial;			// Matrice initiale de la hanche droite

	public:

					Chicken();
					Chicken(const Chicken& other);
					Chicken(const glm::vec3& position);

		Chicken&	operator=(const Chicken& other);

		void		animate(float time);
};

#endif
