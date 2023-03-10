#pragma once

#ifndef SCRIPT
#define SCRIPT

#include <vector>
#include <string>
#include "Parser.h"
#include "Lexor.h"

class ScriptHandler
{
public:
	ScriptHandler();
	~ScriptHandler();

	Lexor lexor;
	void runScript(std::vector<std::string> script, std::string argv);
};

#endif SCRIPT