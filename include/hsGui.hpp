#ifndef HS_GUI_INCLUDED
#define HS_GUI_INCLUDED

#include "GLV/glv.h"
#include "GLV/glv_binding.h"
#include "GLV/glv_util.h"

#include "alloGLV/al_ControlGLV.hpp"
#include "gfx/util/glv_gui.h"


namespace hs{

   using al::GLVDetachable;
   using al::GLVBinding;
   using glv::Gui;

   //A GLV Gui Object with built in GUI for easy prototyping
   struct GuiView_ : public GLVDetachable {
       GuiView_() : GLVDetachable(), gui() {
           *this << gui;
           gui.colors().back.set(.3,.3,.3);
       }
       Gui gui;
   };

   //non-detachable version
   struct GuiView : public GLVBinding {

     Gui gui;

     GuiView() : GLVBinding(), gui(){
       *this << gui;
       gui.colors().back.set(.3,.3,.3);
     }

   };

   /// A simple gui parameter (float, min, max)
   struct GuiParam {

     float * ref;
     std::atomic<float> val; //value to be referenced by both gui and audio thread

     float min, max;

     GuiParam ( float& _ref, float _val = 0.0, float _min = 0.0, float _max = 1.0 ) :
     ref(&_ref), min(_min), max(_max) { val.store(_val); }

   };

   /// A Map of Gui Parameters
   struct GuiMap {


      string mName;	                 ///< Message namespace

      GuiMap& name( string n ) { mName = n; return *this; }

      using VarMap = std::map<string, GuiParam*>;
      using mVarMapIt = VarMap::iterator;
      VarMap mVarMap; ///< A Hash Table for storing pointers to T

      template<typename ... xs>
      GuiMap& operator()(string s, xs ... v){ add(s, v... ); return *this; }

     /// add value and name
      template<typename x, typename ... xs>
      void add(string s, x& p, xs ... v){
        mVarMap[mName+s] = new GuiParam(p,v...);
      }

      /// get atomic value by name
      float get(string s){
        return mVarMap[mName+s]->val.load();
      }

      /// set atomic value by name
      void set(string s, const float& v){
        mVarMap[mName+s]->val.store(v);
      }

      // template<class ParameterType, class T>
      // void create(T& t){
      //   ParameterType io;
      //   io.create(t);
      //   for (auto& i : io.data() ){
      //     add(i.name, *(i.ptr), i.min, i.max);
      //   }
      // }
      //
      // template<typename GUI>
      // void bind( GUI& gui ){
   	 //  	for ( mVarMapIt i = mVarMap.begin(); i != mVarMap.end(); i++){
  		//   	gui( *(i -> second->ref), i -> first, i -> second->min, i -> second->max );
  	  // 	}
      // }

      template<class ParameterType, class T, class GUI>
      void bind(T& t, GUI& gui){
        ParameterType io;
        io.create(t);
        for (auto& i : io.data() ){
          gui(*(i.ptr),i.name, i.min, i.max);
        }
      }

    };



//
//
//    template<class T>
//    struct GuiWidget : GuiMap {
//      T * data;
//      GuiView * gui;
//
//      GuiWidget(T * d, GuiView * g) : data(d); gui(g)
//
//       void init(T& t, GuiView& g){
//         data = &t;
//         gui = &g;
//         onInit();
//         bind(g);//gui -> init(*this);
//       };
//
//       void onInit(){
//           name("unnamed");
//       }
//    };

} // hs::

   #endif /* end of include guard: HS_GUI_INCLUDED */
