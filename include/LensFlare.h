#ifndef LENSFLARE_H
#define LENSFLARE_H

#include "utils.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "Camera.h"

// Classe définissant un lens flare

class LensFlare
{
public:

	static constexpr uint8_t				nb_shapes = 9;		// Nombre des formes
	static constexpr float					size = 2.5f;		// Taille des formes
	static constexpr float					spacing = 0.3f;		// Distance entre les formes

private:

	static std::array<Texture, nb_shapes>	textures;			// Texture de chaque forme
	static std::array<glm::mat4, nb_shapes>	scale_matrices;		// Taille de chaque forme
	static Texture							light_texture;		// Texture de la forme de la lumière
	static glm::mat4						light_scale_matrix;	// Taille de la forme de la lumière
	static VertexBuffer						object;				// Vertex buffer OpenGL

public:

	static void						init();
	static void						draw(const Camera& camera, const glm::vec3 light_position, float intensity, const Color& color);
};

#endif