#include "Texture.h"

Texture Texture::blocks;
Texture Texture::player;
Texture Texture::sun;
Texture Texture::moon;
Texture Texture::water_dudv;
Texture Texture::water_normals;
Texture Texture::cursor;
Texture Texture::wolf;
Texture Texture::fox;
Texture Texture::chicken;

// Crée une texture

Texture::Texture()
{
	id = std::make_shared<GLuint>(0);
}

// Crée une texture à partir d'une autre

Texture::Texture(const Texture& other)
{
	*this = other;
}

// Crée une texture à partir d'une image

Texture::Texture(const std::string& path)
{
	load(path);
}

// Détruit une texture

Texture::~Texture()
{
	if (id.unique())
		glDeleteTextures(1, &(*id));
}

// Opérateur égal (attention ce n'est pas une vraie copie, à utiliser pour déplacer une texture mais pas pour en créer une nouvelle)

Texture& Texture::operator=(const Texture& other)
{
	if (id.unique())
		glDeleteTextures(1, &(*id));

	id = other.id;

	return *this;
}

// Applique une image à la texture

void Texture::load(const std::string& path, GLenum wrapping, GLint filtering)
{
	if (id.unique())
		glDeleteTextures(1, &(*id));

	id = std::make_shared<GLuint>(0);
	SDL_Surface* image = IMG_Load(path.data());
	SDL_Surface* rgba_image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);

	glGenTextures(1, &(*id));
	glBindTexture(GL_TEXTURE_2D, *id);
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rgba_image->w, rgba_image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)rgba_image->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_FreeSurface(image);
	SDL_FreeSurface(rgba_image);
}

// Bind la texture

void Texture::bind(uint8_t index) const
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, *id);
}

// Donne l'id de la texture

GLuint Texture::get_id() const
{
	return *id;
}

// Donne un pointeur vers l'id de la texture

GLuint* Texture::get_id_ptr()
{
	return &(*id);
}

// Change l'id de la texture

void Texture::set_id(GLuint id)
{
	*(this->id) = id;
}

// Unbind la texture

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Ouvre les textures utilisées dans le programme

void Texture::init()
{
	Texture::blocks.load("textures/textures.png");
	Texture::player.load("textures/steve.png");
	Texture::sun.load("textures/sun.png");
	Texture::moon.load("textures/moon.png");
	Texture::water_dudv.load("textures/water_dudv.png", GL_MIRRORED_REPEAT, GL_LINEAR);
	Texture::water_normals.load("textures/water_normals.png", GL_REPEAT, GL_LINEAR);
	Texture::cursor.load("textures/cursor.png");
	Texture::chicken.load("textures/chicken.png");
	Texture::wolf.load("textures/wolf.png");
	Texture::fox.load("textures/fox.png");
}
