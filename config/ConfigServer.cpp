#include "ConfigServer.hpp"

ConfigServer::ConfigServer() { }

ConfigServer::ConfigServer(CommonDirective const &c) : common_directive(c), listen_port(80), listen_host("127.0.0.1") { }

ConfigServer::~ConfigServer() { }

CommonDirective  ConfigServer::getCommonDirective() { return common_directive; }
int ConfigServer::getListenPort() { return listen_port; }
std::string ConfigServer::getListenHost() { return listen_host; }
std::vector<std::string> ConfigServer::getServerName() { return server_name; }
std::vector<ConfigLocation> ConfigServer::getLocations() { return locations; }
std::map<std::string, std::string> ConfigServer::getSimpleDirective() { return simple_directive; }

int ConfigServer::identifyLocationBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);
	std::vector<std::string> block_name_arg = ft_split_space(block_name);

	if (block_name_arg.size() >= 2 && block_name_arg[0] == "location") {
		std::string url = block_name_arg[1];
		for (int i = 2; i < block_name_arg.size(); i++)
			url += " " + block_name_arg[i];
		
		ConfigLocation location(url, common_directive);
		location.parsingLocation(block_content);
		locations.push_back(location);
	} else if (block_name != "") {
		// return ERROR;
	}
	return SUCCESS;
}

int ConfigServer::parseServerDirecive(std::map<std::string, std::string> &simple) {

	if (simple.find(LISTEN) != simple.end()) {
		std::vector<std::string> listen = ft_split_space(simple[LISTEN]);
		if (listen.size() != 1)
			return ERROR;
		std::cout << "listen[1]: " << listen[0] << std::endl;
		if (listen[0].find(":") != std::string::npos) {
			std::cout << "this is :" << std::endl;
			std::vector<std::string> host_port = ft_split(listen[0], ":");
			if (host_port.size() != 2)
				return ERROR;
			// if (host_port[0] //) // 만약 호스트의 주소가 형식에 맞지 않으면 에러 처리
			// 	return ERROR;
			listen_host = host_port[0];

			char *endptr = 0;
			double d_port = std::strtod(host_port[1].c_str(), &endptr);
			int port = static_cast<int>(d_port);
			if (std::string(endptr) != "" || d_port != port || d_port < 0)
				return ERROR;
			listen_port = port;
		} else {
			bool isNumber = true;
			for (int i = 0; i < listen[0].size(); i++) {
				if (std::isdigit(listen[0][i]) == 0)
					isNumber = false;
			}
			if (isNumber) {
				char *endptr = 0;
				double d_port = std::strtod(listen[0].c_str(), &endptr);
				int port = static_cast<int>(d_port);
				if (std::string(endptr) != "" || d_port != port || d_port < 0)
					return ERROR;
				listen_port = port;
			} else {
				listen_host = listen[0];
			}
		}
		std::cout << "!!!! listen_host: " << listen_host << std::endl;
		std::cout << "!!!! listen_port: " << listen_port << std::endl;
	}

	if (simple.find(SERVER_NAME) != simple.end()) {
		std::vector<std::string> names = ft_split_space(simple[SERVER_NAME]);
		// if (names.size() < 1)
		// 	return ERROR;
		server_name = names;
	}
	return SUCCESS;
}

int ConfigServer::parsingServer(std::string const &block) {
	size_t pos;
	std::string modify_block;
	std::vector<std::string> blocks;

	modify_block = sperateBrace(block);
	if ((pos = modify_block.find(MAIN_SEPARATOR)) != std::string::npos) {
		parseSimpleDirective(simple_directive, common_directive, block.substr(0, pos));
		parseServerDirecive(simple_directive);
		blocks = ft_split(modify_block.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	} else {
		blocks = ft_split(modify_block, BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		identifyLocationBlock(blocks[i]);
	}
	return SUCCESS;
}