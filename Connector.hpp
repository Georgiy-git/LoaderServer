#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <optional>
#include "Loader.hpp"

using namespace boost::asio;

class Connector
{
private:
	const unsigned int port;
	io_context& io_context;
	ip::tcp::acceptor& acceptor;
	std::unique_ptr<boost::asio::ip::tcp::socket> socket;

	void _async_accept() {
		socket = std::make_unique<ip::tcp::socket>(io_context);
		acceptor.async_accept(*socket, [&](boost::system::error_code ec) { _join(ec, *socket); });
	}

	void _join(boost::system::error_code ec, ip::tcp::socket& _socket) {
		if (ec) {
			std::cerr << "Произошла ошибка подключения пользователя:  " << ec.message() << std::endl;
			_socket.close();
		}
		else {
			std::cout << "Новое подключение: " << _socket.remote_endpoint().address().to_string() << std::endl;
			Loader* loader = new Loader(std::move(socket));
		}
		_async_accept();
	}

public:
	Connector(boost::asio::io_context& io_context, ip::tcp::acceptor& acceptor, unsigned int port) :
		io_context{ io_context }, acceptor{ acceptor }, port{ port } {
		std::cout << "Сервер запущен на порту " << port << std::endl;
		_async_accept();
	}
};