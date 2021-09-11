#pragma once

#include <Main.h>

class CPU
{
public:
	CPU(const char* fileName);
	~CPU();

	void Clock();
	void ProcessInstruction(uint16_t opcode);
	inline uint64_t GetCurrentTimeEx()
	{
		auto TimeDifference = std::chrono::high_resolution_clock::now() - StartTime;
		return std::chrono::duration_cast<std::chrono::microseconds>(TimeDifference).count(); // Get Microseconds
	}

public:
	bool failed = false;
	int romSize = 0;
	std::chrono::steady_clock::time_point StartTime = std::chrono::high_resolution_clock::now();

	uint16_t Keyboard = 0;
	bool WaitForKey = false;
	uint8_t WaitForKeyX = 0;

	uint8_t memory[4096] = { 0 };
	bool display[64][32] = { 0 };

	uint8_t V[16] = { 0 };
	uint16_t I = 0;

	uint16_t PC = 512;
	//uint8_t SP = 0;

	std::stack<uint16_t> stack;

	uint8_t DelayTimer;
	uint8_t SoundTimer; //"they are automatically decremented at a rate of 60Hz"

};