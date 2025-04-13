#include "Loader.hpp"
#include "Functions.hpp"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <boost/asio.hpp>

using namespace boost::asio;

void Functions::_load_file()
{
	async_write(*obj->socket, obj->buf, [](boost::system::error_code ec, size_t bytes) {
		if (ec) {
			std::string line;
			line += "Ошибка: ";
			line += ec.what();
			message(line);
		}
		obj->_async_read();
	});
}

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
	answer += "Сервер: привет, ";
	answer += obj->socket->remote_endpoint().address().to_string();
	answer += '\n';
	message(answer);
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
		message(str + e.what());
		return;
	}

	std::string line;
	line += std::to_string((int)Com::files_on_server);
	line += ' ';
	for (const auto& entry : std::filesystem::directory_iterator(current_path)) {
		line += entry.path().filename().string();
		line += '&';
	}
	obj->_async_write(line);
}

void Functions::help(std::string)
{
	std::string line;
	line += "/hello\n";
	line += "/files - файлы на сервере\n";
	line += "/help - доступные команды\n";
	line += "/loadfile (filename) - скачать файл\n";
	message(line);
}

void Functions::load_file(std::string file_name)
{
	std::string line;
	std::ifstream file(file_name.c_str(), std::ios::binary);
	if (!file.is_open()) {
		message("Ошибка: сервер не смог открыть файл.\n");
		obj->_async_read();
		return;
	}

	size_t file_size{};
	try {
		file_size = std::filesystem::file_size(file_name);
	}
	catch (...) {
		std::cerr << "Ошибка при получении размера файла.\n";
		message("Ошибка файловой системы сервера.\n");
		obj->_async_read();
		return;
	}

	line += std::to_string((int)Com::load_file);
	line += ' ';
	line += std::to_string(file_size);
	line += '&';
	line += file_name;
	line += '\f';

	std::ostream output(&obj->buf);
	output << file.rdbuf();
	file.close();

	async_write(*(obj->socket), buffer(line.data(), line.size()),
		[&](boost::system::error_code ec, size_t bytes) { _load_file(); });
}

void Functions::message(std::string mess)
{
	mess = ' ' + mess;
	mess = std::to_string((int)Com::message) + mess;
	obj->_async_write(mess);
}

void Functions::message(const char* mess)
{
	std::string line = mess;
	line = ' ' + line;
	line = std::to_string((int)Com::message) + line;
	obj->_async_write(line);
}
