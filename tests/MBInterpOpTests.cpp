#include <gtest/gtest.h>
#include "Proto.H"
#include "Lambdas.H"
#include "MBMap_Shear.H"
#include "MBMap_XPointRigid.H"
//#include "MBMap_CubeSphereShell.H"

using namespace Proto;
#if DIM == 2
#if 1
TEST(MBInterpOp, ShearTest)
{
    HDF5Handler h5;
    
    // interplating function parameters
    Array<double, DIM> exp{4,4,0,0,0,0};
    Array<double, DIM> offset{0,0,0.3,0,0,0};
    
    // grid parameters
    int domainSize = 16;
    int boxSize = 16;
    int ghostSize = 2;
  
    // initialize data
    auto domain = buildShear(domainSize);
    Point boxSizeVect = Point::Ones(boxSize);
    MBDisjointBoxLayout layout(domain, boxSizeVect);
    MBLevelBoxData<double, 1, HOST> hostSrc(layout, Point::Ones(ghostSize));
    
    MBLevelMap<MBMap_Shear, HOST> map;
    map.define(layout, Point::Ones(ghostSize));
    
    for (auto iter : layout)
    {
        auto& src_i = hostSrc[iter];
        auto& x_i = map.map()[iter];
        auto block = layout.block(iter);
        BoxData<double, 1> x_pow = forall_p<double, 1>(f_polyM, block, x_i, exp, offset);
        src_i |= Stencil<double>::CornersToCells(4)(x_pow);
    }

    // interpolate
    hostSrc.exchange();
    interpBoundaries<MBMap_Shear>(hostSrc);
#if PR_VERBOSE > 0
    h5.writeMBLevel({"phi"}, map, hostSrc, "MBInterpOpTests_ShearStandalone");
#endif
}
#endif
#if 1
TEST(MBInterpOp, XPointTest)
{
    HDF5Handler h5;
    int domainSize = 32;
    int boxSize = 16;
    int ghostSize = 3;
    int numIter = 3;
    double order = 5;
    Array<double, DIM> exp{6,6,0,0,0,0};
    Array<double, DIM> offset{0,0,0,0,0,0};
    offset += 0.1;
  
    double err[numIter];
    for (int nn = 0; nn < numIter; nn++)
    {
        err[nn] = 0;
        auto domain = buildXPoint(domainSize);
        Point boxSizeVect = Point::Ones(boxSize);
        MBDisjointBoxLayout layout(domain, boxSizeVect);

        // initialize data and map
        MBLevelBoxData<double, 1, HOST> hostSrc(layout, Point::Ones(ghostSize));
        MBLevelBoxData<double, 1, HOST> hostDst(layout, Point::Ones(ghostSize));
        MBLevelBoxData<double, 1, HOST> hostErr(layout, Point::Ones(ghostSize));
        MBLevelMap<MBMap_XPointRigid, HOST> map;
        map.define(layout, Point::Ones(ghostSize));

        auto C2C = Stencil<double>::CornersToCells(4);
        for (auto iter : layout)
        {
            auto block = layout.block(iter);
            auto& src_i = hostSrc[iter];
            auto& dst_i = hostDst[iter];
            Box b_i = C2C.domain(layout[iter]).grow(ghostSize);
            BoxData<double, DIM> x_i(b_i.grow(PR_NODE));
            // Jacobian and NT are computed but not used
            BoxData<double, 1> J_i(b_i);
            FluxBoxData<double, DIM> NT(b_i);
            map.apply(x_i, J_i, NT, block);
            BoxData<double, 1> x_pow = forall_p<double, 1>(f_polyM, block, x_i, exp, offset);
            //BoxData<double, 1> x_pow = forall<double, 1>(f_bell, x_i, offset);
            src_i |= C2C(x_pow);
            dst_i |= C2C(x_pow);
        }
        hostErr.setVal(0);

        //hostSrc.exchange(); // fill boundary data
        hostDst.exchange(); // fill boundary data
        MBInterpOp interp(map, order);
        interp.apply(hostDst, hostDst);
        for (auto iter : layout)
        {
            auto& src_i = hostSrc[iter];
            auto& dst_i = hostDst[iter];
            auto& err_i = hostErr[iter];
            for (auto dir : Box::Kernel(1))
            {
                if (layout.isBlockBoundary(iter, dir))
                {
                    Box boundBox = layout[iter].adjacent(dir*ghostSize);
                    BoxData<double, 1, HOST> error(boundBox);
                    dst_i.copyTo(error);
                    error -= src_i;
                    err[nn] = max(error.absMax(), err[nn]);
                    error.copyTo(err_i);
                }
            }
        }

#if PR_VERBOSE > 0
        std::cout << "Error (Max Norm): " << err[nn] << std::endl;
        h5.writeMBLevel({"soln"}, map, hostSrc, "MBInterpOpTests_XPointStandalone_Src_N%i",nn);
        h5.writeMBLevel({"interp"}, map, hostDst, "MBInterpOpTests_XPointStandalone_Dst_N%i",nn);
        h5.writeMBLevel({"err"}, map, hostErr, "MBInterpOpTests_XPointStandalone_Err_N%i",nn);
#endif
        domainSize *= 2;
        boxSize *= 2;
    }
    for (int ii = 1; ii < numIter; ii++)
    {
        double rate = log(err[ii-1]/err[ii])/log(2.0);
#if PR_VERBOSE > 0
        std::cout << "Convergence Rate: " << rate << std::endl;
#endif
        EXPECT_TRUE(err[ii] < 1e-12 || rate > order - 0.5);
    }
}
#endif
#endif
#if DIM == 3
#if 1
TEST(MBInterpOp, CubedSphereShellTest)
{
    HDF5Handler h5;
    int domainSize = 16;
    int boxSize = 8;
    int thickness = 1;
    bool cullRadialGhost = true;
    bool use2DFootprint = true;
    double order = 4.0;
    int radialDir = CUBED_SPHERE_SHELL_RADIAL_COORD;
    Array<double, DIM> exp{4,4,4,0,0,0};
    Array<double, DIM> offset{0.1,0.2,0.3,0,0,0};
    Array<Point, DIM+1> ghost;
    ghost.fill(Point::Ones(4));
    ghost[0] = Point::Ones(1);
    if (cullRadialGhost) { ghost[0][radialDir] = 0;}
    std::vector<Point> footprint;
    for (auto pi : Box::Kernel(3))
    {
        if (pi.abs().sum() <= 2)
        {
            if (use2DFootprint && (pi[radialDir] != 0)) { continue; }
            footprint.push_back(pi);
        }
    }
    int N = 3;
    double err[N];
    double errL1[N];
    for (int nn = 0; nn < N; nn++)
    {
        err[nn] = 0.0;
        auto domain = CubedSphereShell::Domain(domainSize, thickness, radialDir);
        Point boxSizeVect = Point::Ones(boxSize);
        boxSizeVect[radialDir] = thickness;
        MBDisjointBoxLayout layout(domain, boxSizeVect);

        ghost[0] = Point::Ones(3);
        if (cullRadialGhost) { ghost[0][radialDir] = 0; }

        auto map = CubedSphereShell::Map<HOST>(layout, ghost[0]);
        ghost[0] = Point::Ones(1);
        if (cullRadialGhost) { ghost[0][radialDir] = 0; }

        // initialize data
        MBLevelBoxData<double, 1, HOST> hostSrc(layout, ghost);
        MBLevelBoxData<double, 1, HOST> hostDst(layout, ghost);
        MBLevelBoxData<double, 1, HOST> hostErr(layout, ghost);
        auto C2C = Stencil<double>::CornersToCells(4);
        for (auto iter : layout)
        {
            auto block = layout.block(iter);
            auto& src_i = hostSrc[iter];
            Box b_i = C2C.domain(layout[iter]).grow(ghost[0]);
            BoxData<double, DIM> x_i(b_i.grow(Point::Ones()));
            // Jacobian and NT are computed but not used
            BoxData<double, 1> J_i(layout[iter].grow(Point::Ones() + ghost[0]));
            FluxBoxData<double, DIM> NT(layout[iter]);
            map.apply(x_i, J_i, NT, block);
            BoxData<double, 1> x_pow = forall_p<double, 1>(f_polyM, block, x_i, exp, offset);
            src_i |= C2C(x_pow);
        }

        hostSrc.exchange();
        hostDst.setVal(0);
        hostErr.setVal(0);
    
        MBInterpOp op = CubedSphereShell::InterpOp(hostDst, order);
        // apply the operator on all block boundaries
        op.apply(hostDst, hostSrc);
        hostDst.exchange();
        for (auto iter : layout)
        {
            auto block = layout.block(iter);
            Box blockDomain = layout.domain().getBlock(block).box();

            auto& err_i = hostErr[iter];
            auto& dst_i = hostDst[iter];
            auto& src_i = hostSrc[iter];

            BoxData<double, 1> tmp(layout[iter]);
            src_i.copyTo(tmp);
            dst_i.copyTo(err_i);
            err_i -= src_i;
            err_i += tmp;
            
            for (auto dir : Box::Kernel(1).grow(-Point::Basis(radialDir)))
            {
                if (dir == Point::Zeros()) {continue; }
                Box bi = blockDomain.adjacent(dir*ghost[0]);
                BoxData<double> ei(bi);
                ei.setVal(0);
                err_i.copyTo(ei);
                err[nn] = max(ei.absMax(), err[nn]);
                errL1[nn] += ei.sum();
            }
        }
        Reduction<double, Max> rxn;
        rxn.reduce(&err[nn], 1);
        err[nn] = rxn.fetch();
        rxn.reset();
        rxn.reduce(&errL1[nn], 1);
        errL1[nn] = rxn.fetch() / domainSize;

#if PR_VERBOSE > 0
        if (procID() == 0)
        {
            std::cout << "Error (Max Norm): " << err[nn];
            std::cout << " | Error (L1 Norm): " << errL1[nn] << std::endl;
        }
        h5.writeMBLevel({"err"}, map, hostErr, "MBInterpOpTests_CubeSphereShell_Err_%i", nn);
        h5.writeMBLevel({"phi"}, map, hostSrc, "MBInterpOpTests_CubeSphereShell_Src_%i", nn);
        h5.writeMBLevel({"phi"}, map, hostDst, "MBInterpOpTests_CubeSphereShell_Dst_%i", nn);
#endif
        domainSize *= 2;
        boxSize *= 2;
    }
    for (int ii = 1; ii < N; ii++)
    {
        double rate = log(err[ii-1]/err[ii])/log(2.0);
        double rateL1 = log(errL1[ii-1]/errL1[ii])/log(2.0);
        //EXPECT_GT(rate, 3.5);
        EXPECT_TRUE(err[ii] < 1e-12 || rateL1 >  order - 0.5);
#if PR_VERBOSE > 0
        if (procID() == 0)
        {
            std::cout << "Convergence Rate (Max Norm): " << rate;
            std::cout << " | (L1 Norm): " << rateL1 << std::endl;
        }
        std::cout << "Convergence pass / fail is determined by L1 Norm" << std::endl;
#endif
    }
}
#endif
#endif
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
#ifdef PR_MPI
    MPI_Init(&argc, &argv);
#endif
    PR_TIMER_SETFILE("MBInterpOpTests_DIM" + to_string(DIM) + "_NProc" + to_string(numProc())
            + ".time.table");
    int result = RUN_ALL_TESTS();
    PR_TIMER_REPORT();
#ifdef PR_MPI
    MPI_Finalize();
#endif
    return result;
}
