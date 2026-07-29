/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChild.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 22:25:37 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/29 22:30:00 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#include "Client.hpp"
#include "Request.hpp"
#include <string>
class	Server;

class	CgiChild
{
public:
	CgiChild(Client& r);
	CgiChild(CgiChild const& other);
	~CgiChild();

	CgiChild& operator=(CgiChild const& other);
	
	int		set_cgi();
	int		execute_script(Server& server);
	int		create_args(std::string const& interpreter);
	int		create_env(Request const& r);
	int		fork_child(Server& server);
	char**	getArgs(void) const;
	char**	getEnv(void) const;
	std::string const&	getFilename(void) const;
	std::string const&	getFilepath(void) const;
	std::string const&	getExtension(void) const;
	std::string&	getOutput(void);
	Client&	getClientOwner(void);
	int		getPid(void) const;
	int *	getReadFromScriptPipe(void);
	void	setReadFromScriptPipe(int* pipe_fd);
	int *	getWrite2Script(void);
	void	setWrite2Script(int* pipe_fd);
	void	appedOutput(std::string const& str);
	void	setFilename(std::string const& str);
	void	setFilepath(std::string const& str);
	void	setExtension(std::string const& str);
	time_t			getLastComm(void) const;
	void			setLastComm(void);
	bool		is_script_running;
	bool		is_done;
private:
	char	**_env;
	char	**_args;
	Client&	_client_owner;
	time_t		_last_communication;
	std::string		_file_name;
	std::string		_interpreter;
	std::string		_file_path;
	std::string		_extension;
	std::string		_output;
	int		_read_from_script_pipe[2];
	int		_write_2_script[2];
	int		_pid;
};
