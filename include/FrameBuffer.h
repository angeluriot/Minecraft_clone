#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "utils.h"
#include "Texture.h"

// Classe d�finissant un frame buffer

class FrameBuffer
{
private:

	std::shared_ptr<GLuint>	fbo;		// Id du frame buffer
	Texture					texture;	// Texture du frame buffer
	std::shared_ptr<GLuint>	rbo;		// Id du depth/stencil buffer
	uint16_t				width;		// R�solution (largeur)
	uint16_t				height;		// R�solution (hauteur)

public:

	static FrameBuffer		game;		// Frame buffer pour le post processing
	static FrameBuffer		reflection;	// Frame buffer pour la r�flection sur l'eau
	static FrameBuffer		refraction;	// Frame buffer pour la r�fraction sur l'eau
	static FrameBuffer		lens_flare;	// Frame buffer pour les lens flares

							FrameBuffer();
							FrameBuffer(const FrameBuffer& other);
							FrameBuffer(uint16_t width, uint16_t height);
							~FrameBuffer();

	FrameBuffer&			operator=(const FrameBuffer& other);

	void					create(uint16_t width, uint16_t height);
	Texture					get_texture() const;
	void					bind() const;

	static void				unbind();
	static void				clear();
	static void				init();
};

#endif