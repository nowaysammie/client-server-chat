#include "Server.h"
#include "States.h"
#include <regex>
#include <iostream>

bool Server::testIp(std::string ip)
{
	std::regex ipv4("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");
	bool state = false;
	if (std::regex_match(ip, ipv4))
	{
		state = true;
	}
	return state;
}

uint8_t Server::init(char *server_ip)
{
	uint8_t state = SRV_NO_VALID_IP;
	std::string temp = server_ip;
	if (testIp(temp))
	{
		state = network_module.init(server_ip);
		if (state == E_CONNECT)
		{
			shutdown();
			state = SRV_SHUTDOWN;
		}
	}
	return state;
}

uint8_t Server::toPoll()
{
	return network_module.toPoll();
}

void Server::authorization(Package package, int32_t client_socket)
{
	if (package.header.payload < LOGIN_SIZE)
	{
		char login[LOGIN_SIZE];
		std::regex login_regex("^[a-zA-Z0-9_]{4,49}$");
		strncpy(login, package.data.s_auth_request.login, LOGIN_SIZE);
		if (std::regex_match(login, login_regex) != true)
		{
			sendErrorPackage(client_socket, E_LOGIN_SIZE);
		}
		else
		{
			uint32_t client_uid = network_module.getClientUid(std::string(package.data.s_auth_request.login));
			uint8_t state = storage.appendClient(client_uid, login, client_socket);

			Package reply_package;
			char buffer[BUFFER_SIZE];
			package_manager.createAuthConfirmPackage(&reply_package, client_uid);
			package_manager.transferToBuffer(reply_package, buffer);
			state = network_module.sendMessage(client_socket, buffer);
			if (state != SUCCESS)
			{
				network_module.removeClient(client_socket);
			}
		}
	}
	else
	{
		sendErrorPackage(client_socket, E_LOGIN_SIZE);
	}
}

void Server::sendErrorPackage(int32_t client_socket, uint8_t error_code)
{
	Package package;
	char buffer[BUFFER_SIZE];
	package_manager.createErrorPackage(&package, error_code);
	package_manager.transferToBuffer(package, buffer);
	uint8_t state = network_module.sendMessage(client_socket, buffer);
	if (state != SUCCESS)
	{
		doOffilneUser(client_socket);
	}
}

uint8_t Server::instruction()
{
	char buffer[BUFFER_SIZE];
	std::cin.getline(buffer, BUFFER_SIZE);
	uint8_t state = SRV_OK;
	if (strstr(buffer, "/shutdown") != NULL)
	{
		state = SRV_SHUTDOWN;
	}
	return state;
}

void Server::sendUserList(Package package, int32_t client_socket)
{
	if (package.header.payload <= PAYLOAD_MAX)
	{
		Package p_user_list;
		std::map<std::string, uint32_t> u_list = storage.getUserList(package.data.s_user_list_request.client_uid);
		char buffer[BUFFER_SIZE];
		package_manager.createUserListPackage(&p_user_list, u_list);
		package_manager.transferToBuffer(p_user_list, buffer);
		uint8_t state = network_module.sendMessage(client_socket, buffer);
		if (state != SUCCESS)
		{
			doOffilneUser(client_socket);
		}
	}
	else
	{
		sendErrorPackage(client_socket, E_DATA);
	}
}

