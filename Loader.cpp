#include "Functions.hpp"
#include "Loader.hpp"

using namespace boost::asio;
using error_code = boost::system::error_code;

Loader::Loader(std::unique_ptr<boost::asio::ip::tcp::socket>&& socket)
	: socket{ std::move(socket) }, Func{ new Functions(this)}
{
	_map_init();
	_async_read();
}

void Loader::command()
{
	std::cout << "Enter command:   ";
}

void Loader::_async_write(const char* mess)
{
	std::string str(mess);
	str += '\f';
	async_write(*socket, buffer(str.data(), str.size()), [](error_code ec, size_t bytes) {
		if (ec) {
			std::cerr << "Ошибка отпраки сообщения: " << ec.message() << std::endl;
		}
	});
}

void Loader::_async_write(std::string mess)
{
	mess += '\f';
	async_write(*socket, buffer(mess.data(), mess.size()), [](error_code ec, size_t bytes) {
		if (ec) {
			std::cerr << "Ошибка отпраки сообщения: " << ec.message() << std::endl;
		}
		});
}

void Loader::_async_write(Com com)
{
	std::string str;
	str += std::to_string((int)com);
	str += " \f";
	async_write(*socket, buffer(str.data(), str.size()), [](error_code ec, size_t bytes) {
		if (ec) {
			std::cerr << "Ошибка отпраки сообщения: " << ec.message() << std::endl;
		}
		});
}

void Loader::_async_read()
{
	async_read_until(*socket, buf, '\f', [&](error_code ec, size_t bytes) {
		_get_command_from_buf(ec, bytes); });
}

void Loader::_get_command_from_buf(boost::system::error_code ec, size_t bytes)
{
	if (ec == error::eof || ec == error::connection_reset) {
		std::cout << socket.get()->remote_endpoint().address().to_string() << " разорвал соединение.\n";
		(*socket).close();
		delete this;
		return;
	}
	else if (ec) {
		std::cerr << "Ошибка при обработке информации: " << ec.message() << std::endl;
		(*socket).close();
		return;
	}

	std::istream data(&buf);
	std::string command;
	std::getline(data, command, '\f');

	std::string _command;
	std::string command_args;
	auto i = command.find(' ');
	if (i != std::string::npos) {
		_command = command.substr(0, command.find(' '));
		command_args = command.substr(command.find(' ') + 1);
	}
	else {
		_command = command;
		command_args = "";
	}

	auto pair = commands.find(_command);
	if (pair == commands.end()) {
		_async_write(Com::command_not_found);
	}
	else {
		pair->second(command_args);
	}
	
	if (_command != "/loadfile")
	{
		_async_read();
	}
}

void Loader::_map_init()
{
	commands["/hello"] = Func->hello;
	commands["/files"] = Func->files_on_server;
	commands["/help"] = Func->help;
	commands["/loadfile"] = Func->load_file;
}
