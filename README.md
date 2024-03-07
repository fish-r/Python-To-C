# Python-To-C
Python to C Compiler written in C

## Getting Started
Clone this project:
`git clone https://github.com/fish-r/Python-To-C.git`

### Using Makefile
Simply run `make` commmand in root directory. This will build the object files into the `/build` directory, and create the `my_program` executable.
```
❯ make
gcc -Wall -Werror -ansi -pedantic -I include -c src/main.c -o build/main.o
gcc -Wall -Werror -ansi -pedantic -I include -c src/utils.c -o build/utils.o
gcc -Wall -Werror -ansi -pedantic -o my_program build/data_types.o build/main.o build/utils.o
```

### Executing Program
```
❯ ./my_program test
Program name: ./my_program
Command-line arguments:
arg1: test
```

