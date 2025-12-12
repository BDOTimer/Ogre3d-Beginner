///----------------------------------------------------------------------------|
/// "debug.h"
///----------------------------------------------------------------------------:
#ifndef DEBUG_H
#define DEBUG_H

#pragma warning(push, 0)
#pragma warning(disable: 4275 4251 4305)
#include <exception>
#include <iostream>

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"
#pragma warning(pop)

#ifdef _MSC_VER
    #include <windows.h>
    #pragma execution_character_set( "utf-8" )
#endif


#define l(a) std::cout << #a << " = " << (a) << '\n';


#endif // DEBUG_H

