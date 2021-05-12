#ifndef LIGHT_H
#define LIGHT_H

#include "utils.h"

// Classe définissant une lumière

struct Light
{
public:

	// Enum définissant le type de lumière

	enum class Type { Ambient = 0, Directional = 1, Point = 2 };

private:

	Type		type;		// type de lumière
	glm::vec3	vector;		// Direction ou position
	Color		color;		// Couleur
	float		intensity;	// intensitée

public:

				Light();
				Light(const Light& other);
				Light(Type type, const glm::vec3& vector, const Color& color = Color(1.f, 1.f, 1.f, 1.f), float intensity = 1.f);

	Light&		operator=(const Light& other);

	Type		get_type() const;
	void		set_type(Type type);
	glm::vec3	get_vector() const;
	void		set_vector(const glm::vec3& vector);
	Color		get_color() const;
	void		set_color(const Color& color);
	float		get_intensity() const;
	void		set_intensity(float intensity);
};

#endif