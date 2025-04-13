#pragma once

#include <string>

class Loader;

class Functions
{
private:
	static inline Loader* obj;

	static void _load_file();

public:
	Functions(Loader* _obj);

	static void null(std::string);
	static void command_not_found(std::string);
	static void hello(std::string);
	static void files_on_server(std::string);
	static void help(std::string);
	static void load_file(std::string);
	static void message(std::string);
	static void message(const char*);
};

