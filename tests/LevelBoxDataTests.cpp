#include <gtest/gtest.h>
#include "Proto.H"
#include "Lambdas.H"
using namespace Proto;

DisjointBoxLayout testLayout(int domainSize, Point boxSize)
{
    Box domainBox = Box::Cube(domainSize); 
    Box patchBox = domainBox.coarsen(boxSize);
    std::vector<Point> patches;
    for (auto patch : patchBox)
    {
        if (patch != Point::Zeros()) { patches.push_back(patch); }
    }
    std::array<bool, DIM> periodicity;
    periodicity.fill(true);
    ProblemDomain domain(domainBox, periodicity);
    return DisjointBoxLayout(domain, patches, boxSize);
}


TEST(LevelBoxData, SetVal) {
    int domainSize = 32;
    double dx = 1.0/domainSize;
    Point boxSize = Point::Ones(16);
    double constVal = 42;
    DisjointBoxLayout layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, 1, HOST> hostData(layout, Point::Ones());
    hostData.setVal(constVal);
#ifdef PROTO_ACCEL
    LevelBoxData<double, 1, DEVICE> deviData(layout, Point::Ones());
    deviData.setVal(constVal);
#endif
    for (auto iter : layout)
    {
        auto& hostData_i = hostData[iter];
        int N = hostData_i.size();
        Box B = hostData_i.box();
        BoxData<double, 1, HOST> soln_i(B);
        soln_i.setVal(constVal);
        for (int ii = 0; ii < N; ii++)
        {
            EXPECT_EQ(hostData_i.data()[ii], soln_i.data()[ii]);
        }
#ifdef PROTO_ACCEL
        BoxData<double, 1, HOST> tmpData_i(B);
        auto& deviData_i = deviData[iter];
        deviData_i.copyTo(tmpData_i);
        for (int ii = 0; ii < N; ii++)
        {
            EXPECT_EQ(tmpData_i.data()[ii], soln_i.data()[ii]);
        }
#endif
    }
}

TEST(LevelBoxData, Iota) {
    int domainSize = 32;
    Point boxSize = Point::Ones(16);
    DisjointBoxLayout layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, DIM, HOST, PR_CELL>   hostCellData(layout, Point::Ones());
    LevelBoxData<double, DIM, HOST, PR_FACE_0> hostFaceData(layout, Point::Ones());
    LevelBoxData<double, DIM, HOST, PR_NODE>   hostNodeData(layout, Point::Ones());
    Array<double, DIM> dx;
    Array<double, DIM> offset;
    for (int dir = 0; dir < DIM; dir++)
    {
        dx[dir] = (dir+1.0)/domainSize;
        offset[dir] = (double)dir;
    }
    hostCellData.iota(dx, offset);
    hostFaceData.iota(dx, offset);
    hostNodeData.iota(dx, offset);
    for (auto iter : layout)
    {
        auto& xc = hostCellData[iter];
        double err = 0;
        for (auto pi : xc.box())
        {
            for (int dir = 0; dir < DIM-1; dir++)
            {
                double x = pi[dir]*dx[dir] + 0.5*dx[dir] + offset[dir];
                err = max(err, abs(xc(pi,dir) - x));
            }
        }
        EXPECT_LT(err, 1e-12);
        auto& xf = hostFaceData[iter];
        err = 0;
        for (auto pi : xf.box())
        {
            for (int dir = 0; dir < DIM-1; dir++)
            {
                double x = pi[dir]*dx[dir] + 0.5*dx[dir] + offset[dir];
                if (dir == 0) {x -= 0.5*dx[dir];}
                err = max(err, abs(xf(pi,dir) - x));
            }
        }
        EXPECT_LT(err, 1e-12);
        auto& xn = hostNodeData[iter];
        err = 0;
        for (auto pi : xn.box())
        {
            for (int dir = 0; dir < DIM-1; dir++)
            {
                double x = pi[dir]*dx[dir] + offset[dir];
                err = max(err, abs(xn(pi,dir) - x));
            }
        }
        EXPECT_LT(err, 1e-12);
    }
}

