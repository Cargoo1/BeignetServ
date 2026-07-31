# TODO LIST

* CGI

- [x] Max body size

- [x] Chunked body test

- [x] Doing epoll before send/write!
    * mod epoll when done reading request

- [x] Requet queue

- [ ] quitting server when 50x

## TODO RVTEL!!

*   POST ✅
    * qd on fait post /testing ...... le fichier cree a pas le nom testing!✅

*   Conf file ✅
    * regler l'erreur qd y a pas un upload_store in cgi-bin avec POST (toujours ca)✅

### Changed
    * POST method ✔️
        - s'y n'a pas de body dans le fichier on le cree de toute facon, avant on le faisait pas ✔️

    * site2 sur port 8082
        - ajout d'une page hmtl et style un peu différente pour démontrer l'écoute des deux ports

## TOCHECK Cargo
    * Au parsing -> si port != IP:PORT j'ajouté des valeur par defauts 
            dflt ip = 0.0.0.0
            dflt port = 8080
            Si ajout de la feature throw une erreur: 
                ➜  BeignetServ git:(exec_server) ✗ ./WebServ conf/valid.conf
                gai: Servname not supported for ai_socktype
            configParser.cpp l:184 -> bool isIP(std::string listen);
            configParser.cpp l:192 -> void managePortSyntax(std::string &port);
            configParser.cpp l:485 -> void configParser::_validateAll();
                                l:496-497 -> else
		                                        managePortSyntax(this->_servers[i]._listen);
            
            Etant donné que getaddrinfo s'attends à avoir une adresse résaux et on lui envoie un hybride. Du coup utiliser ca pour avoir du virtual hosting ?

    * Exec : Je pense que la gestion de reception via recv_msg a un point flou avec les cgi. utils.cpp l:239,240
        if (bytes_read == 0)
		    return 1;
        à la fin du script on est half closed et laisse tourner. Le check qu'on a fait arrive plus tard.
        J'ai donc modifié pour gérer les différent cas de figure, mais je suis pas méga sûre de ce que j'ai fait. De ce que j'ai compris de ton code, di on à recu tout les bytes de rcv_msg mais que le body est pas parsé je pense que c'est une erreur, egalement si on est entrain de lire le body. Donc j'ai fait un fix en nested if pour voir. Dit moi ce que t'en penses. 
        (méga galére le truc de EPOLLIN et EPOLLOUT avec set_2_epoll ... J'ai mis une heure a comprendre qu'il fallait mettre le masque a zéro pour dire a epollwait d'attendre la fin du script)


