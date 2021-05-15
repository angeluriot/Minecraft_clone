#include "utils.h"
#include "Shader.h"

std::mutex		lock;

SDL_Window*		Window::window = NULL;
SDL_GLContext	Window::context = NULL;
glm::ivec2		Window::size = glm::ivec2(10, 10);
glm::ivec2		Window::center = glm::ivec2(5, 5);

// Initialise la fenêtre

bool Window::init()
{
	// Initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		SHADER_ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
		return false;
	}

	SDL_DisplayMode screen;
	SDL_GetCurrentDisplayMode(0, &screen);

	size.x = screen.w * (3.f / 4.f);
	size.y = screen.h * (3.f / 4.f);
	center.x = size.x / 2;
	center.y = size.y / 2;

	// Create a Window
	window = SDL_CreateWindow("Minecraft clone",	// Titre
		SDL_WINDOWPOS_UNDEFINED,					// X Position
		SDL_WINDOWPOS_UNDEFINED,					// Y Position
		size.x, size.y,								// Resolution
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);		// Flags (OpenGL + Show)

	// Initialize OpenGL Version (version 3.0)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Initialize the OpenGL Context (where OpenGL resources (Graphics card resources) lives)
	context = SDL_GL_CreateContext(window);

	// Tells GLEW to initialize the OpenGL function with this version
	glewExperimental = GL_TRUE;
	glewInit();

	// Start using OpenGL to draw something on screen
	glViewport(0, 0, size.x, size.y); // Draw on ALL the screen

	// The OpenGL background color (RGBA, each component between 0.f and 1.f)
	glClearColor(0.f, 0.f, 0.f, 1.f);

	glEnable(GL_DEPTH_TEST); // Active the depth test
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Active le clipping plane
	glEnable(GL_CLIP_DISTANCE0);

	// Active les faces visibles uniquement dans un sens
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	return true;
}

// Supprime la fenêtre

void Window::clear()
{
	if (context != NULL)
		SDL_GL_DeleteContext(Window::context);

	if (window != NULL)
		SDL_DestroyWindow(Window::window);
}

// Passe d'un vecteur de float à un vecteur d'int

glm::ivec3 round(const glm::vec3& vector)
{
	return glm::ivec3(std::round(vector.x), std::round(vector.y), std::round(vector.z));
}

// Donne un int aléatoire entre min et max

int32_t random_int(int32_t min, int32_t max)
{
	return (rand() % (max - min)) + min;
}

// Donne un float aléatoire entre min et max

float random_float(float min, float max)
{
	return (rand() / static_cast<float>(RAND_MAX)) * (max - min) + min;
}

// Donne vrai avec une probabilité donnée

bool rand_probability(float probability)
{
	if (probability < 0.f)
		return false;

	if (probability < 1.f)
		return (rand() / static_cast<float>(RAND_MAX) < probability);

	return true;
}

// Donne la norme horizontale d'un vecteur

float get_horizontal_norm(const glm::vec3& vector)
{
	return sqrt(pow(vector.x, 2) + pow(vector.z, 2));
}

// Change la norme horizontale d'un vecteur

void set_horizontal_norm(glm::vec3& vector, float value)
{
	float norm = get_horizontal_norm(vector);

	if (norm != 0)
	{
		vector.x /= norm;
		vector.z /= norm;

		vector.x *= value;
		vector.z *= value;
	}
}

// Opérateur de multiplication

glm::vec3 operator*(const glm::mat4& matrix, const glm::vec3& vector)
{
	glm::vec4 v = matrix * glm::vec4(vector.x, vector.y, vector.z, 1.f);
	return glm::vec3(v.x, v.y, v.z);
}

// Normalise un nombre

float normalize(float number)
{
	return std::clamp(number, 0.f, 1.f);
}

// Donne le ratio d'un nombre entre deux bornes

float ratio(float number, float min, float max)
{
	return (number - min) / (max - min);
}

// Restreint un nombre entre deux valeurs

float smooth_clamp(float value, float min, float max, float smoothness)
{
	return (1.f / (1.f + exp(-value / smoothness))) * (max - min) + min;
}