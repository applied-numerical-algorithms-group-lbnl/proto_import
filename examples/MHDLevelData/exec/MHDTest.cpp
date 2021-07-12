#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>

#include <vector>
#include <memory>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Proto.H"
#include "MHDLevelDataRK4.H"
#include "Proto_WriteBoxData.H"
#include "Proto_Timer.H"
#include "MHD_Initialize.H"
#include "MHDOp.H"
#include "MHD_Mapping.H"
//#include "MHD_RK4.H"
//#include "MHD_EulerStep.H"
#include "MHD_Output_Writer.H"

// For Chrono Timer (Talwinder)
#include <chrono>
#include <iostream>
//////////////////////////////

using namespace std;
using namespace Proto;

double time_globalll;
int grid_type_global;
int LowBoundType;
int HighBoundType;
bool limiter_apply;
bool slope_flattening_apply;
bool non_linear_visc_apply;
bool linear_visc_apply;
bool pole_correction;
int init_condition_type;
int Riemann_solver_type;
int domainSizex;
int domainSizey;
int domainSizez;
int BoxSize;

void InitializeMHDLevelDataState(MHDLevelDataState& state)
{
	(state.m_U).setToZero();
	for(DataIterator dit=state.m_U.begin(); *dit!=dit.end(); ++dit)
		MHD_Initialize::initializeState((state.m_U)[*dit],state.m_dx, state.m_dy, state.m_dz,state.m_gamma);
}

