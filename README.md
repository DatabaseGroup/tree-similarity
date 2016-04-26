Coding style:
=============

See [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

Exceptions to the Google C++ Style Guide:
==============================================

* We declare AND define all functions in the `.h` files. This decision was made
due to the fact that template functions/classes are used in almost every `.h` file.