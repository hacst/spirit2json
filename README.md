Spirit2JSON
===========

What is it?
-----------
A work in progress json parser/generator written in C++ using the Boost Spirit library.
Up to now the following parts have been implemented:

* JSON AST representation based on Boost::Variant
* Parser for basic JSON syntax using Spirit::Qi
* Test coverage for basic JSON syntax using the Boost Test Library
* Pretty printer for internal AST types


What is it not?
---------------
Done ;-) There's a lot of work left to do but some of the major areas are:

* Better test coverage for parser
* Test coverage for generator/pretty printer
* Proper error reporting
* Creation of a usable API
* Numeric parser adhering to JSON standards
* Proper error reporting

Can I contribute?
-----------------
Yes please. Contributions as well as bugreports are most welcome.
