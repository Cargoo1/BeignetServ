/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiChild.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 22:25:37 by acamargo          #+#    #+#             */
/*   Updated: 2026/07/18 23:49:59 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <string>
class	CgiChild
{
public:
	CgiChild();
	CgiChild(CgiChild const& other);
	~CgiChild();

	CgiChild& operator=(CgiChild const& other);
	
	int		create_args(std::string const& interpreter);
	int		create_env(Request const& r);
	void	fork_child();
	char const**	getArgs(void);
	char const**	getEnv(void);
	std::string const&	getFilename(void) const;
	std::string const&	getFilepath(void) const;
	std::string const&	getExtension(void) const;
	std::string const&	getOutput(void) const;
	int		getPid(void) const;
	int*	getPipe(void) const;
	void	appedOutput(std::string const& str);
	void	setFilename(std::string const& str);
	void	setFilepath(std::string const& str);
	void	setExtension(std::string const& str);
private:
	char	**_env;
	char	**_args;
	std::string		_file_name;
	std::string		_file_path;
	std::string		_extension;
	std::string		_output;
	int		_pipe_fd[2];
	int		_pid;
};
