#ifndef _BDEBUG_H
#define _BDEBUG_H

#include <iostream>
using std::cout;
using std::endl;

//#define DEBUG_SEARCH

//#define BDEBUG

#ifdef BDEBUG 
#define BD(X) X
#else 
#define BD(X)
#endif

#endif
