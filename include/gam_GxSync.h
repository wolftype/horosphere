//
//  gam_GxSync.h
//  allovsr
//
//  Created by Pablo Colapinto on 4/26/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef allovsr_gam_GxSync_h
#define allovsr_gam_GxSync_h

#include "Gamma/Sync.h"

namespace gam{

	class GxSync : public gam::Sync {

		GxSync() : gam::Sync(60){};
	
		public:
		 
		 static GxSync& master() {
			static GxSync TheGxSync;
			return TheGxSync;
		 }
	};
}

#endif
