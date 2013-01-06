#pragma once

#include "Lua.h"

class Scripting{
	public:
		Scripting();
		~Scripting();
	private:
		Lua L;
};
