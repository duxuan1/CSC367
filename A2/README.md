# Compiling the code

See the handout for instructions.

# Pgm creator

This allows you to generate pgm images in the binary format. The binary is
located inside the build directory and it is called "pgm_creator". See the
`pgm_creator.cpp` source code.

# Inspecting pgm images in binary format

You can create a pgm image using the pgm creator provided like so:
(you must build the starter code first)

```sh
./pgm_creator 2 2 input.pgm
```

This creates a 2x2 image. You can see the contents of the image with the
following command:

```sh
od --skip-bytes=11 --width=2 --format=u1 input.pgm
```

This will print the following:

```
0000013   1   2
0000015   3   4
0000017
```

We skip the first 11 bytes because they are part of the header and usually not
relevant. In this example, the image has bytes with contents 1, 2, 3 and 4.

When testing, you can use this to inspect the output of your program.


# Generating graphs

You can and are encouraged to write your own scripts. We provide two scripts
that you can use or get inspiration from. We will not provide support to using
them. They should work out of the box on the lab machines. Using them on your
own machine might require you to install the matplotlib python packages.

## `plotter.py`

This contains a single function that takes data for the x/y axes and plots a graph
based on it. Read the documentation for instructions.

## `perfs_student.py`

This script runs your executable (`main`) in the build directory, parses the
perf output and plots some basic graphs (it depends on the first script). If
you decide to use it, make sure to read all of the code there. Again, we will
not be responsible for improper use of these scripts.