TEST(LevelBoxData, Initialize) {
    int domainSize = 32;
    double dx = 1.0/domainSize;
    Point offset(1,2,3,4,5,6);
    Point k(1,2,3,4,5,6);
    Point boxSize = Point::Ones(16);
    DisjointBoxLayout layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, 1, HOST> hostData(layout, Point::Ones());
    hostData.initialize(f_phi, dx, k, offset);
#ifdef PROTO_ACCEL
    LevelBoxData<double, 1, DEVICE> deviData(layout, Point::Ones());
    deviData.initialize(f_phi, dx, k, offset);
#endif
    for (auto iter : layout)
    {
        auto& hostData_i = hostData[iter];
        int N = hostData_i.size();
        Box B = hostData_i.box();
        BoxData<double, 1, HOST> soln_i(B);
        forallInPlace_p(f_phi, soln_i, dx, k, offset);
        EXPECT_TRUE(compareBoxData(soln_i, hostData_i));
#ifdef PROTO_ACCEL
        BoxData<double, 1, HOST> tmpData_i(B);
        auto& deviData_i = deviData[iter];
        deviData_i.copyTo(tmpData_i);
        EXPECT_TRUE(compareBoxData(soln_i, tmpData_i));
#endif
    }
}

TEST(LevelBoxData, InitializeVariadic) {
    int domainSize = 16;
    double dx = 1.0/domainSize;
    double dx2 = 2*dx;
    Point offset = Point::Zeros();
    Point k(1,2,3,4,5,6);
    Point k2(2,4,6,8,10,12);
    Point boxSize = Point::Ones(8);
    DisjointBoxLayout layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, 1, HOST>   hostData(layout, Point::Ones());
    LevelBoxData<double, DIM, HOST> hostX(layout, Point::Ones());
    hostX.initialize(f_iotaCenter, dx2);
    unsigned int dummyBlock = 0;
    hostData.initialize(f_phiM, dummyBlock, hostX, k, offset);
#ifdef PROTO_ACCEL
    LevelBoxData<double, 1, DEVICE>     deviData(layout, Point::Ones());
    LevelBoxData<double, DIM, DEVICE>   deviX(layout, Point::Ones());
    deviX.initialize(f_iotaCenter, dx2);
    deviData.initialize(f_phiM, dummyBlock, deviX, k, offset);
#endif
    for (auto iter : layout)
    {
        auto& hostData_i = hostData[iter];
        int N = hostData_i.size();
        Box B = hostData_i.box();
        BoxData<double, 1, HOST> soln_i(B);
        forallInPlace_p(f_phi, soln_i, dx, k2, offset);
        EXPECT_TRUE(compareBoxData(soln_i, hostData_i));
#ifdef PROTO_ACCEL
        BoxData<double, 1, HOST> tmpData_i(B);
        auto& deviData_i = deviData[iter];
        deviData_i.copyTo(tmpData_i);
        EXPECT_TRUE(compareBoxData(soln_i, tmpData_i));
#endif
    }
}
/*
TEST(LevelBoxData, FaceCentering) {
    int domainSize = 32;
    double dx = 1.0/domainSize;
    Point boxSize = Point::Ones(16);
    auto layout = testLayout(domainSize, boxSize);
    Point ghost = Point::Zeros();
    LevelBoxData<double, 1, HOST, PR_FACE> src(layout, ghost);
    for (int dir = 0; dir < DIM; dir++)
    {
        src.setVal(dir+1.0, 0, dir);
    }
    for (auto iter : layout)
    {
        auto fluxes = src.fluxes(iter);
        for (int dir = 0; dir < DIM; dir++)
        {
            Box b0 = fluxes[dir].box();
            Box b1 = layout[iter].grow((Centering)dir).grow(ghost);
#if PR_VERBOSE > 0
            std::cout << b0 << " ==? " << b1 << std::endl;
#endif
            EXPECT_EQ(fluxes[dir].box(), layout[iter].grow((Centering)dir));
        }
    }

    for (int dir = 0; dir < DIM; dir++)
    {
        auto absMax = src.absMax(0,dir);
#if PR_VERBOSE > 0
        std::cout << "absMax of flux " << dir << ": " << absMax << std::endl;
#endif
        EXPECT_EQ(absMax, dir+1);
    }

}
*/
TEST(LevelBoxData, LinearSize)
{
    int domainSize = 32;
    double dx = 1.0/domainSize;
    Point boxSize = Point::Ones(16);
    auto layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, 1, HOST> src(layout, Point::Ones(1));
    unsigned int srcSize = 0;
    for (auto iter : layout)
    {
        srcSize += src[iter].box().size();
    }
    EXPECT_EQ(srcSize*sizeof(double), src.linearSize());

}
TEST(LevelBoxData, CopyToHostToHost)
{
    int domainSize = 64;
    double dx = 1.0/domainSize;
    Point offset(1,2,3,4,5,6);
    Point k(1,2,3,4,5,6);
    double ghostVal = 7;
    Point boxSize = Point::Ones(16);
    auto layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, 2, HOST> hostSrc(layout, Point::Ones(2));
    LevelBoxData<double, 2, HOST> hostDstL(layout, Point::Ones(3));
    LevelBoxData<double, 2, HOST> hostDstS(layout, Point::Ones(1));
    hostSrc.initialize(f_phi, dx, k, offset);
    hostDstL.setVal(ghostVal);
    hostDstS.setVal(ghostVal);
    hostSrc.copyTo(hostDstL);
    hostSrc.copyTo(hostDstS);
    EXPECT_TRUE(compareLevelData(hostSrc, hostDstL));
    EXPECT_TRUE(compareLevelData(hostSrc, hostDstS));
}
#ifdef PROTO_ACCEL
TEST(LevelBoxData, CopyToDeviceToHost)
{
    int domainSize = 64;
    double dx = 1.0/domainSize;
    Point offset(1,2,3,4,5,6);
    Point k(1,2,3,4,5,6);
    double ghostVal = 7;
    Point boxSize = Point::Ones(16);
    auto layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, 2, HOST> hostSrc(layout, Point::Ones(2));
    LevelBoxData<double, 2, DEVICE> deviSrc(layout, Point::Ones(2));
    LevelBoxData<double, 2, HOST> hostDstL(layout, Point::Ones(3));
    LevelBoxData<double, 2, HOST> hostDstS(layout, Point::Ones(1));
    hostSrc.initialize(f_phi, dx, k, offset);
    deviSrc.initialize(f_phi, dx, k, offset);
    hostDstL.setVal(ghostVal);
    hostDstS.setVal(ghostVal);
    deviSrc.copyTo(hostDstL);
    deviSrc.copyTo(hostDstS);
    EXPECT_TRUE(compareLevelData(hostSrc, hostDstL));
    EXPECT_TRUE(compareLevelData(hostSrc, hostDstS));
}
TEST(LevelBoxData, CopyToHostToDevice)
{
    int domainSize = 64;
    double dx = 1.0/domainSize;
    Point offset(1,2,3,4,5,6);
    Point k(1,2,3,4,5,6);
    double ghostVal = 7;
    Point boxSize = Point::Ones(16);
    auto layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, 2, HOST> hostSrc(layout, Point::Ones(2));
    LevelBoxData<double, 2, HOST> hostDstL(layout, Point::Ones(3));
    LevelBoxData<double, 2, HOST> hostDstS(layout, Point::Ones(1));
    LevelBoxData<double, 2, DEVICE> deviDstL(layout, Point::Ones(3));
    LevelBoxData<double, 2, DEVICE> deviDstS(layout, Point::Ones(1));
    hostSrc.initialize(f_phi, dx, k, offset);
    hostDstL.setVal(ghostVal);
    hostDstS.setVal(ghostVal);
    deviDstL.setVal(ghostVal);
    deviDstS.setVal(ghostVal);
    hostSrc.copyTo(deviDstL);
    hostSrc.copyTo(deviDstS);
    for (auto iter : layout)
    {
        auto& hostDstL_i = hostDstL[iter];
        auto& hostDstS_i = hostDstS[iter];
        auto& deviDstL_i = deviDstL[iter];
        auto& deviDstS_i = deviDstS[iter];
        deviDstL_i.copyTo(hostDstL_i);
        deviDstS_i.copyTo(hostDstS_i);
    }
    EXPECT_TRUE(compareLevelData(hostSrc, hostDstL));
    EXPECT_TRUE(compareLevelData(hostSrc, hostDstS));
}
TEST(LevelBoxData, CopyToDeviceToDevice)
{
    int domainSize = 64;
    double dx = 1.0/domainSize;
    Point offset(1,2,3,4,5,6);
    Point k(1,2,3,4,5,6);
    double ghostVal = 7;
    Point boxSize = Point::Ones(16);
    auto layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, 2, HOST> hostSrc(layout, Point::Ones(2));
    LevelBoxData<double, 2, DEVICE> deviSrc(layout, Point::Ones(2));
    LevelBoxData<double, 2, HOST> hostDstL(layout, Point::Ones(3));
    LevelBoxData<double, 2, HOST> hostDstS(layout, Point::Ones(1));
    LevelBoxData<double, 2, DEVICE> deviDstL(layout, Point::Ones(3));
    LevelBoxData<double, 2, DEVICE> deviDstS(layout, Point::Ones(1));
    hostSrc.initialize(f_phi, dx, k, offset);
    deviSrc.initialize(f_phi, dx, k, offset);
    hostDstL.setVal(ghostVal);
    hostDstS.setVal(ghostVal);
    deviDstL.setVal(ghostVal);
    deviDstS.setVal(ghostVal);
    deviSrc.copyTo(deviDstL);
    deviSrc.copyTo(deviDstS);
    for (auto iter : layout)
    {
        auto& hostDstL_i = hostDstL[iter];
        auto& hostDstS_i = hostDstS[iter];
        auto& deviDstL_i = deviDstL[iter];
        auto& deviDstS_i = deviDstS[iter];
        deviDstL_i.copyTo(hostDstL_i);
        deviDstS_i.copyTo(hostDstS_i);
    }
    EXPECT_TRUE(compareLevelData(hostSrc, hostDstL));
    EXPECT_TRUE(compareLevelData(hostSrc, hostDstS));
}
#endif

