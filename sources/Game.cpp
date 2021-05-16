#include "Game.h"
#include "LensFlare.h"

World					Game::world;
Sun						Game::sun;
Moon					Game::moon;
Sky						Game::sky;
std::stack<glm::mat4>	Game::matrices		= {};
Player					Game::player;
Camera					Game::fixed_cam;
bool					Game::fix_cam		= false;
bool					Game::fake_cam		= false;
bool					Game::in_water		= false;
bool					Game::stop_moving	= false;
bool					Game::debug			= false;
float					Game::time			= 0.f;

// Initialise le jeu

void Game::init()
{
	// Initialisations
	Shader::init();
	Texture::init();
	FrameBuffer::init();
	LensFlare::init();

	// World
	world.init(std::time(NULL));
	sun.init();
	moon.init();
	sky.init();

	// Joueur
	matrices.push(glm::mat4(1.f));
	player = Player(world.get_spawn_position());
}

// Prend en compte les évenements

void Game::checks_events(const SDL_Event& my_event)
{
	if (my_event.type == SDL_KEYUP)
	{
		// Active le debug
		if (my_event.key.keysym.sym == SDLK_p)
			debug = !debug;

		// Voler
		if (my_event.key.keysym.sym == SDLK_m)
			player.fly = !player.fly;

		// Changer de vue
		if (my_event.key.keysym.sym == SDLK_F5)
			player.first_person = !player.first_person;

		// Fixer la caméra
		if (my_event.key.keysym.sym == SDLK_o)
		{
			fix_cam = !fix_cam;
			fixed_cam = player.camera;
		}
	}

	// Quitter la fenêtre
	if ((my_event.type == SDL_KEYUP && my_event.key.keysym.sym == SDLK_ESCAPE) || (stop_moving && my_event.type == SDL_MOUSEBUTTONUP))
	{
		stop_moving = !stop_moving;
		SDL_ShowCursor(stop_moving);
		SDL_WarpMouseInWindow(Window::window, Window::center.x, Window::center.y);
		glm::ivec2 mouse_pos = Window::center;
		SDL_Delay(100);
	}

	// Casser un bloc
	else if (my_event.type == SDL_MOUSEBUTTONUP)
		player.break_block();
}

// Met à jour le jeu

void Game::update(glm::ivec2& mouse_pos)
{
	time += frame_duration;

	// Déplacement
	player.move(SDL_GetKeyboardState(NULL), world);

	// Caméra

	if (!stop_moving)
	{
		SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
		player.look(mouse_pos);
		SDL_WarpMouseInWindow(Window::window, Window::center.x, Window::center.y);
	}

	player.update_head();
	in_water = player.camera.get_position().y < water_level;

	// Génération
	world.send_meshes();

	// Update des mobs
	world.update_mobs(player.get_position());

	// Météo
	sun.update(player.get_position());
	moon.update(player.get_position());
	sky.update(player.get_position(), sun.get_position().y - player.get_position().y);
}

// Affiche les éléments du jeu

