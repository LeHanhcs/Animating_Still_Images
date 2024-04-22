////////////////////////////////////////////////////////////////////////////////////////////////////
/// File:	BoundarySmoothOptimizer.cpp.
///
/// Summary:	Implements the boundary smooth optimizer class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BoundarySmoothOptimizer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	EXECUTION_STATUS
///
/// Summary:	A macro that defines execution status.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define EXECUTION_STATUS 0		// print out which part is currently executed

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	DEBUG_IMG
///
/// Summary:	A macro that defines debug Image.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG_IMG 0				// print debug image

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	BoundarySmoothOptimizer::BoundarySmoothOptimizer(cv::Mat in_orgImg,
/// 	WARPEDTAG* in_warpingResult, WARPEDTAG* in_markMap, double t)
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

BoundarySmoothOptimizer::BoundarySmoothOptimizer(cv::Mat in_orgImg, WARPEDTAG* in_warpingResult, WARPEDTAG* in_markMap, double t){
	pWarpingResultPair = new WARPEDTAG;
	pMarkMapPair = new WARPEDTAG;
	pWarpingResultPair = in_warpingResult;
	pMarkMapPair = in_markMap;

	orgImg = in_orgImg.clone();

	//DEBUG code====================================================================
	//cv::Mat testValue;
	//testValue = cv::Mat((*markImg).src.rows, (*markImg).src.cols, CV_8UC3);
	//for (int y = 0; y < testValue.rows; y++){
	//	for (int x = 0; x < testValue.cols; x++){
	//		cv::Point pos(x, y);
	//		if ((*markImg).src.at<int>(pos) == -2){
	//			testValue.at<cv::Vec3b>(pos) = cv::Vec3b(0, 0, 0);
	//		}
	//		else if ((*markImg).src.at<int>(pos) == -1){
	//			testValue.at<cv::Vec3b>(pos) = cv::Vec3b(255,255,255);
	//		}
	//		else{
	//			testValue.at<cv::Vec3b>(pos) = cv::Vec3b(0, 255, 0);
	//		}
	//	}
	//}
	//cv::imshow("test", testValue);
	//cv::waitKey();

	tFrame = t;

	AllocateAndStatisticalIdx(0, 3);
	AllocateAndStatisticalIdx(1, 3);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void BoundarySmoothOptimizer::AllocateAndStatisticalIdx(int selectTgt, int fillWidth)
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

void BoundarySmoothOptimizer::AllocateAndStatisticalIdx(int selectTgt, int fillWidth){
	cv::Mat orgOutRange;
	if (selectTgt == 0){
		orgOutRange = (*pMarkMapPair).src.clone();
		pMarkMap = &(*pMarkMapPair).src;
		draw = (*pWarpingResultPair).src.clone();
	}
	else if (selectTgt == 1){
		orgOutRange = (*pMarkMapPair).dst.clone();
		pMarkMap = &(*pMarkMapPair).dst;
		draw = (*pWarpingResultPair).dst.clone();
	}

	// MarkOptimizationRegion 
	for (int y = 0; y < orgOutRange.rows; y++){
		for (int x = 0; x < orgOutRange.cols; x++){// loop all pixels
			cv::Point pos(x, y);
			int curIdx = orgOutRange.at<int>(pos);

			// -3 == out of warping region	-2 == background		-1 == ROI	num == OptimizerMatrix's col Index
			if (curIdx == -2 || curIdx == -3) continue;

			if (IsBoundaryInROI(&orgOutRange, pos)){// check boundary
				MarkOptimizationRegion(pMarkMap, pos, fillWidth);
			}
		}
	}

	// Allocation index
	int idx = 0;
	for (int y = 0; y < (*pMarkMap).rows; y++){
		for (int x = 0; x < (*pMarkMap).cols; x++){// loop all pixels
			cv::Point pos(x, y);
			int curIdx = (*pMarkMap).at<int>(pos);
			if (curIdx == -4){
				//printf("Allocation index= %d\n", idx);
				(*pMarkMap).at<int>(pos) = idx++;

				draw.at<cv::Vec3b>(pos) = cv::Vec3b(0, 0, 255);
			}
		}
	}
	//system("pause");

	if (selectTgt == 0){
		totalVariables.x = idx;// set src totalVariables
#if DEBUG_IMG
		cv::imwrite("../Results/allocateIdx_src.png", draw);
#endif
#if EXECUTION_STATUS
		printf("src total idx= %d\n", idx);
#endif
	}
	else if (selectTgt == 1){
		totalVariables.y = idx;// set dst totalVariables
#if DEBUG_IMG
		cv::imwrite("../Results/allocateIdx_dst.png", draw);
#endif
#if EXECUTION_STATUS
		printf("dst total idx= %d\n", idx);
#endif
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	void BoundarySmoothOptimizer::MarkOptimizationRegion(cv::Mat* img, cv::Point centerP,
/// 	int fillWidth)
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

void BoundarySmoothOptimizer::MarkOptimizationRegion(cv::Mat* img, cv::Point centerP, int fillWidth){
	for (int y = -fillWidth; y <= fillWidth; y++){
		for (int x = -fillWidth; x <= fillWidth; x++){
			cv::Point nbDir(x, y);// nbDir = neighborhood shift direction
			cv::Point neighborhoodP = centerP + nbDir;

			cv::Rect rect(0, 0, (*img).cols, (*img).rows);
			if (!rect.contains(neighborhoodP)) continue;

			// -3 == out of warping region	-2 == background		-1 == ROI	num == OptimizerMatrix's col Index
			if ((*img).at<int>(neighborhoodP) != -2)
				(*img).at<int>(neighborhoodP) = -4;// init
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	bool BoundarySmoothOptimizer::IsBoundaryInROI(const cv::Mat* const img,
/// 	cv::Point checkPoint)
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

bool BoundarySmoothOptimizer::IsBoundaryInROI(const cv::Mat* const img, cv::Point checkPoint){
	const int neighborhoodWidth = 1;
	cv::Point neighborhoodP;
	for (int y = -neighborhoodWidth; y <= neighborhoodWidth; y++){
		for (int x = -neighborhoodWidth; x <= neighborhoodWidth; x++){

			cv::Point nbDir(x, y);// nbDir = neighborhood shift direction
			neighborhoodP = checkPoint + nbDir;

			cv::Rect rect(0, 0, (*img).cols, (*img).rows);
			if (!rect.contains(neighborhoodP)) continue;

			// -3 == out of warping region	-2 == background		-1 == ROI	num == OptimizerMatrix's col Index

			if ((*img).at<int>(neighborhoodP) == -3){
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void BoundarySmoothOptimizer::Run()
///
/// Summary:	Runs this object.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

void BoundarySmoothOptimizer::Run(){
#if EXECUTION_STATUS
	SetSystemColor(11);
	printf("===================================================\n");
	printf("before warping 1 loss\n");
	CalculateLoss(0);
	printf("---------------------------------------------------\n");
	printf("before warping 2 loss\n");
	CalculateLoss(1);
	printf("===================================================\n");
	SetSystemColor(7);
#endif

	SetOptimizerMatrixAndRun(0);// warping 1
	SetOptimizerMatrixAndRun(1);// warping 2

#if EXECUTION_STATUS
	SetSystemColor(11);
	printf("===================================================\n");
	printf("after warping 1 loss\n");
	CalculateLoss(0);
	printf("---------------------------------------------------\n");
	printf("after warping 2 loss\n");
	CalculateLoss(1);
	printf("===================================================\n");
	SetSystemColor(7);
#endif
}

//1direction	1row + [...][r,g,b,cr,cg,cb]=[0,0,0,cr,cg,cb]

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void BoundarySmoothOptimizer::SetOptimizerMatrixAndRun(int selectTgt)
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

void BoundarySmoothOptimizer::SetOptimizerMatrixAndRun(int selectTgt){
	// init optimizer variables
	coefficients.clear();
	tmp_b.clear();
	equationCounter = 0;

	if (selectTgt == 0){
		pTotalVariables = &totalVariables.x;
		pMarkMap = &(*pMarkMapPair).src;
		pWarpingResult = &(*pWarpingResultPair).src;
	}
	else if (selectTgt == 1){
		pTotalVariables = &totalVariables.y;
		pMarkMap = &(*pMarkMapPair).dst;
		pWarpingResult = &(*pWarpingResultPair).dst;
	}

	//DEBUG code====================================================================
	//cv::Mat check = cv::Mat((*pMarkMap).rows, (*pMarkMap).cols, CV_8UC3, cv::Scalar(0, 0, 0));
	//for (int y = 0; y < (*pMarkMap).rows; y++){// loop all pixels
	//	for (int x = 0; x < (*pMarkMap).cols; x++){// skip boundary
	//		cv::Point p(x, y);
	//		if ((*pMarkMap).at<int>(p) >= 0){
	//			check.at<cv::Vec3b>(p) = cv::Vec3b(255, 255, 255);
	//			printf("%d\n", (*pMarkMap).at<int>(p));
	//			
	//		}
	//	}
	//}
	//system("pause");
	//cv::imshow("check", check);
	//cv::waitKey();

	SetOptimizerMatrix(1, selectTgt, &BoundarySmoothOptimizer::StructureAndSourceCoherenceTerm);
	totalOptVariables = (*pTotalVariables) * 3;
	SetOptimizerMatrix(1, selectTgt, &BoundarySmoothOptimizer::SmoothTerm);

	//std::vector mapped to Eigen::VectorXd 
	Eigen::VectorXd b = Eigen::VectorXd::Map(tmp_b.data(), tmp_b.size());// the rbight hand side-vector 
#if EXECUTION_STATUS
	printf("equationCounter, totalOptVariables= %d\t%d\n", equationCounter, totalOptVariables);
#endif
	SpMat SparseA(equationCounter, totalOptVariables);// init SparseA
	SparseA.setFromTriplets(coefficients.begin(), coefficients.end());// fill in the vaule into the SparseA

	//SparseA(m*n) -> adjustSparseA(m*m)
	//Because the input of BiCGSTAB must be a square matrix

	SpMat adjustSparseA = SparseA.transpose()*SparseA;
	//*************************************************************
	//   SOLVE: Now we solve the NLS for RHS b using BICGSTAB  method. 
	//*************************************************************
	Eigen::BiCGSTAB<Eigen::SparseMatrix<double> >  BCGST;
	BCGST.compute(adjustSparseA);
	Eigen::VectorXd X;
	Eigen::VectorXd adjust_b = SparseA.transpose()*b;// b(m*1) = A'*b
#if EXECUTION_STATUS
	printf("Solving......\n");
#endif
	//double START, END;
	//START = clock();
	X = BCGST.solve(adjust_b);
	//END = clock();
	//cout << endl << "optimizer進行運算所花費的時間：" << (END - START) / CLOCKS_PER_SEC << " S" << endl;

#if EXECUTION_STATUS
	std::cout << "#iterations:     " << BCGST.iterations() << std::endl;
	std::cout << "estimated error: " << BCGST.error() << std::endl;
	printf("finish EigenOptimizer==================================\n\n");
#endif

	vector<double> optimizatieResult(X.data(), X.data() + X.size());
	if (optimizatieResult.size() - totalOptVariables != 0){
		printf("%d %d\n", optimizatieResult.size(), (*pTotalVariables));
		printf("optimizatieResult.size() - (*pTotalVariables)*3 != 0 error\n");
		system("pause");
	}

	//DEBUG code====================================================================
	//int cnt = 0;
	//for (int i = 0; i <tmp_b.size(); i++){
	//	if (tmp_b[i] > 254)
	//		cnt++;
	//}
	//printf("cnt= %d\n", cnt);
	////system("pause");
	//cnt = 0;
	//for (int i = 0; i <optimizatieResult.size(); i++){
	//	if (optimizatieResult[i] > 254){
	//		//printf("i= %d\n", i);
	//		cnt++;
	//	}
	//}
	//printf("cnt= %d\n", cnt);
	//
	//for (int i = 0; i < arr.size(); i++){
	//	printf("arr id= %d\toptimizatieResult[i]= %lf\t%lf\t%lf\n", arr[i], optimizatieResult[arr[i]], optimizatieResult[arr[i]+1], optimizatieResult[arr[i]+2]);
	//}
	//printf("arr.size()= %d\n", arr.size());

	UpdateWarpingResult(&optimizatieResult, pMarkMap, pWarpingResult);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	void BoundarySmoothOptimizer::SetOptimizerMatrix(const int width,
/// 	void(BoundarySmoothOptimizer::*selectTerm)(int *markVij2, int *Vij_idx, cv::Point *curP,
/// 	cv::Point *neighborhoodP))
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

void BoundarySmoothOptimizer::SetOptimizerMatrix(const int width, int selectTgt, void(BoundarySmoothOptimizer::*selectTerm)(int *selectTgt, int *markVij2, int *Vij_idx, cv::Point *curP, cv::Point *neighborhoodP)) {
	for (int y = 0; y < (*pMarkMap).rows; y++){// loop all pixels
		for (int x = 0; x < (*pMarkMap).cols; x++){// skip boundary
			cv::Point curP(x, y);// current point

			int markVij = (*pMarkMap).at<int>(curP);
			// -3 == out of warping region	-2 == background		-1 == ROI	num == OptimizerMatrix's col Index
			if (markVij < 0) continue;// skip background or ROI
			int Vij_idx = markVij * 3;// pixel id * 3(rgb)

			// check neightborhood
			cv::Point neighborhoodP;
			for (int nbY = -width; nbY <= width; nbY++){// loop neighborhood
				for (int nbX = -width; nbX <= width; nbX++){
					cv::Point nbDir(nbX, nbY);
					if (nbDir == cv::Point(0, 0)) continue;// skip current point
					neighborhoodP = curP + nbDir;// calculate neighborhood point

					cv::Rect rect(0, 0, (*pMarkMap).cols, (*pMarkMap).rows);
					if (!rect.contains(neighborhoodP)) continue;

					// -3 == out of warping region	-2 == background		-1 == ROI	num == OptimizerMatrix's col Index
					int markVij2 = (*pMarkMap).at<int>(neighborhoodP);

					(this->*selectTerm)(&selectTgt, &markVij2, &Vij_idx, &curP, &neighborhoodP);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	void BoundarySmoothOptimizer::StructureAndSourceCoherenceTerm(int *markVij2, int *Vij_idx,
/// 	cv::Point *curP, cv::Point *neighborhoodP)
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

void BoundarySmoothOptimizer::StructureAndSourceCoherenceTerm(int *selectTgt, int *markVij2, int *Vij_idx, cv::Point *curP, cv::Point *neighborhoodP) {
	double lambda_Structure = 1, lambda_C = 10;
	if ((*markVij2) >= 0){
		//structure term * 1-t
		// ||V(i,j)-V(i,j+1)-strC||^2
		// May produce strange results
		// If it produces strange results, The result should be corrected after removing the term
		int Vij2_idx = (*markVij2) * 3;
		cv::Vec3d structureC = (cv::Vec3d)orgImg.at<cv::Vec3b>(*curP) - (cv::Vec3d)orgImg.at<cv::Vec3b>(*neighborhoodP);

		// R==================================================================================
		coefficients.push_back(T(equationCounter, (*Vij_idx), 1 * lambda_Structure));	// Vij.r
		coefficients.push_back(T(equationCounter, Vij2_idx, -1 * lambda_Structure));	// Vij+1.r
		tmp_b.push_back(structureC[2] * (double)lambda_Structure);
		//tmp_b.push_back(0);// for debug
		equationCounter++;// strC.r = C.r

		// G==================================================================================
		coefficients.push_back(T(equationCounter, (*Vij_idx) + 1, 1 * lambda_Structure));		// Vij.g
		coefficients.push_back(T(equationCounter, Vij2_idx + 1, -1 * lambda_Structure));		// Vij+1.g
		tmp_b.push_back(structureC[1] * (double)lambda_Structure);
		//tmp_b.push_back(0);// for debug
		equationCounter++;// strC.g = C.g

		// B==================================================================================
		coefficients.push_back(T(equationCounter, (*Vij_idx) + 2, 1 * lambda_Structure));		// Vij.b
		coefficients.push_back(T(equationCounter, Vij2_idx + 2, -1 * lambda_Structure));		// Vij+1.b
		tmp_b.push_back(structureC[0] * (double)lambda_Structure);
		//tmp_b.push_back(0);// for debug
		equationCounter++;// strC.b = C.b

		// Similar to itself==================================================================================
		// ||V(i,j) - (orgImgV(i,j)*weightT + pWarpingResult*(1 - weightT))||^2
		cv::Vec3d avgColor(0, 0, 0);
		double weightT;
		if ((*selectTgt) == 1) {
			weightT = tFrame + 0.5;
			if (weightT > 1)weightT = 1;
			if (weightT < 0)weightT = 0;
			avgColor += (cv::Vec3d)orgImg.at<cv::Vec3b>(*curP)*weightT;
			avgColor += (cv::Vec3d)(*pWarpingResult).at<cv::Vec3b>(*curP)*(1 - weightT);
		}
		else if ((*selectTgt) == 0) {
			weightT = tFrame - 0.5;
			if (weightT > 1)weightT = 1;
			if (weightT < 0)weightT = 0;
			avgColor += (cv::Vec3d)orgImg.at<cv::Vec3b>(*curP)*(1 - weightT);
			avgColor += (cv::Vec3d)(*pWarpingResult).at<cv::Vec3b>(*curP)*weightT;
		}		
		// R==================================================================================
		coefficients.push_back(T(equationCounter, (*Vij_idx), 1 * lambda_C));			// Vij.r
		tmp_b.push_back((double)avgColor[2] * (double)lambda_C);
		equationCounter++;// // orgC.r = C.r

		// G==================================================================================
		coefficients.push_back(T(equationCounter, (*Vij_idx) + 1, 1 * lambda_C));		// Vij.g
		tmp_b.push_back((double)avgColor[1] * (double)lambda_C);// g
		equationCounter++;// // orgC.r = C.r

		// B==================================================================================
		coefficients.push_back(T(equationCounter, (*Vij_idx) + 2, 1 * lambda_C));		// Vij.b
		tmp_b.push_back((double)avgColor[0] * (double)lambda_C);// b
		equationCounter++;// // orgC.b = C.b
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	void BoundarySmoothOptimizer::SmoothTerm(int *markVij2, int *Vij_idx, cv::Point *curP,
/// 	cv::Point *neighborhoodP)
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

void BoundarySmoothOptimizer::SmoothTerm(int *selectTgt, int *markVij2, int *Vij_idx, cv::Point *curP, cv::Point *neighborhoodP) {
	double lambda_Smooth = 10;
	// -3 == out of warping region	-2 == background		-1 == ROI	num == OptimizerMatrix's col Index
	// smooth * t
	// ||V(i,j)-V(i,j+1)||^2
	if ((*markVij2) == -1 || (*markVij2) == -3){
		cv::Vec3b smoothC = (*pWarpingResult).at<cv::Vec3b>(*neighborhoodP);
		//cv::Vec3b smoothC(0, 0, 255);// for debug

		//DEBUG code====================================================================
		//printf("markVij= %d\n", (*Vij_idx));
		//draw = (*pWarpingResult).clone();
		//cv::Point rectP = (*curP) - cv::Point(3, 3);
		//cv::Point rectP2 = (*curP) + cv::Point(3, 3);
		//cv::rectangle(draw, cv::Rect(rectP, rectP2), cv::Scalar(0, 0, 255));
		//cv::imshow("draw", draw);
		//cv::waitKey();

		// R==================================================================================
		coefficients.push_back(T(equationCounter, (*Vij_idx), 1 * lambda_Smooth));			// Vij.r
		tmp_b.push_back((double)smoothC[2] * (double)lambda_Smooth);// r
		equationCounter++;// // smoothC.r = C.r

		// G==================================================================================
		coefficients.push_back(T(equationCounter, (*Vij_idx) + 1, 1 * lambda_Smooth));		// Vij.g
		tmp_b.push_back((double)smoothC[1] * (double)lambda_Smooth);// g
		equationCounter++;// // smoothC.g = C.g

		// B==================================================================================
		coefficients.push_back(T(equationCounter, (*Vij_idx) + 2, 1 * lambda_Smooth));		// Vij.b
		tmp_b.push_back((double)smoothC[0] * (double)lambda_Smooth);// b
		equationCounter++;// // smoothC.b = C.b
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	void BoundarySmoothOptimizer::UpdateWarpingResult(vector<double> *pOptimizatieResult,
/// 	const cv::Mat* const in_pMarkMap, cv::Mat* const in_pWarpingResult)
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

void BoundarySmoothOptimizer::UpdateWarpingResult(vector<double> *pOptimizatieResult, const cv::Mat* const in_pMarkMap, cv::Mat* const in_pWarpingResult){
	for (int y = 0; y < (*in_pMarkMap).rows; y++){
		for (int x = 0; x < (*in_pMarkMap).cols; x++){
			cv::Point curP(x, y);
			int idx = (*in_pMarkMap).at<int>(curP);
			if (idx >= 0){
				idx = idx * 3;// convert to rgb index

				//draw = (*pWarpingResult).clone();
				//cv::Point rectP = curP - cv::Point(3, 3);
				//cv::Point rectP2 = curP + cv::Point(3, 3);
				//cv::rectangle(draw, cv::Rect(rectP, rectP2), cv::Scalar(0, 0, 255));
				//cv::imshow("draw", draw);
				//cv::waitKey();

				cv::Vec3b tmpColor;
				tmpColor[2] = (uchar)round((*pOptimizatieResult)[idx]);		// r
				tmpColor[1] = (uchar)round((*pOptimizatieResult)[idx + 1]);	// g
				tmpColor[0] = (uchar)round((*pOptimizatieResult)[idx + 2]);	// b

				(*in_pWarpingResult).at<cv::Vec3b>(curP) = tmpColor;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void BoundarySmoothOptimizer::CalculateLoss(int selectTgt)
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

void BoundarySmoothOptimizer::CalculateLoss(int selectTgt){
	int imgCols = (*pMarkMapPair).src.cols, imgRows = (*pMarkMapPair).src.rows;
	const cv::Mat *pMarkMap;
	cv::Mat *pWarpingResult;
	if (selectTgt == 0){
		pMarkMap = &(*pMarkMapPair).src;
		pWarpingResult = &(*pWarpingResultPair).src;
	}
	else if (selectTgt == 1){
		pMarkMap = &(*pMarkMapPair).dst;
		pWarpingResult = &(*pWarpingResultPair).dst;
	}

	unsigned long long int loss[3] = { 0 };// [2]=R		[1]=G	[0]=B
	//structure term * 1-t
	const int strWidth = 1;
	for (int y = strWidth; y < imgRows - strWidth; y++){// loop all pixels
		for (int x = strWidth; x < imgCols - strWidth; x++){// skip boundary
			cv::Point curP(x, y);// current point
			int markVij = (*pMarkMap).at<int>(curP);
			if (markVij == -1 || markVij == -2) continue;// skip background or ROI

			// check neightborhood
			cv::Point neighborhoodP;
			for (int nbY = -strWidth; nbY <= strWidth; nbY++){// loop neighborhood
				for (int nbX = -strWidth; nbX <= strWidth; nbX++){
					cv::Point nbDir(nbX, nbY);
					if (nbDir == cv::Point(0, 0)) continue;// skip current point
					neighborhoodP = curP + nbDir;// calculate neighborhood point

					// -2 == background		-1 == ROI	num == OptimizerMatrix's col Index
					int markVij2 = (*pMarkMap).at<int>(neighborhoodP);
					// ||V(i,j)-V(i,j+1)-strC||^2
					if (markVij2 >= 0){
						cv::Vec3d structureC = (cv::Vec3d)orgImg.at<cv::Vec3b>(curP)-(cv::Vec3d)orgImg.at<cv::Vec3b>(neighborhoodP);
						cv::Vec3d diffVijC = (cv::Vec3d)(*pWarpingResult).at<cv::Vec3b>(curP)-(cv::Vec3d)(*pWarpingResult).at<cv::Vec3b>(neighborhoodP);
						for (int i = 0; i < 3; i++){
							loss[i] += (unsigned long long int)abs(structureC[i] - diffVijC[i]);
						}
					}
				}
			}
		}
	}
	printf("structure term loss R= %d\tG= %d\tB= %d\n", loss[2], loss[1], loss[0]);

	unsigned long long int lossBC[3] = { 0 }, lossSmooth[3] = { 0 };// [2]=R		[1]=G	[0]=B
	int width = 1;
	// smooth and source coherence
	for (int y = width; y < imgRows - width; y++){// loop all pixels
		for (int x = width; x < imgCols - width; x++){// skip boundary
			cv::Point curP(x, y);// current point
			int markVij = (*pMarkMap).at<int>(curP);
			if (markVij == -1 || markVij == -2) continue;// skip background or ROI

			// check neightborhood and select optimize term
			cv::Point neighborhoodP;
			for (int nbY = -width; nbY <= width; nbY++){// loop neighborhood
				for (int nbX = -width; nbX <= width; nbX++){
					cv::Point nbDir(nbX, nbY);
					if (nbDir == cv::Point(0, 0)) continue;// skip current point
					neighborhoodP = curP + nbDir;// calculate neighborhood point

					// -2 == background		-1 == ROI	num == OptimizerMatrix's col Index
					int markVij2 = (*pMarkMap).at<int>(neighborhoodP);
					//boundary constraints
					// ||V(i,j)-orgImgV(i,j)||^2
					if (markVij2 == -2){
						cv::Vec3b orgImgC = orgImg.at<cv::Vec3b>(curP);
						cv::Vec3b wpImgC = (*pWarpingResult).at<cv::Vec3b>(curP);
						for (int i = 0; i < 3; i++){
							lossBC[i] += (unsigned long long int)abs(orgImgC[i] - wpImgC[i]);
						}
					}

					// -2 == background		-1 == ROI	num == OptimizerMatrix's col Index
					//smooth * t
					// ||V(i,j)-V(i,j+1)||^2
					if (markVij2 == -1){
						cv::Vec3b wpCurP_C = (*pWarpingResult).at<cv::Vec3b>(curP);
						cv::Vec3b smooth_C = (*pWarpingResult).at<cv::Vec3b>(neighborhoodP);
						for (int i = 0; i < 3; i++){
							lossSmooth[i] += (unsigned long long int)abs(wpCurP_C[i] - smooth_C[i]);
						}
					}
				}
			}
		}
	}

	printf("boundary constraints loss R= %d\tG= %d\tB= %d\n", lossBC[2], lossBC[1], lossBC[0]);
	printf("smooth term loss R= %d\tG= %d\tB= %d\n", lossSmooth[2], lossSmooth[1], lossSmooth[0]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	BoundarySmoothOptimizer::~BoundarySmoothOptimizer()
///
/// Summary:	Destructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

BoundarySmoothOptimizer::~BoundarySmoothOptimizer()
{
}

//COLORS LIST
//1: Blue
//2: Green
//3: Cyan
//4: Red
//5: Purple
//6: Yellow (Dark)
//7: Default white
//8: Gray/Grey
//9: Bright blue
//10: Brigth green
//11: Bright cyan
//12: Bright red
//13: Pink/Magenta
//14: Yellow
//15: Bright white
//Numbers after 15 include background colors

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void BoundarySmoothOptimizer::SetSystemColor(int color)
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

void BoundarySmoothOptimizer::SetSystemColor(int color){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}