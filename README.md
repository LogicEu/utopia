# utopia

Utopia is a collection of generic containers and data
structures in C. Some of the containers are:

* Vector
* Queue
* String
* Hash Table
* Map
* Tree
* Doubly Linked List

Written in simple C89 style, each container is implemented 
as an independent header-only solution, making it easy to 
just copy the actual container you need into your project.

## Generic

Implementing a generic container in C is not new. The
approach taken by utopia is the void pointer solution.
At the cost of having an extra member in each struct 
keeping track of the container's element size in bytes,
we get an extremely flexible and comfortable API.
The template approach is undoubtedly faster, but this one 
has other advantages like debuggability, being able to 
compile beforehand, restructure the size of the elements 
at runtime and being able to easily wrap heap memory.

## Example

```C

#define UTOPIA_IMPLEMENTATION
#include <utopia/vector.h>
#include <stdio.h>

#define SIZE 1000

int main(void)
{
    size_t i, *ptr;
    struct vector vector = vector_create(sizeof(size_t));
    for (i = 0; i < SIZE; ++i)
        vector_push(&vector, &i);

    ptr = vector.data;
    for (size_t* end = ptr + vector.size; ptr != end; ++ptr)
        printf("%zu\n", *ptr);

    vector_free(&vector);
}

```

## Header-Only

Every container implemented by utopia is completely
independent. To access the implementation details of any
container you need to define UTOPIA_IMPLEMENTATION
before including the header file.

```C
#define UTOPIA_IMPLEMENTATION
#include <utopia/vector.h>
```

You can include every header file more
than once to keep the declarationd and definitions 
separated.

## Library

You can easily compile utopia to a static or dynamic 
library, you don't need to recompile for every different
type you need to implement, utopia's containers can 
adapt at runtime.

> Static:

```shell
make -j # or ./build.sh static
```

> Shared:

```shell
make shared -j # or ./build.sh shared
```

## Install

To install the header files and compiled libraries to
your system on MacOS or Linux use:

```shell
sudo ./build.sh install
```

This command will install utopia in /usr/local/, modify
the install path in the build script to change this
behavior.
