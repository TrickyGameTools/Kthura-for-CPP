# Kthura for C++

![Kthura](https://raw.githubusercontent.com/TrickyGameTools/KthuraTextEditor/master/Properties/Kthura.png)

This repository contains the CPP library for using Kthura, and a driver to use draw Kthura maps in SDL, but Kthura has been desinged to be portable to other graphic engines as well. Please note, this is only a library in order to attach the Kthura engine to your own code. This repository does not contain an editor (although it's possible to build an editor based upon it. The C# Kthura editor uses the same Kthura classes as my game does, after all. This library is set up in the same manner).

# Installation

I will only distribute Kthura in pure source code, and I figured that for C++ users that is also the preferred way to go. I've only tested these source files in Visual Studio 2019, so I am absolutely not sure about how these files will function on other compilers. I'm all ears for making the code compatibale for other compilers, or if you want to fork this project in order to make it specific for your favorite compiler, by all means, be my guest.

To create a basic Kthura engine (without specific drivers) you need:
Kthura_Core.cpp
Kthura_Draw.cpp
Kthura_Core.hpp
Kthura_Draw.cpp

These two can be optionally compiled if you desire to specifically use Kthura for SDL (which would require you to have SDL installed and in reach by your compiler, of course).
Kthura_SDL_Driver.cpp
Kthura_SDL_Driver.hpp

Kthura uses [JCR6](https://github.com/jcr6/JCR6_CPP) for its file system, so you will need to include that as well.
I may be putting a step-by-step guide for VisualStudio 2019 later.

# Note about JCR6 and the C# Kthura editor

By default the Kthura Editor compresses all data into lzma. I have not yet managed to make JCR6 able to tackle that format in C++. Kthura can be configured to save all data without compression, and that should solve all issues.
