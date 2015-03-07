/*
 * =====================================================================================
 *
 *       Filename:  common.hpp
 *
 *    Description:  linkage data
 *
 *        Version:  1.0
 *        Created:  02/19/2015 16:35:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  linkage_INC
#define  linkage_INC


#include "allocore/io/al_App.hpp"
#include "Cuttlebone/Cuttlebone.hpp"  

#include "vsr/vsr_linkages.h"
#include "gfx/gfx_scene.h"

//state sent from simulator to renderer
struct State{

  float time=0; 
  al::Pose pose;
  gfx::Pose model;

  float numiter=1;          //number of iterations to sweep single instance
  float numx=1;
  float numy=1;        //number of xdim nad ydim linkages
  float d1=1;
  float d2=1;            //root linkage lengths
  float theta=.5;            //skew of root linkage
  float thetax=-1;
  float thetay=-1;    //skew of subsequent linkage, in terms of pecentage of previous
  float rot=0;              //phase of mechanism
  float amtx1=.5;
  float amtx2=.5;      //percentage along length of xcoord link a and b
  float amty1=.5;
  float amty2=.5;      //percentage along length of ycoord link a and b
  float link=2;             //which link to link with
  bool bSubBennett=0;

};

//shared by simulator and renderer to build up local structures
struct Local{

  vector<vsr::Bennett> bennett;
  vector<vector<vsr::Bennett>> subBennett;

};


#endif   /* ----- #ifndef linkage_INC  ----- */
