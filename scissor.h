////////////////////////////////////////////////////////////////////////////////////////////////////
/// File:	scissor.h.
///
/// Summary:	Declares the scissor class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SCISSOR_H
/// Summary:	.
#define SCISSOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <QMouseEvent>
#include <QTime>
#include <queue>
#include"FlowVector.h"
#include"FlowDriver.h"
#include<fstream>
#include "sharedmatting.h"
#include<qmessagebox.h>
#include<qdesktopservices.h>
#include<qurl.h>

#include<Windows.h>
// use to add a floder for specified path
#include <direct.h>

#include<IO_FilePath.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	INITIAL
///
/// Summary:	A macro that defines initial.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define INITIAL 0

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	ACTIVE
///
/// Summary:	A macro that defines active.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define ACTIVE 1

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	EXPANDED
///
/// Summary:	A macro that defines expanded.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define EXPANDED 2

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	INF
///
/// Summary:	A macro that defines inf.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define INF 1000000

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	XOFFSET
///
/// Summary:	A macro that defines xoffset.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define XOFFSET 12

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	YOFFSET
///
/// Summary:	A macro that defines yoffset.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define YOFFSET 50

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	CLOSETHRES
///
/// Summary:	A macro that defines closethres.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLOSETHRES 40

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Namespace:	cv
///
/// Summary:	.
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace cv;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Struct:	Node
///
/// Summary:	A node.
///
/// Author:	Yc Lin.
///
/// Date:	2019/10/4.
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Node_2{
    double linkCost[9];
    /// Summary:	The state.
    int state;
    /// Summary:	The total cost.
    double totalCost;
    /// Summary:	The previous node.
    Node_2 *prevNode;
    /// Summary:	The previous node number.
    int prevNodeNum;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Property:	int col,row
    ///
    /// Summary:	Gets the row.
    ///
    /// Returns:	The row.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    int col,row;
    /// Summary:	Number of.
    int num;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	bool operator< (const Node &a) const { return totalCost>a.totalCost; }
    ///
    /// Summary:	Less-than comparison operator.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// a - 	A Node to process. 
    ///
    /// Returns:	True if the first parameter is less than the second.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool operator < (const Node_2 &a) const {
        return totalCost>a.totalCost;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Namespace:	Ui
///
/// Summary:	.
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Ui {
class scissor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class:	scissor
///
/// Summary:	A scissor.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

class scissor : public QMainWindow
{
    Q_OBJECT

public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	explicit scissor::scissor(QWidget *parent = 0);
    ///
    /// Summary:	Constructor.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// parent - 	[in,out] (Optional) If non-null, the parent. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    explicit scissor(QWidget *parent = 0);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	scissor::~scissor();
    ///
    /// Summary:	Destructor.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    ~scissor();

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	private slots: void scissor::openFile();
///
/// Summary:	Opens the file.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

private slots:
    void openFile();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionOpen_triggered();
    ///
    /// Summary:	Handles action open triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionOpen_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionZoom_In_triggered();
    ///
    /// Summary:	Handles action zoom in triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionZoom_In_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionZoom_Out_triggered();
    ///
    /// Summary:	Handles action zoom out triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionZoom_Out_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionOrigin_Map_triggered();
    ///
    /// Summary:	Handles action origin map triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionOrigin_Map_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionGradient_Map_triggered();
    ///
    /// Summary:	Handles action gradient map triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionGradient_Map_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionStart_triggered();
    ///
    /// Summary:	Handles action start triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionStart_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::Zoom_in_out(double index);
    ///
    /// Summary:	Zoom in out.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// index - 	Zero-based index of the. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void Zoom_in_out(double index);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::show_image(cv::Mat const& src,int n);
    ///
    /// Summary:	Shows the image.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// src - 	   	Source for the. 
    /// n - 	   	An int to process. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void show_image(cv::Mat const& src,int n);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::gradient();
    ///
    /// Summary:	Gradients this object.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void gradient();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::showNode(struct Node *node );
    ///
    /// Summary:	Shows the node.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// node - 	[in,out] If non-null, the node. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void showNode(struct Node_2 *node );

    //void delaymsec(int msec);

    //void mousePressEvent(QMouseEvent *event);

    //void mouseMoveEvent(QMouseEvent *event);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	bool scissor::eventFilter(QObject *obj, QEvent *event);
    ///
    /// Summary:	Event filter.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// obj - 	   	[in,out] If non-null, the object. 
    /// event -    	[in,out] If non-null, the event. 
    ///
    /// Returns:	True if it succeeds, false if it fails.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool eventFilter(QObject *obj, QEvent *event);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::findPathGraph(int x, int y);
    ///
    /// Summary:	Searches for the first path graph.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// x - 	   	The x coordinate. 
    /// y - 	   	The y coordinate. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void findPathGraph(int x, int y);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::moveMouse(int x, int y);
    ///
    /// Summary:	Move mouse.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// x - 	   	The x coordinate. 
    /// y - 	   	The y coordinate. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void moveMouse(int x, int y);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::getSeedImage();
    ///
    /// Summary:	Gets seed image.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void getSeedImage();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::initial();
    ///
    /// Summary:	Initials this object.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initial();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionBack_triggered();
    ///
    /// Summary:	Handles action back triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionBack_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionRestart_triggered();
    ///
    /// Summary:	Handles action restart triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionRestart_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::closeDetect(int x, int y);
    ///
    /// Summary:	Closes a detect.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// x - 	   	The x coordinate. 
    /// y - 	   	The y coordinate. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void closeDetect(int x, int y);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionDrawNodeVector_triggered();
    ///
    /// Summary:	Handles action draw node vector triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionDrawNodeVector_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionSave_Image_triggered();
    ///
    /// Summary:	Handles action save image triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionSave_Image_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::getMask(int x,int y);
    ///
    /// Summary:	Gets a mask.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ///
    /// Parameters:
    /// x - 	   	The x coordinate. 
    /// y - 	   	The y coordinate. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void getMask(int x,int y);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionGet_Mask_triggered();
    ///
    /// Summary:	Handles action get mask triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionGet_Mask_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionSave_Mask_triggered();
    ///
    /// Summary:	Handles action save mask triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionSave_Mask_triggered();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Fn:	void scissor::on_actionExit_triggered();
    ///
    /// Summary:	Handles action exit triggered signals.
    ///
    /// Author:	Yc Lin
    ///
    /// Date:	2019/10/4
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void on_actionExit_triggered();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_runMorphing_clicked();
	///
	/// Summary:	Handles run morphing clicked signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_runMorphing_clicked();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_fixedROIFlag_toggled(bool checked);
	///
	/// Summary:	Handles fixed roi flag toggled signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// checked - 	True if checked. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_fixedROIFlag_toggled(bool checked);// select ROI to fixed

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_animatingROIFlag_toggled(bool checked);
	///
	/// Summary:	Handles animating roi flag toggled signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// checked - 	True if checked. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_animatingROIFlag_toggled(bool checked);// select ROI to animated

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_flowFieldFlag_toggled(bool checked);
	///
	/// Summary:	Handles flow field flag toggled signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// checked - 	True if checked. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_flowFieldFlag_toggled(bool checked);// draw flow vector

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_animatingPictureFlag_toggled(bool checked);
	///
	/// Summary:	Handles animating picture flag toggled signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// checked - 	True if checked. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_animatingPictureFlag_toggled(bool checked);// animating picture with redhawk effect

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_Run_clicked();
	///
	/// Summary:	Handles run clicked signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_Run_clicked();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_showConstraints_clicked();
	///
	/// Summary:	Handles show constraints clicked signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_showConstraints_clicked();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_GetResult_clicked();
	///
	/// Summary:	Handles get result clicked signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_GetResult_clicked();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_mattingROIFlag_toggled(bool checked);
	///
	/// Summary:	Handles matting roi flag toggled signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// checked - 	True if checked. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_mattingROIFlag_toggled(bool checked);// image matting

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_colorComboBox_currentIndexChanged(const QString &arg);
	///
	/// Summary:	Handles color combo box current index changed signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// arg - 	The argument. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_colorComboBox_currentIndexChanged(const QString &arg);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_LoadParameter_clicked();
	///
	/// Summary:	Handles load parameter clicked signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_LoadParameter_clicked();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_combineLayer_clicked();
	///
	/// Summary:	Handles combine layer clicked signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_combineLayer_clicked();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_LoadROI_clicked();
	///
	/// Summary:	Handles load roi clicked signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_LoadROI_clicked();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_ContentAware_clicked();
	///
	/// Summary:	Handles content aware clicked signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_ContentAware_clicked();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::on_actionOutput_Long_Video_triggered();
	///
	/// Summary:	Handles action output long video triggered signals.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void on_actionOutput_Long_Video_triggered();
	
private:
	//UI variables
    /// Summary:	The user interface.
    Ui::scissor *ui;
    /// Summary:	The is load.
    int Is_load = 0;
    /// Summary:	The is graduated.
    int Is_grad = 0;
    /// Summary:	The initialize seed.
    int Init_seed = 0;
    /// Summary:	The is start.
    int Is_start = 0;
    /// Summary:	The is graphed.
    int Is_graphed = 0;
    /// Summary:	The is closed.
    int Is_closed = 0;
    /// Summary:	The is showgrad.
    int Is_showgrad = 0;
    /// Summary:	The is mask.
    int Is_mask = 0;
    /// Summary:	The image.
    Mat image;
    /// Summary:	The back image.
    Mat back_image;
    /// Summary:	The graduated image.
    Mat grad_image;
    /// Summary:	The seed image.
    Mat seed_image;
    /// Summary:	The mask image.
    Mat mask_image;
	/// Summary:	Zero-based index of the zoom.
	double zoom_index = 1;
    /// Summary:	The seed number.
    int seed_num;
    /// Summary:	The seed x coordinate.
    int seed_x;
    /// Summary:	The seed y coordinate.
    int seed_y;
	/// Summary:	The node vector.
	std::vector<struct Node_2> node_vector;
	/// Summary:	The seed vector.
	std::vector<cv::Point2d> seed_vector;
	/// Summary:	The full path vector.
	std::vector<std::vector<cv::Point2d> > fullPath_vector;
	/// Summary:	The short path vector.
	std::vector<cv::Point2d> shortPath_vector;
	
	// animated variable
	/// Summary:	The padding x coordinate.
	int paddingX;
	/// Summary:	The padding y coordinate.
	int paddingY;
	/// Summary:	The organisation roi.
	Rect orgROI;
	/// Summary:	The organisation image.
	Mat orgImg;
	/// Summary:	The animated roi.
	Mat animatedROI;
	/// Summary:	The unprocessed image.
	Mat unprocessedImg;
	/// Summary:	The unprocessed mask.
	Mat unprocessedMask;
	// animated function

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::UpdateImage(Mat animatedMask);
	///
	/// Summary:	Updates the image described by animatedMask.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// animatedMask - 	The animated mask. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void UpdateImage(Mat animatedMask);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::LoadParameters(size_t* resultSize);
	///
	/// Summary:	Loads the parameters.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// resultSize - 	[in,out] If non-null, size of the result. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadParameters(size_t* resultSize);
	//flow Field variables
	/// Summary:	The temporary position.
	Point2i tmpPos;
	/// Summary:	The flow field path image.
	Mat block_flowFieldPathImg;
	Mat flowPathImg;
	Mat tmp_flowPathImg;
	/// Summary:	The flow field vector and path image.
	Mat flowFieldVectorAndPathImg;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Property:	std::vector<Point>UI_path
	///
	/// Summary:	Gets the vector.
	///
	/// Typeparams:
	/// Point - 	Type of the point. 
	///
	/// Value:
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<Point>UI_path;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Property:	std::vector<vector<Point>>UI_pathArr
	///
	/// Summary:	Gets the vector.
	///
	/// Typeparams:
	/// Point - 	Type of the point. 
	///
	/// Value:
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<vector<Point>>UI_pathArr;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Property:	std::vector<FLOWVECTORTAG>UI_flowVector
	///
	/// Summary:	Gets the vector.
	///
	/// Typeparams:
	/// FLOWVECTORTAG - 	Type of the flowvectortag. 
	///
	/// Value:
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<FLOWVECTORTAG>UI_flowVector;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Property:	std::vector<std::vector<FLOWVECTORTAG>>UI_flowVectorArray
	///
	/// Summary:	Gets the vector.
	///
	/// Typeparams:
	/// FLOWVECTORTAG - 	Type of the flowvectortag. 
	///
	/// Value:
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<std::vector<FLOWVECTORTAG>>UI_flowVectorArray;
	//flow Field function

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	vector<FLOWVECTORTAG> scissor::CalculateFlowVector(vector<cv::Point>UI_path);
	///
	/// Summary:	Calculates the flow vector.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	///
	/// Parameters:
	/// parameter1 - 	The first parameter. 
	///
	/// Returns:	The calculated flow vector.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<FLOWVECTORTAG> CalculateFlowVector(vector<cv::Point>UI_path);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::FlowFieldInit();
	///
	/// Summary:	Flow field initialize.
	///
	/// Author:	Yc Lin
	///
	/// Date:	2019/10/4
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void FlowFieldInit();
	//flow Driver
	/// Summary:	The fd.
	FlowDriver FD;
	/// Summary:	The fv.
	FlowVector FV;
	//image matting
	/// Summary:	The matting rough mask.
	Mat mattingRoughMask;
	/// Summary:	The matting result.
	Mat mattingResult;
	/// Summary:	for draw something to show.
	Mat drawImg;
	/// Summary:	The circle r.
	int circleR = 10;
	/// Summary:	The press flag.
	int pressFlag=0;
	/// Summary:	The matting color.
	Scalar mattingColor = Scalar(255, 255, 255,127);
	//debug function

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Fn:	void scissor::PrintMatData(Mat img);
	///
	/// Summary:	Print matrix data.
	///
	/// Author:	Yc Lin.
	///
	/// Date:	2019/10/4.
	///
	/// Parameters:
	/// img - 	The image. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void PrintMatData(Mat img);
	//other function
	void SaveParameters();
	void WriteParameters(string path);
};



#endif // SCISSOR_H
