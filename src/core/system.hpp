#ifndef _NANOS_SYSTEM
#define _NANOS_SYSTEM

#include "config.hpp"
#include "processingelement.hpp"
#include <vector>
#include "schedule.hpp"

namespace nanos {

// This class initializes/finalizes the library
class System {
private:
  Config config;
  std::vector<PE *> pes;

  // disable copy constructor & assignment operation
  System(const System &sys);
  const System & operator= (const System &sys);
public:
  // constructor
  System ();
  ~System ();

  void loadModules();
  void start ();
  void submit (WD &work);
};

extern System sys;

};

#endif

