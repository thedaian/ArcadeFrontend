# Arcade Frontend

This Arcade Frontend is a very simple launching program designed for playing Windows games on an arcade style system. It was created for the [Central Ohio Gamedev Group](http://thecogg.com) and its members. It's designed to be simple to use, and fairly simple to edit and compile as needed. It is lacking in a variety of features provided by other arcade cabinet frontends, but it's designed more for getting things quickly up and running as opposed to a vast amount of features. Plus, it's 100% free.

## Getting Started

To compile the code, download the respository and use a C++11 compiler of your choice. The project was created using Code::Blocks.

An EXE has also been provided, in Arcade_release.zip, along with the necessary DLL files. In those cases, download the ZIP, and extract to any location. gameslist.json will need to be edited to point to the games and content the program needs. 

### Prerequisites

This code needs the [SFML 2.2](https://www.sfml-dev.org/download/sfml/2.2/) library. Later versions of the library might work, as well as earlier 2.x versions. View the SFML changelogs for further details on what will need changing.

### Installing

For the quickest time from download to compiling:
This project was created on [Code::Blocks IDE](http://www.codeblocks.org/) and uses the [SFML 2.2](https://www.sfml-dev.org/download/sfml/2.2/) library.

To build the project, install those, and open ArcadeFrontend.cbp, then compile the code.

This code does not require Code Blocks as an IDE, and should compile fine on any compiler that can handle at least C++11 code. Configuring the project for other IDEs is outside of the scope of this readme.

## Deployment

Upon building the EXE, configure games or other programs the gameslist.json file.
Run the program.

Example of game definition in the JSON file
```
"name": "Display Name",
"description": "Description shown on single game screen.",
"directory": "C:\\path\\to\\exe\\directory",
"exe": "exmaple.exe",
"icon": "game_icon.png", (270x480 image expected)
"playtime": 15, (optional, defaults to 15)
"arguments": "-arcade 15", (optional, arguments to pass into the program when it's run)
"screenshots": [
	"path/to/screenshot.png"
]
```

## Built With

* [Code::Blocks IDE](http://www.codeblocks.org/) - The C++ IDE Used (gcc compiler)
* [SFML 2.2](https://www.sfml-dev.org/download/sfml/2.2/) - Open Source Game Development Library
* [JSON-C](https://github.com/json-c/json-c) - C based JSON parsing library (included)

## Authors

* **Todd Barchok** - *Initial work* - [thedaian](https://github.com/thedaian)

## License

This project is licensed under the [Free Public License 1.0.0](https://opensource.org/licenses/FPL-1.0.0)

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

## Acknowledgments

* StackEchange, where most of the Window handling and monitoring code was sourced from, or at least looked at for ideas
* [Central Ohio Gamedev Group](http://thecogg.com) and its members.