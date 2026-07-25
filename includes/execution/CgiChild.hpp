/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChild.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 22:25:37 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/25 01:00:49 by acamargo         ###   ########.fr       */
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
	
	int		set_cgi();
	int		execute_script();
	int		create_args(std::string const& interpreter);
	int		create_env(Request const& r);
	int		fork_child();
	char**	getArgs(void) const;
	char**	getEnv(void) const;
	std::string const&	getFilename(void) const;
	std::string const&	getFilepath(void) const;
	std::string const&	getExtension(void) const;
	std::string&	getOutput(void);
	Client&	getClientOwner(void);
	int		getPid(void) const;
	int *	getOutputPipe(void);
	void	setOutputPipe(int* pipe_fd);
	int *	getInputPipe(void);
	void	setInputPipe(int* pipe_fd);
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
	int		_output_pipe[2];
	int		_input_pipe[2];
	int		_pid;
};