int main(int argc, char* argv[])
{


#ifdef PR_MPI
	MPI_Init(&argc,&argv);
#endif

	//have to do this to get a time table
	PR_TIMER_SETFILE("proto.time.table");

	//PR_TIME("main");

	int pid = procID();
	int convTestType = 0; // 0 for no convergence test, 1 for space convergence, 2 for space and time convergence
	bool saveConvTestData = false;
	int maxLev;

	// Defining inputs here until parmparse (or something similar becomes available)
	grid_type_global = 2;  // 0: 2D-Rectangular/3D-Rectangular;  1: 2D-Wavy/3D-Not implemented;  2: 2D-Polar/3D-Spherical
	pole_correction = true;
	double tstop = 10.5, CFL  = 0.03, domsizex = 1.0, domsizey = 1.0, domsizez = 1.0, gamma = 5.0/3.0;
	domainSizex = 64, domainSizey = 128, domainSizez = 32;
	int maxStep = 5000, outputInterval = 100;
	BoxSize=16;
	limiter_apply = false;
	slope_flattening_apply = true;
	linear_visc_apply = true;
	non_linear_visc_apply = true;
	bool takedivBstep = true;

	Riemann_solver_type = 2; // 1:Rusanov; 2:Roe8Wave
	init_condition_type = 10;
	/*
	   0. constant solution
	   1. 2D current sheet problem
	   2. flow from 1 side
	   3. 2D Orszag Tang problem
	   4. Alfven wave problem
	   5. Acoustic pulse problem
	   6. Acoustic pulse problem in polar grid
	   7. Cylindrical pulse problem in polar grid
	   8. Shifted Acoustic pulse problem
	   9. Euler problem
	   10. Radially out flow
	   11. Cylindrical pulse problem in cartesian grid
	   12. Acoustic pulse problem in 3D cartesian grid
	   13. 3D MHD blast wave
	   14. 2D MHD blast wave
	   15. Acoustic pulse problem with Bx
	   16. Acoustic pulse problem in spherical grid
	   17. Shifted Acoustic pulse problem in Spherical grid
	 */
	if (grid_type_global == 2) {
		LowBoundType = 1;  // 0 for periodic, 1 for Dirichlet, 2 for open. This is for dir==0 only
		HighBoundType = 2;
	} else {
		LowBoundType = 0;
		HighBoundType = 0;
	}
	// When using mapping computational domain is always from 0 to 1
	if (grid_type_global > 1){
		domsizex = 1.0;
		domsizey = 1.0;
		domsizez = 1.0;
	}
	bool takeviscositystep = false;
	if (non_linear_visc_apply || linear_visc_apply) takeviscositystep = true;




	LevelBoxData<double,NUMCOMPS> U[3];




	if (convTestType != 0) {
		maxLev = 3;
	} else {
		maxLev = 1;
	}

	for (int lev=0; lev<maxLev; lev++)
	{
#if DIM == 1
		Box domain(Point::Zeros(),Point(domainSizex-1));
#endif
#if DIM == 2
		Box domain(Point::Zeros(),Point(domainSizex-1, domainSizey-1));
#endif
#if DIM == 3
		Box domain(Point::Zeros(),Point(domainSizex-1, domainSizey-1, domainSizez-1));
#endif
		array<bool,DIM> per;
		for(int idir = 0; idir < DIM; idir++){
			per[idir]=true;
	 	}
// #if DIM == 3
// 		if (grid_type_global == 2) {
// 			per[0] = false;
// 			per[1] = true;
// 			per[2] = false;
// 		}
// #endif
		ProblemDomain pd(domain,per);
		double dx = domsizex/domainSizex, dy = domsizey/domainSizey, dz = domsizez/domainSizez;
		double dt;
		if (convTestType == 1)
		{
			dt = CFL*(1.0/1024.);
		} else {
#if DIM == 1
			dt = CFL*dx;
#endif
#if DIM == 2
			dt = CFL*std::min({dx,dy});
#endif
#if DIM == 3
			dt = CFL*std::min({dx,dy,dz});
#endif
		}
#if DIM == 1
		if(pid==0) std::cout << "domainSizex: " << domainSizex << std::endl;
#endif
#if DIM == 2
		if(pid==0) std::cout << "domainSizex: " << domainSizex << std::endl;
		if(pid==0) std::cout << "domainSizey: " << domainSizey << std::endl;
#endif
#if DIM == 3
		if(pid==0) std::cout << "domainSizex: " << domainSizex << std::endl;
		if(pid==0) std::cout << "domainSizey: " << domainSizey << std::endl;
		if(pid==0) std::cout << "domainSizez: " << domainSizez << std::endl;
#endif
		if(pid==0) std::cout << "dt: " << dt << std::endl;

		RK4<MHDLevelDataState,MHDLevelDataRK4Op,MHDLevelDataDX> rk4;
		EulerStep<MHDLevelDataState,MHDLevelDataRK4Op,MHDLevelDataDX> eulerstep;
		EulerStep<MHDLevelDataState, MHDLevelDatadivBOp, MHDLevelDataDX> divBstep;
		EulerStep<MHDLevelDataState, MHDLevelDataViscosityOp, MHDLevelDataDX> viscositystep;


		MHDLevelDataState state(pd,BoxSize*Point::Ones(),dx, dy, dz, gamma);
		InitializeMHDLevelDataState(state);

		int count=0;
		for(DataIterator dit=state.m_U.begin(); *dit!=dit.end(); ++dit)
		{
			count++;
		}
		std::cout << "proc_id, num boxes " << pid << ", " << count << std::endl;

		double time = 0.;
		if(pid==0) cout << "starting time loop, maxStep = "<< maxStep << endl;

		//LevelBoxData<double,1> Jacobian_ave(state.m_dbl,Point::Ones(NGHOST));
		//LevelBoxData<double,DIM*DIM> N_ave_f(state.m_dbl,Point::Ones(NGHOST));

		for(DataIterator dit=(state.m_Jacobian_ave).begin(); *dit!=dit.end(); ++dit) {
			MHD_Mapping::Jacobian_Ave_calc((state.m_Jacobian_ave)[*dit],dx,dy,dz,state.m_U[*dit].box());
			MHD_Mapping::N_ave_f_calc_func((state.m_N_ave_f)[*dit],dx, dy, dz);
		}
		(state.m_Jacobian_ave).exchange();

		for (int k = 0; (k < maxStep) && (time < tstop); k++)
		{

			LevelBoxData<double,DIM+NUMCOMPS> OUT[3];

			if (convTestType == 0)
			{

				if((outputInterval > 0) && (k == 0))
				{
					LevelBoxData<double,NUMCOMPS> new_state(state.m_dbl,Point::Ones(NGHOST));
					LevelBoxData<double,DIM> phys_coords(state.m_dbl,Point::Ones(NGHOST));
					LevelBoxData<double,NUMCOMPS+DIM> out_data(state.m_dbl,Point::Ones(NGHOST));
					for(DataIterator dit=new_state.begin(); *dit!=dit.end(); ++dit) {
						//W_bar itself is not 4th order W. But it is calculated from 4th order accurate U for output.
						MHD_Mapping::JU_to_W_bar_calc(new_state[*dit],state.m_U[*dit],dx,dy,dz,gamma);
						MHD_Mapping::phys_coords_calc(phys_coords[*dit],state.m_U[*dit].box(),dx,dy,dz);
						MHD_Mapping::out_data_calc(out_data[*dit],phys_coords[*dit],new_state[*dit]);
						//MHD_Mapping::out_data_calc(out_data[*dit],phys_coords[*dit],state.m_U[*dit]);
					}

					//Solution on a single patch



					if (grid_type_global == 2) {
#if DIM == 1
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex)),Point::Zeros());
#endif
#if DIM == 2
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey)),{{0,1}});
#endif
#if DIM == 3
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey, domainSizez)), {{0,1,0}});
#endif
					} else {
#if DIM == 1
						OUT[lev].define(DisjointBoxLayout(pd,domainSizex*Point::Ones()),Point::Zeros());
#endif
#if DIM == 2
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey)),Point::Zeros());
#endif
#if DIM == 3
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey,  domainSizez)), Point::Zeros());
#endif

					}
					(out_data).copyTo(OUT[lev]);
					OUT[lev].exchange();
					std::string filename_data="Output_"+std::to_string(k);
					MHD_Output_Writer::WriteSinglePatchLevelData(OUT[lev], dx,dy,dz, filename_data);
					if(pid==0) cout << "Written .vtk file after step "<< k << endl;
				}
			}

			auto start = chrono::steady_clock::now();

			if (convTestType == 1) {
				eulerstep.advance(time,dt,state);
			} else {
				rk4.advance(time,dt,state);
			}
			if (takeviscositystep) {
				// Take step for artificial viscosity
				viscositystep.advance(time,dt,state);
			}
			if (takedivBstep) {
				// Take step for divB term
				divBstep.advance(time,dt,state);
			}
			auto end = chrono::steady_clock::now();
			time += dt;
			time_globalll = time;
			if(pid==0) cout <<"nstep = " << k+1 << " time = " << time << " time step = " << dt << " Time taken: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms"  << endl;

			if (convTestType == 0)
			{
				//if(pid==0) cout <<"nstep = " << k << " time = " << time << " time step = " << dt << endl;
				if((outputInterval > 0) && ((k+1)%outputInterval == 0))
				{
					LevelBoxData<double,NUMCOMPS> new_state(state.m_dbl,Point::Ones(NGHOST));
					LevelBoxData<double,DIM> phys_coords(state.m_dbl,Point::Ones(NGHOST));
					LevelBoxData<double,NUMCOMPS+DIM> out_data(state.m_dbl,Point::Ones(NGHOST));
					for(DataIterator dit=new_state.begin(); *dit!=dit.end(); ++dit) {
						//W_bar itself is not 4th order W. But it is calculated from 4th order accurate U for output.
						//JU_to_W_calc is not suitable here as m_U doesn't have ghost cells, and deconvolve doesn't work at boundaries.
						MHD_Mapping::JU_to_W_bar_calc(new_state[*dit],state.m_U[*dit],dx,dy,dz,gamma);
						MHD_Mapping::phys_coords_calc(phys_coords[*dit],state.m_U[*dit].box(),dx,dy,dz);
						MHD_Mapping::out_data_calc(out_data[*dit],phys_coords[*dit],new_state[*dit]);
						//MHD_Mapping::out_data_calc(out_data[*dit],phys_coords[*dit],state.m_U[*dit]);
					}
					//Solution on a single patch

					if (grid_type_global == 2) {
#if DIM == 1
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex)),Point::Zeros());
#endif
#if DIM == 2
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey)),{{0,1}});
#endif
#if DIM == 3
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey, domainSizez)), {{0,1,0}});
#endif
					} else {
#if DIM == 1
						OUT[lev].define(DisjointBoxLayout(pd,domainSizex*Point::Ones()),Point::Zeros());
#endif
#if DIM == 2
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey)),Point::Zeros());
#endif
#if DIM == 3
						OUT[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey,  domainSizez)), Point::Zeros());
