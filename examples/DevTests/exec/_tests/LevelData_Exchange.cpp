#include "Proto.H"
#include "InputParser.H"

#define NUMCOMPS 1

using namespace Proto;

PROTO_KERNEL_START
void
f_rampHostF(const Point& a_pt, Var<int,NUMCOMPS, HOST>& a_data)
{
    Point x = a_pt + Point::Ones();
    for (int comp = 0; comp < NUMCOMPS; comp++)
    {
        a_data(comp) = (comp+1)*10 + x[0];
#if DIM > 1
        a_data(comp) = (comp+1)*100 + 10*x[0] + x[1];
#endif
#if DIM > 2
        a_data(comp) = (comp+1)*1000 + 100*x[0] + 10*x[1] + x[2];
#endif
    }
}
PROTO_KERNEL_END(f_rampHostF, f_rampHost);

PROTO_KERNEL_START
void
f_rampDeviF(const Point& a_pt, Var<int,NUMCOMPS, DEVICE>& a_data)
{
    Point x = a_pt + Point::Ones();
    for (int comp = 0; comp < NUMCOMPS; comp++)
    {
        a_data(comp) = (comp+1)*10 + x[0];
#if DIM > 1
        a_data(comp) = (comp+1)*100 + 10*x[0] + x[1];
#endif
#if DIM > 2
        a_data(comp) = (comp+1)*1000 + 100*x[0] + 10*x[1] + x[2];
#endif
    }
}
PROTO_KERNEL_END(f_rampDeviF, f_rampDevi);

int main(int argc, char** argv)
{
#ifdef PR_MPI
    MPI_Init(&argc, &argv);
#endif

    HDF5Handler h5;

    int domainSize = 64;
    int boxSize = 16;
    int ghostSize = 1;
    std::array<bool, DIM> periodicity;
    int testNum = 0;
    for (int dir = 0; dir < DIM; dir++) { periodicity[dir] = true; }

    InputArgs args;
    args.add("domainSize",      domainSize);
    args.add("boxSize",         boxSize);
    args.add("ghostSize",       ghostSize);
    args.add("testNum",         testNum);
    args.add("periodic_x",      periodicity[0]);
    args.add("periodic_y",      periodicity[1]);
#if DIM > 2
    args.add("periodic_z",      periodicity[2]);
#endif
    args.parse(argc, argv);
    args.print();

    std::array<double, DIM> dx;
    dx.fill(1.0);

    Point boxSizeV = Point::Ones(boxSize);
    Box domainBox = Box::Cube(domainSize);
    ProblemDomain domain(domainBox, periodicity);
    DisjointBoxLayout layout(domain, boxSizeV);

    if (testNum == 0)
    {
        if (procID() == 0)
        {
            std::cout << "RUNNING TEST 0: MEMTYPE = HOST" << std::endl;
        }
        LevelBoxData<double, NUMCOMPS, HOST> srcData(layout, Point::Zeros());
        LevelBoxData<double, NUMCOMPS, HOST> dstData(layout, Point::Ones(ghostSize));
        LevelBoxData<double, NUMCOMPS, HOST> errData(layout, Point::Ones(ghostSize));

        dstData.setVal(-1);
        srcData.setVal(42);
        errData.setVal(42);
        //srcData.initialize(f_rampHost); 
        //errData.initialize(f_rampHost);
        for (auto iter = layout.begin(); iter.ok(); ++iter)
        {
            auto& src_i = srcData[*iter];
            auto& dst_i = dstData[*iter];
            src_i.copyTo(dst_i);
        }
        
        h5.writeLevel(dx, srcData, "SRC");
        h5.writeLevel(dx, dstData, "DST_0");

        dstData.exchange();

        h5.writeLevel(dx, dstData, "DST_1");

        Reduction<double, Abs> rxn;
        for (auto iter = layout.begin(); iter.ok(); ++iter)
        {
            BoxData<double, NUMCOMPS, HOST>& dst_i = dstData[*iter];
            BoxData<double, NUMCOMPS, HOST>& err_i = errData[*iter];

            err_i -= dst_i;
            err_i.absMax(rxn);
        }

        h5.writeLevel(dx, errData, "ERROR");

        if (rxn.fetch() < 1.0e-12) 
        {
            std::cout << "Error: " << rxn.fetch() << " | TEST PASSES." << std::endl;
        } else {
            std::cout << "Error: " << rxn.fetch() << " | TEST FAILS."  << std::endl;
        }
    }
#ifdef PR_MPI
    MPI_Finalize();
#endif
}
