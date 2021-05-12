#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"

// Classe définissant un matériaux

class Material
{
private:

	Color			color;		// Couleur
	float			ambient;	// Niveau de lumière ambiante
	float			diffuse;	// Niveau de lumière diffuse
	float			specular;	// Niveau de lumière spéculaire
	float			shininess;	// Taille du reflet

public:

	static Material	block;		// Matériaux des blocs
	static Material	entity;		// Matériaux des entitées
	static Material	water;		// Matériaux de l'eau

					Material();
					Material(const Material& other);
					Material(const Color& color, float ambient, float diffuse, float specular, float shininess);

	Material&		operator=(const Material& other);

	Color			get_color() const;
	void			set_color(const Color& color);
	float			get_ambient() const;
	void			set_ambient(float ambient);
	float			get_diffuse() const;
	void			set_diffuse(float diffuse);
	float			get_specular() const;
	void			set_specular(float specular);
	float			get_shininess() const;
	void			set_shininess(float shininess);
};

#endif