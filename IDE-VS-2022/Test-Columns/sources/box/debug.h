///----------------------------------------------------------------------------|
/// "debug.h"
///----------------------------------------------------------------------------:
#ifndef DEBUG_H
#define DEBUG_H

#pragma warning(push, 0)
#pragma warning(disable: 4275 4251)
#include <exception>
#include <iostream>
#include <format>

#include "OgreApplicationContext.h"
#include "OgreRTShaderSystem.h"
#include "OgreAssimpLoader.h"
#include "OgreCameraMan.h"
#include "OgreInput.h"
#include "Ogre.h"

#pragma warning(pop)

#ifdef _MSC_VER
    #include <windows.h>
    #pragma execution_character_set( "utf-8" )
#endif


#define l(a) std::cout << #a << " = " << (a) << '\n';

///----------------------------------------------------------------------------|
/// Начинка для ASSERT.
///------------------------------------------------------------------------ Ass:
constexpr char ERR1  []{ "ASSERT_ERROR--->FILE: \"{}\", LINE: {} - {}\n" };
constexpr char WARNING[]{ "WARNING--->FILE: \"{}\", LINE: {} - {}\n" };

using Strv = std::string_view;

struct  Ass
{
    static void error(bool pred, Strv filename, int line, Strv str = "...")
    {   if(!pred)
        {   std::cout << std::format(ERR1, cutStr(filename), line, str);
            throw(-1);
        }
    }

    static void warn(bool b, Strv filename, int line, Strv str)
    {   if(b)
        {   std::cout << std::format(WARNING, cutStr(filename), line, str);
        }
    }

    static Strv cutStr(Strv s)
    {   auto p = s.rfind("Sources"); return s.substr(p, s.size() - p);
    }
};

#define  ASSERT(a)       Ass::error(a, __FILE__, __LINE__);
#define ASSERTM(a, mess) Ass::error(a, __FILE__, __LINE__, mess);
#define WARNING(a, mess) Ass::warn (a, __FILE__, __LINE__, mess);


#endif // DEBUG_H

