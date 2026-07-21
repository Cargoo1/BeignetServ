/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChild.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 22:25:37 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/21 13:51:42 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#include "Client.hpp"
#include "Request.hpp"
#include <string>
class	CgiChild
{
public:
	CgiChild(Client& r);
	CgiChild(CgiChild const& other);
	~CgiChild();

	CgiChild& operator=(CgiChild const& other);
	
	int		execute_cgi();
	int		create_args(std::string const& interpreter);
	int		create_env(Request const& r);
	int		fork_child();
	char**	getArgs(void);
	char**	getEnv(void);
	std::string const&	getFilename(void) const;
	std::string const&	getFilepath(void) const;
	std::string const&	getExtension(void) const;
	std::string const&	getOutput(void) const;
	Client&	getClientOwner(void);
	int		getPid(void) const;
	int const*	getPipe(void) const;
	void	setPipe(int* pipe_fd);
	void	appedOutput(std::string const& str);
	void	setFilename(std::string const& str);
	void	setFilepath(std::string const& str);
	void	setExtension(std::string const& str);
	time_t			getLastComm(void) const;
	void			setLastComm(void);
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
	int		_pipe_fd[2];
	int		_pid;
};
