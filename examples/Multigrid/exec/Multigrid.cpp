#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>

#include <vector>
#include <memory>

#include <iostream>
#include <fstream>
#include <sstream>

#include "SGMultigrid.H"
#include "DebugHooks.H"
#include "WriteBoxData.H"

using namespace std;
using namespace Proto;

typedef Var<double,   1> Scalar;
#define PI 3.141592653589793

class SolveParams
{
public:
  SolveParams()
  {
    maxiter   = 27;
    numsmooth = 4;
    nx        = 64;
    domsize   = 1.0;
    tol       = 1.0e-9;
    alpha = 1.0;
    beta  = -1.0;
    blobrad  = 0.1;
    iusejacoby = 0;
    resetDx();
  }

  int maxiter;
  int numsmooth;
  int nstepmax;
  int iusejacoby;
  int nx;
  double domsize;
  double tol;
  double dx;
  double alpha;
  double beta;
  double blobrad;
  double blobloc[DIM];
  
  void resetDx()
  {
    dx = domsize/nx;

    for(int idir = 0; idir < DIM; idir++)
    {
      blobloc[idir] = domsize/2.;
    }
  }

  void print() const
  {
    cout << "multigrid solve parameters: " << endl;
    cout << "max iterations =  "   << maxiter   << endl;
    cout << "num smooths    =  "   << numsmooth << endl;
    cout << "nx             =  "   << nx        << endl;
    cout << "domain size    =  "   << domsize   << endl;
    cout << "tolerance      =  "   << tol       << endl;
    cout << "dx             =  "   << dx        << endl;
    cout << "alpha          =  "   << alpha     << endl;
    cout << "beta           =  "   << beta      << endl;
    cout << "blobrad        =  "   << blobrad   << endl;
    if (iusejacoby==1)
    {
      cout << "using point jacoby for smoothing  " << endl;
    }
    else
    {
      cout << "using multicolor gauss seidel smoothing  " << endl;
    }

  }
};                  
////////////
void
parseCommandLine(SolveParams & a_params, int argc, char* argv[])
{
  cout << "Multigrid Solve of alpha I + beta Lapl phi = rhs : " << endl;
  cout << "usage:  " << argv[0] << " -n nx -m max_iter -s num_smooth -j use_jacoby -d domain_size  -t solve_tolerance -a alpha -b beta" << endl;
  for(int iarg = 0; iarg < argc-1; iarg++)
  {
    if(strcmp(argv[iarg],"-n") == 0)
    {
      a_params.nx = atoi(argv[iarg+1]);
      a_params.resetDx();
    }
    else if(strcmp(argv[iarg], "-m") == 0)
    {
      a_params.maxiter = atoi(argv[iarg+1]);
    }
    else if(strcmp(argv[iarg], "-j") == 0)
    {
      a_params.iusejacoby = atoi(argv[iarg+1]);
    }
    else if(strcmp(argv[iarg], "-s") == 0)
    {
      a_params.numsmooth = atoi(argv[iarg+1]);
    }
    else if(strcmp(argv[iarg],"-d") == 0)
    {
      a_params.domsize = atof(argv[iarg+1]);
    }
    else if(strcmp(argv[iarg],"-t") == 0)
    {
      a_params.tol = atof(argv[iarg+1]);
    }
    else if(strcmp(argv[iarg],"-a") == 0)
    {
      a_params.alpha = atof(argv[iarg+1]);
    }
    else if(strcmp(argv[iarg],"-b") == 0)
    {
      a_params.beta = atof(argv[iarg+1]);
    }
  }
  a_params.resetDx();
  a_params.print();
}


