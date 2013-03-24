// Synopsis: Runs the CPLEX 'baropt' solver on a given QP problem
// instance which is given as a MPS file.
//
// Usage: run_cplex mps-file
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>
//
// Revision: $Revision: 1094 $ ($Date: 2004-09-02 12:42:47 +0200 (Thu, 02 Sep 2004) $)

#include <string>
#include <iomanip>
#include <stdio.h>
#include <cplex.h>

// We include Seb.h only because we use the timer in there:
#include <Seb.h>

int main(int argn,char **argv) {
  using std::cout;
  using std::endl;
  using std::string;

  int status;                      // CPLEX status
  CPXENVptr env = NULL;
  CPXLPptr instance = NULL;

  try {
    // check paramaterss:
    if (argn < 2)
      throw string("no MPS file name specified as argument to program");

    // initialize CPLEX environment:
    if ((env = CPXopenCPLEXdevelop(&status)) == NULL)
      throw string("couldn't open CPLEX environment");

    // create problem instance:
    if ((instance = CPXcreateprob(env,&status,"Miniball instance")) == NULL)
      throw string("couldn't create CPLEX problem instance");

    // read MPS file:
    if ((status = CPXreadcopyprob(env,instance,argv[1],NULL)) != 0)
      throw string("coudn't read problem from MPS file");

    // start timer:
    Seb::Timer::instance().start("cplex");

    // solve using Barrier solver:
    if ((status = CPXbaropt(env,instance)) != 0)
      throw string("coudn't solve problem instance");

    // get objective value (i.e., the squared radius):
    double radius_square;
    if ((status = CPXgetobjval(env,instance,&radius_square)) != 0)
      throw string("couldn't get objective value");

    // output:
    cout << "====================================================" << endl
	 << "Input file: " << argv[1] << endl
	 << "====================================================" << endl
	 << "Running time: " << Seb::Timer::instance().lapse("cplex") 
	 << "s" << endl
	 << "Squared radius: " << std::setprecision(17)
	 << std::setiosflags(std::ios::scientific)
	 << radius_square << endl
	 << "====================================================" << endl;
      }

  // error handling:
  catch (std::string msg) {
    cout << "Error: " << msg << "." << endl;
    
    // fetch CPLEX error string from status:
    if (env != NULL) {
      char errormsg[1024];
      CPXgeterrorstring(env,status,errormsg);
      cout << "CPLEX message: " << errormsg << endl;
    }
  }

  // deallocate resources:
  if (instance != NULL)
    CPXfreeprob(env,&instance);
  if (env != NULL)
    CPXcloseCPLEX(&env);
}
