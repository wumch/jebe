
#define OMP_NUM_THREADS	4
#define EIGEN_HAS_OPENMP 1

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include "svd.hpp"

namespace jebe {
namespace cluster {
namespace classify {

} /* namespace classify */
} /* namespace cluster */
} /* namespace jebe */



int main(int argc, char* argv[])
{
	typedef Eigen::SparseMatrix<float, 1, int> MatrixType;
//	Eigen::MatrixXf mv = Eigen::MatrixXf::Random(300,50000);
//	mv[0];

	int rows = boost::lexical_cast<int>(argv[1]);
	int cols = boost::lexical_cast<int>(argv[2]);

//	MatrixType m;//(30, 5000);//, 200, mv.innerSize(), mv.outerSize(), mv.data());
	sleep(2);
	MatrixType m(rows, cols);
	sleep(2);
	std::cout << "non-zeros:" << m.nonZeros() << std::endl;
	m.setZero();
	m.makeCompressed();
	// = mv.sparseView();
	m.insertBack(rows - 1, cols - 1) = (MatrixType::Scalar)10.0;
	std::cout << "non-zeros:" << m.nonZeros() << std::endl;
//	m.insert();

//    Eigen::SparseMatrix<float, 0, int> m;
    Eigen::JacobiSVD<Eigen::MatrixXf> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
	sleep(2);

    //*
    using namespace std;
    cout << "Here is the matrix m:" << endl << m.rows() << endl;
    cout << "Its singular values are:" << endl << svd.singularValues().rows() << endl;
    cout << "Its left singular vectors are the columns of the thin U matrix:" << endl << svd.matrixU().rows() << endl;
    cout << "Its right singular vectors are the columns of the thin V matrix:" << endl << svd.matrixV().rows() << endl;
//    cout << "U * V:" << endl << svd.singularValues().dot(svd.matrixV().transpose()) << endl;


/*
    Eigen::JacobiSVD<Eigen::MatrixXf> svd2(mv, Eigen::ComputeThinU | Eigen::ComputeThinV);
    using namespace std;
    cout << "Here is the matrix m:" << endl << mv << endl;
    cout << "Its singular values are:" << endl << svd2.singularValues() << endl;
    cout << "Its left singular vectors are the columns of the thin U matrix:" << endl << svd2.matrixU() << endl;
    cout << "Its right singular vectors are the columns of the thin V matrix:" << endl << svd2.matrixV() << endl;

    // */
    //Vector3f rhs(1, 0, 0);
    //cout << "Now consider this rhs vector:" << endl << rhs << endl;
    //cout << "A least-squares solution of m*x = rhs is:" << endl << svd.solve(rhs) << endl;
}
