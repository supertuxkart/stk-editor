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
2. Download and extract the stk-editor source - either a released package or from our [git/svn repositories](http://supertuxkart.sourceforge.net/Source_control)
4. Download cmake and install it. Start cmake-gui and select the 'stk-editor' root directory as 'Where is the source code', and a new directory 'build' under the 'stk-editor' root directory (next to lib, src, etc) as the build directory ('Where to build the binaries').
5. Click on 'Generate'. If the build directory doesn't exist, you will be asked to create it, then asked to select your VS version. Make sure to select the right version (be aware of the easy to confuse version numbers: VS 2013 = version 12). Click on configure, then generate. This will create the directory 'build', and a VS solution in that directory.
6. Run Visual Studio, and open the solution (.sln) file generated in the 'build' folder
7. Right click on the 'stk-editor' project in the solution explorer, and select 'Set as StartUp Project.'
8. (Optional) From the configuration type drop down menu, select 'Release.'
9. Select Build->Build Solution (or press F7) to compile.



##License
This software is released under the GNU General Public License (GPL) which
can be found in the file `LICENSE` in the same directory as this file.


For additional information check the wiki page:
http://supertuxkart.sourceforge.net/Track_Editor
