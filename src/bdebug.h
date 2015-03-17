#ifndef _BDEBUG_H
#define _BDEBUG_H

#include <iostream>
using std::cout;
using std::endl;

//#define BDEBUG

#ifdef BDEBUG 
#define BD(x) x
#else 
#define BD(x)
#endif

#endif
