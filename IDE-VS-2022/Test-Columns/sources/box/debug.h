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


///----------------------------------------------------------------------------|
/// Элемент ДЕКОРАЦИИ для статического метода test().
///------------------------------------------------------------------- TestInfo:
struct  TestInfo
{       TestInfo(const char* b) : s(b)
        {   std::cout << std::format(a, "start", s, " ---------------------:");
        }
       ~TestInfo()
        {   std::cout << std::format(a, "end", s, " -----------------------.");
        }

    template<typename T, typename TT>
    static std::string showResult(T g, TT h)
    {   
        std::stringstream ss;

        bool b = float(g) == float(h);
        ss  << '\n'
            << "  EXPECTED  : " << g << '\n'
            << "  CALCULATED: " << h << '\n'
            << "  result    : " <<(b ? "GOOD!" : "FAIL...") << '\n';

        return ss.str();
    }

    void Case()
    {   const auto&  w{std::format("| Case: {:2}   |\n", ++cnt)};
        const char*  u{"|------------"};
        std::cout << u << "|\n" << w << u << ":\n";
    }

private:
    int     cnt{};
    const char* s;
    static constexpr const char* a{ "{} TESTCLASS {}::test(){}\n\n" };
};


#endif // DEBUG_H

