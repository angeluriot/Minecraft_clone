#include "FrameBuffer.h"
#include "Game.h"

FrameBuffer FrameBuffer::game;
FrameBuffer FrameBuffer::reflection;
FrameBuffer FrameBuffer::refraction;

// Cr�e un frame buffer

FrameBuffer::FrameBuffer()
{
	fbo = std::make_shared<GLuint>(0);
	rbo = std::make_shared<GLuint>(0);
	width = GameWindow::size.x;
	height = GameWindow::size.y;
}

// Cr�e un frame buffer � partir d'un autre

FrameBuffer::FrameBuffer(const FrameBuffer& other)
{
	*this = other;
}

// Cr�e un frame buffer � partir de sa r�solution

FrameBuffer::FrameBuffer(uint16_t width, uint16_t height)
{
	fbo = std::make_shared<GLuint>(0);
	rbo = std::make_shared<GLuint>(0);

	create(this->width, this->height);
}

// Op�rateur �gal (attention ce n'est pas une vraie copie, � utiliser pour d�placer un frame buffer mais pas pour en cr�er un nouveau)

FrameBuffer& FrameBuffer::operator=(const FrameBuffer& other)
{
	if (fbo.unique())
	{
		glDeleteFramebuffers(1, &(*fbo));
		glDeleteRenderbuffers(1, &(*rbo));
	}

	fbo = other.fbo;
	texture = other.texture;
	rbo = other.rbo;
	width = other.width;
	height = other.height;

	return *this;
}

// D�truit le frame buffer

FrameBuffer::~FrameBuffer()
{
	if (fbo.unique())
	{
		glDeleteFramebuffers(1, &(*fbo));
		glDeleteRenderbuffers(1, &(*rbo));
	}
}

// Initialise le frame buffer � partir de sa taille

void FrameBuffer::create(uint16_t width, uint16_t height)
{
	if (fbo.unique())
	{
		glDeleteFramebuffers(1, &(*fbo));
		glDeleteTextures(1, texture.get_id_ptr());
		glDeleteRenderbuffers(1, &(*rbo));
	}

	else
	{
		fbo = 0;
		texture.set_id(0);
		rbo = 0;
	}

	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &(*fbo));
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
	{
		glGenTextures(1, texture.get_id_ptr());
		glBindTexture(GL_TEXTURE_2D, texture.get_id());
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.get_id(), 0);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenRenderbuffers(1, &(*rbo));
		glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
		{
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Donne la texture du frame buffer

Texture FrameBuffer::get_texture() const
{
	return texture;
}

// Bind le frame buffer

void FrameBuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
	glViewport(0, 0, width, height);
}

// Unbind le frame buffer

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, GameWindow::size.x, GameWindow::size.y);
}

// Vide le frame buffer

void FrameBuffer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// Cr�e les frame buffers utilis�s dans le programme

void FrameBuffer::init()
{
	game.create(GameWindow::size.x, GameWindow::size.y);
	reflection.create(GameWindow::size.x * reflection_quality, GameWindow::size.y * reflection_quality);
	refraction.create(GameWindow::size.x * refraction_quality, GameWindow::size.y * refraction_quality);
}
