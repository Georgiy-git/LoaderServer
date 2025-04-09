#include "Loader.hpp"
#include "Functions.hpp"

#include <filesystem>

Functions::Functions(Loader* _obj)
{
	obj = _obj;
}

void Functions::null(std::string no)
{
	obj->_async_write(Com::null);
}

void Functions::command_not_found(std::string no)
{
	obj->_async_write(Com::command_not_found);
}

void Functions::hello(std::string no)
{
	std::string answer;
	answer += std::to_string((int)Com::message);
	answer += '&';
	answer += "Сервер: привет, ";
	answer += obj->socket->remote_endpoint().address().to_string();
	answer += '\n';
	obj->_async_write(answer);
}

void Functions::files_on_server(std::string no)
{
	std::filesystem::path current_path;

	try {
		current_path = std::filesystem::current_path();
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Ошибка файловой системы: " << e.what() << std::endl;
		std::string str = "На сервере произошла ошибка файловой системы: ";
		obj->_async_write(str + e.what());
		return;
	}

	std::string line;
	line += std::to_string((int)Com::files_on_server);
	line += '&';
	for (const auto& entry : std::filesystem::directory_iterator(current_path)) {
		line += entry.path().filename().string();
		line += '&';
	}
	obj->_async_write(line);
}

void Functions::help(std::string)
{
	std::string line;
	line += std::to_string((int)Com::message);
	line += '&';
	line += "/hello\n";
	line += "/files - файлы на сервере\n";
	line += "/help - доступные команды\n";
	obj->_async_write(line);
}
