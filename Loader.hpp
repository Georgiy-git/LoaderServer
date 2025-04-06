#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <boost/asio.hpp>

class Loader {
public:
	Loader(std::unique_ptr<boost::asio::ip::tcp::socket>&& socket);

	void command();

private:
	void _async_write(const char* mess);
	void _async_write(std::string mess);
	void _async_read();
	void _get_command_from_buf(boost::system::error_code ec, size_t bytes);

	void _password();
	void _map_init();

	std::unordered_map<std::string, std::function<int(std::string)>> commands;
	std::unique_ptr<boost::asio::ip::tcp::socket> socket;
	boost::asio::streambuf buf;
};