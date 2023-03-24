#include <gtest/gtest.h>
#include "Proto.H"
#include "Lambdas.H"
#include <iomanip>

#define NCOMP 1

using namespace Proto;
#if 0
TEST(MBPointInterpOp, RigidXPoint) {
    int domainSize = 8;
    int boxSize = 8;
    HDF5Handler h5;

    Array<double, DIM> exp{1,0,0,0,0,0};
    Array<double, DIM> offset{0,0,0,0,0,0};
    Array<double, DIM> k{1,1,1,1,1,1};

    // input footprint
    std::vector<Point> footprint;
    for (auto pi : Box::Kernel(3))
    {
        if (pi.abs().sum() <= 2)
        {
            footprint.push_back(pi);
        }
    }
    
    constexpr int N = 4;
    double err[N];
    for (int nn = 0; nn < N; nn++)
    {
        auto domain = buildXPoint(domainSize);
        Point boxSizeVect = Point::Ones(boxSize);
        MBDisjointBoxLayout layout(domain, boxSizeVect);

        Array<Point, DIM+1> ghost;
        ghost.fill(Point::Ones(4));
        ghost[0] = Point::Ones(1);
        Point boundGhost = Point::Ones(1);

        // initialize map
        MBMap<XPointMapRigid_t> map(XPointMapRigid, layout, ghost, boundGhost);

        // initialize data
        MBLevelBoxData<double, NCOMP, HOST> hostSrc(layout, ghost);
        MBLevelBoxData<double, NCOMP, HOST> hostDst(layout, ghost);
        MBLevelBoxData<double, NCOMP, HOST> hostErr(layout, ghost);

        hostSrc.initConvolve(f_polyM, map, exp, offset);

        hostSrc.fillBoundaries();
        hostDst.setVal(0);
        hostErr.setVal(0);
        
        Box blockDomainBox(Point::Ones(domainSize));
        err[nn] = 0;
        for (auto iter : layout)
        {
            int block = layout.block(iter);
            Box patchBox = layout[iter];
            for (auto dir : Box::Kernel(1))
            {
                auto bounds = hostSrc.bounds(iter, dir);
                for (auto bound : bounds)
                {
                    Box boundBox = patchBox.adjacent(ghost[0]*dir);
                    if (blockDomainBox.contains(boundBox)) { continue; }
                    for (auto bi : boundBox)
                    {
                        // Apply Point
                        MBDataPoint dstDataPoint(iter, bi, layout);
                        MBPointInterpOp pointInterp(dstDataPoint, ghost[0], map, footprint, 4);
                        pointInterp.apply(hostDst, hostSrc);
                        
                        auto coefs = pointInterp.coefs(hostSrc, 0);
                        pout() << "Application Point: " << bi << std::endl;
                        coefs.print();


                        double interpValue = hostDst[dstDataPoint](0);
                        double exactValue =  hostSrc[dstDataPoint](0);
                        double errorValue = interpValue - exactValue;
                        hostErr[dstDataPoint](0) = std::abs(errorValue);
                        err[nn] = std::max(std::abs(errorValue), err[nn]);
                    }
                }
            }
        }
#if PR_VERBOSE > 0
        std::cout << "error: " << err[nn] << std::endl;
        h5.writeMBLevel({"err"}, map, hostErr, "MBInterpOpTests_XPoint_Err_%i", nn);
        h5.writeMBLevel({"phi"}, map, hostSrc, "MBInterpOpTests_XPoint_Src_%i", nn);
        h5.writeMBLevel({"phi"}, map, hostDst, "MBInterpOpTests_XPoint_Dst_%i", nn);
#endif
        domainSize *= 2;
        boxSize *= 2;
    }

    for (int ii = 1; ii < N; ii++)
    {
        double rate = log(err[ii-1]/err[ii])/log(2.0);
#if PR_VERBOSE > 0
        std::cout << "convergence rate: " << rate << std::endl;
#endif
    }
}
#endif
#if 1
TEST(MBPointInterpOp, Shear) {
    pout() << "SHEAR TEST START" << std::endl;
    int domainSize = 8;
    int boxSize = 8;
    HDF5Handler h5;

    Array<double, DIM> exp{1,0,0,0,0,0};
    Array<double, DIM> offset{0,0,0,0,0,0};
    Array<double, DIM> k{1,1,1,1,1,1};

    // input footprint
    std::vector<Point> footprint;
    for (auto pi : Box::Kernel(3))
    {
        if (pi.abs().sum() <= 2)
        {
            footprint.push_back(pi);
        }
    }

    constexpr int N = 1;
    double err[N];
    for (int nn = 0; nn < N; nn++)
    {
        auto domain = buildShear(domainSize);
        Point boxSizeVect = Point::Ones(boxSize);
        MBDisjointBoxLayout layout(domain, boxSizeVect);

        Array<Point, DIM+1> ghost;
        ghost.fill(Point::Ones(4));
        ghost[0] = Point::Ones(1);
        Point boundGhost = Point::Ones(1);

        // initialize map
        MBMap<ShearMap_t> map(ShearMap, layout, ghost, boundGhost);

        // initialize data
        MBLevelBoxData<double, NCOMP, HOST> hostSrc(layout, ghost);
        MBLevelBoxData<double, NCOMP, HOST> hostDst(layout, ghost);
        MBLevelBoxData<double, NCOMP, HOST> hostErr(layout, ghost);

        hostSrc.initConvolve(f_polyM, map, exp, offset);

        hostSrc.fillBoundaries();
        hostDst.setVal(0);
        hostErr.setVal(0);
        
        Box blockDomainBox(Point::Ones(domainSize));
        err[nn] = 0;
        for (auto iter : layout)
        {
            int block = layout.block(iter);
            Box patchBox = layout[iter];
            for (auto dir : Box::Kernel(1))
            {
                auto bounds = hostSrc.bounds(iter, dir);
                for (auto bound : bounds)
                {
                    Box boundBox = patchBox.adjacent(ghost[0]*dir);
                    if (blockDomainBox.contains(boundBox)) { continue; }
                    for (auto bi : boundBox)
                    {
                        // Apply Point
                        MBDataPoint dstDataPoint(iter, bi, layout);
                        MBPointInterpOp pointInterp(dstDataPoint, ghost[0], map, footprint, 4);
                        pointInterp.apply(hostDst, hostSrc);
                        
                        auto coefs = pointInterp.coefs(hostSrc, 0);
                        pout() << "Application Point: " << bi << std::endl;
                        coefs.print();

                        double interpValue = hostDst[dstDataPoint](0);
                        double exactValue =  hostSrc[dstDataPoint](0);
                        double errorValue = interpValue - exactValue;
                        hostErr[dstDataPoint](0) = std::abs(errorValue);
                        err[nn] = std::max(std::abs(errorValue), err[nn]);
                    }
                }
            }
        }
#if PR_VERBOSE > 0
        std::cout << "error: " << err[nn] << std::endl;
        h5.writeMBLevel({"err"}, map, hostErr, "MBInterpOpTests_Shear_Err_%i", nn);
        h5.writeMBLevel({"phi"}, map, hostSrc, "MBInterpOpTests_Shear_Src_%i", nn);
        h5.writeMBLevel({"phi"}, map, hostDst, "MBInterpOpTests_Shear_Dst_%i", nn);
#endif
        domainSize *= 2;
        boxSize *= 2;
    }

    for (int ii = 1; ii < N; ii++)
    {
        double rate = log(err[ii-1]/err[ii])/log(2.0);
#if PR_VERBOSE > 0
        std::cout << "convergence rate: " << rate << std::endl;
#endif
    }
}
#endif

