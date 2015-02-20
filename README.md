stk-editor
==========

SuperTuxKart - Track Editor

##---BUILD---

To build the editor on linux you need zlib and physfs development packages. If
you use distribution based on Debian, you can install it using following
command:
    sudo apt-get install libphysfs1 libphysfs-dev zlib1g zlib1g-dev

If you have Windows operating system, it uses static physfs and zlib libraries.
You don't need to install any dependencies.

Just create build directory and use
    cmake ..
    make


For additional information check the wiki page:
http://supertuxkart.sourceforge.net/Track_Editor
