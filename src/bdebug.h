#ifndef _BDEBUG_H
#define _BDEBUG_H

//#define DEBUG_SEARCH

//#define BDEBUG

#ifdef BDEBUG 
#include <iostream>
using namespace std;
#define BD(X) X
#else 
#define BD(X)
#endif

#endif
