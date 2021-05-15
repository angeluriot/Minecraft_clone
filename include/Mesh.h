#ifndef MESH_H
#define MESH_H

#include "utils.h"

class Skin;
class Block;
class Entity;

// Classe d�finissant une maillage

class Mesh
{
private:

	std::vector<float>	positions;		// Positions
	std::vector<float>	normals;		// Normales
	std::vector<float>	texcoords;		// Coordonn�es de texture
	
public:

	static const Mesh	circle_64;		// Cercle avec 64 c�t�s
	static const Mesh	cone_64;		// C�ne avec 64 latitudes
	static const Mesh	cube;			// Cube
	static const Mesh	empty_cube;		// Cube avec juste les arr�tes
	static const Mesh	cylinder_64;	// Cylinde avec 64 latitudes
	static const Mesh	null;			// Vide
	static const Mesh	sphere_64;		// Sph�re avec 64 latitudes et 64 longitudes
	static const Mesh	square;			// Carr�
	static const Mesh	screen;			// Ecran

						Mesh();
						Mesh(const Mesh& other);

	Mesh&				operator=(const Mesh& other);
	Mesh&				operator+=(const Mesh& other);
	Mesh				operator+(const Mesh& other);

	const float*		get_positions() const;
	const float*		get_normals() const;
	const float*		get_texcoords() const;

	uint32_t			get_data_size() const;
	uint32_t			get_positions_size() const;
	uint32_t			get_normals_size() const;
	uint32_t			get_texcoords_size() const;
	uint32_t			get_nb_vertices() const;

	void				clear();

	static Mesh			Circle(uint16_t nb_edges);
	static Mesh			Cone(uint16_t nb_lattitudes);
	static Mesh			Cube();
	static Mesh			EmptyCube();
	static Mesh			Cylinder(uint16_t nb_lattitudes);
	static Mesh			Sphere(uint16_t nb_latitudes, uint16_t nb_longitudes);
	static Mesh			Square();
	static Mesh			Screen();

	friend				Block;
	friend				Skin;
	friend				Entity;
	friend Mesh			operator*(const glm::mat4& matrix, const Mesh& mesh);
};

struct Cube
{
	struct Face
	{
		static constexpr uint8_t	nb_points = 6;	// Nombre de points par face
	};

	static constexpr uint8_t		nb_faces = 6;	// Nombre de faces par cube
	static constexpr uint8_t		nb_points = 36;	// Nombre de points par cube
};

Mesh				operator*(const glm::mat4& matrix, const Mesh& mesh);
std::vector<float>	oriented_top_texcoord(uint8_t orientation);

#endif
