#ifndef SHADER_H
#define SHADER_H

#include "libraries.h"

// ?

#define ERROR_MAX_LENGTH 500

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define BOLD  "\x1B[1m"
#define RESET "\x1B[0m"

// ??

#define SET_BIT(bits, pos)   (bits |= 1 << (pos))
#define RESET_BIT(bits, pos) (bits &= ~(1 << (pos)))

#ifndef __FILENAME__
#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)
#endif

// ???

#ifndef SHADER_ERROR
#define SHADER_ERROR(x, ...)     (fprintf(stderr, RED "Error : " GRN "%s:%d " RESET x, __FILENAME__, __LINE__, ## __VA_ARGS__))
#endif

#ifndef SHADER_WARNING
#define SHADER_WARNING(x, ...)   (fprintf(stdout, YEL "Warning : " GRN "%s:%d " RESET x, __FILENAME__, __LINE__, ## __VA_ARGS__))
#endif

#ifndef SHADER_INFO
#define SHADER_INFO(x, ...)   (fprintf(stdout, BOLD WHT "INFO : " RESET GRN "%s:%d " RESET x , __FILENAME__, __LINE__, ## __VA_ARGS__))
#endif

// std::cout ???

// Classe définissant un shader

class Shader
{
private:

	std::shared_ptr<GLuint>	program_id;	// Id du programme
	std::shared_ptr<GLuint>	vertex_id;	// Id du vertex shader
	std::shared_ptr<GLuint>	frag_id;	// Id du fragment shader

	int						loadShader(const std::string& code, int type);
	bool					loadFromFiles(FILE* vertexFile, FILE* fragFile);
	bool					loadFromStrings(const std::string& vertexString, const std::string& fragString);

public:

	static Shader			block;		// Shader des blocs
	static Shader			entity;		// Shader des mobs
	static Shader			sky;		// Shader du ciel
	static Shader			luminary;	// Shader des astres
	static Shader			water;		// Shader de l'eau
	static Shader			in_water;	// Shader de l'eau quand la caméra est sous l'eau
	static Shader			screen;		// Shader du post-processing
	static Shader			lens_flare;	// Shader des lens flares
	static Shader			debug;		// Shader de debug

							Shader();
							Shader(const Shader& other);
							Shader(const std::string& vert_path, const std::string& frag_path);
							~Shader();
	
	Shader&					operator=(const Shader& other);

	bool					load(const std::string& vert_path, const std::string& frag_path);
	GLuint					get_id() const;
	void					bind() const;
	
	static void				unbind();
	static void				init();
};

#endif
