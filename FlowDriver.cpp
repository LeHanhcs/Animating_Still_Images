////////////////////////////////////////////////////////////////////////////////////////////////////
/// File:	FlowDriver.cpp.
///
/// Summary:	Implements the flow driver class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FlowDriver.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	EXECUTION_STATUS
///
/// Summary:	A macro that defines execution status.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define EXECUTION_STATUS 0		//print out which part is currently executed

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	DEBUG_IMG
///
/// Summary:	A macro that defines debug Image.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG_IMG 0				//print debug image

//public================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	FlowDriver::FlowDriver(cv::Mat inputImg)
///
/// Summary:	Constructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// inputImg - 	The input image. 
////////////////////////////////////////////////////////////////////////////////////////////////////

FlowDriver::FlowDriver(cv::Mat inputImg){
	orgImg = inputImg.clone();
	fixedMask = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	trgWeight = cv::Mat(inputImg.rows, inputImg.cols, CV_64FC3, cv::Scalar(0, 0, 0));
	motionMap = cv::Mat(inputImg.rows, inputImg.cols, CV_64FC1, cv::Scalar(0));
	
	isLayer0_Empty = 1;
	isLayer1_Empty = 1;
	threadNum = 30;

	fps = 20;
	totalFrame = 29;
	results.resize(totalFrame);//最後一個不用放因為跟原圖很像，放了造成視覺上有停頓會不順
	L0_Results.resize(totalFrame);
	L1_Results.resize(totalFrame);
	blockCounter = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	FlowDriver::FlowDriver()
///
/// Summary:	Default constructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

FlowDriver::FlowDriver(){
	fps = 20;
	totalFrame = 29;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	cv::Mat FlowDriver::GetBoundryConstraintsImg()
///
/// Summary:	Gets boundry constraints image.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Returns:	The boundry constraints image.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat FlowDriver::GetBoundryConstraintsImg(){
	cv::Mat canny_output = fixedMask.clone();
	cv::cvtColor(canny_output, canny_output, CV_RGB2GRAY, 0);//轉成單通道給findContours用
	//cv::threshold(canny_output, canny_output, 100, 200, cv::THRESH_BINARY);//findContours輸入必須是二元圖

	vector<vector<cv::Point> > contours;
	cv::findContours(canny_output, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);//input image is single-channel

	cv::Mat drawing = fixedMask.clone(); //cv::Mat::zeros(canny_output.size(), CV_8UC3);

	int contoursPolyTotalPoint = 0;
	for (size_t i = 0; i < boundryConstraints.size(); i++){
		contoursPolyTotalPoint += (int)boundryConstraints[i].size();
	}
	printf("%d * ind[0]%d\n", boundryConstraints.size(), boundryConstraints[0].size());
	printf("contoursPolyTotalPoint= %d\n", contoursPolyTotalPoint);
	double colorDiff = (double)1 / contoursPolyTotalPoint;
	int order = 0;

	for (size_t i = 0; i < boundryConstraints.size(); i++){
		cv::Scalar color = SelcetOrderColor((double)1 / boundryConstraints.size()*(int)i);
		cv::drawContours(drawing, boundryConstraints, (int)i, color, 2);

		for (size_t j = 0; j < boundryConstraints[i].size(); j++){
			cv::circle(drawing, boundryConstraints[i][j], (int)3, SelcetOrderColor(colorDiff*order), 2);
			//printf("%d,%d\t", contours_poly[i][j].x, contours_poly[i][j].y);
			order++;
		}
		printf("\n");
	}

	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::drawContours(drawing, contours, (int)i, cv::Scalar(255, 255, 255), 1);
	}

	//imshow("BoundryConstraints", drawing);
	return drawing.clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SetBoundryConstraints(cv::Mat inMask, int select)
///
/// Summary:	Sets boundry constraints.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// inMask -   	The in mask. 
/// select -   	The select. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SetBoundryConstraints(cv::Mat inMask, int select){
	vector<vector<cv::Point> >tmpBoundryConstraints = BoundryConstraints(inMask.clone(), false);
	for (rsize_t i = 0; i < tmpBoundryConstraints.size(); i++){
		boundryConstraints.push_back(tmpBoundryConstraints[i]);
	}

	UpdateFixedMask(inMask, select);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	vector<vector<cv::Point> > FlowDriver::BoundryConstraints(cv::Mat inMask, bool Is_show)
///
/// Summary:	Boundry constraints.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// inMask -   	The in mask. 
/// Is_show -  	True if is show, false if not. 
///
/// Returns:	A vector&lt;vector&lt;cv::Point&gt; &gt;
////////////////////////////////////////////////////////////////////////////////////////////////////

vector<vector<cv::Point> > FlowDriver::BoundryConstraints(cv::Mat inMask, bool Is_show){
	cv::Mat canny_output = inMask.clone();
	cv::cvtColor(canny_output, canny_output, CV_RGB2GRAY, 0);//轉成單通道給findContours用
	//cv::threshold(canny_output, canny_output, 100, 200, cv::THRESH_BINARY);//findContours輸入必須是二元圖

	vector<vector<cv::Point> > contours;
	cv::findContours(canny_output, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);//input image is single-channel
	vector<vector<cv::Point> > contours_poly(contours.size());
	//vector<Rect> boundRect(contours.size());//方形
	//vector<Point2f>centers(contours.size());//圓形
	//vector<float>radius(contours.size());//半徑
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(contours[i], contours_poly[i], 10, true);
		//boundRect[i] = boundingRect(contours_poly[i]);
		//minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
	}

	if (Is_show){
		cv::Mat drawing = cv::Mat::zeros(canny_output.size(), CV_8UC3);
		cv::RNG rng(12345);
		for (size_t i = 0; i < contours.size(); i++)
		{
			cv::Scalar color = SelcetOrderColor((double)1 / contours.size()*(int)i);
			cv::drawContours(drawing, contours_poly, (int)i, color, 2);
			cv::drawContours(drawing, contours, (int)i, cv::Scalar(255, 255, 255), 1);
			/*rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2);
			circle(drawing, centers[i], (int)radius[i], color, 2);*/
		}

		int contoursPolyTotalPoint = 0;
		for (size_t i = 0; i < contours_poly.size(); i++){
			contoursPolyTotalPoint += (int)contours_poly[i].size();
		}
		printf("%d * ind[0]%d\n", contours_poly.size(), contours_poly[0].size());
		printf("contoursPolyTotalPoint= %d\n", contoursPolyTotalPoint);
		double colorDiff = (double)1 / contoursPolyTotalPoint;
		int order = 0;

		for (size_t i = 0; i < contours_poly.size(); i++){
			for (size_t j = 0; j < contours_poly[i].size(); j++){
				cv::circle(drawing, contours_poly[i][j], (int)3, SelcetOrderColor(colorDiff*order), 2);
				//printf("%d,%d\t", contours_poly[i][j].x, contours_poly[i][j].y);
				order++;
			}
			//printf("\n");
		}
		imshow("BoundryConstraints", drawing);
	}

	return contours_poly;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	cv::Mat FlowDriver::GetFixedMask()
///
/// Summary:	Gets fixed mask.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Returns:	The fixed mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat FlowDriver::GetFixedMask(){
	return fixedMask;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SetFixedMask(cv::Mat loadMask)
///
/// Summary:	Sets fixed mask.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// loadMask - 	The load mask. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SetFixedMask(cv::Mat loadMask){
	fixedMask = loadMask.clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SetMattingMask(cv::Mat loadMask)
///
/// Summary:	Sets matting mask.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// loadMask - 	The load mask. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SetMattingMask(cv::Mat loadMask){
	mattingMask = loadMask.clone();

	//update fixedMask by mattingMask
	//for (int y = 0; y < fixedMask.rows; y++){
	//	for (int x = 0; x < fixedMask.cols; x++){
	//		if (fixedMask.at<cv::Vec3b>(cv::Point(x, y)) == cv::Vec3b(0, 0, 255))
	//			continue;
	//		else{
	//			fixedMask.at<cv::Vec3b>(cv::Point(x, y)) = mattingMask.at<cv::Vec3b>(cv::Point(x, y));
	//		}
	//	}
	//}

	//set mattingAlphaMask
	cv::Mat greyMat;
	cv::cvtColor(mattingMask, greyMat, CV_BGR2GRAY);
	greyMat.convertTo(mattingAlphaMask, CV_64F, 1.0 / 255.0);

	printf("SetMattingMask done\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::ShowMattingMask()
///
/// Summary:	Shows the matting mask.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::ShowMattingMask(){
	cv::imshow("mattingMask", mattingMask);
	cv::waitKey();
	cv::destroyAllWindows();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	FlowDriver::~FlowDriver()
///
/// Summary:	Destructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

FlowDriver::~FlowDriver()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SetFlowVector(vector<vector<FLOWVECTORTAG>> inUserFlowVector)
///
/// Summary:	Sets flow vector.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// inUserFlowVector - 	The in user flow vector. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SetFlowVector(vector<vector<FLOWVECTORTAG>> inUserFlowVector){
	userFlowVector = inUserFlowVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SetUserPathArr(vector<vector<cv::Point>> inUserPathArr)
///
/// Summary:	Sets user path array.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// inUserPathArr - 	Array of in user paths. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SetUserPathArr(vector<vector<cv::Point>> inUserPathArr){
	userPathArr = inUserPathArr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SetSampleRate(cv::Point sampleRate)
///
/// Summary:	Sets sample rate.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// sampleRate - 	The sample rate. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SetSampleRate(cv::Point sampleRate){
	colSampleRate = sampleRate.x;
	rowSampleRate = sampleRate.y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SetSamplePointsN(cv::Point pointsN)
///
/// Summary:	Sets sample points n.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// pointsN - 	The points n. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SetSamplePointsN(cv::Point pointsN){
	colPointsN = pointsN.x;
	rowPointsN = pointsN.y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SetFlowVector(vector<FLOWVECTORTAG> inFlowVector)
///
/// Summary:	Sets flow vector.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// inFlowVector - 	The in flow vector. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SetFlowVector(vector<FLOWVECTORTAG> inFlowVector){
	flowVector = inFlowVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SetFlowSpeed(int speedValue)
///
/// Summary:	Sets flow speed.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// speedValue - 	The speed value. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SetFlowSpeed(int speedValue){
	eachSegmentPoints = speedValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::Run(int selectLayer)
///
/// Summary:	Runs.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// selectLayer - 	The select layer. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::Run(int selectLayer){
	cv::Mat resultTmp;
	double tStride = (double)1 / (totalFrame+1);// (totalFrame+1) default =30 越高可能越流暢但相對的步伐越小

	// 1sec		30pixels 
	// 30 (totalFrame+1)->smooth	
	// 1frame	最多 1 pace

	// execute thread
	thread init(&FlowDriver::InitFlowPathSegment, this, eachSegmentPoints, 0);
	thread init2(&FlowDriver::InitBoundryWeight, this);

	InitMotionMap();
	init.join();
	init2.join();

	double START, END;
	vector<cv::Mat>arrResultTmp;
	arrResultTmp.resize(30);
	vector<thread>threads;
	double t = 0;

	START = clock();

	for (int frameCounter = 0; frameCounter < threadNum; frameCounter++){
		threads.push_back(thread(&FlowDriver::ThreadFunc, this, frameCounter, t, ref(arrResultTmp[frameCounter])));
		t += tStride;
	}

	//DEBUG code ====================================================================

	//t = 14 * tStride;
	//ThreadFunc(14, t, arrResultTmp[14]);

	Write2LayerArr(selectLayer, &arrResultTmp, &threads);

	threads[threadNum - 1].join();

	END = clock();

	vector<cv::Mat>* showTmp;
	if (selectLayer == 0)	
		showTmp = &L0_Results;
	else if (selectLayer == 1)	
		showTmp = &L1_Results;

	MKvideo(showTmp);
	ShowResultVideo(showTmp);
	cout << endl << "execition time\t" << (END - START) / CLOCKS_PER_SEC << " sec" << endl;
	blockCounter++;// Complete the animation of this block
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	vector<cv::Mat> FlowDriver::GetResult()
///
/// Summary:	Gets the result.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Returns:	The result.
////////////////////////////////////////////////////////////////////////////////////////////////////

vector<cv::Mat> FlowDriver::GetResult(){
	return results;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::CombineLayer()
///
/// Summary:	Combine layer.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::CombineLayer(){
	if (!L1_Results[0].empty()){
		for (size_t i = 0; i < L0_Results.size(); i++){
			results[i] = cv::Mat(orgImg.rows, orgImg.cols, CV_8UC3, cv::Scalar(0, 0, 0));
			for (int y = 0; y < L0_Results[i].rows; y++){
				for (int x = 0; x < L0_Results[i].cols; x++){
					double maskAlpha = mattingAlphaMask.at<double>(cv::Point(x, y));
					cv::Vec3b L0_color = L0_Results[i].at<cv::Vec3b>(cv::Point(x, y));
					cv::Vec3b L1_color = L1_Results[i].at<cv::Vec3b>(cv::Point(x, y));
					cv::Vec3b blackColor(0, 0, 0);

					if (L0_color == blackColor)
						results[i].at<cv::Vec3b>(cv::Point(x, y)) = L1_color;
					else if (L1_color == blackColor)
						results[i].at<cv::Vec3b>(cv::Point(x, y)) = L0_color;
					else
						results[i].at<cv::Vec3b>(cv::Point(x, y)) = L0_color*maskAlpha + L1_color*(1 - maskAlpha);
				}
			}
		}
	}
	else{
		for (size_t i = 0; i < L0_Results.size(); i++){
			results[i] = cv::Mat(orgImg.rows, orgImg.cols, CV_8UC3, cv::Scalar(0, 0, 0));
			for (int y = 0; y < L0_Results[i].rows; y++){
				for (int x = 0; x < L0_Results[i].cols; x++){
					results[i].at<cv::Vec3b>(cv::Point(x, y)) = L0_Results[i].at<cv::Vec3b>(cv::Point(x, y));
				}
			}
		}
	}

	MKvideo(&results);
	ShowResultVideo(&results);
	//ShowMattingMask();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::UpdateOrgImg(cv::Mat In_orgImg)
///
/// Summary:	Updates the organisation image described by In_orgImg.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// In_orgImg - 	The in organisation image. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::UpdateOrgImg(cv::Mat In_orgImg){
	orgImg = In_orgImg.clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::LoadParameter2Result(string filePath, string imgType, size_t resultSize)
///
/// Summary:	Loads parameter 2 result.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// filePath -   	Full pathname of the file. 
/// imgType -    	Type of the image. 
/// resultSize - 	Size of the result. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::LoadParameter2Result(string filePath, string imgType, size_t resultSize){
	for (size_t i = 0; i < resultSize; i++){
		cv::Mat tmp = cv::imread(filePath + to_string(i) + imgType);		
		if (tmp.empty()) return;

		L0_Results[i] = tmp.clone();
	}
	isLayer0_Empty = 0;
	ShowResultVideo(&L0_Results);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	cv::Point FlowDriver::GetOutputVideoParameter()
///
/// Summary:	Gets output video parameter.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Returns:	The output video parameter.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Point FlowDriver::GetOutputVideoParameter(){
	return cv::Point(fps, totalFrame);
}

//private================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	void FlowDriver::Write2LayerArr(int selectLayer, vector<cv::Mat>*arrResultTmp,
/// 	vector<thread>*threads)
///
/// Summary:	Writes a 2 layer array.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// selectLayer -  	The select layer. 
/// arrResultTmp - 	[in,out] If non-null, the array result temporary. 
/// threads - 	   	[in,out] If non-null, the threads. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::Write2LayerArr(int selectLayer, vector<cv::Mat>*arrResultTmp, vector<thread>*threads){
	int* isLayerN_Empty;
	vector<cv::Mat>* tmpArr;
	if (selectLayer == 0){
		isLayerN_Empty = &isLayer0_Empty;
		tmpArr = &L0_Results;
	}
	else if (selectLayer == 1){
		isLayerN_Empty = &isLayer1_Empty;
		tmpArr = &L1_Results;
	}

	//The last frame does not need to be written to the array
	//because it is close to the original image
	//Writes it cause a pause in the playback of the movie.
	for (int i = 0; i < threadNum - 1; i++){
		(*threads)[i].join();
		if ((*isLayerN_Empty))// init result
			(*tmpArr)[i] = (*arrResultTmp)[i].clone();
		else{
			for (int y = 0; y < (*arrResultTmp)[i].rows; y++){
				for (int x = 0; x < (*arrResultTmp)[i].cols; x++){
					if ((*arrResultTmp)[i].at<cv::Vec3b>(cv::Point(x, y))[0] == 0)
						continue;
					else
						(*tmpArr)[i].at<cv::Vec3b>(cv::Point(x, y)) = (*arrResultTmp)[i].at<cv::Vec3b>(cv::Point(x, y));
				}
			}
		}
	}

	(*isLayerN_Empty) = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::ShowResultVideo(const vector<cv::Mat>* const animatingResult)
///
/// Summary:	Shows the result video.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// animatingResult - 	The animating result. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::ShowResultVideo(const vector<cv::Mat>* const animatingResult){
	int frameIdx = 0;
	while (1){
		if (frameIdx == (*animatingResult).size())
			frameIdx = 0;

		// Display the resulting frame
		if ((*animatingResult)[frameIdx].empty()){
			printf("frame= %d is empty\n", frameIdx);
			printf("size= %d\n", (*animatingResult).size());
			system("pause");
		}
		
		cv::imshow("(*animatingResult)", (*animatingResult)[frameIdx]);

		// Press  ESC on keyboard to exit
		char c = (char)cv::waitKey(25);
		if (c == 27){
			cv::destroyAllWindows();
			break;
		}

		frameIdx++;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::ThreadFunc(int frameCounter, double t, cv::Mat &resultTmp)
///
/// Summary:	Thread function.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// frameCounter - 	The frame counter. 
/// t - 		   	A double to process. 
/// resultTmp -    	[in,out] The result temporary. 
////////////////////////////////////////////////////////////////////////////////////////////////////

// check by H
void FlowDriver::ThreadFunc(int frameCounter, double t, cv::Mat &resultTmp){
#if EXECUTION_STATUS
	printf("frame %d\tt= %lf--------------------------------------------------------------\n", frameCounter, t);
#endif
	vector<cv::Point2d>timePoints = CalculateWarpingPath(t);	// Calculate the target point of warping

	WARPEDTAG markMap; //by H: WARPEDTAG: src img and dst img

	WARPEDTAG warpedResult = Warping(timePoints, &markMap);

	BoundarySmoothOptimizer BS_opter(orgImg, &warpedResult, &markMap, t);
	BS_opter.Run();

	// save warping results
	string fileName;
	fileName = "block" + to_string(blockCounter) + "_warp1_" + to_string(frameCounter) + ".jpg";
	cv::imwrite(outputVideoPath+ fileName, warpedResult.src);
	//by H:
	//fileName = to_string(frameCounter) + ".jpg";
	//cv::imwrite(outputVideoPath + fileName, warpedResult.src);
	//end by H
	fileName = "block" + to_string(blockCounter) + "_warp2_" + to_string(frameCounter) + ".jpg";
	cv::imwrite(outputVideoPath + fileName, warpedResult.dst);

#if DEBUG_IMG
	// save debugResult results
	fileName = "debug_markMap" + to_string(blockCounter) + "_warp1_" + to_string(frameCounter) + ".jpg";
	cv::imwrite(outputVideoPath + fileName, markMap.src);
	fileName = "debug_markMap" + to_string(blockCounter) + "_warp2_" + to_string(frameCounter) + ".jpg";
	cv::imwrite(outputVideoPath + fileName, markMap.dst);
#endif

	//blending..............
	resultTmp = cv::Mat(orgImg.rows, orgImg.cols, orgImg.type(), cv::Scalar(0, 0, 0));
#if EXECUTION_STATUS
	printf("blending....\n");
#endif
	for (int y = 0; y < warpedResult.src.rows; y++){
		for (int x = 0; x < warpedResult.src.cols; x++){
			resultTmp.at<cv::Vec3b>(cv::Point(x, y)) = warpedResult.src.at<cv::Vec3b>(cv::Point(x, y))*(1 - t) + warpedResult.dst.at<cv::Vec3b>(cv::Point(x, y))*t;
		}
	}
#if EXECUTION_STATUS
	printf("pushback\n");
#endif
	fileName = "block" + to_string(blockCounter) + "_results_" + to_string(frameCounter) + ".jpg";
	cv::imwrite(outputVideoPath + fileName, resultTmp);
	//by H
	fileName = to_string(frameCounter) + ".jpg";
	cv::imwrite(outputVideoPath + fileName, resultTmp);
	//end by H
}

// v1's old method

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::SmoothBoundary(cv::Mat* pOutRange, cv::Mat* pWarpedResult, double t)
///
/// Summary:	Smooth boundary.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// pOutRange - 		[in,out] If non-null, the out range. 
/// pWarpedResult - 	[in,out] If non-null, the warped result. 
/// t - 				A double to process. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::SmoothBoundary(cv::Mat* pOutRange, cv::Mat* pWarpedResult, double t){
	cv::Mat orgOutRange = (*pOutRange).clone();
	int width = 3;

	for (int y = width; y < orgOutRange.rows - width; y++){
		for (int x = width; x < orgOutRange.cols - width; x++){// loop all pixels
			cv::Point pos(x, y);
			cv::Vec3b curColor = orgOutRange.at<cv::Vec3b>(pos);

			if (curColor != cv::Vec3b(0, 0, 0) && curColor != cv::Vec3b(255, 0, 0)){// check flow field
				if (IsBoundaryInROI(&orgOutRange, pos, width)){// check boundary
					//顯示
					//cv::circle((*pOutRange), pos, (int)1, cv::Scalar(0, 0, 255), 1); // 劃出來較明顯
					(*pOutRange).at<cv::Vec3b>(pos) = cv::Vec3b(0, 0, 255); // 很細很不明顯

					cv::Vec3d avgColor(0, 0, 0);
					double T = t + 0.5;
					if (T > 1)T = 1;
					if (T < 0)T = 0;
					avgColor += (cv::Vec3d)orgImg.at<cv::Vec3b>(pos)*T;
					avgColor += (cv::Vec3d)(*pWarpedResult).at<cv::Vec3b>(pos)*(1 - T);

					(*pWarpedResult).at<cv::Vec3b>(pos) = (cv::Vec3b)avgColor;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	bool FlowDriver::IsBoundaryInROI(const cv::Mat* const img, cv::Point checkPoint,
/// 	int width)
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
/// width - 	 	The width. 
///
/// Returns:	True if boundary in roi, false if not.
////////////////////////////////////////////////////////////////////////////////////////////////////

bool FlowDriver::IsBoundaryInROI(const cv::Mat* const img, cv::Point checkPoint, int width){
	cv::Point pos;
	for (int y = -width; y <= width; y++){
		for (int x = -width; x <= width; x++){
			cv::Point tmp(x, y);
			pos = checkPoint + tmp;
			if ((*img).at<cv::Vec3b>(pos) == cv::Vec3b(255, 0, 0)){
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::InitMotionMap()
///
/// Summary:	Initializes the motion map.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

// check by H:
void FlowDriver::InitMotionMap(){
	printf("----------------------------------------------------------------> Running InitMotionMap\n");
	for (int y = 0; y < motionMap.rows; y++){
		for (int x = 0; x < motionMap.cols; x++){
			cv::Point2d tmp_tragetPoint(x, y);//now loop's position
			if (fixedMask.at<cv::Vec3b>(tmp_tragetPoint)[0] == 0)
				continue;
			motionMap.at<double>(tmp_tragetPoint) = CalculateCornerVector(tmp_tragetPoint);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::InitBoundryWeight()
///
/// Summary:	Initializes the boundry weight.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::InitBoundryWeight(){

	for (int y = 0; y < trgWeight.rows; y++){
		for (int x = 0; x < trgWeight.cols; x++){
			cv::Point2d tmp_tragetPoint(x, y);//now loop's position
			if (fixedMask.at<cv::Vec3b>(tmp_tragetPoint)[0] == 0)
				continue;

			cv::Point2d sumX(0, 0);
			double sumWeight = 0;

			// add boundry constraints
			for (size_t i_BC = 0; i_BC < boundryConstraints.size(); i_BC++){
				for (size_t idx = 0; idx < boundryConstraints[i_BC].size(); idx++){
					double dist = EuclideanDist((cv::Point2d)boundryConstraints[i_BC][idx], tmp_tragetPoint);
					double motion = 1;// const value
					//1.2	2.0
					double weight = pow((pow(motion, 1.2) / (1e-6 + dist)), 2.0);

					//#if DEBUG_WARPING
					//					debugImg = orgImg.clone();
					//					cv::line(debugImg, tmp_tragetPoint, boundryConstraints[i_BC][idx], cv::Scalar(0, 0, 255), 3);// boundry constraints的點
					//					cv::imshow("debug", debugImg);
					//					cv::waitKey();
					//#endif

					sumX += tmp_tragetPoint*weight;
					sumWeight += weight;
				}
			}

			trgWeight.at<cv::Vec3d>(tmp_tragetPoint) = cv::Vec3d(sumX.x, sumX.y, sumWeight);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	void FlowDriver::MKvideo(const vector<cv::Mat>* const animatingResult, int Cycles,
/// 	string fileName)
///
/// Summary:	Kvideoes.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// animatingResult - 	The animating result. 
/// Cycles - 		  	The cycles. 
/// fileName - 		  	Filename of the file. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::MKvideo(const vector<cv::Mat>* const animatingResult, int Cycles, string fileName){
	cv::Size videoSize = cv::Size((*animatingResult)[0].cols, (*animatingResult)[0].rows);

	cv::VideoWriter writer;
	writer.open(outputVideoPath + fileName + ".avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, videoSize);
	//cv::namedWindow("show image", 0);
	double totalSymbol = 20;
	for (int j = 0; j < Cycles; j++){
		for (int i = 0; i < (*animatingResult).size(); i++){
			cv::Mat frame;
			frame = (*animatingResult)[i];
			if (!frame.empty()){
				writer.write(frame);
				//imshow("show image", frame);
			}
		}
		if (j % 5 == 0){
			system("cls");
			printf("MKvideo progress\n");
			double symbolCnt = (double)j / Cycles * 20;
			printf("|");
			for (int i = 0; i < symbolCnt; i++) printf("#");
			for (int i = 0; i < totalSymbol - symbolCnt; i++) printf(" ");
			printf("|");
			printf(" %.2lf%%\n", (double)j / Cycles * 100);
		}
	}

	// 把avi轉成mp4
	string cmd = "ffmpeg -i " + outputVideoPath + fileName
		+ ".avi -c:v libx264 -crf 19 -preset slow -c:a libfdk_aac -b:a 192k -ac 2 -y " + outputVideoPath
		+ fileName + ".mp4";
	//system("ffmpeg -i ../Results/finalResult.avi -c:v libx264 -crf 19 -preset slow -c:a libfdk_aac -b:a 192k -ac 2 -y ../Results/finalResult.mp4");
	system(cmd.c_str());
	printf("============================================================\n");
	printf("MKvideo done\n");
	printf("============================================================\n");
}

inline cv::Point2d FlowDriver::CalculateTargetPoint(double dist, double radians, cv::Point2d selectedSrcPoint){
	return cv::Point2d(selectedSrcPoint.x + dist*cos(radians), selectedSrcPoint.y - dist*sin(radians));
}

inline double FlowDriver::EuclideanDist(cv::Point2d& centerPoint, cv::Point2d& q){
	cv::Point2d diff = q - centerPoint;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

// check by H: dot product???

inline double FlowDriver::VirtualTheta(cv::Point2d& centerPoint, cv::Point2d& q){
	if ((cv::Point2i)centerPoint == (cv::Point2i)q)// 同一點時會發生0除0的狀況
		return 0;

	// centerPoint is center
	const double PI = acos(-1);
	cv::Point2d VirtualPlane(centerPoint.x + 1000, centerPoint.y);
	cv::Point2d v = q - centerPoint;
	cv::Point2d u = VirtualPlane - centerPoint;

	// theta is radian
	//cos(theta)=a.dot(b)/|a||b|
	double theta = acos(((u.dot(v)) / (sqrt(u.dot(u))*sqrt(v.dot(v)))));
	double thetaAns;

	// because cos() is using radians, it is not necessary to convert to angle
	if ((u.x*v.y - u.y*v.x) > 0)// Because the coordinate axis of the picture is opposite to the normal coordinate axis.
		thetaAns = 2 * PI - theta;// that would make the y-axis coordinate values differ by a minus sign.
	else
		thetaAns = theta;

	//double degrees;
	//if ((u.x*v.y - u.y*v.x)>0)//因為圖片的座標軸跟一般的座標軸是反的
	//	degrees = theta / PI * 180 * -1 + 360;
	//else
	//	degrees = theta / PI * 180;
	//printf("%lf\tdegrees\n", degrees);

	return thetaAns;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	WARPEDTAG FlowDriver::Warping(vector<cv::Point2d>timePoints, WARPEDTAG* pMarkMap)
///
/// Summary:	Warpings.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// parameter1 - 	The first parameter. 
/// pMarkMap -   	[in,out] If non-null, the mark map. 
///
/// Returns:	A WARPEDTAG.
////////////////////////////////////////////////////////////////////////////////////////////////////

// check by H: Warping algorithm

WARPEDTAG FlowDriver::Warping(vector<cv::Point2d>timePoints, WARPEDTAG* pMarkMap){
#if EXECUTION_STATUS
	printf("warping...\n");
#endif

	// -3 == out of warping region	-2 == background		-1 == ROI	num == OptimizerMatrix's col Index

	(*pMarkMap).src = cv::Mat(orgImg.rows, orgImg.cols, CV_32SC1, -2);
	(*pMarkMap).dst = cv::Mat(orgImg.rows, orgImg.cols, CV_32SC1, -2);

	WARPEDTAG warpingResults;

	warpingResults.src = cv::Mat(orgImg.rows, orgImg.cols, orgImg.type(), cv::Scalar(0, 0, 0));
	warpingResults.dst = cv::Mat(orgImg.rows, orgImg.cols, orgImg.type(), cv::Scalar(0, 0, 0));

	for (int y = 0; y < warpingResults.src.rows; y++){
		for (int x = 0; x < warpingResults.src.cols; x++){
			cv::Point2d tmp_tragetPoint(x, y);//now loop's position ; by H: for each pixel p belongs to src image
			cv::Vec3b maskColor = fixedMask.at<cv::Vec3b>(tmp_tragetPoint);

			if (maskColor == cv::Vec3b(0, 0, 0) || maskColor == cv::Vec3b(0, 0, 255))
				continue;

			cv::Point2d srcSumX(0, 0), dstSumX(0, 0);
			double sumWeight = 0;

			//calculate position after each point of backward ---------------------------

			for (size_t i_segment = 0; i_segment < timePoints.size(); i_segment++){

				//by H: tmp_tragetPoint: each point in image; timepoints[]: list of pixel in the refined path
				double dist = EuclideanDist(timePoints[i_segment], tmp_tragetPoint);
				double radians = VirtualTheta(timePoints[i_segment], tmp_tragetPoint); 
				double motion = motionMap.at<double>(tmp_tragetPoint); // by H: return to a double
				double weight = pow((pow(motion, 1.2) / (1e-6 + dist)), 1.0); // by H: in equation (4) alpha = 1.2, epsilon = 1e-6, beta = 1.0

				// select t+1 or t-1

				cv::Point2d srcPoint, dstPoint;
				srcPoint = (cv::Point2d)flowPathSegment[i_segment].front(); // by H: is the fist point of Flowvector[i_segment]
				dstPoint = (cv::Point2d)flowPathSegment[i_segment].back(); // by H: is the last point of Flowvector[i_segment]

				//by H: CalculateTargetPoint: return cv::Point2d(selectedSrcPoint.x + dist*cos(radians), selectedSrcPoint.y - dist*sin(radians));

				cv::Point2d srcTargetPoint = CalculateTargetPoint(dist, radians, srcPoint); // by H: calculate the displacement
				cv::Point2d dstTargetPoint = CalculateTargetPoint(dist, radians, dstPoint);


				srcSumX += srcTargetPoint*weight;// by H: d_sum
				dstSumX += dstTargetPoint*weight; 

				sumWeight += weight; // by H: w_sum

			}

			// add boundry constraints
			cv::Point2d boundryPos(trgWeight.at<cv::Vec3d>(tmp_tragetPoint)[0], trgWeight.at<cv::Vec3d>(tmp_tragetPoint)[1]);
			srcSumX += boundryPos;
			dstSumX += boundryPos;
			sumWeight += trgWeight.at<cv::Vec3d>(tmp_tragetPoint)[2];

			// calculate new point color
			srcSumX /= sumWeight; // by H: d_sum/w_sum
			dstSumX /= sumWeight;

			// -3 == out of warping region	-2 == background		-1 == ROI	num == OptimizerMatrix's col Index

			// draw src image
			if (IsFixedMaskOut(srcSumX)){
				// Replace with the original point
				warpingResults.src.at<cv::Vec3b>(tmp_tragetPoint) = orgImg.at<cv::Vec3b>(tmp_tragetPoint);
				(*pMarkMap).src.at<int>(tmp_tragetPoint) = -3;
			}
			else{
				// Draw the new pixel color
				warpingResults.src.at<cv::Vec3b>(tmp_tragetPoint) = RGB_bilinearInterpolation(srcSumX);
				(*pMarkMap).src.at<int>(tmp_tragetPoint) = -1;
			}

			// draw dst image
			if (IsFixedMaskOut(dstSumX)){
				// Replace with the original point
				warpingResults.dst.at<cv::Vec3b>(tmp_tragetPoint) = orgImg.at<cv::Vec3b>(tmp_tragetPoint);
				(*pMarkMap).dst.at<int>(tmp_tragetPoint) = -3;
			}
			else{
				// Draw the new pixel color
				//printf("p= %lf,%lf\tw= %lf\n", dstSumX.x, dstSumX.y, sumWeight);
				warpingResults.dst.at<cv::Vec3b>(tmp_tragetPoint) = RGB_bilinearInterpolation(dstSumX);
				(*pMarkMap).dst.at<int>(tmp_tragetPoint) = -1;
			}
		}
	}

	return warpingResults;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	cv::Vec3b FlowDriver::RGB_bilinearInterpolation(cv::Point2d p)
///
/// Summary:	RGB bilinear interpolation.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// p - 	A cv::Point2d to process. 
///
/// Returns:	A cv::Vec3b.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Vec3b FlowDriver::RGB_bilinearInterpolation(cv::Point2d p){
	int u1 = ((int)floor(p.x) < 0) ? 0 : (int)floor(p.x);
	int v2 = ((int)floor(p.y) < 0) ? 0 : (int)floor(p.y);//down
	int u2 = ((int)ceil(p.x) >= fixedMask.cols) ? (fixedMask.cols - 1) : (int)ceil(p.x);
	int v1 = ((int)ceil(p.y) >= fixedMask.rows) ? (fixedMask.rows - 1) : (int)ceil(p.y);//up

	double t = p.x - u1;
	double t2 = p.y - v2;

	cv::Vec3b a = orgImg.at<cv::Vec3b>(cv::Point(u1, v2));
	cv::Vec3b b = orgImg.at<cv::Vec3b>(cv::Point(u2, v2));
	cv::Vec3b c = orgImg.at<cv::Vec3b>(cv::Point(u2, v1));
	cv::Vec3b d = orgImg.at<cv::Vec3b>(cv::Point(u1, v1));

	cv::Vec3b tmp = a*(1 - t) + b*t;
	cv::Vec3b tmp2 = d*(1 - t) + c*t;
	cv::Vec3b dstV = tmp*(1 - t2) + tmp2*t2;

	return dstV;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	bool FlowDriver::IsFixedMaskOut(cv::Point2d inP)
///
/// Summary:	Query if 'inP' is fixed mask out.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// inP - 	The in p. 
///
/// Returns:	True if fixed mask out, false if not.
////////////////////////////////////////////////////////////////////////////////////////////////////

bool FlowDriver::IsFixedMaskOut(cv::Point2d inP){
	int px = (int)round(inP.x);
	int py = (int)round(inP.y);
	cv::Point p(px, py);
	cv::Rect rect(0, 0, fixedMask.cols, fixedMask.rows);
	if (!rect.contains(p)){// if contain point p
		return true;
	}
	else{// inside
		if (fixedMask.at<cv::Vec3b>((cv::Point)p) != cv::Vec3b(0, 0, 255)){// if the color of the point p is red
			return false;
		}
		else{
			return true;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	template<typename T> bool compareCoordinate(const T v1, const T* v2)
///
/// Summary:	Compare coordinate.
///
/// Typeparams:
/// T - 	Generic type parameter. 
/// Parameters:
/// v1 - 	   	The first value. 
/// v2 - 	   	The second value. 
///
/// Returns:	True if it succeeds, false if it fails.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> bool compareCoordinate(const T v1, const T* v2){
	return (bool)(v1.coordinate == (*v2).coordinate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	double FlowDriver::CalculateCornerVector(cv::Point2d p)
///
/// Summary:	Calculates the corner vector.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// p - 	A cv::Point2d to process. 
///
/// Returns:	The calculated corner vector.
////////////////////////////////////////////////////////////////////////////////////////////////////

// check by H: Calculate the motion of motionmap at each point p

double FlowDriver::CalculateCornerVector(cv::Point2d p){
	double absVector;
	vector<size_t>idxArr;
	// 1－2
	// |  |
	// 3－4
	// by H: ex p (12, 20)
	int leftTopX = (int)(p.x / colSampleRate)*colSampleRate; // by H: leftTopX = 20
	int leftTopY = (int)(p.y / rowSampleRate)*rowSampleRate; // by H: leftTopY = 10

	/*
	printf("%d, %d——", lx, ly);
	printf("%d, %d\n", lx + ColSampleRate, ly);
	printf("｜\t\t｜\n");
	printf("｜\t\t｜\n");
	printf("｜\t\t｜\n");
	printf("%d, %d——", lx, ly + RowSampleRate);
	printf("%d, %d\n", lx + ColSampleRate, ly + RowSampleRate);
	*/

	int xdir[4] = { 0, colSampleRate, colSampleRate, 0 };// corner point dir ; by H: {0,10,10,0}
	int ydir[4] = { 0, 0, rowSampleRate, rowSampleRate };// corner point dir; by H:  {0,0,  10,10}

	for (int i = 0; i < 4; i++){
		cv::Point newCorrdinate(leftTopX + xdir[i], leftTopY + ydir[i]);
		int idx = 0;
		if (newCorrdinate.x >= orgImg.cols){//校正右方邊界的問題
			newCorrdinate.x = orgImg.cols - 1;
			idx += 1;
		}
		if (newCorrdinate.y >= orgImg.rows){//校正下方邊界的問題
			newCorrdinate.y = orgImg.rows - 1;
			idx += colPointsN;
		}

		idx += newCorrdinate.x / colSampleRate + (newCorrdinate.y / rowSampleRate*colPointsN);

		if (newCorrdinate != flowVector[idx].coordinate)
		{
			printf("meshPoint miss error in FlowDriver.cpp at 733 line ==================================================\n");
			printf("p.x, p.y=\t%lf, %lf\n", p.x, p.y);
			printf("leftTopX, leftTopY=\t%d, %d\n", leftTopX, leftTopY);
			printf("xdir[i], ydir[i]=\t%d, %d\n", xdir[i], ydir[i]);
			printf("newCorrdinate.coordinate=%d, %d\n", newCorrdinate.x, newCorrdinate.y);
			system("pause");
		}
		else
		{
			idxArr.push_back(idx);
		}
	}

	double t1 = p.x - leftTopX;
	double t2 = p.y - leftTopY;
	cv::Point2d tmpV1 = flowVector[idxArr[0]].vector * (1 - t1) + flowVector[idxArr[1]].vector*t1;
	cv::Point2d tmpV2 = flowVector[idxArr[3]].vector * (1 - t1) + flowVector[idxArr[2]].vector*t1;
	cv::Point2d tmpAbs = tmpV1*(1 - t2) + tmpV2*t2;
	absVector = sqrt(tmpAbs.dot(tmpAbs));

	return absVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	vector<cv::Point2d>FlowDriver::CalculateWarpingPath(double t)
///
/// Summary:	Constructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Typeparams:
/// cv::Point2d - 	Type of the cv point 2D. 
/// Parameters:
/// t - 	A double to process. 

////////////////////////////////////////////////////////////////////////////////////////////////////

// check by H

vector<cv::Point2d>FlowDriver::CalculateWarpingPath(double t){
	vector<cv::Point2d> timePoints;
	double index;
	for (size_t i_segment = 0; i_segment < flowPathSegment.size(); i_segment++){ //by H: flowPathSegment: the refined paths

		// it may be a double, then interpolation it

		index = (double)flowPathSegment[i_segment].size()*t;
		if (Is_integer(index)){
			// directly input point
			timePoints.push_back((cv::Point2d)flowPathSegment[i_segment][(size_t)index]);
		}
		else
		{
			// interpolation point
			int floorIdx = (int)floor(index);//downward
			int ceilIdx = (int)ceil(index);//upward

			//printf("floorIdx= %d\tceilIdx= %d\n", floorIdx, ceilIdx);
			double idxT = index - floorIdx;

			cv::Point2d interpolationPoint;
			if (ceilIdx < flowPathSegment[i_segment].size())
				interpolationPoint = (cv::Point2d)flowPathSegment[i_segment][floorIdx] * (1 - idxT) + (cv::Point2d)flowPathSegment[i_segment][ceilIdx] * idxT;
			else
				interpolationPoint = (cv::Point2d)flowPathSegment[i_segment][floorIdx];

			/*
				printf("p1= %d, %d\tt= %lf\n", flowPathSegment[i_segment][floorIdx].x, flowPathSegment[i_segment][floorIdx].y, idxT);
				printf("p2= %d, %d\n", flowPathSegment[i_segment][ceilIdx].x, flowPathSegment[i_segment][ceilIdx].y);
				printf("p= %lf, %lf\n", interpolationPoint.x, interpolationPoint.y);
				system("pause");
			*/

			timePoints.push_back(interpolationPoint);
		}
	}

	return timePoints;
}

inline bool FlowDriver::Is_integer(double inVal)
{
	return (int)floor(inVal) == (int)ceil(inVal);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::InitFlowPathSegment(int eachSegmentPoints, int overlap)
///
/// Summary:	Initializes the flow path segment.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// eachSegmentPoints - 	The each segment points. 
/// overlap - 				The overlap. 
////////////////////////////////////////////////////////////////////////////////////////////////////

// check by H: refine the path by equation (2)

void FlowDriver::InitFlowPathSegment(int eachSegmentPoints, int overlap){
	vector<vector<VECTORTAG>>strokeNarr;
	for (size_t i = 0; i < userPathArr.size(); i++){
		vector<VECTORTAG> strokeN;
		//printf("now i=%d, userPathArr size= %d\n", i, userPathArr.size());
		int segmentCnt; // 
		if ((int)userPathArr[i].size() <= eachSegmentPoints) // by H: eachSegmentPoints: number of point in a particle???
		{
			// 檢測是否小於eachSegmentPoints
			segmentCnt = eachSegmentPoints;
			flowPathSegment.push_back(userPathArr[i]);

			strokeN.push_back(VECTORTAG(userPathArr[i].front(), userPathArr[i].back()));
			strokeNarr.push_back(strokeN);
			continue;
		}
		else
			segmentCnt = (int)round((double)userPathArr[i].size() / eachSegmentPoints);

		int reSampleRate = (int)userPathArr[i].size() / segmentCnt;

		
		for (size_t segmentBegPoint = 0; segmentBegPoint < userPathArr[i].size(); segmentBegPoint += reSampleRate){
			//printf("now segmentBegPoint=%d, segmentBegPoint size= %d\n", segmentBegPoint, userPathArr[i].size());
			size_t segmentEndPoint;
			if (segmentBegPoint != 0){
				segmentBegPoint -= overlap;
			}

			segmentEndPoint = segmentBegPoint + reSampleRate;

			if (segmentEndPoint > userPathArr[i].size())break;
			vector<cv::Point>segment(userPathArr[i].begin() + segmentBegPoint, userPathArr[i].begin() + segmentEndPoint);

			flowPathSegment.push_back(segment);
			strokeN.push_back(VECTORTAG(segment.front(), segment.back()));
		}
		strokeNarr.push_back(strokeN);
				
	}

	//test flowPathSegment result
	cv::Mat flowPathSegmentImg(orgImg.clone());
	for (size_t j = 0; j < strokeNarr.size(); j++) {
		for (size_t i = 0; i < strokeNarr[j].size(); i++) {
			cv::Scalar color = SelcetOrderColor((double)1 / (strokeNarr[j].size() - 1)*(int)i);
			//cv::arrowedLine(flowPathSegmentImg, strokeNarr[j][i].beg, strokeNarr[j][i].end, color, (int)2);
			cv::arrowedLine(flowPathSegmentImg, strokeNarr[j][i].beg, strokeNarr[j][i].end, cv::Scalar(0, 0, 255), (int)2);// by H					
		}

		/* by H: show each path vector
		cv::imshow("flowPathSegmentImg", flowPathSegmentImg);
		imwrite(outputVideoPath + "path_vector.png", flowPathSegmentImg);
		cv::waitKey(0);
		*/
	}

	cv::imshow("flowPathSegmentImg", flowPathSegmentImg);
	imwrite(outputVideoPath+"flowPathSegmentImg.png", flowPathSegmentImg);
	cv::waitKey();
	cv::destroyAllWindows();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::UpdateFixedMask(cv::Mat inMask, int select)
///
/// Summary:	Updates the fixed mask.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// inMask -   	The in mask. 
/// select -   	The select. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::UpdateFixedMask(cv::Mat inMask, int select){
	cout << "--------------> in UpdateFixedMask";

	//by H: filtering the animating region
	
	
	if (select == 0) {// Record white pixels as pixels that can be animated  // by H: 06/20/21 (source: select == 0)
		fixedMask += inMask;
	}
	else if (select == 1) {// Record red pixels as animation-fixed pixels    // by H: 06/20/21 (source: select == 1)
		cv::Mat inv;
		cv::Mat redMap = cv::Mat(fixedMask.rows, fixedMask.cols, CV_8UC3, cv::Scalar(0, 0, 255));
		redMap = inMask&redMap;
		for (int y = 0; y < redMap.rows; y++) {
			for (int x = 0; x < redMap.cols; x++) {
				if (redMap.at<cv::Vec3b>(cv::Point(x, y)) == cv::Vec3b(0, 0, 255)) {
					fixedMask.at<cv::Vec3b>(cv::Point(x, y)) = redMap.at<cv::Vec3b>(cv::Point(x, y));
				}
				else
					continue;
			}
		}
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowDriver::Reset()
///
/// Summary:	Resets this object.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowDriver::Reset(){
	fixedMask = cv::Mat(orgImg.rows, orgImg.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	trgWeight = cv::Mat(orgImg.rows, orgImg.cols, CV_64FC3, cv::Scalar(0, 0, 0));
	motionMap = cv::Mat(orgImg.rows, orgImg.cols, CV_64FC1, cv::Scalar(0));
	boundryConstraints.clear();
	userFlowVector.clear();
	userPathArr.clear();
	flowPathSegment.clear();
	flowVector.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	cv::Scalar FlowDriver::SelcetOrderColor(double step)
///
/// Summary:	Selcet order color.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// step - 	Amount to increment by. 
///
/// Returns:	A cv::Scalar.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Scalar FlowDriver::SelcetOrderColor(double step){
	double r, g, b;
	r = g = b = step;
	//printf("step %f\n", step);
	if (r < ((double)1 / 3)){
		r = 0;
		g = g * 3;
		b = 1 - (b * 3);
	}
	else if (r < ((double)2 / 3)){
		r = (r - ((double)1 / 3)) * 3;
		g = 1;
		b = 0;
	}
	else {
		r = 1;
		g = 1 - (g - ((double)2 / 3)) * 3;
		b = 0;
	}
	return cv::Scalar(b * 255, g * 255, r * 255);
}






