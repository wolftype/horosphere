///@filename data containers that can be loaded and saved to file (JSON formatted)

#ifndef HS_DATA_INCLUDED
#define HS_DATA_INCLUDED

//#include "allocore/system/al_Parameter.hpp"
//#include "ohio.hpp"

namespace hs{

  /// A Named float parameter pointer to data with minimum and maximum values
  /// used for automatic GUI building
  template<class T>
  struct Param {

    struct Data {
    	string name;
      T * ptr;
    	T min, max;
    };

    std::vector<Data> mData;
    std::vector<Data>& data() { return mData; }

    template<class S>
    void create(S& t){
      specify(t);
    }

    template<class S>
    void specify(S& t){}

  };



  struct AudioParam {

    struct Data {
    	string name;
      float * ptr;
    	float min, max;
    };

    std::vector<Data> mData;
    std::vector<Data>& data() { return mData; }

    template<class T>
    void create(T& t){
      specify(t);
      general(t);
    }

    template<class T>
    void specify(T& t){}

    template<class T>
    void general(T& t){
      mData.push_back({t.mName, &t.mMix, 0, 1});
    }

    void save(){
      //user types in filename
    }

  //  void load();
  };


//  struct SafeParam {
//
//    struct Data {
//    	string name;
//      float * ptr;
//    	float min, max;
//      al::Parameter val;
//    };
//
//    std::vector<Data> mData;
//    std::vector<Data>& data() { return mData; }
//
//    template<class T>
//    void create(T& t){
//      specify(t);
//      general(t);
//    }
//
//    template<class T>
//    void specify(T& t){}
//
//    template<class T>
//    void general(T& t){
//      mData.push_back({t.mName, &t.mMix, 0, 1});
//    }
//  };  

//  //Thread Safe al::Parameter
//  struct SafeParam {
//    struct Data {
//      al::Parameter 
//    }
//     std::vector<al::Parameter> mData;
//     std::vector<al::Parameter>& data() { return mData; }
//    //};
//
//    template<class S>
//    void create(S& t){
//      specify(t); 
//    }
//  };

} //hs::



#endif /* end of include guard: HS_DATA_INCLUDED */