/****************/
PROTO_KERNEL_START
unsigned int setRHSF(Point                a_p,
                     Scalar            & a_rhs,
                     SolveParams         a_params)
{
  double x[DIM];
  for(int idir = 0; idir < DIM; idir++)
  {    
    x[idir] = (a_p[idir] + 0.5)*a_params.dx;
  }

  double rad0sq = a_params.blobrad*a_params.blobrad;
  const double* const x0 = a_params.blobloc;
  double radsq = 0;
  for(int idir = 0; idir < DIM; idir++)
  {  
    radsq += (x[idir]-x0[idir])*(x[idir]- x0[idir]);
  }
  if(radsq < rad0sq)
  {
    double cosval = cos(0.5*PI*(radsq/rad0sq));
    //an attempt at having a rhs that sums to zero
    double rhsval =  (x[0]-x0[0])*cosval*cosval; 

    a_rhs(0) = rhsval;
  }
  else
  {
    a_rhs(0) = 0.0;
  }
  return 0;
}
PROTO_KERNEL_END(setRHSF, setRHS) 

PROTO_KERNEL_START void initParabolaT(Point& p, Var<double>& data)
{
  data(0) = 0;
  for(int idir = 0; idir < DIM; idir ++)
  {
    data(0) += p[idir]*p[idir];
  }
}
PROTO_KERNEL_END(initParabolaT, initParabola);
/****************/
void
multigridSolve(const SolveParams& a_params)
{
  SolveParams params = a_params;
  int nghost = 1;
  Point lo = Point::Zeros();
  Point hi = Point::Ones(a_params.nx - 1);
  Bx domain(lo, hi);
  Bx ghostBox = domain.grow(nghost);

  Bx coardom =  domain.coarsen(2);

  
  BoxData<double, 1> fineTest(domain);
  BoxData<double, 1> coarTest(coardom);
  coarTest.setVal(1.);
  fineTest.setVal(0.);
  cout << "after set val fine = " << endl;
  fineTest.dump();
  cout << "after set val coar = " << endl;
  coarTest.dump();

/**
  //define and initialize scalar phi and rhs
  BoxData<double, 1> phi(ghostBox);
  phi.setVal(0.);

  BoxData<double, 1> res(domain);
  BoxData<double, 1> rhs(domain);

  forallInPlace_p(setRHS, domain, rhs, params);

  SGMultigrid solver(a_params.alpha, a_params.beta, a_params.dx, domain);

  solver.prolongIncrement(fineTest, coarTest);
  cout << "after prolong  should be one, max =  "<< fineTest.max() << ", min = "<< fineTest.min() << endl;
  coarTest.setVal(0.);
  fineTest.setVal(1.);
  solver.restrictResidual(coarTest, fineTest);
  cout << "after average  should be one, max =  "<< coarTest.max() << ", min = "<< coarTest.min() << endl;

  BoxData<double, 1> parabola(ghostBox);
  BoxData<double, 1> lapParab(domain);

  forallInPlace_p(initParabola, ghostBox, parabola);
  Stencil<double> lapsten = Stencil<double>::Laplacian(2);

  lapsten.apply(parabola, lapParab, domain, true, 1.0);
  cout << "after apply on paraboloid  should be 2*DIM, max =  "<< lapParab.max() << ", min = "<< lapParab.min() << endl;


  SGMultigrid::s_numSmoothUp   = a_params.numsmooth;
  SGMultigrid::s_numSmoothDown = a_params.numsmooth;
  SGMultigrid::s_usePointJacoby = (a_params.iusejacoby == 1);

  int iter = 0;
  double rhsmax = rhs.absMax();
  double resStart = std::max(rhs.absMax(), a_params.tol);
  double resIter  = rhsmax;
  cout << "iter = " << iter << ", ||resid|| = " << resIter << endl;
  while((resIter > a_params.tol*resStart) && (iter <  a_params.maxiter))
  {
    solver.vCycle(phi, rhs);
    solver.residual(res, phi, rhs);
  
    iter++;
    resIter = res.absMax();
    cout << "iter = " << iter << ", ||resid|| = " << resIter << endl;
  }

  BoxData<double,1> phiPrint(domain);
  phi.copyTo(phiPrint);
  WriteData<1>(phiPrint, -1, a_params.dx, string("phi"), string("phi"));
  WriteData<1>(     rhs, -1, a_params.dx, string("rhs"), string("rhs"));
**/
}
int main(int argc, char* argv[])
{
  SolveParams params;
  parseCommandLine(params, argc, argv);
  multigridSolve(params);
}  