void Server::forwardMsg(Package package, uint32_t client_socket, char *buffer)
{
	if (package.data.s_msg.src_uid != package.data.s_msg.dest_uid)
	{
		if (package.header.payload <= MSG_SIZE)
		{
			uint8_t state = storage.appendFriend(package.data.s_msg.src_uid, package.data.s_msg.dest_uid);
			if (state != E_FRIEND_WRONG)
			{
				int32_t dest_socket;
				state = storage.getClientSocket(package.data.s_msg.dest_uid, &dest_socket);
				if (state != E_FRIEND_WRONG)
				{
					if (dest_socket != 0)
					{
						state = network_module.sendMessage(dest_socket, buffer);
						if (state == E_CONNECT)
						{
							doOffilneUser(dest_socket);
						}
					}
					else
					{
						sendErrorPackage(client_socket, E_FRIEND_OFFLINE);
					}
				}
				else
				{
					sendErrorPackage(client_socket, E_FRIEND_WRONG);
				}
			}
			else
			{
				sendErrorPackage(client_socket, E_FRIEND_WRONG);
			}
		}
		else
		{
			sendErrorPackage(client_socket, E_MSG_SIZE);
		}
	}
	else
	{
		sendErrorPackage(client_socket, E_SELF_MSG);
	}
}

uint8_t Server::eventHandler()
{
	pollfd *ready_fd = network_module.readyFd();
	uint8_t state;
	if (ready_fd != NULL)
	{
		char buffer[BUFFER_SIZE];
		ready_fd->revents = 0;
		if (ready_fd->fd == network_module.getFd(0)->fd)
		{
			state = instruction();
			if (state == SRV_SHUTDOWN)
			{
				shutdown();
			}
			else
			{
				std::cout << "Wrong instruction!" << std::endl;
				state = SRV_OK;
			}
		}
		else if (ready_fd->fd == network_module.getFd(1)->fd)
		{
			state = network_module.appendClient();
			if (state != SUCCESS)
			{
				state = SRV_POLL_E_CONNECT;
			}
			else
			{
				state = SRV_OK;
			}
		}
		else
		{
			state = network_module.getMessage(ready_fd->fd, buffer);
			if (state == E_DATA)
			{
				sendErrorPackage(ready_fd->events, E_DATA);
			}
			else if (state == E_CONNECT)
			{
				doOffilneUser(ready_fd->fd);
			}
			else
			{
				Package package;
				state = package_manager.parseToPackage(&package, buffer);
				if (state != SUCCESS)
				{
					sendErrorPackage(ready_fd->fd, E_DATA);
				}
				else
				{
					switch (package.header.cmd)
					{
					case AUTH_REQUEST:
						authorization(package, ready_fd->fd);
						break;
					case USER_LIST_REQUEST:
						sendUserList(package, ready_fd->fd);
						break;
					case MSG:
						forwardMsg(package, ready_fd->fd, buffer);
						break;
					case ERROR_MSG:
						clientErrorHandler(package, ready_fd->fd);
						break;
					default:
						sendErrorPackage(ready_fd->fd, E_DATA);
					}
				}
				state = SRV_OK;
			}
		}
	}
	else
	{
		state = SRV_HANDLE_END;
	}
	return state;
}

void Server::clientErrorHandler(Package package, int32_t client_socket)
{
	uint8_t state = E_LOGIN_WRONG;
	if (package.data.s_error_msg.error_code == E_LOGIN_WRONG)
	{
		while (state != SUCCESS)
		{
			state = storage.deleteClient(client_socket);
		}
	}
}

void Server::notifyFriends(uint32_t client_uid, std::set<uint32_t> waiting_friends)
{
	if (!waiting_friends.empty())
	{
		std::set<uint32_t>::iterator iter;
		for (iter = waiting_friends.begin(); iter != waiting_friends.end(); iter++)
		{
			Package package;
			char buffer[BUFFER_SIZE];
			package_manager.createExitFriendPackage(&package, client_uid);
			package_manager.transferToBuffer(package, buffer);
			int32_t dest_sock;
			storage.getClientSocket(*iter, &dest_sock);
			network_module.sendMessage(dest_sock, buffer);
		}
	}
}

void Server::doOffilneUser(int32_t client_socket)
{
	uint32_t client_uid = storage.getClientUidToSocket(client_socket);
	std::set<uint32_t> waiting_friends = storage.offlineClient(client_socket);
	notifyFriends(client_uid, waiting_friends);
	network_module.removeClient(client_socket);
}

void Server::shutdown()
{
	network_module.closeSocket();
}
