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

#include "run_server.hpp"
#include <iostream>
#include <stdlib.h>
#include <configParser.hpp>

int	main(int ac, char **av)
{
	(void)av;
	if (ac != 2)
	{
		std::cerr << "Error: args" << std::endl;
		return (EXIT_FAILURE);
	}
	configParser	server;
	try {
		server.parse(av[1]);
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	int exit_sts = run(server.getServers());
	return (exit_sts);
}
