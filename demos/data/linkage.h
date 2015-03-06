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

  float time; 
  al::Pose pose;
  gfx::Pose model;

  float numiter;          //number of iterations to sweep single instance
  float numx,numy;        //number of xdim nad ydim linkages
  float d1,d2;            //root linkage lengths
  float theta;            //skew of root linkage
  float thetax,thetay;    //skew of subsequent linkage, in terms of pecentage of previous
  float rot;              //phase of mechanism
  float amtx1,amtx2;      //percentage along length of xcoord link a and b
  float amty1,amty2;      //percentage along length of ycoord link a and b
  float link;             //which link to link with
  bool bSubBennett;

};

//shared by simulator and renderer to build up local structures
struct Local{

  vector<vsr::Bennett> bennett;
  vector<vector<vsr::Bennett>> subBennett;

};


#endif   /* ----- #ifndef linkage_INC  ----- */
