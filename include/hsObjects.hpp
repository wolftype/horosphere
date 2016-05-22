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
#include "form/vsr_graph.h"

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


  vector<Vertex> sphereMesh(int w, int h){
  
    vector<Vertex> v;
  
    for (int i = 0;i<w;++i){
      for (int j=0;j<h;++j){
        auto tu = (float)i/w * TWOPI;
        auto tv = -PIOVERTWO + (float)j/h * PI;
        auto rot = Gen::rot(tu,tv);    
        v.push_back(  Vec3f( Vec::x.spin(rot) ) );         
      }
    }
  
    return v;
  } 

  
   void build() {
     
     int w = 20; int h = 20;
     /// Make symmetry
     mResult = mPointGroup.apply( motif().tx() );
     /// Calc mesh
 //    mMesh = sphereMesh(w,h);
 //    
 //    for (auto& i : mMesh){
 //     auto p1 = Construct::point( i.Pos[0], i.Pos[1], i.Pos[2] );
 //     Pair tpar;
 //     for (auto& j : mResult){
 //       float t = Round::sqd( p1, Round::location(j) );
 //       tpar += j * t;
 //     }
 //     p1 = Round::location( p1.spin( Gen::bst( tpar * -.5 * mForce) ) );
 //     i.Pos = Vec3f( p1[0], p1[1], p1[2] );
 //    }
 //
 ///   /// Set neighbors
 //     mGraph.UV(w,h, mMesh, true);//.vertex());
 //
 //    /// Calc Normals
 //    for (auto& i : mGraph.node()){
 //   
 //      auto va = i->data().Pos;
 //      auto vb = i->edge -> a().Pos;
 //      auto vc = i->edge -> next -> a().Pos;
 //      Vec3f normal =(vb-va).cross(vc-va).unit() ;
 //   
 //      i->data().Norm = normal;
 //    }

  }


  int mP=5;
  int mQ=3;
  bool abar, bbar, abbar;
  gfx::Vec4f mColor = gfx::Vec4f(1,1,1,1);
  PointGroup3D<cga::Vec> mPointGroup;
  Frame mMotif;
  vector<Pair> mResult;
  vector<Vertex> mMesh;
  HEGraph<Vertex> mGraph;
  float mForce = .1;


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

  
  // either apply to incoming motif or generate one from scratch
  vector<Point>& apply( const vector<Point>& pointgroup = vector<Point>(0)){
    if (pointgroup.empty()){
      mPoint = sg.hang( sg.apply( generateMotif() ), mNumX, mNumY, mNumZ);
    } else {
      mPoint = sg.hang( pointgroup, mNumX, mNumY, mNumZ);
    }
    return mPoint;
  }

  
  template<class T> 
  vector<T> apply(const vector<T>& input){
    return sg.hang( input, mNumX, mNumY, mNumZ);
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
   int mMode = 0; ///< POINTS, DIAMOND, or CUBE
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
  for (auto& i : d.mResult ){
    render::begin( d.mColor[0], d.mColor[1], d.mColor[2], d.mColor[3] );
    render::draw( i);
  }
  
//   glBegin(GL_TRIANGLES);
//   int iter =0;
//   bool bChecker = false;
//   for (auto& i : d.mGraph.face()){
//        iter++;
//        float t = (float)iter/d.mGraph.face().size(); 
//        auto& a = i->a();
//        auto& b = i->b();
//        auto& c = i->c(); 
//        glColor3f(bChecker,bChecker,bChecker);
//        GL::normal( a.Norm[0], a.Norm[1],a.Norm[2]);//.begin() );
//        GL::vertex( a.Pos[0], a.Pos[1], a.Pos[2] );
//        GL::normal( b.Norm[0], b.Norm[1], b.Norm[2] );
//        GL::vertex( b.Pos[0], b.Pos[1], b.Pos[2] );
//        GL::normal( c.Norm[0], c.Norm[1], c.Norm[2] );
//        GL::vertex( c.Pos[0], c.Pos[1], c.Pos[2] );
//        if (!(iter&1)) bChecker = !bChecker;
//   }
//   glEnd();

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



#endif   /* ----- #ifndef hsObjects_INC  ----- */