#endif

					}
					(out_data).copyTo(OUT[lev]);
					OUT[lev].exchange();
					std::string filename_data="Output_"+std::to_string(k+1);
					MHD_Output_Writer::WriteSinglePatchLevelData(OUT[lev], dx,dy,dz, filename_data);
					if(pid==0) cout << "Written .vtk file after step "<< k+1 << endl;
				}
			}
		}



		if (convTestType != 0) {
			//Solution on a single patch
#if DIM == 1
			U[lev].define(DisjointBoxLayout(pd,domainSizex*Point::Ones()),Point::Zeros());
#endif
#if DIM == 2
			U[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey)),Point::Zeros());
#endif
#if DIM == 3
			U[lev].define(DisjointBoxLayout(pd,Point(domainSizex, domainSizey, domainSizez)), Point::Zeros());
#endif
			(state.m_U).copyTo(U[lev]);
			domainSizex *= 2;
			domainSizey *= 2;
			domainSizez *= 2;
			BoxSize *= 2; //For debugging: if you want to keep the number of boxes the same
			if (convTestType == 2){
				maxStep *= 2;
			}

		}


	}

	//Until we have a coarsening operation for LevelBoxData, we perform the error calculations on a single patch.
	if(pid==0 && (convTestType != 0))
	{
		for (int varr = 0; varr < 6; varr++) {
			Reduction<double> rxn;
			double ErrMax[2];
			for(int ilev=0; ilev<2; ilev++)
			{
				rxn.reset();
				DataIterator dit_lev=U[ilev].begin();
				DataIterator dit_levp1=U[ilev+1].begin();

				BoxData<double,1> err=slice(U[ilev][*dit_lev],varr);
				err-=Stencil<double>::AvgDown(2)(slice(U[ilev+1][*dit_levp1],varr));
				err.absMax(rxn);
				ErrMax[ilev]=rxn.fetch();
				//ErrMax[ilev]=err.absMax();
				std::string filename="Comp_"+std::to_string(varr)+"_err_"+std::to_string(ilev);
				//NOTE: this assumes that the domain length is 1.0, which is assumed throughout this code. May cause errors if this changes.
				double dx=1./(err.box().size(0));
				if (saveConvTestData){
					WriteBoxData(filename.c_str(),err,dx);
				}
				std::cout << "Lev: " << ilev << " , " << ErrMax[ilev] << std::endl;

			}
			double rate = log(abs(ErrMax[0]/ErrMax[1]))/log(2.0);
			std::cout << "order of accuracy = " << rate << std::endl;
		}
	}

	PR_TIMER_REPORT();

#ifdef PR_MPI
	MPI_Finalize();
#endif
}
