#include "vsr.h"
#include "vsr/vsr_op.h"
#include "vsr/vsr_field.h"
#include "vsr/vsr_draw.h"
//#include "vsr/vsr_gl.h"
#include "OmniAppRedux.h"


using namespace al;
using namespace vsr;
//using namespace gam;


struct MyApp : OmniApp {
	
	  MyApp() : OmniApp() {

	     // SES_IP_ADDRESS = "192.168.0.1";

	       mOmni.mode( OmniStereo::ACTIVE ).stereo(true);

	       omniEnable(true);//.disable();

	      // glv.parentWindow(*this);
	      // glv.gui.colors().back.set(.3,.3,.3);

	      // ws = &mScheduler.add<WindSound>();
	      // es = &mScheduler.add<EventSound>();
	  }

	  virtual ~MyApp() {


	  }

	virtual void onDraw(Graphics& g);

	virtual void onAnimate(al_sec dt) {
	    //SEND CAMERA POSITION AND ORIENTATION TO ALL OTHER COMPUTERS
	    osc::Packet p;
	    p.beginMessage("/nav");
	    p << nav().pos().x << nav().pos().y << nav().pos().z << nav().quat().x << nav().quat().y << nav().quat().z << nav().quat().w;
	    p.endMessage();

	    osc::Send(12001, "192.168.0.26").send(p);
	    osc::Send(12001, "192.168.0.27").send(p);
	    osc::Send(12001, "192.168.0.28").send(p);
	    osc::Send(12001, "192.168.0.29").send(p);

	}

	  virtual void onSound(gam::AudioIOData& io) {

	//		Scheduler& s = io.user<Scheduler>();
	//		s.update(io);

	  }

	  virtual void onMessage(osc::Message& m) {
		if (m.addressPattern() == "/nav"){
	        //std::cout << "nav" << std::endl; 
	        double x,y,z,qx,qy,qz,qw;
	        m >> x; m >> y; m >> z; m >> qx; m >> qy; m >> qz; m >> qw;
	        nav().pos() = al::Vec3d(x,y,z);
	        nav().quat() = al::Quatd(qx, qy, qz, qw);
	    }
	    OmniApp::onMessage(m);
	  }

	  virtual bool onKeyDown(const Keyboard& k){
	    return true;
	  }
	};
	
void test(MyApp& app){
	
	glPointSize(5);
	Field<Pnt> f(10,10,10);
	TorusKnot tk(3,2);
	
	static double time = 0.0;  time += .01;
	tk.HF.vec() = vsr::Vec::x.sp( Gen::rot( Biv::xy * time) );

	for (int i = 0; i < f.num(); ++i){
		double dist = .1 + Ro::sqd( f[i], Ro::loc( tk.HF.cir() ) );
		Bst bst = tk.bst( 1./dist );
		f[i] = Ro::loc ( f[i].sp(bst));
	}
	
	DRAW(f);
	
}
	
void MyApp::onDraw(Graphics& g) {

   // light();
    shader().uniform("lighting", 0.5);    

    //arrows should point away from each other
    test(*this);
    //fields(*this);
  }


int main(int argc, char * argv[]) {
  MyApp().start();
  return 0;
}