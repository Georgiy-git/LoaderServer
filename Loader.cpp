#include "Loader.hpp"

using namespace boost::asio;
using error_code = boost::system::error_code;

Loader::Loader(std::unique_ptr<boost::asio::ip::tcp::socket>&& socket) : socket{std::move(socket)} {
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
	str += '\f';
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

	auto pair = commands.find(command);
	if (pair == commands.end()) {
		_async_write(Com::command_not_found);
	}
	else {
		pair->second(command);
	}

	_async_read();
	_async_write(Com::unlock_write_command);
}

void Loader::_password()
{
	std::cout << "Enter password:\n";
}

void Loader::_map_init()
{
	//commands.insert{ "/hello"s, [] {cout << "Hello!"; } };
}