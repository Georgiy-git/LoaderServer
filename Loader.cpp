#include "Loader.hpp"

using namespace boost::asio;
using error_code = boost::system::error_code;

Loader::Loader(std::unique_ptr<boost::asio::ip::tcp::socket>&& socket) : socket{std::move(socket)}
{
	_async_write("Подключение выполнено.\nВведите команду:  ");
}

void Loader::command()
{
	std::cout << "Enter command:   ";
}

void Loader::_async_write(const char* mess)
{
	std::string str(mess);
	async_write(*socket, buffer(str.data(), str.size()), [](error_code ec, size_t bytes) {
		if (ec) {
			std::cerr << "Ошибка отпраки сообщения: " << ec.message() << std::endl;
		}
	});
}

void Loader::_async_write(std::string mess)
{
	async_write(*socket, buffer(mess.data(), mess.size()), [](error_code ec, size_t bytes) {
		if (ec) {
			std::cerr << "Ошибка отпраки сообщения: " << ec.message() << std::endl;
		}
		});
}

void Loader::_async_read()
{
	async_read(*socket, buf, transfer_at_least(1), [&](error_code ec, size_t bytes) {
		_get_command_from_buf(ec, bytes); });
}

void Loader::_get_command_from_buf(boost::system::error_code ec, size_t bytes)
{
	if (ec == error::eof) {
		std::cout << socket.get()->remote_endpoint().address().to_string() << " разорвал соединение.\n";
		(*socket).close();
		delete this;
		return;
	}
	else if (ec) {
		std::cerr << "Ошибка при обработке информации: " << ec.message() << std::endl;
		_async_read();
		return;
	}

	std::istream data(&buf);
	std::string command;
	data >> command;

	std::cout << command << std::endl;
}

void Loader::_password()
{
	std::cout << "Enter password:\n";
}

void Loader::_map_init()
{
	//commands.insert{ "/hello"s, [] {cout << "Hello!"; } };
}