TEST(LevelBoxData, ExchangeHost)
{
    constexpr unsigned int C = 2;
    int domainSize = 64;
    double dx = 1.0/domainSize;
    int ghostSize = 2;
    Point boxSize = Point::Ones(16);
    auto layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, C, HOST> hostData(layout, Point::Ones(ghostSize) + Point::X());
    hostData.setToZero();
    for (auto iter : layout)
    {
        auto& hostData_i = hostData[iter];
        BoxData<double, C, HOST> tmpData(layout[iter]);
        forallInPlace_p(f_pointID, tmpData);
        tmpData.copyTo(hostData_i);
    }
    hostData.exchange();
    EXPECT_TRUE(testExchange(hostData));
}
#if 1
TEST(LevelBoxData, ExchangeHost_Node)
{
#if PR_VERBOSE > 0
    HDF5Handler h5;
#endif
    constexpr unsigned int C = 2;
    int domainSize = 16;
    double dx = 1.0/domainSize;
    int ghostSize = 1;
    Point boxSize = Point::Ones(8);
    auto layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, C, HOST, PR_NODE> hostData(layout, Point::Ones(ghostSize) + Point::X());
    hostData.setVal(-1);
    for (auto iter : layout)
    {
        auto& hostData_i = hostData[iter];
        BoxData<double, C, HOST> tmpData(layout[iter]);
        //tmpData.setVal(1);
        forallInPlace_p(f_pointID, tmpData);
        tmpData.copyTo(hostData_i);
    }
#if PR_VERBOSE > 0
    h5.writeLevel(1,hostData,"ExchangeHost_Node_0");
#endif
    hostData.exchange();
#if PR_VERBOSE > 0
    h5.writeLevel(1,hostData,"ExchangeHost_Node_1");
#endif
    EXPECT_TRUE(testExchange(hostData));
}
#endif
#ifdef PROTO_ACCEL
TEST(LevelBoxData, ExchangeDevice)
{
    constexpr unsigned int C = 2;
    int domainSize = 64;
    double dx = 1.0/domainSize;
    int ghostSize = 1;
    Point boxSize = Point::Ones(16);
    auto layout = testLayout(domainSize, boxSize);
    LevelBoxData<double, C, HOST> hostData(layout, Point::Ones(ghostSize));
    LevelBoxData<double, C, DEVICE> deviData(layout, Point::Ones(ghostSize));
    hostData.setToZero();
    for (auto iter : layout)
    {
        auto& deviData_i = deviData[iter];
        BoxData<double, C, DEVICE> tmpData(layout[iter]);
        forallInPlace_p(f_pointID, tmpData);
        tmpData.copyTo(deviData_i);
    }
 //   HDF5Handler h5;
  //  h5.writeLevel(dx, deviData, "EXCHANGE_DEVICE_0");
    deviData.exchange();
    //h5.writeLevel(dx, deviData, "EXCHANGE_DEVICE_1");
    for (auto iter : layout)
    {
        auto& deviData_i = deviData[iter];
        auto& hostData_i = hostData[iter];
        deviData_i.copyTo(hostData_i);
    }
    EXPECT_TRUE(testExchange(hostData));
}
#endif
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
#ifdef PR_MPI
    MPI_Init(&argc, &argv);
#endif
    int result = RUN_ALL_TESTS();
#ifdef PR_MPI
    MPI_Finalize();
#endif
    return result;
}
