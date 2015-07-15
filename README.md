# libfby

idioms so that I start projects in C++, rather than just finishing
them there.

Dan Lyke
danlyke@flutterby.com
http://www.flutterby.net/User:DanLyke

## Philosophy

I have started a couple of projects in Python, and every time I get
into it, at some point I start thinking "why am I not just doing this
in C++?" I have recently been maintaining a lot of Perl, and as the
projects have grown larger, those too have similarly been wanting
typed values, compile-time warnings, and similar functionality.

And, of course, if you're going to write code for limited resource
computing, C or C++ is the appropriate language.

This is an attempt to take common idioms that I'd use in other
languages and toss them into a library that I can use to start
projects in C++, so that I don't end up thinking "gee, I wish I hadn't
used language X".

Documentation and style are... uh... evolving. See also my
FlutterbyNetCPP Wiki/CMS-ish thing, and my "irrigation" irrigation
controller daemon.

## Design & Code Notes

I had a couple of goals with this system:

* A lot of working in a dynamically typed language (Perl) convinced me
  that I really wanted to be developing new personal code in a typed
  language.

* I had a Perl system that was built on top of the parser that
  runs Flutterby.com. There were operations that weren't as fast as
  they should have been, and I knew that I some structural performance
  issues that it'd be nice to get fixed. I didn't realize how much
  just moving the same things into C++ would speed things up, but I
  was also (and still am) rebuilding too many files at a whack.

* I wanted to get comfortable with some of the C++11
  features. Hellooooo, "auto" and lambdas! And then I realized that
  much of what NodeJS does could as easily be implemented in C++.

There are vestiges of two early decisions still in this code:

When I started building this system I grabbed some code I'd written to
move code between Microsoft Managed .NET "C++" and regular C++, for a
code base that was originally C# (Porting a C# app to be
iOS/Windows/OSX), so there were some kludges for "property"
attributes, typing for "^" vs "*" pointer dereferences, strings,
dynamic arrays. I'm trying to rip that out and just go straight C++.

I also started doing a Lex/Flex based parser, and then realized that
for text processing just copying the regexes that the Perl system used
let me better control parsing element priority. But those vestiges
remain.

## Todo

* The system needs more tests (the system always needs more tests).

* It'd be nice to split the library away from the application a little
  further. As I try to hone my C++ chops, I'm dropping back into Perl
  more often than I'd like because I can't just "-lfby", or use
  "sqlextractor" independently.

* Once the app is split from the library, there's a lot of test
  framework that needs to be built for both. As time allows.

* The Net server is starting to get pretty asynchronous and event
  driven, the database libraries have a long way to go to catch up
  with this.

## Philosophy & Submodules

I'm trying to make this one cohesive whole, but right now it's a bunch
of disparate parts. There's the ORM for dealing with SQL servers, some
wrappers for Perl regexes, and a basic event driven network server,
which also has a bunch of helpers for becoming an HTTP server.

I'm trying to throw actual tests in test_*.cpp files, and examples and
boilerplate in example_*.cpp files.

The stylings are still mutable. The regex, markdown and HTML
processing stuff were written at one time, the fbynet stuff patterns
itself a bit after NodeJS, I'm still trying to figure out the right
direction.

### ORM

Object models are created in a mix of C++ and SQL.

`sqlextractor` parses a .h file into C++ modules, and an SQL file that
can be used to initialize a database.

The `fbydb` stuff provides a mapping that queries and saves those
objects, wrapped on top of SQLite and PostgreSQL.

### Parsers

One built on flex, one built on Perl style regexes.

### Accessories

There are some functions for finding image info, basic string
operations, stuff like that.


### Network Framework

A basic network event loop.

See the example_netserver.cpp and example_httpserver*.cpp

## Environment

I'm working to extract all of the Managed Visual C++ isms out of
this. Undoubtedly shortly after I do so someone will pay me to put
them back in.

Other than that, I'm fumbling through learning CMake and hoping that
most of this stuff will end up cross-platform.

### Ubuntu packages I know I depend on

libboost-dev libsqlite3-dev libpq-devl libboost-filesystem-dev
libboost-system-dev libgdbm-dev libpcre3-dev libboost-iostreams-dev
libboost-program-options-dev libcgicc-dev libpstreams-dev
