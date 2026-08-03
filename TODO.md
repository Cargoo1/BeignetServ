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

- [X] Conf file ❓
    *   location homepage {...} compte pas comme une erreur, et tu le laisse passer comme un valid location mais a mon avis ca a pas trop de sens, t'en penses quoi? ❓
            ➡️ REP-> J'avoue que j'ai du mal a trouver un cas où ca pourrais être une erreur. C'est à l'user de gerer son fichier conf. S'il veut appeler sa location homepage ou même YouTube il peut. C'est à lui de gerer.
                Tu pense toi que / est un equivalent à homepage et qu'il ne peut avoir d'équivalent ? et donc un fichier .conf ne peut contenir le mot clès "homepage" ? Pourquoi pas.
                    l:353

- [ ] POST METHOD BUG
    * si le filename est envoyer par le multipart body type ou dans le query string on met rien dans le fichier pour l'instant j'ai commenter

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
