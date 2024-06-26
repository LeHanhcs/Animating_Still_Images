////////////////////////////////////////////////////////////////////////////////////////////////////
/// File:	EigenOptimizaterDef.h.
///
/// Summary:	Declares the eigen optimizater definition class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include<Eigen/Sparse>//cause scissor code complete function doesn't work




//typedef				 =======================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Typedef:	Eigen::SparseMatrix<double> SpMat
///
/// Summary:	Defines an alias representing the sp matrix.
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Eigen::SparseMatrix<double> SpMat; // declares a column-major sparse matrix type of double
typedef Eigen::Triplet<double> T; // declares a Triplet type of double




