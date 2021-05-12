#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include "libraries.h"
#include "Mesh.h"
#include "Shader.h"

// Enum définissant le type de dessin

enum class DrawType { Triangles = GL_TRIANGLES, Lines = GL_LINES, Points = GL_POINTS };

// Enum définissant le type de données à envoyer au shader

enum class DataType : uint8_t { Positions = 1, Normals = 2, TexCoords = 4, All = 7 };

// Classe définissant un vertex buffer

class VertexBuffer
{
private:

	Shader*					shader;			// Pointeur vers le shader utilisé
	std::shared_ptr<GLuint>	vbo;			// Id du vertex buffer oject
	std::shared_ptr<GLuint>	vao;			// Id du vertex array object
	uint32_t				nb_vertices;	// Nombre de vertices

public:

							VertexBuffer();
							VertexBuffer(const VertexBuffer& other);
							VertexBuffer(Shader& shader);
							VertexBuffer(Shader& shader, const Mesh& mesh, DataType data_sent = DataType::All);
							~VertexBuffer();

	VertexBuffer&			operator=(const VertexBuffer& other);

	void					set_shader(Shader& shader);
	void					send_data(Shader& shader, const Mesh& mesh, DataType data_sent = DataType::All);
	void					send_data(const Mesh& mesh, DataType data_sent = DataType::All);

	void					send_uniform(const std::string& name, float number) const;
	void					send_uniform(const std::string& name, int number) const;

	void					send_uniform(const std::string& name, const std::vector<float>& numbers) const;
	void					send_uniform(const std::string& name, const std::vector<int>& numbers) const;

	void					send_uniform_2(const std::string& name, const std::vector<float>& numbers) const;
	void					send_uniform_2(const std::string& name, const std::vector<int>& numbers) const;

	void					send_uniform_3(const std::string& name, const std::vector<float>& numbers) const;
	void					send_uniform_3(const std::string& name, const std::vector<int>& numbers) const;

	void					send_uniform_4(const std::string& name, const std::vector<float>& numbers) const;
	void					send_uniform_4(const std::string& name, const std::vector<int>& numbers) const;

	void					send_uniform(const std::string& name, const glm::vec2& vector) const;
	void					send_uniform(const std::string& name, const glm::vec3& vector) const;
	void					send_uniform(const std::string& name, const Color& color) const;

	void					send_uniform(const std::string& name, const std::vector<glm::vec2>& vectors) const;
	void					send_uniform(const std::string& name, const std::vector<glm::vec3>& vectors) const;
	void					send_uniform(const std::string& name, const std::vector<Color>& color) const;

	void					send_uniform(const std::string& name, const glm::mat2& matrix) const;
	void					send_uniform(const std::string& name, const glm::mat3& matrix) const;
	void					send_uniform(const std::string& name, const glm::mat4& matrix) const;

	void					send_texture(const std::string& name, uint8_t texture_index) const;

	void					draw(DrawType draw_type = DrawType::Triangles) const;

	bool					is_valid() const;
	void					bind() const;
	static void				unbind();
};

DataType	operator&(DataType type_1, DataType type_2);
DataType	operator|(DataType type_1, DataType type_2);
DataType	operator^(DataType type_1, DataType type_2);
DataType	operator~(DataType type);
DataType	operator<<(DataType type, uint8_t shift);
DataType	operator>>(DataType type, uint8_t shift);

#endif
