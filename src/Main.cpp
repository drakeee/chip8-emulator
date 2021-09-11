#include <Main.h>

#undef main
int main(int argn, char* argc[])
{
	char* fileName = nullptr;

	if (argn <= 2)
	{
		printf("USAGE: chip8-emulator --file [ROM File]\n");
		printf("Example: chip8-emulator --file pong.rom\n");
		exit(1);
	} else {
		if (strcmp(argc[1], "--file") == 0)
			fileName = argc[2];
	}

	CPU* cpu = new CPU(fileName);

	if (cpu->failed)
		exit(-1);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failed to initialize the SDL2 library");
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, NULL);

	bool quit = false;

	//CPU* cpu = new CPU("./games/Pong 2 (Pong hack) [David Winter, 1997].ch8");
	uint64_t lastTime = cpu->GetCurrentTimeEx();

	//CPU* cpu = new CPU("heart_monitor.ch8");
	while (!quit)
	{
		if(!cpu->WaitForKey) cpu->Clock();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				quit = true;

			else if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
			{
				SDL_Keycode key = event.key.keysym.sym;
				uint8_t keyCode = 0;

				if (key < 58)
					keyCode = key - 48;
				else
					keyCode = key - 87;

				if (cpu->WaitForKey)
				{
					cpu->WaitForKey = false;
					cpu->V[cpu->WaitForKeyX] = keyCode;
					cpu->WaitForKeyX = 0;
					cpu->PC += 2;
				}

				if (event.type == SDL_KEYDOWN) cpu->Keyboard |= (1 << keyCode);
				else if (event.type == SDL_KEYUP) cpu->Keyboard &= ~(1 << keyCode);
			}
		}

		if (cpu->GetCurrentTimeEx() > lastTime + ((1 / 30) * 1000))
		{
			if (cpu->DelayTimer > 0)
			{
				cpu->DelayTimer--;
			}

			SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
			uint8_t* pixels;
			int pitch;

			uint32_t WHITE_COLOR = SDL_MapRGBA(format, 255, 255, 255, 255);
			uint32_t BLACK_COLOR = SDL_MapRGBA(format, 0, 0, 0, 255);

			SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

			for (int y = 0; y < 32; y++)
			{
				for (int x = 0; x < 64; x++)
				{
					uint32_t* p = (uint32_t*)&pixels[x * 4 + y * pitch];

					*p = cpu->display[x][y] ? WHITE_COLOR : BLACK_COLOR;
				}
			}

			SDL_UnlockTexture(texture);

			int wW, wH;
			SDL_GetWindowSize(window, &wW, &wH);

			SDL_Rect rect;
			rect.x = 0;
			rect.y = 0;
			rect.w = wW;
			rect.h = wH;

			SDL_RenderCopy(renderer, texture, NULL, &rect);

			SDL_RenderPresent(renderer);

			lastTime = cpu->GetCurrentTimeEx();
			SDL_Delay(1);
		}
	}

	/*CPU* cpu = new CPU("./programs/IBM Logo.ch8");

	for(int i = 0; i < cpu->romSize; i += 2)
		cpu->ProcessInstruction(*(uint16_t*)&cpu->memory[512 + i]);*/

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}
