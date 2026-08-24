*This project has been created as part of the 42 curriculum by stripet, odruke-s.*

# Webserv

## Table of Contents

[Description](#Description)

[Technical Stack](#Technical-Stack)

[Instructions](#Instructions)

[Implemented Scope and Limitations](#Implemented-Scope-and-Limitations)

[Individual Contributions](#Individual-Contributions)

[Resources](#Resources)


## Description

Webserv is an HTTP server written in C++98 as part of the 42 curriculum. The
project implements a configurable server capable of hosting static websites,
receiving uploaded files, deleting files, serving multiple configured ports,
and executing Python CGI scripts.

The program reads one configuration file, creates the configured listening
sockets, and manages client connections through a non-blocking `select()` event
loop. Requests are parsed by `Request`, connections are managed by `Client`,
and `ResponseFactory` dispatches each valid request to the appropriate response
handler.

Once launched with one of the provided configuration files, he server is
hosting a series of very simple sites who's only purpose is to test the
requested features for the project.

### Key Features

- HTTP/1.0 request-line parsing.
- Configurable servers, ports, roots, locations, indexes, and allowed methods.
- Static file serving with MIME-type detection.
- Directory indexes and optional automatic directory listings.
- Custom error pages with built-in error responses as a fallback.
- `GET`, `POST`, and `DELETE` handling.
- Text and multipart file uploads.
- Configurable upload storage paths and request body limits.
- HTTP redirects through location configuration.
- Python CGI execution for configured `.py` scripts, including query strings
  and POST input.
- Multiple listening servers from one configuration file.

## Technical Stack

- **Language:** C++98.
- **Compiler:** `c++` (the Makefile uses the compiler available as `c++`).
- **Build tool:** GNU Make or a compatible `make` implementation.
- **Networking:** POSIX TCP sockets and `select()`.
- **I/O:** Non-blocking sockets configured with `fcntl()`.
- **Filesystem:** POSIX filesystem APIs for static files, uploads, deletion,
  directory traversal, and directory listings.
- **CGI process model:** `pipe()`, `fork()`, `dup2()`, `execve()`, and
  `waitpid()`.
- **CGI interpreter:** Python 3, when a configuration maps an extension to a
  Python executable.

## Instructions

### Prerequisites

The project is intended to run on a POSIX/Linux environment and requires:

- A C++ compiler with C++98 support.
- `make`.
- Standard POSIX networking and filesystem support.
- Python 3 installed at the executable path configured for Python CGI, usually
  `/usr/bin/python3`.
- Write permission for any configured upload directory.
- Write permission for `/tmp`, where request bodies are temporarily stored.

No `.env` file or third-party library is required by the repository.

### Build

Run the commands from the repository root. Relative paths in the configuration
files and website roots are resolved from the current working directory.

```sh
make
```

The Makefile compiles all C++ sources under `src/` together with `main.cpp`
using `-Wall -Wextra -Werror --std=c++98 -g`.

Other available targets are:

```sh
make clean   # Remove object files
make fclean  # Remove object files and the executable
make re      # Clean and build again
```

### Run

With no argument, the server uses `configs/default.conf`:

```sh
./webserv
```

To select another configuration, pass exactly one file:

```sh
./webserv configs/default.conf
./webserv configs/config.conf
./webserv configs/galery.conf
./webserv configs/multiple.conf
```

The default configuration listens on `127.0.0.1:8080`. After starting the
server, open `http://127.0.0.1:8080/` in a browser.

### Configuration

Configuration files use `server` blocks and optional nested `location` blocks.
Directives end with semicolons, comments begin with `#`, and values may be
quoted. The syntax and structure are based on NGINX

Example:

```conf
server {
    server_name example;
    host 127.0.0.1:8080;
    root ./www/default_site/;
    index /html/index.html;
    allow_methods GET POST DELETE;
    client_max_body_size 1000M;
    autoindex off;

    location /upload {
        root ./www/default_site/upload/;
        allow_methods GET POST DELETE;
        autoindex on;
    }

    location /cgi-bin {
        root ./cgi-bin/;
        allow_methods GET POST;
        cgi_pass .py /usr/bin/python3;
    }
}
```

The parser supports `server_name`, `host`, `root`, `index`, `error_page`,
`allow_methods` or `methods`, `autoindex`, `client_max_body_size` or
`max_body_size`, `store`, `return` or `redirect`, and `cgi_pass`.
Location matching uses the longest configured path prefix. A location inherits
the server root when it does not define its own root.

### Manual Checks

The repository contains browser fixtures under `www/` and CGI fixtures under
`cgi-bin/`. With the default server running, examples include:

```sh
curl -i http://127.0.0.1:8080/
curl -i http://127.0.0.1:8080/location_web_test/
curl -i -F file=@somefile http://127.0.0.1:8080/upload
curl -i -X DELETE http://127.0.0.1:8080/delete/file.txt
curl -i "http://127.0.0.1:8080/cgi-bin/test_need_query.py?name=John"
curl -i -d 'message=hello' \
  -H 'Content-Type: text/plain' \
  http://127.0.0.1:8080/cgi-bin/test_POST.py
```

The CGI examples are [test.py](cgi-bin/test.py),
[testEnv.py](cgi-bin/testEnv.py), [test_POST.py](cgi-bin/test_POST.py), and
[test_need_query.py](cgi-bin/test_need_query.py). The website fixtures include
[index.html](www/default_site/html/index.html),
[sendfile.html](www/default_site/html/sendfile.html), and
[delfile.html](www/default_site/delete/delfile.html).

There is no automated test runner in the repository. Testing is performed with
the included pages, `curl`, a browser, or raw HTTP clients such as `telnet`.

## Implemented Scope and Limitations

The implementation supports request bodies described by `Content-Length` and
stores them temporarily under `/tmp` before processing. Connections are closed
after the response, as is based on HTTP1.0 protocol.

The following behavior is outside the currently implemented scope:

- Chunked request decoding.
- Persistent connections and request pipelining.
- Full request timeout handling.
- `.sh` and `.php` CGI execution; those configured extensions currently return
  `501 Not Implemented`.
- POST content types that are not supported by the response implementation may
  return `415 Unsupported Media Type`.
- CGI pipes are not handled as part of the central socket `select()` set.

These limitations are documented to distinguish the current implementation
from the broader HTTP capabilities described in the project subject.

## Individual Contributions

The contribution breakdown was provided by the project team:

- **odruke-s:** Configuration parsing, HTTP request handling, HTTP response
  handling, and CGI implementation.
- **stripet:** Initial project setup, socket management, and client connection
  management.


## Resources

Classic references used for the project topic and implementation include:

- [RFC 7230: HTTP/1.1 Message Syntax and Routing](https://www.rfc-editor.org/rfc/rfc7230)
- [RFC 7231: HTTP/1.1 Semantics and Content](https://www.rfc-editor.org/rfc/rfc7231)
- [RFC 7232: HTTP/1.1 Conditional Requests](https://www.rfc-editor.org/rfc/rfc7232)
- [MDN HTTP overview](https://developer.mozilla.org/en-US/docs/Web/HTTP)
- Linux manual pages for [`select(2)`](https://man7.org/linux/man-pages/man2/select.2.html),
  [`socket(2)`](https://man7.org/linux/man-pages/man2/socket.2.html),
  [`fcntl(2)`](https://man7.org/linux/man-pages/man2/fcntl.2.html),
  [`fork(2)`](https://man7.org/linux/man-pages/man2/fork.2.html), and
  [`execve(2)`](https://man7.org/linux/man-pages/man2/execve.2.html)
- [Python CGI documentation](https://docs.python.org/3/library/cgi.html)
- [NGINX documentation](https://nginx.org/en/docs/), used as a behavioral
  comparison for HTTP responses and configuration concepts.

### AI Usage

AI was used for documentation-related and learning tasks:

- Drafting and structuring this `README.md`.
- Writing inline explanations of the project's architecture and behavior.
- Clarifying unfamiliar HTTP, CGI, POSIX, and configuration concepts while
  learning.
- Help with new languages like Python.
- Writing repetitive code, like the HTTP error codes list.

All the code generated with AI was inspected, corrected when neccesary,
and approved before implementation.

AI was not used here to claim authorship of the project's implementation code.
