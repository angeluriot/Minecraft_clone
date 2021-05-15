#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"

// Classe définissant une texture

class Texture
{
private:

	std::shared_ptr<GLuint>	id;				// Id de la texture

public:

	static Texture			blocks;			// Texture des blocs
	static Texture			player;			// Texture du joueur
	static Texture			sun;			// Texture du soleil
	static Texture			moon;			// Texture de la lune
	static Texture			water_dudv;		// Texture pour déformer les reflets sur l'eau
	static Texture			water_normals;	// Texture représentant les normales de l'eau
	static Texture			cursor;			// Curseur
	static Texture          wolf;           // Texture du loup
	static Texture          fox;            // Texture du renard
	static Texture          chicken;        // Texture de la poule

							Texture();
							Texture(const Texture& other);
							Texture(const std::string& path);
							~Texture();

	Texture&				operator=(const Texture& other);

	void					load(const std::string& path, GLenum wrapping = GL_REPEAT, GLint filtering = GL_NEAREST);
	void					bind(uint8_t index) const;
	GLuint					get_id() const;
	GLuint*					get_id_ptr();
	void					set_id(GLuint id);

	static void				unbind();
	static void				init();
};

#endif