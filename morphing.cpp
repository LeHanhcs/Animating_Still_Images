////////////////////////////////////////////////////////////////////////////////////////////////////
/// File:	morphing.cpp.
///
/// Summary:	Implements the morphing class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "morphing.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	morphing::morphing(string fileName)
///
/// Summary:	Constructor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// fileName - 	Filename of the file. 
////////////////////////////////////////////////////////////////////////////////////////////////////

morphing::morphing(string fileName)
{
	//string fileName = "org_01.png";
	this->imgFlag = 1;
	this->src = cv::imread(imgPath + fileName);
	this->srcWin = "this->src image";
	cv::namedWindow(this->srcWin, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(this->srcWin, this->onMouse, this);

	this->dst = cv::imread(imgPath + fileName);
	this->dstWin = "this->dst image";
	cv::namedWindow(this->dstWin, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(this->dstWin, this->onMouse, this);

	this->orgSrc = this->src.clone();
	this->orgDst = this->dst.clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void morphing::Run()
///
/// Summary:	Runs this object.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

void morphing::Run(){
	while (true){
		//init image show ---------------------------------------------------
		if (this->tmp_global.begVertex.x == -1 && this->tmp_global.endVertex.x == -1){
			cv::imshow(this->srcWin, this->src);
			cv::imshow(this->dstWin, this->dst);
		}

		//when change lineVertex draw line
		if (this->tmp_global.begVertex.x != -1 && this->tmp_global.endVertex.x != -1){
			switch (this->imgFlag){
			case 1:
				cv::line(this->src, this->tmp_global.begVertex, this->tmp_global.endVertex, cv::Scalar(255, 0, 0), 2);
				break;
			case 2:
				cv::line(this->dst, this->tmp_global.begVertex, this->tmp_global.endVertex, cv::Scalar(255, 0, 0), 2);
				break;
			default:
				break;
			}

			this->tmp_global.begVertex.x = -1;
			this->tmp_global.begVertex.y = -1;
			this->tmp_global.endVertex.x = -1;
			this->tmp_global.endVertex.y = -1;
			cv::imshow(this->srcWin, this->src);
			cv::imshow(this->dstWin, this->dst);
		}

		//close window --------------------------------------------------------
		if (cv::waitKey(33) == 27){
			break;
		}

		//print featureVector --------------------------------------------------------
		char GUIInput = cvWaitKey(33);
		if (GUIInput == 'q'){
			//print this->src featureVector -------------------------------------------------
			printf("this->src featureVector size= %d\n", this->srcFeatureVector.size());
			for (int i = 0; i < this->srcFeatureVector.size(); i++){
				printf("%d,%d\t->\t%d,%d\n", this->srcFeatureVector[i].begVertex.x, this->srcFeatureVector[i].begVertex.y, this->srcFeatureVector[i].endVertex.x, this->srcFeatureVector[i].endVertex.y);
			}
			printf("\n");
			//print this->dst featureVector -------------------------------------------------
			printf("this->dst featureVector size= %d\n", this->dstFeatureVector.size());
			for (int i = 0; i < this->dstFeatureVector.size(); i++){
				printf("%d,%d\t->\t%d,%d\n", this->dstFeatureVector[i].begVertex.x, this->dstFeatureVector[i].begVertex.y, this->dstFeatureVector[i].endVertex.x, this->dstFeatureVector[i].endVertex.y);
			}
			printf("\n");
		}
		else if (GUIInput == 'g'){
			cv::Mat morphingResult[21];
			int i = 0;
			float frameInterval = 0.05;
			for (float t = 0.0; t < 1 + frameInterval; t += frameInterval){
				printf("frame %f--------------------------------------------------------------\n", t);
				morphingResult[i] = morphingCalculate(this->orgSrc, this->orgDst, t);
				i++;
			}
			int totalFrame = 1 / frameInterval;
			MKvideo(morphingResult, totalFrame);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	void morphing::MKvideo(cv::Mat morphingResult[], int totalFrame)
///
/// Summary:	Kvideoes.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// morphingResult - 	The morphing result. 
/// totalFrame - 	 	The total frame. 
////////////////////////////////////////////////////////////////////////////////////////////////////

void morphing::MKvideo(cv::Mat morphingResult[], int totalFrame){
	cv::Size videoSize = cv::Size(morphingResult[0].cols, morphingResult[0].rows);
	cv::VideoWriter writer;
	int fps = 30;
	writer.open(resultPath + "VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps, videoSize);
	//cv::namedWindow("show image", 0);

	for (int i = 0; i < totalFrame; i++){
		cv::Mat frame;
		frame = morphingResult[i];
		if (!frame.empty()){
			writer.write(frame);
			//imshow("show image", frame);
		}
	}

}

cv::Mat morphing::morphingCalculate(const cv::Mat src, const cv::Mat dst, float morphingT){
	if (this->srcFeatureVector.size() != this->dstFeatureVector.size()){
		printf("this->srcFeatureVector.size() != this->dstFeatureVector.size() error\n");
		system("pause");
	}

	//morphingVector calculate ------------------------------------------------
	vector<lineVertex>morphingVector;
	lineVertex tmp;
	printf("morphingVector calculate ------------------------------------------------\n");
	for (int i = 0; i < this->srcFeatureVector.size(); i++){
		tmp.begVertex = this->srcFeatureVector[i].begVertex*(1 - morphingT) + this->dstFeatureVector[i].begVertex*morphingT;
		tmp.endVertex = this->srcFeatureVector[i].endVertex*(1 - morphingT) + this->dstFeatureVector[i].endVertex*morphingT;
		morphingVector.push_back(tmp);

		/*printf("begVertex=\t%f,%f\t%f,%f\n", this->srcFeatureVector[i].begVertex.x, this->srcFeatureVector[i].begVertex.y, this->dstFeatureVector[i].begVertex.x, this->dstFeatureVector[i].begVertex.y);
		printf("tmp=\t%f,%f\n", tmp.begVertex.x, tmp.begVertex.y);
		printf("endVertex=\t%f,%f\t%f,%f\n", this->srcFeatureVector[i].endVertex.x, this->srcFeatureVector[i].endVertex.y, this->dstFeatureVector[i].endVertex.x, this->dstFeatureVector[i].endVertex.y);
		printf("tmp=\t%f,%f\n", tmp.endVertex.x, tmp.endVertex.y);*/
	}
	cv::Mat warpedSrc = warp(src, morphingVector, 1);
	cv::Mat warpedDst = warp(dst, morphingVector, 2);
	string fileName = "warp1 " + Float2Str(morphingT) + ".jpg";
	cv::imwrite(resultPath + fileName, warpedSrc);
	fileName = "warp2 " + Float2Str(morphingT) + ".jpg";
	cv::imwrite(resultPath + fileName, warpedDst);

	cv::Mat result(warpedSrc.rows, warpedSrc.cols, warpedSrc.type());
	//blending ----------------------------------------------
	for (int y = 0; y < warpedSrc.rows; y++){
		for (int x = 0; x < warpedSrc.cols; x++){
			result.at<cv::Vec3b>(cv::Point(x, y)) = warpedSrc.at<cv::Vec3b>(cv::Point(x, y))*(1 - morphingT) + warpedDst.at<cv::Vec3b>(cv::Point(x, y))*morphingT;
		}
	}
	cv::imshow("morphing result", result);
	fileName = "morphing " + Float2Str(morphingT) + ".jpg";
	cv::imwrite(resultPath + fileName, result);
	return result;
}

string morphing::Float2Str(float number){
	ostringstream buff;
	buff << number;
	string result = buff.str();
	while (result.size() < 4)
		result += "0";
	return result;
}

cv::Mat morphing::warp(const cv::Mat src, const vector<lineVertex>morphingVector, int selectSrc){
	printf("warp ------------------------------------------------\n");
	cv::Mat dstWarping(this->src.rows, this->src.cols, this->src.type());
	for (int y = 0; y < this->src.rows; y++){
		for (int x = 0; x < this->src.cols; x++){
			cv::Point2f tmp_x(x, y);//now loop's position
			//printf("xytest %f,%f\n", tmp_x.x, tmp_x.y);
			cv::Point2f sumX(0, 0);
			float sumWeight = 0;

			//calculate position after each point of backward ---------------------------
			for (int i_mv = 0; i_mv < morphingVector.size(); i_mv++){
				cv::Point2f qpV = (morphingVector[i_mv].endVertex - morphingVector[i_mv].begVertex);
				cv::Point2f xpV = (tmp_x - morphingVector[i_mv].begVertex);
				float qpAbs = sqrt(pow(qpV.x, 2.0) + pow(qpV.y, 2.0));

				float u = PointDot(xpV, qpV);
				u /= pow(qpAbs, 2.0);//Convert to unit vector

				cv::Point2f perpendicularQP(qpV.y, -qpV.x);
				float v = PointDot(xpV, perpendicularQP) / qpAbs;

				lineVertex selectedSrcFeatureVector; 
				if (selectSrc == 1)
					selectedSrcFeatureVector = this->srcFeatureVector[i_mv];
				else if (selectSrc == 2)
					selectedSrcFeatureVector = this->dstFeatureVector[i_mv];

				cv::Point2f dot_qpV = selectedSrcFeatureVector.endVertex - selectedSrcFeatureVector.begVertex;
				float dot_qpAbs = sqrt(pow(dot_qpV.x, 2.0) + pow(dot_qpV.y, 2.0));
				cv::Point2f dot_perpendicularQP(dot_qpV.y, -dot_qpV.x);

				cv::Point2f newX = selectedSrcFeatureVector.begVertex + u*dot_qpV + ((v*dot_perpendicularQP) / dot_qpAbs);

				float dist = weigthDist(u, selectedSrcFeatureVector, newX, v);

				float weight = (dot_qpAbs / (1e-6 + dist));

				/*
				if (weight < 0){
				printf("weight<0\tqpAbs,v\t%f\t%f\n\n", qpAbs, v);
				}
				*/

				newX = weight*newX;
				/*printf("newX= %f,%f\n", newX.x, newX.y);
				printf("weight= %f\n", weight);*/
				sumX += newX;
				sumWeight += weight;
			}
			//printf("sumX= %f,%f\n\n",sumX.x, sumX.y);
			sumX /= sumWeight;

			//boundaryCheck ------------------------------------------------------------
			if (IsBoundaryOut(this->src, &sumX.x, &sumX.y)){

				//printf("BoundaryOut xy= %d,%d\tsumX= %f,%f\n", x, y, sumX.x, sumX.y);

				/*dstWarping.at<cv::Vec3b>(cv::Point(x, y))[0] = 0;
				dstWarping.at<cv::Vec3b>(cv::Point(x, y))[1] = 0;
				dstWarping.at<cv::Vec3b>(cv::Point(x, y))[2] = 255;*/

				dstWarping.at<cv::Vec3b>(cv::Point(x, y)) = src.at<cv::Vec3b>(cv::Point((int)sumX.x, (int)sumX.y));
			}
			else{
				int u1 = floor(sumX.x);
				int v2 = floor(sumX.y);
				int u2 = ceil(sumX.x);
				int v1 = ceil(sumX.y);
				//printf("%d,%d\t%d,%d\n", u1, v2,u2,v1);
				float t = sumX.x - u1;
				float t2 = sumX.y - v2;
				/*if (IsBoundaryOut(this->src, &u1, &v2)){
				printf("BoundaryOut %d,%d\txy= %d,%d\tu1,v2= %d,%d\n", this->src.cols, this->src.rows, x, y, u1, v2);
				}*/
				cv::Vec3b a = src.at<cv::Vec3b>(cv::Point(u1, v2));
				cv::Vec3b b = src.at<cv::Vec3b>(cv::Point(u2, v2));
				cv::Vec3b c = src.at<cv::Vec3b>(cv::Point(u2, v1));
				cv::Vec3b d = src.at<cv::Vec3b>(cv::Point(u1, v1));
				//printf("%d,%d,%d\n", a[0], a[1], a[2]);
				//printf("t= %f\n", t);
				cv::Vec3b tmp = a*(1 - t) + b*t;
				cv::Vec3b tmp2 = d*(1 - t) + c*t;
				cv::Vec3b dstV = tmp*(1 - t2) + tmp2*t2;

				dstWarping.at<cv::Vec3b>(cv::Point(x, y)) = dstV;
				//printf("%d,%d,%d\n", dstV[0], dstV[1], dstV[2]);
			}
		}
	}

	if (selectSrc == 1)
		cv::imshow("srcWarped", dstWarping);
	else if (selectSrc == 2)
		cv::imshow("dstWarped", dstWarping);

	return dstWarping;
}

float morphing::weigthDist(float u, lineVertex line, cv::Point2f newX, float v){
	cv::Point2f tmp;
	if (u > 1){
		tmp = newX - line.endVertex;
	}
	else if (u < 0){
		tmp = newX - line.begVertex;
	}
	else{

		cv::Point2f dot_qpV = line.endVertex - line.begVertex;
		tmp = line.begVertex + u*dot_qpV;
		tmp -= newX;
	}
	return sqrt(PointDot(tmp, tmp));
}

bool morphing::IsBoundaryOut(cv::Mat img, float *x, float *y){
	bool flag = false;
	if ((*x) < FLT_EPSILON){
		(*x) = 0;
		flag = true;
	}
	if ((*y) < FLT_EPSILON){
		(*y) = 0;
		flag = true;
	}
	if ((*x) > img.cols - 1){
		(*x) = img.cols - 1;
		flag = true;
	}
	if ((*y) > img.rows - 1){
		(*y) = img.rows - 1;
		flag = true;
	}

	if (flag)
		return true;
	else
		return false;
}

bool morphing::IsBoundaryOut(cv::Mat img, int *x, int *y){
	bool flag = false;
	if ((*x) < FLT_EPSILON){
		(*x) = 0;
		flag = true;
	}
	if ((*y) < FLT_EPSILON){
		(*y) = 0;
		flag = true;
	}
	if ((*x) >= img.cols){
		(*x) = img.cols - 1;
		flag = true;
	}
	if ((*y) >= img.rows){
		(*y) = img.rows - 1;
		flag = true;
	}

	if (flag)
		return true;
	else
		return false;
}

inline float morphing::PointDot(cv::Point2f a, cv::Point2f b){
	return (a.x*b.x) + (a.y*b.y);
}

void morphing::onMouse(int Event, int x, int y, int flags, void* userdata)
{
	// Check for null pointer in userdata and handle the error
	morphing* temp = reinterpret_cast<morphing*>(userdata);
	temp->on_Mouse(Event, x, y);
}

void morphing::on_Mouse(int Event, int x, int y){
	//draw feature control --------------------------------------------
	if (Event == CV_EVENT_LBUTTONDOWN){
		this->tmp_global.begVertex.x = x;
		this->tmp_global.begVertex.y = y;
		printf("L_buttonDown= %d,%d\n", x, y);
	}
	if (Event == CV_EVENT_LBUTTONUP){
		this->tmp_global.endVertex.x = x;
		this->tmp_global.endVertex.y = y;
		printf("L_buttonUp= %d,%d\n", x, y);
		switch (this->imgFlag){
		case 1:
			printf("add to this->srcFeatureVector\n\n");
			this->srcFeatureVector.push_back(this->tmp_global);
			break;
		case 2:
			printf("add to this->dstFeatureVector\n\n");
			this->dstFeatureVector.push_back(this->tmp_global);
			break;
		default:
			break;
		}
	}

	/*if (Event == CV_EVENT_MOUSEMOVE){
	printf("mouse= %d,%d\n", x, y);
	}*/

	//select img --------------------------------------------

	if (Event == CV_EVENT_RBUTTONDOWN){
		switch (this->imgFlag){
		case 1:
			this->imgFlag = 2;
			break;
		case 2:
			this->imgFlag = 1;
			break;
		default:
			break;
		}
		printf("change to img %d---------------------------------------\n", this->imgFlag);
	}
}


morphing::~morphing()
{
}
