////////////////////////////////////////////////////////////////////////////////////////////////////
/// File:	FlowVector.cpp.
///
/// Summary:	Implements the flow vector class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include"FlowVector.h"
#include<EigenOptimizaterDef.h>

//public================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	FlowVector::FlowVector()
///
/// Summary:	Default constructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

FlowVector::FlowVector(){

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	FlowVector::FlowVector(cv::Mat inputImg, vector<vector<FLOWVECTORTAG>>UI_flowVectorArray)
///
/// Summary:	Constructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// inputImg -   	The input image. 
/// parameter2 - 	The second parameter. 
////////////////////////////////////////////////////////////////////////////////////////////////////

// check by H: FlowVector session

FlowVector::FlowVector(cv::Mat inputImg, vector<vector<FLOWVECTORTAG>>UI_flowVectorArray){
	int imgHeigh = inputImg.rows;
	int imgWidth = inputImg.cols;

	this->orgImg = inputImg.clone();
	this->showImg = inputImg.clone();	
	this->RowSampleRate = 10;//each n pixels sample
	this->ColSampleRate = 10;//each n pixels sample
	this->RowPointsN = (imgHeigh %RowSampleRate != 0) ? (imgHeigh / RowSampleRate + 2) : (imgHeigh / RowSampleRate + 1);
	this->ColPointsN = (imgWidth %ColSampleRate != 0) ? (imgWidth / ColSampleRate + 2) : (imgWidth / ColSampleRate + 1);
	//printf("n*n=%d*%d\n", this->RowPointsN, this->ColPointsN);
	std::cout << "n*n= " << this->RowPointsN << "*" << this->ColPointsN << endl;

	this->initVector = -15;
	this->UI_flowVectorArray = UI_flowVectorArray;
	FLOWVECTORTAG tmp_flowVectorTag;
	tmp_flowVectorTag.vector = cv::Point2d(initVector, initVector);
	for (int y = 0; y < imgHeigh; y += RowSampleRate){
		for (int x = 0; x < imgWidth; x += ColSampleRate){
			cv::Point tmp(x, y);
			tmp_flowVectorTag.coordinate = tmp;
			this->flowVector.push_back(tmp_flowVectorTag);
			//keep Boundary
			if (x + ColSampleRate >= imgWidth){
				cv::Point tmp(imgWidth - 1, y);
				tmp_flowVectorTag.coordinate = tmp;
				this->flowVector.push_back(tmp_flowVectorTag);
			}
		}

		//keep Boundary

		if (y + RowSampleRate >= imgHeigh){
			int yBoundary = imgHeigh - 1;
			for (int x = 0; x < imgWidth; x += ColSampleRate){
				cv::Point tmp(x, yBoundary);
				tmp_flowVectorTag.coordinate = tmp;
				this->flowVector.push_back(tmp_flowVectorTag);
				//keep Boundary
				if (x + ColSampleRate >= imgWidth){
					cv::Point tmp(imgWidth - 1, yBoundary);
					tmp_flowVectorTag.coordinate = tmp;
					this->flowVector.push_back(tmp_flowVectorTag);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowVector::EigenOptimizer()
///
/// Summary:	Eigen optimizer.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

// check by H: follow the equation (3)

void FlowVector::EigenOptimizer(){

	printf("start EigenOptimizer==================================\n");

	std::vector<T> coefficients; // list of non-zeros coefficients
	int UI_totalVectorPoint = 0;
	for (int i = 0; i < this->UI_flowVectorArray.size(); i++)
	{ 
		// UI_flowVectorArray.size = N strokes ~ (by H: user-specified paths)
		UI_totalVectorPoint += this->UI_flowVectorArray[i].size(); // each stroke N vectorPoint
	}

	/*		*2 代表 (x,y)
		flowVector.size*2 = N mesh points * [x,y]
		UI_totalVectorPoint*2 = UI_totalVectorPoint * [x,y]
	*/
	int totalVariables = flowVector.size() * 2 + UI_totalVectorPoint * 2;

	/*		*2 代表 (x,y)
		equationN = flowVector.size() * NEIGHBOR_DIRECTION - (Out of range index: (perimeter: (row+col-4)*2*(NEIGHBOR_DIRECTION/2 - 1))* [x,y] )
		+ UI_totalVectorPoint *(2 constrains:fixed C and ||V(i,j)-C||^2) * [x,y]
		PS:boundary constrain = +(2 * col + 2 * (row - 2)) * [x,y]
	*/
	int cornerOutOfRange_N = (NEIGHBOR_DIRECTION == 4) ? 2 : 5;
	int equationN = (flowVector.size() * NEIGHBOR_DIRECTION - ((NEIGHBOR_DIRECTION/2 - 1) * 2 * (this->ColPointsN + this->RowPointsN - 4) + 4 * cornerOutOfRange_N)) * 2
		+ UI_totalVectorPoint * 4;
	vector<double>tmp_b;

	//------------------------------------------
	// Now create/populate an according Sparse Matrix
	//------------------------------------------
	//			neighborDirection							
	cv::Point neighborDirection[8] = { cv::Point(1, 0), cv::Point(0, -1), cv::Point(-1, 0), cv::Point(0, 1),
									   cv::Point(1, -1), cv::Point(-1, -1), cv::Point(-1, 1), cv::Point(1, 1) };
	
	int equationCounter = 0;

	// by H: Checking here (11/23/2021)
	
	// This is the first term in Eq.(3)
	
	for (int row = 0; row < this->flowVector.size(); row++){// fill smooth term
		cv::Point currentMatrixCoordinate = Convert2MatrixCoordinate(flowVector[row].coordinate);
		int arrayIndex_Vij = CoordinateTransformation_Matrix2Array(currentMatrixCoordinate);// V(i,j)

		
		for (int j = 0; j < NEIGHBOR_DIRECTION; j++){// j < smoothDirection size
			cv::Point neighborMatrixCoordinate = currentMatrixCoordinate + neighborDirection[j];// calculate neighbor
			if (!IsOutOfMatrix(neighborMatrixCoordinate)){
				
				// check index
				int arrayIndex_Vij2 = CoordinateTransformation_Matrix2Array(neighborMatrixCoordinate);// V(i,j+1)
				
				// ||V(i,j)-V(i,j+1)||^2

				coefficients.push_back(T(equationCounter, arrayIndex_Vij, 1));		// Vij.x
				coefficients.push_back(T(equationCounter, arrayIndex_Vij2, -1));	// Vij+1.x
				tmp_b.push_back(0);// x
				equationCounter++;
				coefficients.push_back(T(equationCounter, arrayIndex_Vij + 1, 1));	// Vij.y
				coefficients.push_back(T(equationCounter, arrayIndex_Vij2 + 1, -1));	// Vij+1.y
				tmp_b.push_back(0);// y
				equationCounter++;

				
				//printf("||V[%d]-V[%d]||^2 = 0\n", arrayIndex_Vij, arrayIndex_Vij2);
				//printf("||V[%d]-V[%d]||^2 = 0\n", arrayIndex_Vij+1, arrayIndex_Vij2+1);
				
			}
		}
		//printf("\n");
	}// smooth term finish
	
	printf("smooth term equationCounter=%d\n", equationCounter);
	

	// This is the second term in Eq.(3)
	int UI_variableCounter = 0;
	
	for (int i = 0; i < this->UI_flowVectorArray.size(); i++){
		for (int j = 0; j < this->UI_flowVectorArray[i].size(); j++){
				//corner: 1		2	4	3
			vector<BILINEARCORNERTAG>corners = Calculate4CornerIndex(UI_flowVectorArray[i][j].coordinate);
			
			cout << "UI_flowVectorArray[i][j].coordinate: " << UI_flowVectorArray[i][j].coordinate.x << "; " << UI_flowVectorArray[i][j].coordinate.y << endl;

			// x part ------------------------------------------------------------------------
			// ||(w1v1+w2v2+w3v3+w4v4)-c||^2	w1,w2,w3,w4,-1=0

			double lambda = 10, lambda2 = 100;
			// ||(w1v1+w2v2+w3v3+w4v4)-c||^2 = 0
			for (int k = 0; k < corners.size(); k++){
				coefficients.push_back(T(equationCounter, corners[k].index, corners[k].weight*lambda));
				cout << "weight: " <<corners[k].weight << endl;				 								
			}
			
			coefficients.push_back(T(equationCounter, flowVector.size() * 2 + UI_variableCounter, -1* lambda));//variable Cx
			tmp_b.push_back(0);
			equationCounter++;
			// 0,0,0,0,1=c
			coefficients.push_back(T(equationCounter, flowVector.size() * 2 + UI_variableCounter, 1 * lambda2));//variable Cx
			tmp_b.push_back(this->UI_flowVectorArray[i][j].vector.x*lambda2);
			equationCounter++;
			UI_variableCounter++;

			// y part ------------------------------------------------------------------------
			// ||(w1v1+w2v2+w3v3+w4v4)-c||^2 = 0
			for (int k = 0; k < corners.size(); k++)
				coefficients.push_back(T(equationCounter, corners[k].index + 1, corners[k].weight*lambda));
			coefficients.push_back(T(equationCounter, flowVector.size() * 2 + UI_variableCounter, -1 * lambda));//variable Cy
			tmp_b.push_back(0);
			equationCounter++;
			// 0,0,0,0,1=c
			coefficients.push_back(T(equationCounter, flowVector.size() * 2 + UI_variableCounter, 1 * lambda2));//variable Cy
			tmp_b.push_back(this->UI_flowVectorArray[i][j].vector.y*lambda2);
			equationCounter++;
			UI_variableCounter++;
		}
	}// UI constraint term finish
	

	printf("UI_variableCounter.size= UI_vector(%d)*2[x,y]= %d\n", UI_variableCounter / 2, UI_variableCounter);
	
	
	//std::vector mapped to Eigen::VectorXd 
	Eigen::VectorXd b = Eigen::VectorXd::Map(tmp_b.data(), tmp_b.size());// the right hand side-vector 
	/*
	for (int i = 0; i < b.size(); i++){
	printf("b[%d]=\t%f\n",i, b[i]);
	}
	*/

	printf("---------------------------------------------------------------------------------------------------------------------\n");
	printf("(this->flowVector.size() * 4 - 2 * (this->ColPointsN + this->RowPointsN)) * 2 + UI_totalVectorPoint * 4\n");
	printf("=> (%d * 4 - 2 * (%d + %d)) * 2 + %d * 4 = %d\n", this->flowVector.size(), this->ColPointsN, this->RowPointsN, UI_totalVectorPoint, equationN);
	printf("check total equationCounter= %d\tb.size=%d\n", equationCounter, b.size());
	printf("flowVector.size() * 2 + UI_totalVectorPoint * 2 = totalVariables\n");
	printf("=> %d * 2 + %d * 2 = %d\n", this->flowVector.size(), UI_totalVectorPoint, totalVariables);
	SpMat SparseA(equationN, totalVariables);// init SparseA
	printf("SparseA(equationN, totalVariables)=\t(%d, %d)\n", equationN, totalVariables);
	printf("---------------------------------------------------------------------------------------------------------------------\n");
	SparseA.setFromTriplets(coefficients.begin(), coefficients.end());// fill in the value into the SparseA
	
	/*
	SparseA(m*n) -> adjustSparseA(m*m)
	Because the input of BiCGSTAB must be a square matrix
	*/
	SpMat adjustSparseA = SparseA.transpose()*SparseA;
	printf("adjustSparseA size=\t(%d, %d)\n", adjustSparseA.rows(), adjustSparseA.cols());
	//------------------------------------------
	/*
	std::cout << "Here is the Sparse matrix SparseA:\n" << SparseA << std::endl;
	std::cin.get();
	*/
	//*************************************************************
	//   SOLVE: Now we solve the NLS for RHS b using BICGSTAB  method. 
	//*************************************************************
	Eigen::BiCGSTAB<Eigen::SparseMatrix<double> >  BCGST;
	BCGST.compute(adjustSparseA);
	Eigen::VectorXd X;
	Eigen::VectorXd adjust_b = SparseA.transpose()*b;// b(m*1) = A'*b
	printf("adjust_b.size=%d\n", adjust_b.size());
	printf("Solving......\n");
	double START, END;
	START = clock();
	X = BCGST.solve(adjust_b);
	END = clock();
	//cout << endl << "程式執行所花費：" << (double)clock() / CLOCKS_PER_SEC << " S";
	cout << endl << "optimizer" << (END - START) / CLOCKS_PER_SEC << " S" << endl;
	/*
	std::cout << "Here is the Vector x using BICGSTAB :\n" << std::endl;
	for (int i = 0; i < adjust_b.size(); i++){
	printf("X[%d]=\t%f\tadjust_b[%d]=\t%f\n", i, X[i], i, adjust_b[i]);
	}
	*/
	printf("X.size=\t%d\t\n", X.size());
	std::cout << "#iterations:     " << BCGST.iterations() << std::endl;
	std::cout << "estimated error: " << BCGST.error() << std::endl;
	printf("finish EigenOptimizer==================================\n\n");
	vector<double> flowVectorOptimizatieResult(X.data(), X.data() + X.size());
	
	UpdateFlowVector(flowVectorOptimizatieResult);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	void FlowVector::ShowResult(int color_R, int color_G, int color_B, string windowName,
/// 	int windowFlag)
///
/// Summary:	Shows the result.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// color_R -    	The color. 
/// color_G -    	The color g. 
/// color_B -    	The color b. 
/// windowName - 	Name of the window. 
/// windowFlag - 	The window flag. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowVector::ShowResult(int color_R, int color_G, int color_B, string windowName,int windowFlag){
	this->showImg = this->orgImg.clone();	
	for (int i = 0; i < this->flowVector.size(); i++){
		cv::arrowedLine(this->showImg, this->flowVector[i].coordinate, (this->flowVector[i].coordinate + (cv::Point)this->flowVector[i].vector), cv::Scalar(color_B, color_G, color_R), 1);
	}
	cv::namedWindow(windowName,windowFlag);
	cv::imshow(windowName, this->showImg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	cv::Mat FlowVector::GetResultImg(int color_R, int color_G, int color_B)
///
/// Summary:	Gets result image.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// color_R -  	The color. 
/// color_G -  	The color g. 
/// color_B -  	The color b. 
///
/// Returns:	The result image.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat FlowVector::GetResultImg(int color_R, int color_G, int color_B){	
	this->showImg = this->orgImg.clone();
	for (int i = 0; i < this->flowVector.size(); i++){		
		cv::arrowedLine(this->showImg, this->flowVector[i].coordinate, (this->flowVector[i].coordinate + (cv::Point)this->flowVector[i].vector), cv::Scalar(color_B, color_G, color_R), 1.5);
	}
	return this->showImg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowVector::Write2AVI(int color_R, int color_G, int color_B, string gifPath)
///
/// Summary:	Writes a 2 a vi.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// color_R -  	The color. 
/// color_G -  	The color g. 
/// color_B -  	The color b. 
/// gifPath -  	Full pathname of the GIF file. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowVector::Write2AVI(int color_R, int color_G, int color_B, string gifPath){
	//open gif
	cv::VideoCapture gif(gifPath);
	cv::Mat frame;
	vector<cv::Mat>videoVector;
	while (true){
		gif >> frame;
		if (frame.empty())
			break;
		frame = this->DrawFlowFieldOnFrame(color_R, color_G, color_B, frame.clone());
		videoVector.push_back(frame.clone());
	}
	this->MKvideo(videoVector);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	vector<FLOWVECTORTAG>FlowVector::GetFlowVector()
///
/// Summary:	Default constructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Typeparams:
/// FLOWVECTORTAG - 	Type of the flowvectortag. 
////////////////////////////////////////////////////////////////////////////////////////////////////

vector<FLOWVECTORTAG>FlowVector::GetFlowVector(){
	return flowVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowVector::SetFlowVector(vector<FLOWVECTORTAG> inFlowVector)
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

void FlowVector::SetFlowVector(vector<FLOWVECTORTAG> inFlowVector){
	flowVector = vector<FLOWVECTORTAG>(inFlowVector);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	vector<vector<FLOWVECTORTAG>>FlowVector::GetUserFlowVector()
///
/// Summary:	Default constructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Typeparams:
/// FLOWVECTORTAG - 	Type of the flowvectortag. 
////////////////////////////////////////////////////////////////////////////////////////////////////

vector<vector<FLOWVECTORTAG>>FlowVector::GetUserFlowVector(){
	return UI_flowVectorArray;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	cv::Point FlowVector::GetSampleRate()
///
/// Summary:	Gets sample rate.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Returns:	The sample rate.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Point FlowVector::GetSampleRate(){
	return cv::Point(ColSampleRate, RowSampleRate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	cv::Point FlowVector::GetSamplePointsN()
///
/// Summary:	Gets sample points n.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Returns:	The sample points n.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Point FlowVector::GetSamplePointsN(){
	return cv::Point(ColPointsN, RowPointsN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowVector::SetSampleRate(cv::Point sampleRate)
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

void FlowVector::SetSampleRate(cv::Point sampleRate){
	ColSampleRate = sampleRate.x;
	RowSampleRate = sampleRate.y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowVector::SetSamplePointsN(cv::Point samplePointsN)
///
/// Summary:	Sets sample points n.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// samplePointsN - 	The sample points n. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowVector::SetSamplePointsN(cv::Point samplePointsN){
	ColPointsN = samplePointsN.x;
	RowPointsN = samplePointsN.y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowVector::SetOrgImg(cv::Mat img)
///
/// Summary:	Sets organisation image.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// img - 	The image. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowVector::SetOrgImg(cv::Mat img){
	this->orgImg = img.clone();
}

//private================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:
/// 	cv::Mat FlowVector::DrawFlowFieldOnFrame(int color_R, int color_G, int color_B,
/// 	cv::Mat frame)
///
/// Summary:	Draw flow field on frame.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// color_R -  	The color. 
/// color_G -  	The color g. 
/// color_B -  	The color b. 
/// frame -    	The frame. 
///
/// Returns:	A cv::Mat.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat FlowVector::DrawFlowFieldOnFrame(int color_R, int color_G, int color_B, cv::Mat frame){
	cout << "DrawFlowFieldOnFrame ?????????????????" << endl;
	for (int i = 0; i < this->flowVector.size(); i++){
		cv::arrowedLine(frame, this->flowVector[i].coordinate, (this->flowVector[i].coordinate + (cv::Point)this->flowVector[i].vector), cv::Scalar(color_B, color_G, color_R), 1);
	}
	return frame.clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowVector::MKvideo(vector<cv::Mat>videoVector)
///
/// Summary:	Kvideoes the given parameter 1.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// parameter1 - 	The first parameter. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowVector::MKvideo(vector<cv::Mat>videoVector){
	cv::Size videoSize = cv::Size(videoVector[0].cols, videoVector[0].rows);
	cv::VideoWriter writer;
	int fps = 5;
	writer.open("flowFieldResult.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps, videoSize);
	//cv::namedWindow("show image", cv::WINDOW_AUTOSIZE);

	for (int i = 0; i < videoVector.size(); i++){
		writer.write(videoVector[i]);
		//imshow("show image", videoVector[i]);
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
/// Fn:	cv::Point FlowVector::Convert2MatrixCoordinate(cv::Point imgCoordinate)
///
/// Summary:	Convert 2 matrix coordinate.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// imgCoordinate - 	The image coordinate. 
///
/// Returns:	The 2 converted matrix coordinate.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Point FlowVector::Convert2MatrixCoordinate(cv::Point imgCoordinate){
	cv::Point results;
	results.x = (imgCoordinate.x % (this->ColSampleRate) == 0) ? imgCoordinate.x / (this->ColSampleRate) : imgCoordinate.x / (this->ColSampleRate)+1;
	results.y = (imgCoordinate.y % (this->RowSampleRate) == 0) ? imgCoordinate.y / (this->RowSampleRate) : imgCoordinate.y / (this->RowSampleRate)+1;
	return results;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	bool FlowVector::IsOutOfMatrix(cv::Point p)
///
/// Summary:	Query if 'p' is out of matrix.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// p - 	A cv::Point to process. 
///
/// Returns:	True if out of matrix, false if not.
////////////////////////////////////////////////////////////////////////////////////////////////////

bool FlowVector::IsOutOfMatrix(cv::Point p){
	if (p.y >= this->RowPointsN || p.x >= this->ColPointsN || p.x < 0 || p.y < 0)
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	int FlowVector::CoordinateTransformation_Matrix2Array(cv::Point p)
///
/// Summary:	Coordinate transformation matrix 2 array.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// p - 	A cv::Point to process. 
///
/// Returns:	An int.
////////////////////////////////////////////////////////////////////////////////////////////////////

int FlowVector::CoordinateTransformation_Matrix2Array(cv::Point p){
	//*2是因為xy分開來所以間格為2
	return (p.x + p.y*this->ColPointsN)*2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	vector<BILINEARCORNERTAG> FlowVector::Calculate4CornerIndex(cv::Point p)
///
/// Summary:	Calculates the 4 corner index.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// p - 	A cv::Point to process. 
///
/// Returns:	The calculated 4 corner index.
////////////////////////////////////////////////////////////////////////////////////////////////////

// chek by H:

vector<BILINEARCORNERTAG> FlowVector::Calculate4CornerIndex(cv::Point p){
	vector<BILINEARCORNERTAG>cornerIndexArr;
	cv::Point leftTop, leftDown, rightTop, rightDown;
	leftTop.x = this->ColSampleRate * (p.x / this->ColSampleRate);
	leftTop.y = this->RowSampleRate * (p.y / this->RowSampleRate);
		
	rightTop = rightDown = leftDown = leftTop;//init
	rightDown.x += this->ColSampleRate;// rightDown
	rightDown.y += this->RowSampleRate;// rightDown
	rightTop.x += this->ColSampleRate;//	rightTop
	leftDown.y += this->RowSampleRate;//	leftDown

	
	printf("p(%d, %d)\n", p.x, p.y);
	printf("leftTop(%d, %d)\n", leftTop.x, leftTop.y);
	printf("rightTop(%d, %d)\n", rightTop.x, rightTop.y);
	printf("rightDown(%d, %d)\n", rightDown.x, rightDown.y);
	printf("leftDown(%d, %d)\n", leftDown.x, leftDown.y);

	double weightP = (double)(p.x - leftTop.x) / (rightTop.x - leftTop.x);// calculate P
	double weightQ = (double)(p.y - leftTop.y) / (leftDown.y - leftTop.y);// calculate Q
	printf("P=\t%f\tQ=\t%f\n", weightP, weightQ);

	FLOWVECTORTAG target;
	target.coordinate = leftTop;
	target.vector = cv::Point(this->initVector, this->initVector);

	vector<FLOWVECTORTAG>::iterator iter_meshPoint;
	iter_meshPoint = find_if(this->flowVector.begin(), this->flowVector.end(),
		bind2nd(ptr_fun(compareCoordinate<FLOWVECTORTAG>), &target));

	if (iter_meshPoint == this->flowVector.end()){
		printf("meshPoint miss error ==================================================\n");
		system("pause");
	}
	else{
		BILINEARCORNERTAG cornerTmp;
		int index = std::distance(this->flowVector.begin(), iter_meshPoint);
		cornerTmp.index = index*2;//因為xy分開所以要*2
		cornerTmp.weight = weightP*weightQ;
		cornerIndexArr.push_back(cornerTmp);// leftTop

		cornerTmp.index = (index+1)*2;
		cornerTmp.weight = (1 - weightP)*weightQ;
		cornerIndexArr.push_back(cornerTmp);// rightTop

		cornerTmp.index = (index + 1 + this->ColPointsN)*2;
		cornerTmp.weight = (1 - weightP)*(1 - weightQ);
		cornerIndexArr.push_back(cornerTmp);// rightDown

		cornerTmp.index = (index + 1 + this->ColPointsN)*2;
		cornerTmp.weight = weightP*(1 - weightQ);
		cornerIndexArr.push_back(cornerTmp);// leftDown
		return cornerIndexArr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void FlowVector::UpdateFlowVector(vector<double>newFlowVector)
///
/// Summary:	Updates the flow vector described by parameter1.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// parameter1 - 	The first parameter. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void FlowVector::UpdateFlowVector(vector<double>newFlowVector){
	for (int i = 0; i < flowVector.size(); i++){	
		flowVector[i].vector.x = newFlowVector[i * 2];
		flowVector[i].vector.y = newFlowVector[i * 2 + 1];					
	}
}



