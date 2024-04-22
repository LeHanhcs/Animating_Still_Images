////////////////////////////////////////////////////////////////////////////////////////////////////
/// File:	BoundarySmoothOptimizer.h.
///
/// Summary:	Declares the boundary smooth optimizer class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include<opencv2/opencv.hpp>
#include<math.h>
#include<Windows.h>
#include<cstdio>
#include<EigenOptimizaterDef.h>
#include <FlowDriver.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Namespace:	std
///
/// Summary:	.
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Struct:	warpedTag
///
/// Summary:	A warped tag.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct warpedTag WARPEDTAG;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class:	BoundarySmoothOptimizer
///
/// Summary:	A boundary smooth optimizer.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

class BoundarySmoothOptimizer
{
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:
	/// 	BoundarySmoothOptimizer::BoundarySmoothOptimizer(cv::Mat in_orgImg,
	/// 	WARPEDTAG *in_warpingResult, WARPEDTAG *in_markMap, double t);
	///
	/// Summary:	Constructor.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// in_orgImg - 	   	The in organisation image. 
	/// in_warpingResult - 	[in,out] If non-null, the in warping result. 
	/// in_markMap - 	   	[in,out] If non-null, the in mark map. 
	/// t - 			   	A double to process. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	BoundarySmoothOptimizer(cv::Mat in_orgImg, WARPEDTAG *in_warpingResult, WARPEDTAG *in_markMap, double t);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	BoundarySmoothOptimizer::~BoundarySmoothOptimizer();
	///
	/// Summary:	Destructor.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~BoundarySmoothOptimizer();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void BoundarySmoothOptimizer::Run();
	///
	/// Summary:	Runs this object.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void Run();

private:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void BoundarySmoothOptimizer::SetOptimizerMatrixAndRun(int selectTgt);
	///
	/// Summary:	Sets optimizer matrix and run.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// selectTgt - 	The select tgt. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void SetOptimizerMatrixAndRun(int selectTgt);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:
	/// 	void BoundarySmoothOptimizer::UpdateWarpingResult(vector<double> *pOptimizatieResult,
	/// 	const cv::Mat* const in_pMarkMap, cv::Mat* const in_pWarpingResult);
	///
	/// Summary:	Updates the warping result.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// pOptimizatieResult - 	[in,out] If non-null, the optimizatie result. 
	/// in_pMarkMap - 		 	The in p mark map. 
	/// in_pWarpingResult -  	[in,out] If non-null, the in p warping result. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void UpdateWarpingResult(vector<double> *pOptimizatieResult, const cv::Mat* const in_pMarkMap, cv::Mat* const in_pWarpingResult);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void BoundarySmoothOptimizer::CalculateLoss(int selectTgt);
	///
	/// Summary:	Calculates the loss.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// selectTgt - 	The select tgt. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void CalculateLoss(int selectTgt);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void BoundarySmoothOptimizer::SetSystemColor(int color);
	///
	/// Summary:	Sets system color.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// color - 	The color. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void SetSystemColor(int color);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:
	/// 	void BoundarySmoothOptimizer::SetOptimizerMatrix(const int width,
	/// 	void (BoundarySmoothOptimizer::*selectTerm)(int *markVij2, int *Vij_idx, cv::Point *curP,
	/// 	cv::Point *neighborhoodP));
	///
	/// Summary:	Sets optimizer matrix.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// width - 	 	The width. 
	/// selectTerm - 	[in,out] If non-null, the select term. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void SetOptimizerMatrix(const int width, int selectTgt, void (BoundarySmoothOptimizer::*selectTerm)(int *selectTgt, int *markVij2, int *Vij_idx, cv::Point *curP, cv::Point *neighborhoodP));

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:
	/// 	void BoundarySmoothOptimizer::StructureAndSourceCoherenceTerm(int *markVij2, int *Vij_idx,
	/// 	cv::Point *curP, cv::Point *neighborhoodP);
	///
	/// Summary:	Structure and source coherence term.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// markVij2 - 			[in,out] If non-null, the second mark vij. 
	/// Vij_idx - 			[in,out] If non-null, zero-based index of the vij. 
	/// curP - 				[in,out] If non-null, the current p. 
	/// neighborhoodP - 	[in,out] If non-null, the neighborhood p. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void StructureAndSourceCoherenceTerm(int *selectTgt, int *markVij2, int *Vij_idx, cv::Point *curP, cv::Point *neighborhoodP);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:
	/// 	void BoundarySmoothOptimizer::SmoothTerm(int *markVij2, int *Vij_idx, cv::Point *curP,
	/// 	cv::Point *neighborhoodP);
	///
	/// Summary:	Smooth term.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// markVij2 - 			[in,out] If non-null, the second mark vij. 
	/// Vij_idx - 			[in,out] If non-null, zero-based index of the vij. 
	/// curP - 				[in,out] If non-null, the current p. 
	/// neighborhoodP - 	[in,out] If non-null, the neighborhood p. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void SmoothTerm(int *selectTgt, int *markVij2, int *Vij_idx, cv::Point *curP, cv::Point *neighborhoodP);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void BoundarySmoothOptimizer::AllocateAndStatisticalIdx(int selectTgt, int fillWidth);
	///
	/// Summary:	Allocate and statistical index.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// selectTgt - 	The select tgt. 
	/// fillWidth - 	Width of the fill. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void AllocateAndStatisticalIdx(int selectTgt, int fillWidth);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:
	/// 	bool BoundarySmoothOptimizer::IsBoundaryInROI(const cv::Mat* const img,
	/// 	cv::Point checkPoint);
	///
	/// Summary:	Query if 'img' is boundary in roi.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// img - 		 	The image. 
	/// checkPoint - 	The check point. 
	///
	/// Returns:	True if boundary in roi, false if not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool IsBoundaryInROI(const cv::Mat* const img, cv::Point checkPoint);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:
	/// 	void BoundarySmoothOptimizer::MarkOptimizationRegion(cv::Mat* img, cv::Point centerP,
	/// 	int fillWidth);
	///
	/// Summary:	Mark optimization region.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// img - 			[in,out] If non-null, the image. 
	/// centerP -   	The center p. 
	/// fillWidth - 	Width of the fill. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void MarkOptimizationRegion(cv::Mat* img, cv::Point centerP, int fillWidth);

	// variables

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Property:	WARPEDTAG *pWarpingResultPair, *pMarkMapPair
	///
	/// Summary:	Gets the mark map pair.
	///
	/// Returns:	The p mark map pair.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	WARPEDTAG *pWarpingResultPair, *pMarkMapPair;
	/// Summary:	The frame.
	double tFrame;
	/// Summary:	(src,dst)
	cv::Point2i totalVariables;
	/// Summary:	The organisation image.
	cv::Mat orgImg;
	/// Summary:	The mark map.
	cv::Mat *pMarkMap;
	/// Summary:	The warping result.
	cv::Mat *pWarpingResult;
	/// Summary:	The total variables.
	int *pTotalVariables;

	/// Summary:	show something img for testing.
	cv::Mat draw;

	// Optimizer variables
	/// Summary:	list of non-zeros coefficients.
	vector<T> coefficients;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Property:	vector<double>tmp_b
	///
	/// Summary:	Gets the vector.
	///
	/// Typeparams:
	/// double - 	Type of the double. 
	///
	/// Value:
	////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<double>tmp_b;
	/// Summary:	The equation counter.
	int equationCounter;
	int totalOptVariables;
};

