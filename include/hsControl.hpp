/*
 * =====================================================================================
 *
 *       Filename:  hsControl.hpp
 *
 *    Description:  controller methods
 *
 *        Version:  1.0
 *        Created:  04/28/2016 16:53:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#ifndef  hsControl_INC
#define  hsControl_INC

#include <string>

namespace hs {

  template<class T>
  struct Command {
    /// Function Pointer takes type T and some float val
    typedef void(*Func)(T&& udata, float amt);
    vector<Func> func;
  };

  auto cmdLineCB = []( const std::string& s, auto& udata ){
    
    cout << s << endl;
    printf("COMMMAND: %s\n", s.c_str() );

    if (s=="pulse") pulse( udata );
    if (s=="grow")  grow( udata, .01 );
    if (s=="move")  move( udata );
  };



  auto userCB = []( auto&& udata ){
    
    return [&](const std::string& s ){ // value returned from event
    
        printf("COMMMAND: %s\n", s.c_str() );

        if (s=="pulse") pulse( udata );
        if (s=="grow")  grow( udata, .01 );
        if (s=="move")  move( udata );
        if (s=="0")  behave( udata, 0 );
        if (s=="1") behave( udata, 1 );
        if (s=="2") behave( udata, 2 );
        if (s=="3") behave( udata, 3 );
        if (s=="4") behave( udata, 4 );

        return true;
      
    };
    
  };
  
  
  template<class T>
  inline void pulse(T& udata){
    printf("no specification for PULSE on this type\n");
  }
  template<class T>
  inline void grow(T& udata, float amt){
    printf("no specification for GROW on this type\n");
  }
  template<class T>
  inline void move(T& udata){
    printf("no specification for MOVE on this type\n");
  }
  template<class T>
  inline void behave(T& udata, int idx){
    printf("no specification for BEHAVE on this type\n");
  }
}

#endif   /* ----- #ifndef hsControl_INC  ----- */
