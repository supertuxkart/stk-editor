stk-editor
==========

SuperTuxKart - Track Editor

##---BUILD---

###Linux

To build the editor on linux you need zlib and physfs development packages. If you use distribution based on Debian, you can install it using following command:
```
    sudo apt-get install libphysfs1 libphysfs-dev zlib1g zlib1g-dev
```

If you have it already installed, just create build directory and use:
```
    cmake ..
    make
```

###Windows

If you have Windows operating system, it uses static physfs and zlib libraries. You don't need to install any dependencies.

1. Install VS 2013 (or later). The free express versions work fine. Older versions may or may not work.
2. Download and install a source package.
4. Download cmake and install it. Then start cmake-gui and select the STK-editor root directory as 'Where is the source code', and a new directory in the root directory (next to src, data etc) as build directory (for now I assume that this directory is called bld).
5. Click on configure. You will be asked to create the directory (yes), then for your VS version. Make sure to select the right version (be aware of the easy to confuse version numbers: VS 2013 = version 12). Click on configure, then generate. This will create the directory 'bld', and a VS solution in that directory.
6. In Visual Studio open the project file generated in the 'bld' folder
7. Right click on the supertuxkart project in the solution explorer, and select "Set as StartUp Project".
8. Select Build->Build Solution (or press F7) to compile.


##License
This software is released under the GNU General Public License (GPL) which
can be found in the file `LICENSE` in the same directory as this file.


For additional information check the wiki page:
http://supertuxkart.sourceforge.net/Track_Editor
