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
projects have grown larger have similarly been wanting typed values,
compile-time warnings, and similar functionality.

And, of course, if you're going to write code for limited resource
computing, C or C++ is the appropriate language.

This is an attempt to take common idioms that I'd use in other
languages and toss them into a library that I can use to start
projects in C++, so that I don't end up thinking "gee, I wish I hadn't
used language X".

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
  features. Hellooooo, "auto" and lambdas!

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

## Philosophy & Submodules

### ORM

Object models are created in a mix of C++ and SQL.

`sqlextractor` parses `wikiobjects.h` into `sqldefinitions.{cpp,h}` and `sqldefinitions.sql`

The `fbydb` stuff provides a mapping that queries and saves those
objects, wrapped on top of SQLite and PostgreSQL.

### Parsers

One built on flex, one built on Perl style regexes.

### Ubuntu packages I know I depend on

libboost-dev libsqlite3-dev libpq-devl libboost-filesystem-dev
libboost-system-dev libgdbm-dev libpcre3-dev libboost-iostreams-dev
libboost-program-options-dev libcgicc-dev libpstreams-dev
