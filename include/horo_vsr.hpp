//
//  vsr_allovsr.h
//  Versor
/*
    Glue for AlloCore Project (at UCSB)
*/
//  Created by Pablo Colapinto on 11/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Versor_vsr_allovsr_h
#define Versor_vsr_allovsr_h

//al::pose
#include "allocore/spatial/al_Pose.hpp"
//vsr::frame
#include "vsr/vsr_cga3D_frame.h"
//gfx::pose
#include "gfx/gfx_scene.h"

namespace vsr2al {

    using vsr::Rot;
    using vsr::Frame;
    
    al::Quatd Rot2Quat(const vsr::Rot& r){
        return al::Quatd(r[0], -r[3], r[2], r[1]);
    }

    vsr::Rot Quat2Rot(const al::Quatd& q){
        return vsr::Rot(q[0], -q[3], q[2], q[1]);
    }

    al::Vec3d V2V(const vsr::Vec& v){
        return al::Vec3d(v[0], v[1], v[2]);
    }

    vsr::Vec V2V(const al::Vec3d& v){
        return vsr::Vec(v[0], v[1], v[2]);
    }
    
    al::Pose Frame2Pose(const Frame& f){
        return al::Pose( V2V( f.vec() ) , Rot2Quat( f.rot() ) );    
    }

    Frame Pose2Frame(const al::Pose& p){
        return Frame( vsr::Ro::null( p.pos()[0], p.pos()[1], p.pos()[2] ), Quat2Rot( p.quat() ) );
    }

}

namespace gfx2al {

    al::Vec3d vec( const gfx::Vec3f& v ){
        return al::Vec3d( v[0],v[1],v[2] );
    }

    al::Quatd quat( const gfx::Quat& q ){
        return al::Quatd( q.w, q.x, q.y, q.z );
    }

    al::Pose pose(const gfx::Pose& p){
        return al::Pose( vec(p.mPos), quat(p.mQuat) );
    }

}

#endif
