Google Drive Filesystem
=======================

This project contains two parts: libgdrive and gdrivefs.

* libgdrive is a C++ wrapper of Google Drive/Docs API.
* gdrivefs uses libgdrive and fuse to imeplement filesystem interfaces.

Requires
========

libgdrive

* libcurl
* libxml2

gdrivefs

* libgdrive
* fuse (fuse4x for Mac)

Usage
=====

> gdrivefs mountpoint [options]
> 
> FUSE options:
>    -d   -o debug          enable debug output (implies -f)
>    -f                     foreground operation
>    -s                     disable multi-threaded operation

You should also create a file in working directory namd "user.txt" containing email and password of your Google accounts.

For example:

user.txt
> byvoid.kcp@gmail.com
> MyPlainPassWord

And run:
> mkdir gdrive
> ./gdrivefs gdrive

Warning
=======

This project is under development. The quaility is currently not asured. Use it at your own risk.
