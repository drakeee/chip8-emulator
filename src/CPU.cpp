#include <Main.h>

CPU::CPU(const char* fileName)
{
	FILE* rom = fopen(fileName, "rb");
	if (rom == nullptr)
	{
		printf("Failed to load file: %s\n", fileName);
		failed = true;
		return;
	}

	fseek(rom, 0, SEEK_END);
	this->romSize = ftell(rom);
	fseek(rom, 0, SEEK_SET);

	uint8_t digits[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10, 0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0, 0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0, 0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0, 0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80, 0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80 };
	memcpy(&this->memory[0], (void*)digits, sizeof(digits));

	//std::copy(this->memory, this->memory + (16 * 5), digits); //copy predefined sprites into the memory

	fread(&this->memory[512], 1, this->romSize, rom);
}

CPU::~CPU()
{

}

void CPU::Clock()
{
	uint16_t opcode = *(uint16_t*)&this->memory[this->PC];
	opcode = SDL_Swap16(opcode);

	this->ProcessInstruction(opcode);
}

void CPU::ProcessInstruction(uint16_t opcode)
{
	//printf("Process opcode: %04X\n", opcode);
	uint16_t lastNibble = (opcode & 0xF000);

	this->PC += 2;

	switch (lastNibble)
	{
	case 0x0000:
	{
		if (opcode == 0x00E0)
		{
			memset(this->display, 0, sizeof(this->display));
		}
		else if (opcode == 0x00EE)
		{
			this->PC = this->stack.top();
			this->stack.pop();
		}
		else
		{
			printf("Unsupported opcode: %04X\n", opcode);
		}
		break;
	}
	case 0x1000:
	{
		this->PC = (opcode & 0x0FFF);
		break;
	}
	case 0x2000:
	{
		this->stack.push(this->PC);
		this->PC = (opcode & 0x0FFF);
		break;
	}
	case 0x3000:
	{
		uint16_t kk = (opcode & 0x00FF);
		uint8_t x = ((opcode & 0x0F00) >> 8);

		if (V[x] == kk)
			PC += 2;

		break;
	}
	case 0x4000:
	{
		uint16_t kk = (opcode & 0x00FF);
		uint8_t x = ((opcode & 0x0F00) >> 8);

		if (V[x] != kk)
			PC += 2;

		break;
	}
	case 0x5000:
	{
		uint8_t x = ((opcode & 0x0F00) >> 8);
		uint8_t y = ((opcode & 0x00F0) >> 4);

		if (V[x] == V[y])
			this->PC += 2;

		break;
	}
	case 0x6000:
	{
		uint16_t kk = (opcode & 0x00FF);
		uint8_t x = ((opcode & 0x0F00) >> 8);

		V[x] = kk;
		break;
	}
	case 0x7000:
	{
		uint16_t kk = (opcode & 0x00FF);
		uint8_t x = ((opcode & 0x0F00) >> 8);

		V[x] = V[x] + kk;

		break;
	}
	case 0x8000:
	{
		uint8_t firstNibble = (opcode & 0x000F);
		uint8_t x = ((opcode & 0x0F00) >> 8);
		uint8_t y = ((opcode & 0x00F0) >> 4);

		switch (firstNibble)
		{
		case 0: V[x] = V[y]; break;
		case 1: V[x] = V[x] | V[y]; break;
		case 2: V[x] = V[x] & V[y]; break;
		case 3: V[x] = V[x] ^ V[y]; break;
		case 4: V[15] = (uint8_t)(V[x] + V[y] > 255); V[x] = V[x] + V[y]; break;
		case 5: V[15] = (uint8_t)(V[x] > V[y]); V[x] = V[x] - V[y]; break;
		case 6: V[15] = (V[x] & 0b00000001); V[x] = (V[x] >> 1);
		case 7: V[15] = (uint8_t)(V[y] > V[x]); V[x] = V[y] - V[x]; break;
		case 0xE: V[15] = (V[x] & 0b10000000); V[x] = (V[x] << 1); break;
		}
		break;
	}
	case 0x9000:
	{
		uint8_t x = ((opcode & 0x0F00) >> 8);
		uint8_t y = ((opcode & 0x00F0) >> 4);

		if (V[x] != V[y])
			this->PC += 2;

		break;
	}
	case 0xA000:
	{
		this->I = (opcode & 0x0FFF);
		break;
	}
	case 0xB000:
	{
		this->PC = ((opcode & 0x0FFF) + V[0]);
		break;
	}
	case 0xC000:
	{
		uint8_t x = ((opcode & 0x0F00) >> 8);
		uint8_t kk = (opcode & 0x00FF);

		srand(time(NULL));
		V[x] = ((rand() % 256) & kk);
		break;
	}
	case 0xD000:
	{
		uint8_t vx = ((opcode & 0x0F00) >> 8);
		uint8_t vy = ((opcode & 0x00F0) >> 4);
		uint8_t n = (opcode & 0x000F);

		V[15] = 0;

		for (int y = 0; y < n; y++)
		{
			uint8_t mem = this->memory[I + y];

			for (int x = 0; x < 8; x++)
			{
				//modulo used to wrap around the pixels to the opposite sides if it reaches the max pixels
				//printf("S: %d - %d | %d - %d\n", x, y, vx, vy);
				const uint8_t dX = (V[vx] + x) % 64;
				const uint8_t dY = (V[vy] + y) % 32;
				uint8_t pixel = (mem >> (7 - x)) & 0x1;

				if (display[dX][dY] && pixel)
					V[15] = (display[dX][dY] && pixel);

				display[dX][dY] = display[dX][dY] ^ pixel;
			}
		}

		break;
	}
	case 0xE000:
	{
		uint8_t x = ((opcode & 0x0F00) >> 8);
		uint8_t lastTwoNibble = (opcode & 0x00FF);
		uint16_t pressed = (Keyboard >> V[x]) & 0x01;

		switch (lastTwoNibble)
		{
		case 0x9E: if (pressed) this->PC += 2; break; //key down
		case 0xA1: if (!pressed) this->PC += 2; break; //key up
		}

		break;
	}
	case 0xF000:
	{
		uint8_t x = ((opcode & 0x0F00) >> 8);
		uint8_t lastTwoNibble = (opcode & 0x00FF);

		switch (lastTwoNibble)
		{
		case 0x07:
		{
			V[x] = DelayTimer;
			break;
		}
		case 0x0A:
		{
			WaitForKey = true;
			WaitForKeyX = x;

			this->PC -= 2;

			break;
		}
		case 0x15:
		{
			DelayTimer = V[x];
			break;
		}
		case 0x18:
		{
			SoundTimer = V[x];
			break;
		}
		case 0x1E:
		{
			I = I + V[x];
			break;
		}
		case 0x29:
		{
			I = V[x] * 5;
			break;
		}
		case 0x33:
		{
			this->memory[I + 0] = (uint8_t)((V[x] / 100) % 10);
			this->memory[I + 1] = (uint8_t)((V[x] / 10) % 10);
			this->memory[I + 2] = (uint8_t)(V[x] % 10);
			break;
		}
		case 0x55:
		{
			for (int i = 0; i <= x; i++)
			{
				this->memory[I + i] = V[i];
			}
			break;
		}
		case 0x65:
		{
			for (int i = 0; i <= x; i++)
			{
				V[i] = this->memory[I + i];
			}
			break;
		}
		}

		break;
	}
	default:
		printf("Unimplemented instruction: %04X\n", opcode);
		break;
	}
}