template<typename T, MemType MEM>
PROTO_KERNEL_START
void f_computeExp_tmp(Var<T, 1, MEM>& a_xp, const Var<T, DIM, MEM>& a_x, Point a_p)
{
    a_xp(0) = 1.0;
    for (int dir = 0; dir < DIM; dir++)
    {
        a_xp(0) *= pow(a_x(dir), a_p[dir]);
    }
}
PROTO_KERNEL_END(f_computeExp_tmp, f_computeExp)

template<typename MAP>
std::vector<Matrix<double>> computeM(MBMap<MAP>& a_map, int a_boxSize, Point a_dst)
{
    double h = 1.0/a_boxSize;
    auto C2C = Stencil<double>::CornersToCells(4);
    
    Box Bx = Box::Cube(a_boxSize*2);
    Box Bx0 = C2C.domain(Bx).extrude(Point::Ones(),-1);
   
    std::vector<BoxData<double, DIM>> X;
    std::vector<Box> B;
    auto Xg = a_map(a_dst, Point::Zeros(), 0);
    for (int ii = 0; ii < 4; ii++)
    {
        X.push_back(a_map(Bx0,ii,0));
        X[ii] -= Xg;
    }
    B.push_back(Box::Cube(a_boxSize));
    B.push_back(B[0].shift(Point::X()*a_boxSize));
    B.push_back(B[1].shift(Point::Y()*a_boxSize));
    B.push_back(B[0].shift(Point::Y()*a_boxSize));

    std::vector<Point> exponents;
    for (auto pi : Box::Cube(3))
    {
        if (pi.sum() <= 2)
        {
            exponents.push_back(pi);
        }
    }
   
    std::vector<std::vector<BoxData<double>>> Xp;
    Xp.resize(4);
    std::vector<std::vector<BoxData<double>>> Xp_avg;
    Xp_avg.resize(4);
    for (int bi = 0; bi < 4; bi++)
    {
        Xp[bi].resize(exponents.size());
        for (int ei = 0; ei < exponents.size(); ei++)
        {
            Xp[bi][ei].define(X[bi].box());
            forallInPlace(f_computeExp, Xp[bi][ei], X[bi], exponents[ei]);
            Xp_avg[bi].push_back(C2C(Xp[bi][ei]));
        }
    }

    std::vector<Point> footprint;
    for (auto pi : Box::Kernel(2))
    {
        if (pi.abs().sum() <= 2)
        {
            footprint.push_back(pi + a_dst);
        }
    }
    std::vector<std::tuple<Point, int>> srcs;
    for (int si = 0; si < footprint.size(); si++)
    {
        Point s = footprint[si];
        int block = -1;
        for (int bi = 0; bi < 4; bi++)
        {
            if (B[bi].contains(s)){ block = bi; break; }
        }
        if (block >= 0)
        {
            srcs.push_back(std::make_tuple(s, block));
        }
        
    }

    std::vector<Matrix<double>> M(2);
    M[0].define(srcs.size(), exponents.size());
    for (int si = 0; si < srcs.size(); si++)
    {
        Point s = std::get<0>(srcs[si]);
        int block = std::get<1>(srcs[si]);
        auto& src = srcs[si];
        for (auto ei = 0; ei < exponents.size(); ei++)
        {
            M[0](si, ei) = Xp_avg[block][ei](s);
        }
    }
 
    M[1].define(1, exponents.size());
    for (auto ei = 0; ei < exponents.size(); ei++)
    {
        M[1](0, ei) = Xp_avg[0][ei](a_dst);
    }

    return M;
}

