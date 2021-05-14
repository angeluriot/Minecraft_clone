#include "LensFlare.h"
#include "Shader.h"

std::array<Texture, LensFlare::nb_shapes>	LensFlare::textures;
std::array<glm::mat4, LensFlare::nb_shapes>	LensFlare::scale_matrices;
Texture										LensFlare::light_texture;
glm::mat4									LensFlare::light_scale_matrix;
VertexBuffer								LensFlare::object;

// Initialise les lens flares

void LensFlare::init()
{
	light_texture.load("textures/lens_flare/light.png", GL_REPEAT, GL_LINEAR);

	SDL_Surface* light_image = IMG_Load("textures/lens_flare/light.png");
	SDL_Surface* light_rgba_image = SDL_ConvertSurfaceFormat(light_image, SDL_PIXELFORMAT_RGBA32, 0);

	light_scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(size * ((float)light_rgba_image->w / (float)Window::size.x) / 2.f,
		size * ((float)light_rgba_image->h / (float)Window::size.y) / 2.f, 1.f));

	SDL_FreeSurface(light_image);
	SDL_FreeSurface(light_rgba_image);

	for (int i = 0; i < nb_shapes; i++)
	{
		textures[i].load("textures/lens_flare/shape_" + std::to_string(i) + ".png", GL_REPEAT, GL_LINEAR);

		SDL_Surface* image = IMG_Load(("textures/lens_flare/shape_" + std::to_string(i) + ".png").data());
		SDL_Surface* rgba_image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);

		scale_matrices[i] = glm::scale(glm::mat4(1.f), glm::vec3(size * (float)rgba_image->w / (float)Window::size.x, size * (float)rgba_image->h / (float)Window::size.y, 1.f));

		SDL_FreeSurface(image);
		SDL_FreeSurface(rgba_image);
	}

	object.send_data(Shader::lens_flare, Mesh::screen, DataType::Positions | DataType::TexCoords);
}

// Afficher les lens flares

void LensFlare::draw(const Camera& camera, const glm::vec3 light_position, float intensity, const Color& color)
{
	glm::vec4 light_pos = camera.get_matrix() * glm::vec4(light_position, 1.f);

	if (light_pos.w <= 0.f)
		return;

	glm::vec2 light = glm::vec2(light_pos.x / light_pos.w, light_pos.y / light_pos.w);
	float brightness = intensity * (1.f - ratio(glm::length(light), 0.f, 1.3f));

	if (brightness <= 0.f)
		return;

	Shader::lens_flare.bind();
	light_texture.bind(0);

	for (uint8_t i = 1; i < nb_shapes; i++)
		textures[i].bind(i + 1);

	object.bind();

	glm::mat4 position_matrix = glm::translate(glm::mat4(1.f), glm::vec3(light, 0.f));

	object.send_uniform("u_light_color", ColorRGB(color));
	object.send_uniform("u_brightness", std::min(brightness * 5.f, 0.5f));
	object.send_uniform("u_model", position_matrix * light_scale_matrix);
	object.send_texture("u_texture", 0);

	object.draw();

	for (uint8_t i = 0; i < nb_shapes; i++)
	{
		if (i == 0)
			continue;

		glm::mat4 position_matrix = glm::translate(glm::mat4(1.f), glm::vec3(light + (-light * (spacing * (i + 1))), 0.f));
		glm::mat4 z_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -((float)(i + 1) / (float)nb_shapes) / 2.f));

		object.send_uniform("u_light_color", ColorRGB(1.f, 1.f, 1.f));
		object.send_uniform("u_brightness", brightness);
		object.send_uniform("u_model", position_matrix * z_matrix * scale_matrices[i]);
		object.send_texture("u_texture", i + 1);

		object.draw();
	}

	VertexBuffer::unbind();

	for (uint8_t i = 0; i < nb_shapes; i++)
		Texture::unbind();

	Texture::unbind();
	Shader::unbind();
}
