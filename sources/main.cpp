#include "Game.h"

int main(int argc, char* argv[])
{
	bool running = true;

	// Initiation des librairies
	if (!Window::init())
		return EXIT_FAILURE;

	// Le jeu
	Game::init();
	std::thread generation_thread([&running]() { while (running) Game::world.generate(Game::player.get_position()); });
	
	// La souris
	glm::ivec2 mouse_pos = Window::center;
	SDL_WarpMouseInWindow(Window::window, Window::center.x, Window::center.y);
	SDL_ShowCursor(SDL_DISABLE);
	SDL_Delay(100);
	
	// Boucle du programme

	while (running)
	{
		uint32_t time_begin = SDL_GetTicks();

		// Evenements

		SDL_Event sdl_event;

		while (SDL_PollEvent(&sdl_event))
		{
			switch (sdl_event.type)
			{
			case SDL_WINDOWEVENT:
				switch (sdl_event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					lock.lock();
					running = false;
					lock.unlock();
					break;

				default:
					break;
				}
				break;

			default:
				lock.lock();
				Game::checks_events(sdl_event);
				lock.unlock();
			}
		}

		// Mises à jour
		lock.lock();
		Game::update(mouse_pos);
		lock.unlock();

		// Affichage
		
		FrameBuffer::clear();

		lock.lock();
		Game::draw();
		lock.unlock();

		SDL_GL_SwapWindow(Window::window);

		// Images par seconde

		uint32_t time_end = SDL_GetTicks();

		if (time_end - time_begin < frame_duration_ms)
			SDL_Delay(frame_duration_ms - (time_end - time_begin));
	}

	generation_thread.join();
	Window::clear();

	return 0;
}
