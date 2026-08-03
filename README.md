*This project has been created as part of the 42 curriculum by acamargo, barmarti.*

# BeignetServ (webserv)

## Description

BeignetServ is an HTTP server written from scratch in C++98, using a single `epoll`-based
event loop to handle every client non-blockingly (accepting connections, reading
requests and writing responses, no matter how many clients are connected at once).

On startup, it reads a configuration file, builds the list of servers/ports/routes it
describes, and opens one listening socket per server. For every incoming connection the
server parses the raw HTTP request (headers, chunked or `Content-Length` bodies,
multipart bodies for uploads), matches the request's URL against the routing rules of
the corresponding server (the `location` blocks), and dispatches it to the matching
handler:

- **Static files**: `GET` reads a file from disk (or lists a directory when
  `autoindex` is enabled) and returns it with the right status code and headers.
- **Uploads**: `POST` writes the request body to the configured `upload_store`
  directory.
- **Deletion**: `DELETE` removes the targeted file from disk.
- **CGI**: if the requested file's extension matches a `cgi` directive, the server
  forks, sets up the CGI environment variables (`QUERY_STRING`, `CONTENT_LENGTH`,
  etc.) and pipes, and executes the mapped interpreter. The project has been tested
  with Python, JavaScript (Node) and compiled C/C++ CGI programs.
- **Redirections**: a `return` directive on a location short-circuits the request
  with the configured HTTP redirect code. If the file extention .ws format is used then the servers will 
  automaticly looks for index_cgi and executes it. If there is no index_cgi the name of the 
  file won't be replaced.

On top of this, the server also manages `Set-Cookie`/`Cookie` headers and a small
session mechanism (demonstrated by a cookie-clicker demo page), and falls back to
built-in or configured custom error pages (403, 404, 413, 50X, ...) whenever a request
cannot be completed.

## Instructions

Build the server with:

```
make
```

Then run it with a configuration file:

```
./WebServ <conffile.conf>
```

Once running, connect to it from a browser at `http://localhost:<PORT>` (the port is
defined in the configuration file). The homepage lets you try out all the core features
of the server.


When you want to stop the execution the SIGINT signal (ctrl + c) will stop the program proprely.

Then to clear the directory from binari an objects files use

```
make fclean
```

## Configuration file

The syntax is inspired by NGINX: the file is made of one or more `server { ... }`
blocks, each containing one or more `location { ... }` blocks. Every directive ends
with a `;`, and `#` starts a comment.

### `server` block directives

| Directive              | Description                                                                 |
|-------------------------|-------------------------------------------------------------------------------|
| `listen`                | Address/port to bind to. Accepts `ip:port`, `ip` alone or `port` alone (missing parts default to `0.0.0.0` and `8080`). |
| `server_name`           | Name of the virtual server (informational).                                  |
| `root`                  | Default filesystem root used by locations that don't set their own `root`.   |
| `index`                 | Default file served when a directory is requested.                           |
| `error_page`            | One or more HTTP status codes followed by the path of the page to serve for them, e.g. `error_page 500 502 503 504 /errors/50x.html;`. |
| `client_max_body_size`  | Maximum size accepted for a request body, e.g. `50`, `1k`, `10m`, `1g` (default `1m`). |

### `location` block directives

| Directive              | Description                                                                 |
|-------------------------|-------------------------------------------------------------------------------|
| `methods`               | Space-separated list of accepted HTTP methods for this route (e.g. `GET POST DELETE`). `POST` requires `upload_store` to be set. |
| `root`                  | Filesystem directory this location maps to (overrides the server's `root`). |
| `index`                 | Default file served when the route resolves to a directory.                 |
| `index_cgi`             | CGI script used as the default "index" for this location.                   |
| `autoindex`             | `on` or `off` — enables/disables directory listing when no index is found.  |
| `upload_store`          | Directory where uploaded files (`POST`) are written. Required to allow `POST`. |
| `cgi`                   | Maps a file extension to an interpreter, e.g. `cgi .py /usr/bin/python3;`. Can be repeated for several extensions. |
| `return`                | HTTP redirection: a 3xx code, optionally followed by a target URL, e.g. `return 301 /new-page;`. |
| `client_max_body_size`  | Same as the server-level directive, but scoped to this location (overrides it). |

A location must define either a path or an `upload_store`, and each server needs at
least one location. See [`conf/valid.conf`](conf/valid.conf) for a complete, working
example with several servers, ports and CGI mappings.

## Resources

- [HTTP/1.1 RFC 9110/9112](https://httpwg.org/specs/) — reference for the protocol semantics and message syntax.
- [MDN Web Docs – HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP) — general reference used throughout the project (headers, status codes, methods, CGI, cookies...).
- [NGINX documentation](https://nginx.org/en/docs/) — used as inspiration for the configuration file syntax.
- [NotebookLM](https://notebook.google.com/) — used to research and summarize specific topics around NGINX, HTTP and the relevant RFCs.

**AI usage:** Claude was used to generate the static HTML pages of the demo website and
a few auxiliary scripts (not the core server logic in C++). NotebookLM was used as a
research aid to better understand NGINX behaviour, HTTP semantics and the RFC.
