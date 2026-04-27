/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acamargo <acamargo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 15:12:53 by acamargo          #+#    #+#             */
/*   Updated: 2026/04/27 15:56:35 by acamargo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Trouvé sur le Notion de Clothilde
//
//                                                                  ┌─────────────────────────┐
//                                                                  │        DÉMARRAGE         │
//                                                                  └─────────────┬───────────┘
//                                                                                │
//                                                                                ▼
//                                                             socket() → bind() → listen()
//                                                                                │
//                                                                                ▼
//                                                               listenFd (IP:PORT, non bloquant)
//                                                                                │
//                                                                                ▼
//                                                                epoll_ctl(ADD, listenFd)
//                                                                                │
//                                                                                ▼
//                                                                   ┌────────────────────┐
//                                                                   │   epoll_wait()     │  ← le thread dort ici
//                                                                   └─────────┬──────────┘
//                                                                             │
//                                                    ┌────────────────────────┴────────────────────────┐
//                                                    │                                                   │
//                                                    ▼                                                   ▼
//                                          listenFd prêt (EPOLLIN)                           clientFd prêt (EPOLLIN)
//                                          (connexion(s) en attente)                         (données à lire)
//                                                    │                                                   │
//                                                    ▼                                                   ▼
//                                           handleAccept(listenFd)                           handleClient(clientFd)
//                                                    │                                                   │
//                                                    │   accept() en boucle                              │
//                                                    │   ────────────────────                            │
//                                                    │   tant que accept() réussit                       │
//                                                    │                                                   │
//                                                    ▼                                                   ▼
//                                           ┌──────────────────────┐                       recv(clientFd)
//                                           │  accept()            │                       ─────────────
//                                           │  → nouveau clientFd  │                       lire les octets
//                                           └─────────┬────────────┘                       stocker dans buffer
//                                                     │
//                                                     ▼
//                                           setNonBlocking(clientFd)
//                                                     │
//                                                     ▼
//                                           epoll_ctl(ADD, clientFd)
//                                                     │
//                                                     ▼
//                                                clientFd surveillé
//                                                par epoll (EPOLLIN)
//                                                     │
//                                                     ▼
//                                         buffer complet ?
//                                         ─────────────────
//                                         headers ? body ?
//                                                     │
//                           ┌─────────────────────────┴─────────────────────────┐
//                           │                                                     │
//                           ▼                                                     ▼
//                      NON (requête partielle)                          OUI (requête complète)
//                           │                                                     │
//                           ▼                                                     ▼
//                  retour à epoll_wait()                          parser HTTP + construire réponse
//                                                                                  │
//                                                                                  ▼
//                                                                      epoll_ctl(MOD, clientFd,
//                                                                               EPOLLOUT)
//                                                                                  │
//                                                                                  ▼
//                                                                  clientFd prêt (EPOLLOUT)
//                                                                                  │
//                                                                                  ▼
//                                                                     send(clientFd)
//                                                                     ─────────────
//                                                                     envoi partiel possible
//                                                                                  │
//                                                                                  ▼
//                                                                   réponse entièrement envoyée ?
//                                                                                  │
//                                          ┌───────────────────────────┴───────────────────────────┐
//                                          │                                                           │
//                                          ▼                                                           ▼
//                                OUI + Connection: close                                NON ou keep-alive
//                                          │                                                           │
//                                          ▼                                                           ▼
//                                  close(clientFd)                                         epoll_ctl(MOD,
//                                  ───────────────                                         clientFd,
//                                  retirer d’epoll                                        EPOLLIN)
//                                                                                                │
//                                                                                                ▼
//                                                                                      retour à epoll_wait()

#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Error: args" << std::endl;
		return (EXIT_FAILURE);
	}
	struct addrinfo addrinfo;
	memset(&addrinfo, 0, sizeof addrinfo);
	struct addrinfo *result;
	addrinfo.ai_family = AF_INET;
	addrinfo.ai_socktype = SOCK_STREAM;
	addrinfo.ai_protocol = 0;
	int error = getaddrinfo("localhost", "8888", &addrinfo, &result);
	if (error != 0)
	{
		perror("?\n");
		exit(error);
	}
	int sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (connect(sfd, result->ai_addr, result->ai_addrlen) != 0)
	{
		perror("connect");
		exit(1);
	}
	send(sfd, "hola", 4, 0);
	close(sfd);

	(void)argv;
	return EXIT_SUCCESS;
}
