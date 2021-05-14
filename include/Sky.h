#ifndef SKY_H
#define SKY_H

#include "utils.h"
#include "Light.h"
#include "Material.h"
#include "Camera.h"
#include "VertexBuffer.h"

// Classe définissant le ciel

class Sky
{
private:

	Color					high_color;			// Couleur du haut du ciel
	Color					low_color;			// Couleur du bas du ciel
	Light					light;				// Lumière ambiente du ciel
	glm::mat4				model;				// Matrice de transformation
	VertexBuffer			object;				// Vertex buffer OpenGL

	static glm::mat4		scale_matrix;		// Matrice d'échelle

public:

	static constexpr float	distance = 550.f;	// Distance du ciel

							Sky();
							Sky(const Sky& other);

	Sky&					operator=(const Sky& other);

	void					init();
	void					update(const glm::vec3& player_pos, float sun_height);
	void					draw(const Camera& camera, const std::vector<const Light*>& lights) const;
	const Light*			get_light() const;
};

#endif