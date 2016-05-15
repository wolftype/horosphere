/*
 * =====================================================================================
 *
 *       Filename:  hsObjects.hpp
 *
 *    Description:  Geometric Objects, subclassed on cga::Frame
 *
 *        Version:  1.0
 *        Created:  05/02/2016 18:17:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#ifndef  hsObjects_INC
#define  hsObjects_INC


#include "draw/vsr_draw.h" 
#include "form/vsr_group.h"
#include "form/vsr_knot.h"

namespace vsr { namespace cga {

struct Object : public Frame {
  
  gfx::Vec4f mColor = gfx::Vec4f(1,1,1,1);

  virtual void update(){
    Frame::step();
  }

};


/*-----------------------------------------------------------------------------
 *  DIATOM SYMMETRY
 *-----------------------------------------------------------------------------*/
struct Diatom : public Frame {

  Diatom(int p=5, int q=3) { set(p,q); }
  
  void set( int p, int q){
    mP = p; mQ = q;
    update();
  }
  
  void update(){
    mPointGroup.set(mP, mQ);
  }

  void step(){
    Frame::step();
    mMotif.step();
  }

  Frame motif() const{
    return mMotif;
  };
 
  Frame& motif() {
    return mMotif;
  };
  
  vector< Pair > build() const {
    return mPointGroup.apply( motif().tx() );
  };

  int mP=5;
  int mQ=3;
  bool abar, bbar, abbar;
  gfx::Vec4f mColor = gfx::Vec4f(1,1,1,1);
  PointGroup3D<cga::Vec> mPointGroup;
  Frame mMotif;

};



/*-----------------------------------------------------------------------------
 *  KNOT OBJECT
 *-----------------------------------------------------------------------------*/
struct Knot : public Frame {

  void onUpdate(){
    mTorusKnot.HF.vec() = mVec.y();
    mVec.step();
  }

  template<typename T>
  void calc(const T& p){
    
  }

  TorusKnot mTorusKnot;
  Frame mVec;
  bool bElliptical = false;
};



/*-----------------------------------------------------------------------------
 *  PARTICLE FIELD
 *-----------------------------------------------------------------------------*/
/// Particles in a field of knots
struct Particles {
  
  void init(int num = 1000){
    mNum = num;
    mPoint = std::vector<Point>(num);
  }

  void step(){
    for (auto& i : mPoint){
      Pair pair;
      for (auto& j : mKnot){
        float dist = Round::distance( i, j.pos() );
      }
  
    }
  }

  int mNum = 1000;
  std::vector<cga::Point> mPoint;
  std::vector<Knot> mKnot;

};


struct Crystal {

  
  //template<class T>
  vector<Point>& apply(){

    auto pointgroup = sg.apply( generateMotif() );
    mPoint = sg.hang( pointgroup, mNumX, mNumY, mNumZ);
    return mPoint;
  }

  vector<Point> generateMotif(){
    vector<Point> pnt;
    switch (mMode){
      //Diamond
      case DIAMOND:
      {    
          for (int i=0;i<4;++i){
            float t = (float)i/4;
            Rotor r = Gen::rot( Biv::xz * PI * t);
            pnt.push_back( Construct::point( mFrame.bound(), Vec::x.spin( r ) ) );
          }

          pnt.push_back( mFrame.pos().translate(-.1,.25,0) );
          pnt.push_back( mFrame.pos().translate(.1,-.25,0) );

        break;
      }
      case CUBE:
      {

        float phi = PIOVERTWO * mWidth;
        auto angle = mFrame.rot();
        for (int i=0;i<4;++i){
          float t = (float)TWOPI * i/4.0;
          auto rota = Gen::rot(t,-phi);
          auto rotb = Gen::rot(t,phi);
          pnt.push_back( Construct::point(mFrame.bound(), Vec::x.spin( angle * rota ) ) );
          pnt.push_back( Construct::point(mFrame.bound(), Vec::x.spin( angle * rotb ) ) );
        }
        
        break;
      }
    }

    mStride = pnt.size();
    return pnt;
  }

   SpaceGroup3D<Vec> sg;
   vector<Point> mPoint;
   
   Frame mFrame;
   int mNumX = 1;
   int mNumY = 1;
   int mNumZ = 1;
   int mMode = 0;
   float mWidth = .1;
   int mStride;

   enum {
     POINTS = 0,
     DIAMOND,
     CUBE
   };

};

} }//vsr::cga::


