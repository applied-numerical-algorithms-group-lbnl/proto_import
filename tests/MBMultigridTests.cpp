#include <gtest/gtest.h>
#include "Proto.H"
#include "TestFunctions.H"
#include "BoxOp_MBLaplace.H"
#include "MBMap_XPointRigid.H"

using namespace Proto;

namespace
{
    typedef BoxOp_MBLaplace<double, MBMap_XPointRigid> OP;

    PROTO_KERNEL_START
    void f_linearF(Var<double> &a_data, Var<double, DIM> &a_X, double a_slope, int a_comp)
    {
        a_data(0) = a_X(a_comp) * a_slope;
    }
    PROTO_KERNEL_END(f_linearF, f_linear)

    template <typename T, unsigned int C, MemType MEM, Centering CTR, template<MemType> typename MAP>
    void initializeData(
        MBLevelBoxData<T, C, MEM, CTR> &phi,
        MBLevelBoxData<T, C, MEM, CTR> &lphi,
        MBLevelMap<MAP, MEM>& map)
    {
        auto& layout = phi.layout();
        Array<double, DIM> offset{0,0,0,0,0,0};
        auto C2C = Stencil<double>::CornersToCells(4);
        for (auto iter : layout)
        {
            auto block = layout.block(iter);
            auto &phi_i = phi[iter];
            auto &rhs_i = lphi[iter];
            Box b_i = C2C.domain(layout[iter]).grow(OP::ghost());
            BoxData<double, DIM> x_i(b_i.grow(PR_NODE));
            BoxData<double, 1> J_i(b_i);
            // compute coordinate / jacobian values from map
            map.apply(x_i, J_i, block);
            BoxData<double, 1> phi_node = forall<double, 1>(f_bell, x_i, offset);
            BoxData<double, 1> lphi_node = forall<double, 1>(f_Lbell, x_i, offset);

            phi_i |= C2C(phi_node);
            rhs_i |= C2C(lphi_node);
        }
    }
}

#if 1
TEST(MBMultigridTests, LaplaceXPoint) {
    HDF5Handler h5;
    
    // Constants
    
    int domainSize = 16;
    int boxSize = 8;
    int numBlocks = 5;
    int numLevels = log(domainSize)/log(2.0);
    double slope = 1.0;
    int comp = 0;
    Point refRatio = Point::Ones(2);
    std::vector<Point> refRatios;
    for (int bi = 0; bi < numBlocks; bi++)
    {
        refRatios.push_back(refRatio);
    }
   
    for (int nn = 0; nn < 1; nn++)
    {
        // Create Layout
        auto domain = buildXPoint(domainSize, numBlocks);
        MBDisjointBoxLayout layout(domain, Point::Ones(boxSize));

        // Create Multigrid Operator
        MBMultigrid<BoxOp_MBLaplace, MBMap_XPointRigid, double> mg(layout, refRatio, numLevels); 
        EXPECT_TRUE(mg.validate(layout, refRatios, numLevels));

#if PR_VERBOSE > 0
        h5.writeMBLevel({"x", "y"}, mg.map(numLevels-1), mg.map(numLevels-1).map(), "MAP_N%i",nn);
#endif        

        // Declare Data Holders
        MBLevelBoxData<double, 1, HOST> phi(layout, OP::ghost());
        MBLevelBoxData<double, 1, HOST> rhs(layout, Point::Zeros());
        MBLevelBoxData<double, 1, HOST> res(layout, Point::Zeros());

        // Build the (finest level) map
        MBLevelMap<MBMap_XPointRigid, HOST> map(layout, OP::ghost());
        for (BlockIndex bi = 0; bi < numBlocks; bi++)
        {
            map[bi].setNumBlocks(numBlocks);
        }

        initializeData(phi, rhs, map);
        
        mg.op(numLevels-1)(rhs, phi);
        phi.setVal(0);
#if PR_VERBOSE > 0
        h5.writeMBLevel({"rhs"}, map, rhs, "RHS_0_N%i",nn);
        h5.writeMBLevel({"phi"}, map, phi, "PHI_0_N%i",nn);
        h5.writeMBLevel({"residual"}, map, res, "RES_0_N%i",nn);
#endif
        mg.solve(phi, rhs, 20, 1e-10);
#if PR_VERBOSE > 0
#endif
        domainSize *= 2;
        boxSize *= 2;
    }
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
