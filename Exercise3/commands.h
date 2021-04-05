#pragma once

struct Command
{
	enum Type
	{
		Left,
		Right,
		Up,
		Down,
		Fire,
		Pause,
		DebugTextures,
		DebugBoxes,
		DebugGrid,
	};

	Type type;
	bool ended = false;
};