void Game::draw()
{
	std::vector<const Light*> lights = { sun.get_light(), moon.get_light(), sky.get_light() };
	Camera render_camera = fix_cam ? fixed_cam : player.camera;

	if (in_water)
	{
		// Fait le rendu de la refraction

		FrameBuffer::refraction.bind();
		{
			FrameBuffer::clear();
			
			player.draw(render_camera, lights, Plane(0.f, 1.f, 0.f, -water_level));
			world.draw_mobs(render_camera, lights, Plane(0.f, 1.f, 0.f, -water_level));
			world.draw(render_camera, lights, Plane(0.f, 1.f, 0.f, -water_level + 0.4f));
			sun.draw(render_camera, lights);
			moon.draw(render_camera, lights);
			sky.draw(render_camera, lights);

			if (debug)
				world.draw_debug(render_camera);
		}
		FrameBuffer::unbind();

		// Fait le rendu du jeu

		FrameBuffer::game.bind();
		{
			FrameBuffer::clear();
			
			player.draw(render_camera, lights, Plane(0.f, -1.f, 0.f, water_level));
			world.draw_mobs(render_camera, lights, Plane(0.f, -1.f, 0.f, water_level));
			world.draw(render_camera, lights, Plane(0.f, -1.f, 0.f, water_level));
			sky.draw(render_camera, lights);

			if (debug)
				world.draw_debug(render_camera);

			FrameBuffer::refraction.get_texture().bind(1);
			{
				world.draw_water(render_camera, lights);
			}
			Texture::unbind();
		}
		FrameBuffer::unbind();
	}

	else
	{
		// Fait le rendu de la reflection

		render_camera.invert(water_level);
		render_camera.change_resolution(Window::size.x * reflection_quality, Window::size.y * reflection_quality);
		fake_cam = true;

		FrameBuffer::reflection.bind();
		{
			FrameBuffer::clear();

			player.draw(render_camera, lights, Plane(0.f, 1.f, 0.f, -water_level));
			world.draw_mobs(render_camera, lights, Plane(0.f, 1.f, 0.f, -water_level));
			world.draw(render_camera, lights, Plane(0.f, 1.f, 0.f, -water_level + 0.4f));
			sun.draw(render_camera, lights);
			moon.draw(render_camera, lights);
			sky.draw(render_camera, lights);
		}
		FrameBuffer::unbind();

		// Fait le rendu de la refraction

		render_camera = fix_cam ? fixed_cam : player.camera;
		render_camera.change_resolution(Window::size.x, Window::size.y);
		fake_cam = false;

		FrameBuffer::refraction.bind();
		{
			FrameBuffer::clear();

			player.draw(render_camera, lights, Plane(0.f, -1.f, 0.f, water_level));
			world.draw_mobs(render_camera, lights, Plane(0.f, -1.f, 0.f, water_level));
			world.draw(render_camera, lights, Plane(0.f, -1.f, 0.f, water_level + 0.6f));

			if (debug)
				world.draw_debug(render_camera);
		}
		FrameBuffer::unbind();

		// Fait le rendu du jeu

		FrameBuffer::game.bind();
		{
			FrameBuffer::clear();

			player.draw(render_camera, lights, Plane(0.f, 1.f, 0.f, -water_level));
			world.draw_mobs(render_camera, lights, Plane(0.f, 1.f, 0.f, -water_level));
			world.draw(render_camera, lights, Plane(0.f, 1.f, 0.f, -water_level));
			sun.draw(render_camera, lights);
			moon.draw(render_camera, lights);
			sky.draw(render_camera, lights);

			if (debug)
				world.draw_debug(render_camera);

			FrameBuffer::reflection.get_texture().bind(0);
			FrameBuffer::refraction.get_texture().bind(1);
			{
				world.draw_water(render_camera, lights);
			}
			Texture::unbind();
			Texture::unbind();
		}
		FrameBuffer::unbind();
	}

	// Lens flare

	FrameBuffer::lens_flare.bind();
	{
		FrameBuffer::clear();

		if (!in_water)
		{
			LensFlare::draw(render_camera, sun.get_position(), sun.get_light()->get_intensity() / 2.f, sun.get_light()->get_color());
			LensFlare::draw(render_camera, moon.get_position(), moon.get_light()->get_intensity() / 5.f, moon.get_light()->get_color());
		}
	}
	FrameBuffer::unbind();

	// Fait le rendu du final

	VertexBuffer game(Shader::screen, Mesh::screen, DataType::Positions | DataType::TexCoords);

	Shader::screen.bind();
	FrameBuffer::game.get_texture().bind(0);
	FrameBuffer::lens_flare.get_texture().bind(1);
	Texture::cursor.bind(2);
	game.bind();
	{
		game.send_uniform("u_draw_cursor", int(!Game::fix_cam && player.first_person));
		game.send_texture("u_texture", 0);
		game.send_texture("u_lens_flare", 1);
		game.send_texture("u_cursor", 2);
		game.draw();
	}
	VertexBuffer::unbind();
	Texture::unbind();
	Texture::unbind();
	Shader::unbind();
}
