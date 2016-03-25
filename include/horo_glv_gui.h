/*
 * =====================================================================================
 *
 *       Filename:  horo_glv_gui.h
 *
 *    Description:  allosphere glv interface
 *
 *        Version:  1.0
 *        Created:  01/20/2014 16:48:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), wolftype (gmail)
 *   Organization:  Media Arts and Technology Program, UCSB
 *
 * =====================================================================================
 */


#ifndef  al_glv_gui_INC
#define  al_glv_gui_INC


#include "alloGLV/al_ControlGLV.hpp"
#include "gfx/util/glv_gui.h"

namespace al{
    //A GLV Gui Object with built in GUI for easy prototyping
    struct GLVGui : public GLVDetachable {
        GLVGui() : GLVDetachable(), gui() { 
            *this << gui; 
            gui.colors().back.set(.3,.3,.3);
        }
        glv::Gui gui;
    };

    //non-detachable version
   struct GLVGui_ : public GLVBinding {

      glv::Gui gui;
      
      GLVGui_() : GLVBinding(), gui(){
        *this << gui;
        gui.colors().back.set(.3,.3,.3);
      }
      
      
      template<typename T, typename V>
      void init( GuiMap<T,V>& g ){
   	  	for ( g.mVarMapIt i = g.mVarMap.begin(); i != g.mVarMap.end(); i++){
  	  		cout << (i->first) << " " << (i -> second -> val) << " " << (i -> second -> min) << " " << (i -> second -> max) << endl; 
  		  	gui( i -> second -> val, i -> first, i -> second -> min, i -> second -> max );
  	  	}       
      }
   };


    /// A simple gui parameter (float, min, max)
    struct GuiParamf {
    	  
    	double val;
    	
    	double min, max;
    	
    	Paramf ( const Paramf& p ) : val (p.val), min( p.min), max(p.max) {}
    	
    	Paramf ( double _min = 0.0, double _max = 1.0 ) :
    	val(0), min(_min), max(_max) {}
    
    };    

   /// A GLV Gui Factory, templated on some state container
   /// type T container must have a val() method which returns reference to some type V
   /// e.g. struct Param{ float t; float& val(){ return t; } };
   /// Then GuiMap<Param,float>
   template<typename T, typename V>
   struct GuiMap {
     // glv::Gui * mGuiPtr;          ///< A glv::Table with added argument deduction
      string mName;	               ///< Message namespace

      typedef std::map<string, T*> VariablePtrMap;
	    typedef VariablePtrMap::iterator mVarMapIt;
	    VariablePtrMap mVarMap; ///< A Hash Table for storing pointers to T 

	    /// add string to map for gui
      template<typename ... xs>
	    void add(string s, xs ... v){
		   T * p = new T(v...);
		   mVarMap[mName+s] = p;
	    }

      /// get string to map
	    V& get(string s){
		   return mVarMap[mName+s] -> val;
	    }

      void set(string s, const V& v){
        mVarMap[mName+s] -> val = v;
      }
  	
   };
}
    
#endif   /* ----- #ifndef alv_interface_INC  ----- */
