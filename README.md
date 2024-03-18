# Python-To-C
Python to C Compiler written in C

## Getting Started
Clone this project:
`git clone https://github.com/fish-r/Python-To-C.git`

### Using Makefile
Simply run `make` commmand in root directory. This will build the object files into the `/build` directory, and create the `main` executable.

#### Example: 
```
❯ make
// Terminal showing compilation
gcc -Wall -Werror -ansi -pedantic -I include -c src/main.c -o build/main.o
gcc -Wall -Werror -ansi -pedantic -I include -c src/utils.c -o build/utils.o
gcc -Wall -Werror -ansi -pedantic -o main build/data_types.o build/main.o build/utils.o
```

### Executing Program
Make sure that a Python file exists under the directory `input`.
Run the command `./main <your_file_name>` to parse the given python (or text) file.

#### Example
```
❯ ./main helloworld.py
Reading File: helloworld.py
Pathname: input/helloworld.py
Source code:
 print("Hello World!")
```