namespace gfx {
/// Draw Diatom in Immediate Mode
template<> void Renderable< vsr::cga::Diatom > :: DrawImmediate(const vsr::cga::Diatom& d) {
  for (auto& i : d.build() ){
    render::begin( d.mColor[0], d.mColor[1], d.mColor[2], d.mColor[3] );
    render::draw( i);
  }
};

template<> void Renderable< vsr::cga::Crystal > ::DrawImmediate(const vsr::cga::Crystal& s){
  
  auto& latticeRes = s.mPoint;
  float t = 1.0;

  switch ( s.mMode ){

    case vsr::cga::Crystal::POINTS:
    {
      for (auto& i : latticeRes) { glColor3f(1,1,1); render::draw(i);}
      break;
    }
    
    case vsr::cga::Crystal::DIAMOND:
    { 

      for (int i=0;i< latticeRes.size(); i+=s.mStride ) {
  
         auto& a = latticeRes[i]; 
         auto& b = latticeRes[i+1]; 
         auto& c = latticeRes[i+2]; 
         auto& d = latticeRes[i+3];
         auto& e = latticeRes[i+4];  
         auto& f = latticeRes[i+5];
         glColor4f(.6,.2,.2,t);
         Glyph::Triangle(a,b,e);
         glColor4f(.2,.6,.6,t);
         Glyph::Triangle(b,c,e);
         glColor4f(.6,.2,.6,t);
         Glyph::Triangle(c,d,e);
         glColor4f(.3,.3,.6,t);
         Glyph::Triangle(d,a,e);
         glColor4f(.4,1,.4,t);
         Glyph::Triangle(a,b,f);
         glColor4f(.4,1,.3,t);
         Glyph::Triangle(b,c,f);
         glColor4f(.3,1,.1,t);
         Glyph::Triangle(c,d,f);
         glColor4f(.1,1,.4,t);
         Glyph::Triangle(d,a,f);
  
      }
      break;
    }
    case vsr::cga::Crystal::CUBE:
      for (int i=0;i<latticeRes.size(); i+=s.mStride ) {
        
         auto& a = latticeRes[i]; 
         auto& b = latticeRes[i+1]; 
         auto& c = latticeRes[i+2]; 
         auto& d = latticeRes[i+3];
         auto& e = latticeRes[i+4];  
         auto& f = latticeRes[i+5];
         auto& g = latticeRes[i+6];
         auto& h = latticeRes[i+7];
        
         glBegin(GL_QUADS);
        
         glColor3f(.6,.2,.2);
         GL::normal( ((c-a)^(b-a)).duale().unit().begin() );
         GL::Quad(a,c,d,b);         
         glColor3f(.2,.6,.6);
         GL::normal( ((e-c)^(d-c)).duale().unit().begin() );
         GL::Quad(c,e,f,d);         
         glColor3f(.6,.2,.6);
         GL::normal( ((g-e)^(f-e)).duale().unit().begin() );
         GL::Quad(e,g,h,f);         
         glColor3f(.1,1,.4);
         GL::normal( ((a-g)^(h-g)).duale().unit().begin() );
         GL::Quad(g,a,b,h);  
         glColor3f(.3,.3,.6);
         GL::normal( ((c-a)^(g-a)).duale().unit().begin() );
         GL::Quad(a,c,e,g);         
         glColor3f(.8,1,.2);
         GL::normal( ((d-b)^(h-b)).duale().unit().begin() );
         GL::Quad(b,d,f,h);         

         glEnd();

         glBegin(GL_LINES);
         glColor3f(.6,0,0);
         GL::Line(a,b);
         GL::Line(c,d);
         GL::Line(e,f);
         GL::Line(g,h);
         GL::Line(a,c);
         GL::Line(c,e);
         GL::Line(g,a);
         GL::Line(b,d);
         GL::Line(d,f);
         GL::Line(f,h);
         GL::Line(h,b);
         glEnd();

      }
      break; 
  }
}

} //gfx::

namespace hs {

template<> void pulse(vsr::cga::Diatom& d){
  printf(" diatom pulsing....\n");
  d.mMotif.db() = vsr::cga::Biv( Rand::Num() * .2, Rand::Num() *.2, Rand::Num() *.2 );
}

template<> void grow(vsr::cga::Diatom& d, float amt){
  printf(" diatom growing...\n");
  d.mMotif.dx() = d.x() * amt;
}

}

#endif   /* ----- #ifndef hsObjects_INC  ----- */
