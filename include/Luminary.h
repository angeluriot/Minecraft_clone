#ifndef LUMINARY_H
#define LUMINARY_H

#include "utils.h"
#include "Light.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "Camera.h"

// Classe définissant un astre

class Luminary
{
protected:

	Texture*				texture;				// Texture de l'astre
	Light					light;					// Lumière de l'astre
	glm::mat4				model;					// Matrice de transformation
	VertexBuffer			object;					// Vertex buffer OpenGL

	static const glm::mat4	scale_matrix;			// Matrice d'échelle
	static const glm::mat4	translation_matrix;		// Matrice de translation
	static const glm::mat4	rotation_matrix;		// Matrice de rotation

public:

	static constexpr float	angle		= pi / 6.f;	// L'inclinaison du ciel
	static constexpr float	speed		= 0.01f;	// Vitesse des astres
	static constexpr float	size		= 30.f;		// Taille des astres
	static constexpr float	distance	= 500.f;	// Distance des astres
	static constexpr float	start_time	= pi / 4.f;	// Temps de départ des astres

							Luminary();
							Luminary(const Luminary& other);

	Luminary&				operator=(const Luminary& other);

	virtual void			init();
	virtual void			update(const glm::vec3& player_pos) = 0;
	void					draw(const Camera& camera, const std::vector<const Light*>& lights) const;
	glm::vec3				get_position() const;
	const Light*			get_light() const;
};

#endif