#if 0
TEST(MBPointInterpOp, CheckMatrix)
{

    auto C2C = Stencil<double>::CornersToCells(4);

    int domainSize = 8;
    int boxSize = 8;
    
    auto domain = buildShear(domainSize);
    Point boxSizeVect = Point::Ones(boxSize);
    MBDisjointBoxLayout layout(domain, boxSizeVect);

    Array<Point, DIM+1> ghost;
    ghost.fill(Point::Ones(4));
    ghost[0] = Point::Ones(1);
    Point boundGhost = Point::Ones();

    // initialize map
    MBMap<ShearMap_t> map(ShearMap, layout, ghost, boundGhost);

    // input footprint
    std::vector<Point> footprint;
    for (auto pi : Box::Kernel(2))
    {
        if (pi.abs().sum() <= 2)
        {
            footprint.push_back(pi);
            std::cout << pi << std::endl;
        }
    }
    
    MBInterpOp interp(map, footprint, ghost[0], 4);
    
    auto M = computeM(map, boxSize, Point::X()*boxSize);
    Box B0 = Box::Cube(boxSize);
    double CErr = 0;
    double DErr = 0;
    double SErr = 0;
    for (auto pi : B0.extrude(Point::Ones(), 1))
    {
        if (B0.contains(pi)) { continue; }
        MBDataPoint t(*layout.begin(), pi, layout);
        auto& op = interp(t);
        auto M = computeM(map, boxSize, pi);
        auto Cinv = M[0].inverse();
        auto S = M[1]*Cinv;
        auto EC = M[0] - op.MC();
        auto ED = M[1] - op.MD();
        auto ES = S - op.MS();
        //EC.print();
        //ED.print();
        //ES.print();
        CErr = std::max(CErr, (EC.absMax()));
        DErr = std::max(DErr, (ED.absMax()));
        SErr = std::max(SErr, (ES.absMax()));
        //M[0].print();
        //M[1].print();
        pout() << "-----------------------------------------------------" << std::endl;
        pout() << "Stencil at point: " << pi << std::endl;
        pout() << "Source data: " << std::endl;
        pout() << "Sum of stencil coefficients: " << op.MS().sum() << std::endl;
        for (auto& si : op.sources())
        {
            pout() << si.point << ", ";
        }
        pout() << std::endl;
        S.print();
    }
    
    EXPECT_LT(CErr, 1e-12);
    EXPECT_LT(DErr, 1e-12);
    EXPECT_LT(SErr, 1e-12);

#if PR_VERBOSE > 0
    std::cout << "Error in C: " << CErr << std::endl;;
    std::cout << "Error in D: " << DErr << std::endl;;
    std::cout << "Error in S: " << SErr << std::endl;;
#endif
}
#endif
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
#ifdef PR_MPI
    MPI_Init(&argc, &argv);
#endif
    PR_TIMER_SETFILE("MBInterpOpTests.D" + to_string(DIM) + ".N" + to_string(numProc())
        + ".time.table");
    int result = RUN_ALL_TESTS();
    PR_TIMER_REPORT();
#ifdef PR_MPI
    MPI_Finalize();
#endif
    return result;
}
