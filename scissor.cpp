////////////////////////////////////////////////////////////////////////////////////////////////////
/// File:	scissor.cpp.
///
/// Summary:	Implements the scissor class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "scissor.h"
#include "ui_scissor.h"
#include"morphing.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Def:	OPEN_FILE_PATH
///
/// Summary:	A macro that defines open file path.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace cv;
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	QImage Mat2QImage(cv::Mat const& src)
///
/// Summary:	Matrix 2 q image.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// src - 	Source for the. 
///
/// Returns:	A QImage.
////////////////////////////////////////////////////////////////////////////////////////////////////

QImage Mat2QImage(cv::Mat const& src) {
	/*
	Mat inv_src(src.cols, src.rows,CV_8UC3, Scalar(255,255,255));
	for(int i = 0; i<inv_src.rows;i++)
	for(int j = 0;j<inv_src.cols;j++)
	{
	inv_src.at<Vec3b>(i,j) = src.at<Vec3b>(j,i);
	}
	*/


	cv::Mat temp;
	cvtColor(src, temp, CV_BGR2RGB);
	QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	/*cvtColor(src, temp, CV_BGRA2RGBA);
	QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGBA8888);*/
	dest.bits(); // enforce deep copy, see documentation
	// of QImage::QImage ( const uchar * data, int width, int height, Format format )
	return dest;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	cv::Mat QImage2Mat(QImage const& src)
///
/// Summary:	Image 2 matrix.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// src - 	Source for the. 
///
/// Returns:	A cv::Mat.
////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat QImage2Mat(QImage const& src) {
	cv::Mat tmp(src.height(), src.width(), CV_8UC3, (uchar*)src.bits(), src.bytesPerLine());
	cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
	cvtColor(tmp, result, CV_BGR2RGB);

	Mat inv_result(result.cols, result.rows, CV_8UC3, Scalar(255, 255, 255));
	for (int i = 0; i < result.rows; i++)
		for (int j = 0; j < result.cols; j++) {
			inv_result.at<Vec3b>(i, j) = result.at<Vec3b>(j, i);
		}


	return inv_result;
}


scissor::scissor(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::scissor) {
	this->setMouseTracking(true);
	ui->setupUi(this);
	qApp->installEventFilter(this);
	//ui->label->installEventFilter(this);
}

void scissor::initial() {
	seed_vector.clear();
	fullPath_vector.clear();
	shortPath_vector.clear();
	Is_graphed = 0;
	Init_seed = 0;
	Is_closed = 0;
	Is_showgrad = 0;
	Is_mask = 0;
	//Is_start = 0;

	back_image = image.clone();
	seed_image = image.clone();
	mask_image = Mat(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));
	animatedROI = Mat(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));

	ui->label->clear();
	//ui->label_2->clear();
	show_image(image, 0);
	printf("current image size = %d * %d\n", image.rows, image.cols);
	cout << "rows * cols" << image.rows * image.cols << endl;
	FlowFieldInit();
}

void scissor::openFile() {
	//"D:\\Desktop\\org.png"
	///home/tony-pc/qt
	QString filename = QFileDialog::getOpenFileName(
		this,
		tr("Open Image"),
		OPEN_FILE_PATH,
		tr("Image files (*.jpg *.png );;All files (*.*)"));

	if (filename.length() != 0) {
		// scissor variable
		Is_load = 1;
		Is_grad = 0;
		Init_seed = 0;
		Is_start = 0;
		Is_graphed = 0;
		Is_closed = 0;
		Is_showgrad = 0;
		Is_mask = 0;

		// ui
		ui->label->clear();
		ui->label_2->clear();
		ui->fixedROIFlag->setEnabled(true);
		ui->flowFieldFlag->setEnabled(true);
		ui->animatingROIFlag->setEnabled(true);
		ui->animatingPictureFlag->setEnabled(true);
		ui->mattingROIFlag->setEnabled(true);
		ui->GetResult->setEnabled(false);

		std::string file = filename.toUtf8().constData();
		orgImg = cv::imread(file, 1);
		printf("original size= %d, %d\n", orgImg.rows, orgImg.cols);

		if (orgImg.rows % 2 != 0 || orgImg.cols % 2 != 0) {
			QMessageBox msgBox;
			msgBox.setText("The height or width of the input image cannot be divisible by two.");
			msgBox.setInformativeText("If you continue execution, an error may occur.");
			msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Ok);
			msgBox.exec();// launch hint window
		}

		//shift original
		paddingX = 10, paddingY = 10;
		copyMakeBorder(orgImg, image, paddingY, paddingY, paddingX, paddingX, BORDER_CONSTANT, Scalar(255, 255, 255));
		orgROI = Rect(paddingX, paddingY, orgImg.cols, orgImg.rows);
		
		/*
		Mat borderImage;
		copyMakeBorder(src, borderImage, paddingY, paddingY, paddingX, paddingX, BORDER_REPLICATE);
		*/

		
		FD = FlowDriver(orgImg.clone());
		unprocessedImg = orgImg.clone();
		unprocessedMask = Mat(orgImg.rows, orgImg.cols, CV_8UC3, Scalar(0, 0, 0));

		flowPathImg = orgImg.clone();
		tmp_flowPathImg = orgImg.clone();

		mask_image = Mat(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));
		animatedROI = Mat(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));
		back_image = image.clone();
		seed_image = image.clone();
		//cv::namedWindow("image",CV_WINDOW_AUTOSIZE);
		show_image(image, 0);
		printf("image size = %d * %d\n", image.rows, image.cols);
		cout << "rows * cols" << image.rows * image.cols << endl;

		FlowFieldInit();
	}
}


scissor::~scissor() {
	delete ui;
}

void scissor::on_actionOpen_triggered() {
	openFile();
}

void scissor::on_actionZoom_In_triggered() {
	if (!Is_load)
		return;
	if (Is_start)
		return;
	zoom_index = 2;
	Zoom_in_out(zoom_index);
	std::cout << "zoom in" << std::endl;
}

void scissor::on_actionZoom_Out_triggered() {
	if (Is_start)
		return;
	if (!Is_load)
		return;
	zoom_index = 0.5;
	Zoom_in_out(zoom_index);
	std::cout << "zoom out" << std::endl;
}

void scissor::Zoom_in_out(double index) {
	if (!Is_load)
		return;

	Mat tmp;
	tmp = back_image.clone();
	cout << index << endl;

	if (index > 1)
		pyrUp(tmp, image, Size(tmp.cols * index, tmp.rows * index));
	else if (index < 1)
		pyrDown(tmp, image, Size(tmp.cols * index, tmp.rows * index));

	show_image(image, 0);

}

void scissor::on_actionOrigin_Map_triggered() {
	if (!Is_load)
		return;
	image = back_image.clone();
	show_image(image, 0);
}

void scissor::show_image(cv::Mat const& src, int n) {
	//imshow( "image", src );

	QPixmap new_pixmap = QPixmap::fromImage(Mat2QImage(src));

	int w = new_pixmap.width();
	int h = new_pixmap.height();
	if (n == 0) {
		ui->label->resize(w, h);
		ui->label->setPixmap(new_pixmap);

	}
	else if (n == 1) {
		ui->label_2->resize(w, h);
		ui->label_2->setPixmap(new_pixmap);
	}

	ui->scrollArea->show();
}

void scissor::gradient() {
	cout << "gradient()" << endl;
	Is_grad = 1;
	Mat tmp_image((image.rows - 2) * 3, (image.cols - 2) * 3, CV_8UC3, Scalar(255, 255, 255));
	grad_image = tmp_image.clone();
	double maxD = 0;
	double link = 0;
	for (int i = 1; i < image.rows - 1; i++)
		for (int j = 1; j < image.cols - 1; j++) {

			int x = (i - 1) * 3 + 1;
			int y = (j - 1) * 3 + 1;

			//x,y
			grad_image.at<Vec3b>(x, y) = image.at<Vec3b>(i, j);

			//Dlink
			//x-1,y-1
			double sum = 0;
			for (int k = 0; k <= 2; k++) {
				sum += pow((image.at<Vec3b>(i, j - 1)[k] - image.at<Vec3b>(i - 1, j)[k]), 2);
			}
			link = sqrt(sum / 6.0);
			if (link > maxD)
				maxD = link;
			for (int k = 0; k <= 2; k++) {
				grad_image.at<Vec3b>(x - 1, y - 1)[k] = link;
			}

			//x+1,y-1
			sum = 0;
			for (int k = 0; k <= 2; k++) {
				sum += pow((image.at<Vec3b>(i, j - 1)[k] - image.at<Vec3b>(i + 1, j)[k]), 2);
			}
			link = sqrt(sum / 6.0);
			if (link > maxD)
				maxD = link;
			for (int k = 0; k <= 2; k++)
				grad_image.at<Vec3b>(x + 1, y - 1)[k] = link;

			//x-1,y+1
			sum = 0;
			for (int k = 0; k <= 2; k++) {
				sum += pow((image.at<Vec3b>(i - 1, j)[k] - image.at<Vec3b>(i, j + 1)[k]), 2);
			}
			link = sqrt(sum / 6.0);
			if (link > maxD)
				maxD = link;
			for (int k = 0; k <= 2; k++)
				grad_image.at<Vec3b>(x - 1, y + 1)[k] = link;

			//x+1,y+1
			sum = 0;
			for (int k = 0; k <= 2; k++) {
				sum += pow((image.at<Vec3b>(i, j + 1)[k] - image.at<Vec3b>(i + 1, j)[k]), 2);
			}
			link = sqrt(sum / 6.0);
			if (link > maxD)
				maxD = link;
			for (int k = 0; k <= 2; k++)
				grad_image.at<Vec3b>(x + 1, y + 1)[k] = link;

			//x,y-1
			sum = 0;
			for (int k = 0; k <= 2; k++) {
				sum += pow((image.at<Vec3b>(i - 1, j)[k] + image.at<Vec3b>(i - 1, j - 1)[k] - image.at<Vec3b>(i + 1, j)[k] - image.at<Vec3b>(i + 1, j - 1)[k]) / 4.0, 2);
			}
			link = sqrt(sum / 3.0);
			if (link > maxD)
				maxD = link;
			for (int k = 0; k <= 2; k++)
				grad_image.at<Vec3b>(x, y - 1)[k] = link;

			//x-1,y
			sum = 0;
			for (int k = 0; k <= 2; k++) {
				sum += pow((image.at<Vec3b>(i - 1, j - 1)[k] + image.at<Vec3b>(i, j - 1)[k] - image.at<Vec3b>(i - 1, j + 1)[k] - image.at<Vec3b>(i, j + 1)[k]) / 4.0, 2);
			}
			link = sqrt(sum / 3.0);
			if (link > maxD)
				maxD = link;
			for (int k = 0; k <= 2; k++)
				grad_image.at<Vec3b>(x - 1, y)[k] = link;

			//x+1,y
			sum = 0;
			for (int k = 0; k <= 2; k++) {
				sum += pow((image.at<Vec3b>(i, j - 1)[k] + image.at<Vec3b>(i + 1, j - 1)[k] - image.at<Vec3b>(i, j + 1)[k] - image.at<Vec3b>(i + 1, j + 1)[k]) / 4.0, 2);
			}
			link = sqrt(sum / 3.0);
			if (link > maxD)
				maxD = link;
			for (int k = 0; k <= 2; k++)
				grad_image.at<Vec3b>(x + 1, y)[k] = link;

			//x,y+1
			sum = 0;
			for (int k = 0; k <= 2; k++) {
				sum += pow((image.at<Vec3b>(i - 1, j)[k] + image.at<Vec3b>(i - 1, j + 1)[k] - image.at<Vec3b>(i + 1, j)[k] - image.at<Vec3b>(i + 1, j + 1)[k]) / 4.0, 2);
			}
			link = sqrt(sum / 3.0);
			if (link > maxD)
				maxD = link;
			for (int k = 0; k <= 2; k++)
				grad_image.at<Vec3b>(x, y + 1)[k] = link;
		}
	//show_image(grad_image);

	cout << "gradient maxD" << maxD << endl;

	//update cost
	double a = 1.0;
	for (int i = 1; i < image.rows - 1; i++)
		for (int j = 1; j < image.cols - 1; j++) {
			int x = (i - 1) * 3 + 1;
			int y = (j - 1) * 3 + 1;
			for (int k = 0; k <= 2; k++) {
				grad_image.at<Vec3b>(x, y)[k] = 255;
				grad_image.at<Vec3b>(x - 1, y - 1)[k] = (maxD - grad_image.at<Vec3b>(x - 1, y - 1)[k]) * 1.414 * a;
				grad_image.at<Vec3b>(x + 1, y - 1)[k] = (maxD - grad_image.at<Vec3b>(x + 1, y - 1)[k]) * 1.414 * a;
				grad_image.at<Vec3b>(x - 1, y + 1)[k] = (maxD - grad_image.at<Vec3b>(x - 1, y + 1)[k]) * 1.414 * a;
				grad_image.at<Vec3b>(x + 1, y + 1)[k] = (maxD - grad_image.at<Vec3b>(x + 1, y + 1)[k]) * 1.414 * a;
				grad_image.at<Vec3b>(x, y - 1)[k] = (maxD - grad_image.at<Vec3b>(x, y - 1)[k]) * a;
				grad_image.at<Vec3b>(x - 1, y)[k] = (maxD - grad_image.at<Vec3b>(x - 1, y)[k]) * a;
				grad_image.at<Vec3b>(x + 1, y)[k] = (maxD - grad_image.at<Vec3b>(x + 1, y)[k]) * a;
				grad_image.at<Vec3b>(x, y + 1)[k] = (maxD - grad_image.at<Vec3b>(x, y + 1)[k]) * a;
			}
		}
}

void scissor::on_actionGradient_Map_triggered() {
	if (!Is_load)
		return;

	gradient();

	imshow("image", grad_image);
	//imwrite( "Gray_Image.jpg", grad_image );
}

void scissor::on_actionStart_triggered() {
	cout << "actionStart()" << endl;
	if (!Is_load)
		return;
	if (!Is_grad)
		gradient();
	Is_start = 1;

	// Node Struct
	node_vector.clear();
	for (int i = 0; i < image.rows; i++)
		for (int j = 0; j < image.cols; j++) {
			struct Node_2 node;
			node.row = i;
			node.col = j;
			node.state = INITIAL;
			node.prevNode = NULL;
			node.totalCost = INF;
			node.prevNodeNum = 0;
			node.num = i * image.cols + j;
			int x = (i - 1) * 3 + 1;
			int y = (j - 1) * 3 + 1;
			int cnt = 0;
			if (i == 0 || j == 0 || i == image.rows - 1 || j == image.cols - 1) {
				for (int k = 0; k < 9; k++)
					node.linkCost[k] = INF;
			}
			else
				for (int l = -1; l <= 1; l++)
					for (int m = -1; m <= 1; m++) {
						if (l == 0 && m == 0)
							node.linkCost[cnt] = 0;
						else
							node.linkCost[cnt] = grad_image.at<Vec3b>(x + l, y + m)[0];
						cnt++;
					}
			//cout<<"cnt"<<cnt<<endl;
			node_vector.push_back(node);
		}
}
/*
void scissor::mousePressEvent(QMouseEvent *event)

{

if (event->button()==Qt::LeftButton)
{
int x=event->x();
int y=event->y();
cout<<"x  "<<x<<"y   "<<y<<endl;
}


}
*/

bool scissor::eventFilter(QObject *obj, QEvent *event) {
	//animatingROIFlag and fixedROIFlag
	if (ui->animatingROIFlag->isChecked() || ui->fixedROIFlag->isChecked()) {
		if (qobject_cast<QLabel*>(obj) == ui->label && event->type() == QEvent::MouseButtonPress) {
			cout << "click label_1" << endl;
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

			if (mouseEvent->buttons() == Qt::RightButton) {
				int x = mouseEvent->pos().y();
				int y = mouseEvent->pos().x();
				cout << "mouse right button click " << x << "  " << y << endl;
				if (x < image.rows && y < image.cols) {
					getMask(x, y);
					if (ui->animatingROIFlag->isChecked()) {
						FD.SetBoundryConstraints(mask_image(orgROI).clone(), 0);//animation
					}
					if (ui->fixedROIFlag->isChecked()) {
						FD.SetBoundryConstraints(mask_image(orgROI).clone(), 1);//fixed
					}
					initial();
				}
			}
			else if (mouseEvent->buttons() == Qt::LeftButton) {
				if (Is_start) {
					Is_graphed = 0;
					if (!Init_seed) {
						seed_vector.clear();
						fullPath_vector.clear();
						shortPath_vector.clear();
						cout << "init_seed" << endl;
						Init_seed = 1;
					}
					//QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

					int x = mouseEvent->pos().y();
					int y = mouseEvent->pos().x();
					cout << "mouse click " << x << "  " << y << endl;
					if (x < image.rows && y < image.cols)
						if (!Is_closed)
							findPathGraph(x, y);
						else
							if (Is_mask)
								getMask(x, y);
					closeDetect(x, y);
				}
			}
		}
	}

	//flowFieldFlag
	if (ui->flowFieldFlag->isChecked() && Is_start) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		//MouseButtonPress
		if (qobject_cast<QLabel*>(obj) == ui->label &&event->type() == QEvent::MouseButtonPress) {
			if (mouseEvent->buttons() == Qt::LeftButton) {
				//QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
				int y = mouseEvent->pos().y();
				int x = mouseEvent->pos().x();

				tmpPos = cv::Point2i(x, y);
				UI_path.reserve(1000);// Reserve space to avoid push back delay
				UI_path.push_back(tmpPos);
				printf("start draw flow\t===============================================\n");
			}
		}

		//MouseMove
		if (qobject_cast<QLabel*>(obj) == ui->label &&event->type() == QEvent::MouseMove) {
			if (mouseEvent->buttons() == Qt::LeftButton) {
				int y = mouseEvent->pos().y();
				int x = mouseEvent->pos().x();
				cv::Point2i currentPos(x, y);

				if (x < image.cols && y < image.rows && (tmpPos != currentPos)) {
					UI_path.push_back(cv::Point(x, y));
										
					line(block_flowFieldPathImg, tmpPos, currentPos, cv::Scalar(0, 255, 0), 2, 8, 0);					
					line(tmp_flowPathImg, tmpPos, currentPos, cv::Scalar(0, 255, 0), 2, 8, 0);
					//printf("block_flowFieldPathImg size= %d %d\n", block_flowFieldPathImg.cols, block_flowFieldPathImg.rows);
					show_image(block_flowFieldPathImg, 0);
					tmpPos = currentPos;
				}
				//cv::arrowedLine(block_flowFieldPathImg, currentPos, currentPos, cv::Scalar(0, 255, 0), 1.5);
				//cv::arrowedLine(block_flowFieldPathImg, currentPos, (currentPos*5), cv::Scalar(0, 255, 0), 1);
			}
			
		}

		//MouseButtonRelease
		if (qobject_cast<QLabel*>(obj) == ui->label &&event->type() == QEvent::MouseButtonRelease) {
			//QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);			
			cout << ">>> " << UI_path.size() << endl;
			//cout << " UI_path[]: " << UI_path[10].x << "; " << UI_path[10].y << endl;
			cv::imwrite(outputVideoPath + "USP.png", tmp_flowPathImg);		
			

			UI_pathArr.push_back(UI_path); // by H: list of user-specified paths
			UI_flowVector = CalculateFlowVector(UI_path); // by H: UI_flowVector are the refined paths
			UI_flowVectorArray.push_back(UI_flowVector);
			//printf("UI_path size= %d\n", UI_path.size());
			UI_path.clear();

			//printf("UI_flowVectorArray size= %d\n", UI_flowVectorArray.size());
			printf("draw flow end\t===============================================\n\n");
			//cout << UI_flowVectorArray[1][1].vector.x << endl;
				

			FV = FlowVector(block_flowFieldPathImg.clone(), UI_flowVectorArray);
			cout << "Before optimizing --------------------------" << endl;
			flowFieldVectorAndPathImg = FV.GetResultImg(203, 16, 26).clone();
			cv::imwrite(outputVideoPath + "path_before_optimized.png", flowFieldVectorAndPathImg);

			FV.EigenOptimizer();
			flowFieldVectorAndPathImg = FV.GetResultImg(203, 16, 26).clone();
			show_image(flowFieldVectorAndPathImg, 1); // by H: after apply Eq.(3)

			flowFieldVectorAndPathImg = FV.GetResultImg(203, 16, 26).clone();
			cv::imwrite(outputVideoPath + "path.png", flowFieldVectorAndPathImg);

		}
	}

	//image matting
	if (ui->mattingROIFlag->isChecked()) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

		//control The radius of the circle of the drawing
		if (keyEvent->key() == Qt::Key_BracketLeft) {
			if (event->type() == QEvent::KeyPress && pressFlag != 1) {
				circleR++;
				pressFlag = 1;
			}
			else if (event->type() == QEvent::KeyRelease && pressFlag == 1) {
				pressFlag = 0;
			}
			//printf("circleR= %d\n", circleR);
		}
		if (keyEvent->key() == Qt::Key_BracketRight) {
			if (event->type() == QEvent::KeyPress && pressFlag != 1) {
				if (circleR != 1)
					circleR--;
				pressFlag = 1;
			}
			else if (event->type() == QEvent::KeyRelease && pressFlag == 1) {
				pressFlag = 0;
			}
			//printf("circleR= %d\n", circleR);
		}

		//when MouseMove || MouseButtonPress draw mask
		if (qobject_cast<QLabel*>(obj) == ui->label && (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress)) {
			int y = mouseEvent->pos().y();
			int x = mouseEvent->pos().x();
			Point2i cursorPos(x, y);

			if (y < orgImg.rows && x < orgImg.cols) {
				if (mouseEvent->buttons() == Qt::LeftButton) {
					//cout << "mouse on here " << x << "  " << y << endl;
					if (ui->colorComboBox->currentText() == "ForeGround_fill") {
						Rect ccomp;
						floodFill(mattingRoughMask, cursorPos, mattingColor, &ccomp, Scalar(120, 120, 120), Scalar(120, 120, 120));
					}
					else {
						if (!mattingResult.empty())
							circle(mattingResult, cursorPos, circleR, mattingColor, CV_FILLED, 8);

						circle(mattingRoughMask, cursorPos, circleR, mattingColor, CV_FILLED, 8);
					}

					//The place the user has drawn will directly cover up
					//for (int dy = 0; dy < drawImg.rows; dy++){
					//	for (int dx = 0; dx < drawImg.cols; dx++){
					//		if (mattingRoughMask.at<cv::Vec3b>(Point(dx, dy)) == cv::Vec3b(0, 0, 0))
					//			drawImg.at<cv::Vec3b>(Point(dx, dy)) = orgImg.at<cv::Vec3b>(Point(dx, dy));
					//		else
					//			drawImg.at<cv::Vec3b>(Point(dx, dy)) = mattingRoughMask.at<cv::Vec3b>(Point(dx, dy));
					//	}
					//}

					if (!mattingResult.empty()) {
						addWeighted(orgImg, 0.5, mattingResult, 0.5, 0, drawImg);
						show_image(drawImg, 0);
					}
					else {
						addWeighted(orgImg, 0.5, mattingRoughMask, 0.5, 0, drawImg);
						show_image(drawImg, 0);
					}

					show_image(mattingRoughMask, 1);
				}
				else {
					//cout << "mouse on here " << x << "  " << y << endl;
					Mat mattingTmp = drawImg.clone();
					circle(mattingTmp, cursorPos, circleR, mattingColor, CV_FILLED, 8);
					show_image(mattingTmp, 0);
				}

				if (mouseEvent->buttons() == Qt::RightButton) {
					// fill black color
					for (int y = 0; y < mattingRoughMask.rows; y++) {
						for (int x = 0; x < mattingRoughMask.cols; x++) {
							if (mattingRoughMask.at<Vec3b>(Point(x, y)) == Vec3b(255, 255, 255) || mattingRoughMask.at<Vec3b>(Point(x, y)) == Vec3b(127, 127, 127))
								continue;
							else
								mattingRoughMask.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
						}
					}

					SharedMatting sm;
					sm.SetImage(orgImg.clone());
					sm.SetTrimap(mattingRoughMask.clone());
					sm.solveAlpha();

					cvtColor(sm.GetResult(), mattingResult, CV_GRAY2RGB);
					//printf("ch= %d\ttype= %d\n", mattingRoughMask.channels(), mattingRoughMask.type());
					//printf("ch= %d\ttype= %d\n", tmpResult.channels(), tmpResult.type());

					addWeighted(orgImg, 0.5, mattingResult, 0.5, 0, drawImg);
					show_image(drawImg, 0);
					show_image(mattingResult, 1);
					FD.SetBoundryConstraints(mattingResult, 0);// set animatingROI
					FD.SetMattingMask(mattingResult);
					//sm.save("shared_surfingMask.png");
				}
			}
		}
	}

	//default
	if (qobject_cast<QLabel*>(obj) == ui->label &&event->type() == QEvent::MouseMove) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

		statusBar()->showMessage(QString("Mouse move (%1,%2)").arg(mouseEvent->pos().y()).arg(mouseEvent->pos().x()));
		if (Is_graphed == 1) {
			int x = mouseEvent->pos().y();
			int y = mouseEvent->pos().x();
			//cout<<"x  "<<x<<"y   "<<y<<endl;
			if (x < image.rows && y < image.cols && !Is_closed) {
				moveMouse(x, y);
				cout << "move mouse" << x << "  " << y << endl;
			}
		}
	}
	return false;
}
/*
void scissor::delaymsec(int msec)
{
QTime dieTime = QTime::currentTime().addMSecs(msec);

while( QTime::currentTime() < dieTime )

QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


void scissor::delaymsec(int msec)
{

QTime n=QTime::currentTime();

QTime now;
do{
now=QTime::currentTime();
}while (n.msecsTo(now)<=msec);

}
*/
void scissor::findPathGraph(int x, int y) {
	cout << " findPathGraph seed   " << x << "   " << y << endl;
	/*
	for(int i = 20 ;i < node_vector.size();i++)
	{
	cout<<"up"<<i<<endl;

	cout<<node_vector[i].linkCost[7]<<endl;

	cout<<"down"<<i + image.cols - 2 +1<<endl;

	cout<<node_vector[i + image.cols - 2 +1].linkCost[0]<<endl;
	if(i>26)
	break;
	}
	*/

	cv::Point2d point(y, x);
	seed_vector.push_back(point);
	fullPath_vector.push_back(shortPath_vector);
	getSeedImage();
	//build edge
	//int node_num = node_vector.size();
	//int deges[node_num][node_num];

	//init total cost
	//cout<<"node "<<node_vector[x * image.cols + y].row<<"  "<<node_vector[x * image.cols + y].col;

	//vector initiate
	for (int i = 0; i < node_vector.size(); i++) {
		node_vector[i].state = INITIAL;
		node_vector[i].totalCost = INF;
		node_vector[i].prevNodeNum = 0;
		node_vector[i].prevNode = NULL;
	}
	seed_x = x;
	seed_y = y;
	seed_num = x * image.cols + y;
	node_vector[seed_num].state = INITIAL;
	node_vector[seed_num].totalCost = 0;
	node_vector[seed_num].prevNodeNum = seed_num;
	priority_queue<Node_2> que;
	que.push(node_vector[seed_num]);
	while (!que.empty()) {
		Node_2 q = que.top();
		que.pop();
		if (node_vector[q.num].state == EXPANDED)
			continue;
		q.state = EXPANDED;
		node_vector[q.num].state = EXPANDED;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++) {
				//q.row q.col = boundry
				if (q.row == 0 || q.col == 0 || q.row == image.rows - 1 || q.col == image.cols - 1)
					continue;
				if (i == 0 && j == 0)
					continue;
				Node_2 r = node_vector[(q.row + i) * image.cols + (q.col + j)];
				if (r.state == INITIAL) {
					//cout<<"DEbug i "<<i<<"j  "<<j<<endl;
					//cout<<"linkcost"<<(i+1) * 3 + j + 1<<endl;
					r.prevNode = &q;
					r.totalCost = q.totalCost + q.linkCost[(i + 1) * 3 + j + 1];
					r.state = ACTIVE;
					r.prevNodeNum = q.num;
					node_vector[r.num].prevNode = &q;
					node_vector[r.num].prevNodeNum = q.num;
					node_vector[r.num].totalCost = q.totalCost + q.linkCost[(i + 1) * 3 + j + 1];
					node_vector[r.num].state = ACTIVE;
					que.push(r);
				}
				else if (r.state == ACTIVE) {
					if (q.totalCost + q.linkCost[(i + 1) * 3 + j + 1] < r.totalCost) {
						r.prevNode = &q;
						r.totalCost = q.totalCost + q.linkCost[(i + 1) * 3 + j + 1];
						r.prevNodeNum = q.num;
						node_vector[r.num].prevNode = &q;
						node_vector[r.num].prevNodeNum = q.num;
						node_vector[r.num].totalCost = q.totalCost + q.linkCost[(i + 1) * 3 + j + 1];
						//update que
						que.push(r);


					}

				}

			}
	}
	Is_graphed = 1;
	cout << "find path Graph " << endl;

	//showNode(&node_vector[seed_num]);
}

void scissor::showNode(struct Node_2 *node) {
	cout << "num  " << node->num << endl;
	cout << "position  " << node->row << "  " << node->col << endl;
	cout << "prevNodeNum  " << node->prevNodeNum << endl;
	cout << "total cost  " << node->totalCost << endl;
	for (int i = 0; i < 3; i++)
		cout << "  " << node->linkCost[i];
	cout << endl;
	for (int i = 3; i < 6; i++)
		cout << "  " << node->linkCost[i];
	cout << endl;
	for (int i = 6; i < 9; i++)
		cout << "  " << node->linkCost[i];
	cout << endl;
}

void scissor::moveMouse(int x, int y) {
	if (!Is_graphed)
		return;
	Node_2 des_node = node_vector[x * image.cols + y];
	Mat line_image = seed_image.clone();
	shortPath_vector.clear();
	shortPath_vector.push_back(Point2f(des_node.col, des_node.row));
	while (des_node.prevNodeNum != seed_num) {
		cv::Point2d pointa(des_node.col, des_node.row);
		int next_num = des_node.prevNodeNum;
		des_node = node_vector[next_num];
		cv::Point2d pointb(des_node.col, des_node.row);
		cv::line(line_image, pointa, pointb, cv::Scalar(255, 0, 255), 2, 8, 0);
		shortPath_vector.push_back(pointb);

	}
	shortPath_vector.push_back(Point2d(seed_y, seed_x));
	show_image(line_image, 0);
}

void scissor::getSeedImage() {
	cout << "getSeedImage()" << endl;
	seed_image = image.clone();
	if (!shortPath_vector.empty()) {
		for (int i = 0; i < seed_vector.size(); i++) {
			if (i == 0)
				circle(seed_image, seed_vector[i], 2, cv::Scalar(0, 255, 255), 2);
			else
				circle(seed_image, seed_vector[i], 2, cv::Scalar(0, 255, 0), 2);
			circle(grad_image, Point2d(((seed_vector[i].x - 1) * 3 + 1), (seed_vector[i].y - 1) * 3 + 1), 2, cv::Scalar(255, 0, 255), 2);
			//cout<<"i"<<i<<endl;
			//cout<<"size"<<fullPath_vector.size()<<endl;
			if (fullPath_vector[i].size() < 2)
				continue;
			for (int j = 0; j < fullPath_vector[i].size() - 1; j++) {
				if (Is_closed)
					cv::line(seed_image, fullPath_vector[i][j], fullPath_vector[i][j + 1], cv::Scalar(255, 255, 255), 2, 8, 0);
				else
					cv::line(seed_image, fullPath_vector[i][j], fullPath_vector[i][j + 1], cv::Scalar(255, 0, 0), 2, 8, 0);
				cv::line(mask_image, fullPath_vector[i][j], fullPath_vector[i][j + 1], cv::Scalar(255, 255, 255), 1, 8, 0);
				cv::line(grad_image, Point2d(fullPath_vector[i][j].x * 3 - 2, fullPath_vector[i][j].y * 3 - 2), Point2d(fullPath_vector[i][j + 1].x * 3 - 2, fullPath_vector[i][j + 1].y * 3 - 2), cv::Scalar(255, 0, 0), 2, 8, 0);

			}

		}

	}
	show_image(seed_image, 0);
	if (Is_showgrad)
		imshow("image", grad_image);

	//imshow("image",grad_image);

}

void scissor::on_actionBack_triggered() {
	if (Is_closed)
		return;
	cout << "actionBack()" << endl;
	if (seed_vector.size() > 0) {
		seed_vector.pop_back();
		fullPath_vector.pop_back();

		if (seed_vector.size() > 0) {
			Point2d point = seed_vector.back();
			findPathGraph((int)point.y, (int)point.x);
			seed_vector.pop_back();
			fullPath_vector.pop_back();
			if (seed_vector.empty()) {
				Is_graphed = 0;
				shortPath_vector.clear();
			}

		}
		else {
			Is_graphed = 0;
			shortPath_vector.clear();
		}
		getSeedImage();
	}

}

void scissor::on_actionRestart_triggered() {
	if (!Is_load)
		return;

	initial();
	FlowFieldInit();
	getSeedImage();
}

void scissor::closeDetect(int x, int y) {
	cout << "closeDetect" << endl;
	if (seed_vector.size() <= 1)
		return;
	else {
		int distance;
		distance = (x - seed_vector[0].y) * (x - seed_vector[0].y) + (y - seed_vector[0].x) * (y - seed_vector[0].x);
		if (distance < CLOSETHRES) {
			Is_closed = 1;
			cout << "close" << endl;
			moveMouse(seed_vector[0].y, seed_vector[0].x);
			findPathGraph(seed_vector[0].y, seed_vector[0].x);
			//getMask();
		}
	}

}

void scissor::getMask(int x, int y) {
	cout << "get MASK" << endl;
	Rect ccomp;
	floodFill(mask_image, Point2d(y, x), Scalar(255, 255, 255), &ccomp, Scalar(20, 20, 20), Scalar(20, 20, 20));

	image.copyTo(animatedROI, mask_image);
	show_image(animatedROI, 1);// show mask image on label2

	//ui->addConstraints->setEnabled(true);
}

void scissor::on_actionDrawNodeVector_triggered() {
	Is_showgrad = ~Is_showgrad;

}

void scissor::on_actionSave_Image_triggered() {
	if (!Is_start)
		return;
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "../iamge.jpg", "Image files (*.jpg)");
	std::string file = fileName.toUtf8().constData();
	cout << "image " << file << endl;
	if (!file.empty()) {
		imwrite(file, seed_image);
	}
}

void scissor::on_actionGet_Mask_triggered() {
	Is_mask = 1;
}

void scissor::on_actionSave_Mask_triggered() {
	if (mask_image.empty()) {
		printf("mask_image is empty\n");
		return;
	}
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "../mask_iamge.jpg", "Image files (*.jpg)");
	std::string file = fileName.toUtf8().constData();
	cout << "mask saved " << file << endl;
	imwrite(file, mask_image);
}

void scissor::on_actionExit_triggered() {
	this->close();
}

void scissor::on_runMorphing_clicked() {
	morphing morphingMethod("glaxy2.PNG");
	morphingMethod.Run();
}

void scissor::on_fixedROIFlag_toggled(bool checked) {
	if (checked) {
		if (!mattingResult.empty()) {
			addWeighted(orgImg, 0.5, mattingResult, 0.5, 0, drawImg);
			show_image(drawImg, 1);
		}
		show_image(image, 0);
	}
}

void scissor::on_animatingROIFlag_toggled(bool checked) {
	if (checked) {
		show_image(image, 0);
		flowPathImg = tmp_flowPathImg.clone();
	}
}

void scissor::on_flowFieldFlag_toggled(bool checked) {
	if (checked) {
		if (!mattingResult.empty()) {
			addWeighted(orgImg, 0.5, mattingResult, 0.5, 0, drawImg);
			show_image(drawImg, 1);
		}
		FlowFieldInit();
		orgImg.copyTo(block_flowFieldPathImg, FD.GetFixedMask());
		show_image(block_flowFieldPathImg, 0);
		tmp_flowPathImg = flowPathImg.clone();
	}
}

void scissor::on_mattingROIFlag_toggled(bool checked) {
	if (checked) {
		drawImg = orgImg.clone();
		mattingRoughMask = Mat(orgImg.rows, orgImg.cols, CV_8UC3, cv::Scalar(0, 0, 0));
		/*printf("ch= %d\n", image.channels());
		printf("ch= %d\n", mattingRoughMask.channels());*/
		show_image(drawImg, 0);
		ui->colorComboBox->setEnabled(true);
		flowPathImg = tmp_flowPathImg.clone();
	}
	else {
		ui->colorComboBox->setEnabled(false);
	}
}

void scissor::on_animatingPictureFlag_toggled(bool checked) {
	if (checked) {
		ui->Run->setEnabled(true);
	}
	else {
		ui->Run->setEnabled(false);
	}
}

// check by H : UI_flowVector = CalculateFlowVector(UI_path) --> generate vector on each path

vector<FLOWVECTORTAG> scissor::CalculateFlowVector(vector<cv::Point>UI_path) {
	int sampleRate = 10;
	vector<FLOWVECTORTAG>flowVecResults;//vector<cv::Point>flowVecResults;
	FLOWVECTORTAG tmp_FLOWVECTORTAG;

	tmp_FLOWVECTORTAG.coordinate = UI_path[0];

	bool oddNumber = ((int)UI_path.size() / sampleRate) % 2 ? true : false;
	for (int i = sampleRate; i < UI_path.size(); i += sampleRate) {
		cv::Point2d flowV = UI_path[i] - tmp_FLOWVECTORTAG.coordinate;

		tmp_FLOWVECTORTAG.vector = flowV;
		flowVecResults.push_back(tmp_FLOWVECTORTAG);

		//update tmp_FLOWVECTORTAG
		tmp_FLOWVECTORTAG.coordinate = UI_path[i];
	}

	if (oddNumber) { //by H: handle in case the number of pixel in UI_path is odd

		cv::Point2d flowV = UI_path.back() - tmp_FLOWVECTORTAG.coordinate;
		tmp_FLOWVECTORTAG.vector = flowV;
		flowVecResults.push_back(tmp_FLOWVECTORTAG);
	}

	/*
		printf("flowVecResults check======================================================\n");
		for (int i = 0; i < flowVecResults.size(); i++){
		printf("(%d,%d)\t",flowVecResults[i].coordinate.x, flowVecResults[i].coordinate.y);
		if (i % 5 == 0 && i!=0)printf("\n");
		}
		printf("\n");
		printf("end flowVecResults check======================================================\n");
	*/

	printf("flowVecResults size= %d\n", flowVecResults.size());
	return flowVecResults;
}

void scissor::on_LoadParameter_clicked() {
	block_flowFieldPathImg = imread(loadParameterPath + "block_flowFieldPathImg.png");
	size_t resultSize = 0;
	LoadParameters(&resultSize);
	FV.SetOrgImg(block_flowFieldPathImg);

	FD.LoadParameter2Result("../Parameters/result_", ".png", resultSize);

	printf("LoadParameters done\n");
}

void scissor::on_Run_clicked() {

	//run something
	
	FD.SetFlowSpeed(ui->speedValue->value());

	if (ui->SaveParametersFlag->isChecked())
		SaveParameters();	

	imwrite(outputVideoPath + "block_flowFieldPathImg.png", block_flowFieldPathImg);
	imwrite(outputVideoPath + "flowPathImg.png", tmp_flowPathImg);

	if (flowFieldVectorAndPathImg.empty()) // if load parameters
		flowFieldVectorAndPathImg = FV.GetResultImg(203, 16, 26).clone();
		
	imwrite(outputVideoPath + "flowFieldVectorAndPathImg.png", flowFieldVectorAndPathImg);

	FD.SetUserPathArr(UI_pathArr);
	FD.SetSampleRate(FV.GetSampleRate());
	FD.SetSamplePointsN(FV.GetSamplePointsN());
	FD.SetFlowVector(FV.GetFlowVector());

	int layer_N;
	if (ui->Layer_0->isChecked())
		layer_N = 0;
	else if (ui->Layer_1->isChecked())
		layer_N = 1;
	FD.Run(layer_N);

	UpdateImage(FD.GetFixedMask());

	//reset
	FD.Reset();
	FlowFieldInit();
}

void scissor::on_combineLayer_clicked() {
	FD.CombineLayer();
	ui->GetResult->setEnabled(true);
}

void scissor::WriteParameters(string path) {
	imwrite(path + "/block_flowFieldPathImg.png", block_flowFieldPathImg);

	// save fixed mask
	imwrite(path + "/fixedMask.png", FD.GetFixedMask());

	if (!mattingResult.empty())
		imwrite(path + "/mattingResult.png", mattingResult);
	
	// save result
	vector<Mat>resultArr(FD.GetResult());
	if (!resultArr[0].empty())
		for (size_t i = 0; i < resultArr.size(); i++)
			imwrite(path + "/result_" + to_string(i) + ".png", resultArr[i]);

	fstream fp;
	string fileName = path + "/parameter.txt";
	fp.open(fileName, ios::out);
	fp << "Load_UI_pathArr" << endl;
	fp << UI_pathArr.size() << endl;

	for (size_t i = 0; i < UI_pathArr.size(); i++) {
		
		fp << UI_pathArr[i].size() << endl;
		for (size_t j = 0; j < UI_pathArr[i].size(); j++) {

			fp << UI_pathArr[i][j].x << " " << UI_pathArr[i][j].y << " ";
			//fprintf(fp, "\"%d %d\",", UI_pathArr[i][j].x, UI_pathArr[i][j].y);
		}
		fp << endl;
	}

	fp << "Load_GetSampleRate" << endl;
	fp << FV.GetSampleRate().x << " " << FV.GetSampleRate().y << endl;

	fp << "Load_GetSamplePointsN" << endl;
	fp << FV.GetSamplePointsN().x << " " << FV.GetSamplePointsN().y << endl;

	vector<FLOWVECTORTAG>tmpVector = FV.GetFlowVector();
	fp << "Load_GetFlowVector" << endl;

	fp << tmpVector.size() << endl;
	for (int i = 0; i < tmpVector.size(); i++) {
		fp << tmpVector[i].coordinate.x << " " << tmpVector[i].coordinate.y << " ";
	}
	fp << endl;
	for (int i = 0; i < tmpVector.size(); i++) {
		fp << tmpVector[i].vector.x << " " << tmpVector[i].vector.y << " ";
	}
	fp << endl;
	fp << "Load_resultImageSize" << endl;
	fp << resultArr.size() << endl;

	fp << "FlowSpeed" << endl;
	fp << (int)ui->speedValue->value() << endl;

	fp.close();//關閉檔案
	printf("on_SaveParameters_clicked done\n");
}

void scissor::LoadParameters(size_t* resultSize) {
	UI_pathArr.clear();
	FD.SetFixedMask(imread(loadParameterPath + "fixedMask.png"));
	Mat mattingMask_tmp = imread(loadParameterPath + "mattingResult.png");
	PrintMatData(mattingMask_tmp);
	if (!mattingMask_tmp.empty())
		FD.SetMattingMask(imread(loadParameterPath + "mattingResult.png"));

	fstream fp;
	string line, title;
	string fileName = loadParameterPath + "parameter.txt";
	fp.open(fileName, ios::in);

	if (!fp) {//如果開啟檔案失敗，fp為0；成功，fp為非0
		cout << "Fail to open file: " << fileName << endl;
		system("pause");
	}
	else {
		while (getline(fp, title)) {
			if (title == "Load_UI_pathArr") {
				getline(fp, line);
				int UI_pathArrSize = stoi(line);
				printf("UI_pathArrSize= %d\n", UI_pathArrSize); // by H: number of path
				UI_pathArr.resize(UI_pathArrSize);
				for (int i = 0; i < UI_pathArr.size(); i++) {
					getline(fp, line);
					int tmpSize = stoi(line);
					printf("tmpSize= %d\n", tmpSize);
					UI_pathArr[i].resize(tmpSize); // by H: length of path[i]
					for (int j = 0; j < UI_pathArr[i].size(); j++) 
					{
						fp >> UI_pathArr[i][j].x >> UI_pathArr[i][j].y; // by H: coordinate of each point of path[i]
						if (j == 0)
							printf("%d, %d\n", UI_pathArr[i][j].x, UI_pathArr[i][j].y);
					}
					getline(fp, line);// 吃掉換行符號....吧?
				}
			}
			else if (title == "Load_GetSampleRate") {
				cv::Point tmp;
				fp >> tmp.x >> tmp.y;
				FV.SetSampleRate(tmp);
			}
			else if (title == "Load_GetSamplePointsN") {
				cv::Point tmp;
				fp >> tmp.x >> tmp.y;
				FV.SetSamplePointsN(tmp);
			}
			else if (title == "Load_GetFlowVector") {
				getline(fp, line);
				int vectorSize = stoi(line);
				vector<FLOWVECTORTAG>tmpVector;
				tmpVector.resize(vectorSize);
				for (int i = 0; i < tmpVector.size(); i++) {
					fp >> tmpVector[i].coordinate.x >> tmpVector[i].coordinate.y;
				}
				for (int i = 0; i < tmpVector.size(); i++) {
					fp >> tmpVector[i].vector.x >> tmpVector[i].vector.y;
				}
				printf("last= %d, %d ;%lf, %lf\n", tmpVector[0].coordinate.x, tmpVector[0].coordinate.y, tmpVector[0].vector.x, tmpVector[0].vector.y);
				FV.SetFlowVector(tmpVector);
			}
			else if (title == "Load_resultImageSize") {
				fp >> (*resultSize);
			}
		}
	}
}

void scissor::UpdateImage(Mat animatedMask) {
	for (int y = 0; y < animatedMask.rows; y++) {
		for (int x = 0; x < animatedMask.cols; x++) {
			if (animatedMask.at<Vec3b>(Point(x, y))[0] != 255)//未處理 or fixed
				continue;
			else {//已產生動畫
				Vec3b fillColor(0, 255, 0);
				unprocessedImg.at<Vec3b>(Point(x, y)) = orgImg.at<Vec3b>(Point(x, y))*0.7 + fillColor*0.3;
				unprocessedMask.at<Vec3b>(Point(x, y)) = fillColor;
			}
		}
	}
	show_image(unprocessedImg, 0);
}

void scissor::on_showConstraints_clicked() {
	show_image(FD.GetBoundryConstraintsImg(), 1);
}

void scissor::FlowFieldInit() {
	UI_path.clear();
	UI_flowVector.clear();
	UI_flowVectorArray.clear();
	UI_pathArr.clear();
	block_flowFieldPathImg = Mat(orgImg.rows, orgImg.cols, CV_8UC3, Scalar(0, 0, 0));
}

void scissor::on_GetResult_clicked() {
	vector<Mat>result(FD.GetResult());
	for (int i = 0; i < result.size(); i++) {
		for (int y = 0; y < result[i].rows; y++) {
			for (int x = 0; x < result[i].cols; x++) {
				if (result[i].at<Vec3b>(Point(x, y)) != Vec3b(0, 0, 0))//if (unprocessedMask.at<Vec3b>(Point(x, y))[1] == 255)
					continue;
				else
					result[i].at<Vec3b>(Point(x, y)) = orgImg.at<Vec3b>(Point(x, y));
			}
		}
		string fileName = "finalResult_" + to_string(i) + ".jpg";
		imwrite(outputVideoPath + fileName, result[i]);
	}

	//output video
	FD.MKvideo(&result, 1, "finalResult");
	cv::Size videoSize = cv::Size(result[0].cols, result[0].rows);
	cv::VideoWriter writer;
	int fps = 20;
	writer.open(outputVideoPath + "finalResult.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps, videoSize);
	for (int i = 0; i < result.size(); i++) {
		cv::Mat frame;
		frame = result[i];
		if (!frame.empty()) {
			writer.write(frame);
		}
	}
	// 把avi轉成mp4

	string cmd{ "ffmpeg -i " + outputVideoPath +
		"finalResult.avi -c:v libx264 -crf 19 -preset slow -c:a libfdk_aac -b:a 192k -ac 2 -y " +
		outputVideoPath + "finalResult.mp4" };
	system(cmd.c_str());
	printf("Get finalResult done\n");

	QMessageBox msgBox;
	msgBox.setText("The final result has been saved.");
	msgBox.setInformativeText("Do you want to open the video directly?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Yes);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes) {
		string tmp_path = outputVideoPath + "finalResult.mp4";
		QString path(tmp_path.c_str());
		QUrl pathUrl = QUrl::fromLocalFile(path);
		QDesktopServices::openUrl(pathUrl);
	}
}

void scissor::on_colorComboBox_currentIndexChanged(const QString &arg) {
	if (arg == "ForeGround_fill" || arg == "ForeGround_draw") {
		//printf("ForeGround\n");
		mattingColor = Scalar(255, 255, 255);
	}
	else if (arg == "Gray") {
		//printf("Gray\n");
		mattingColor = Scalar(127, 127, 127);
	}
	else if (arg == "BackGround") {
		//printf("BackGround\n");
		mattingColor = Scalar(0, 0, 0);
	}
}

void scissor::on_ContentAware_clicked() {
	QString filename = QFileDialog::getOpenFileName(
		this,
		tr("Open Image"),
		"D:\\Desktop\\goodResult\\optimizationMethod\\surfingHradCase",
		tr("Image files (*.jpg *.png );;All files (*.*)"));
	Mat BG;
	if (filename.length() != 0) {
		std::string file = filename.toUtf8().constData();
		BG = cv::imread(file, 1);

		//Mat mask = FD.GetFixedMask();
		//for (int y = 0; y < mask.rows; y++){
		//	for (int x = 0; x < mask.cols; x++){
		//		if (mask.at<Vec3b>(Point(x, y)) == Vec3b(255, 255, 255) || mask.at<Vec3b>(Point(x, y)) == Vec3b(0, 0, 255))
		//			BG.at<Vec3b>(Point(x, y)) = orgImg.at<Vec3b>(Point(x, y));
		//	}
		//}
		//unprocessedImg = BG.clone();

		orgImg = BG.clone();

		copyMakeBorder(orgImg, image, paddingY, paddingY, paddingX, paddingX, BORDER_CONSTANT, Scalar(255, 255, 255));
		FD.UpdateOrgImg(orgImg);
		show_image(image, 0);
	}
}

void scissor::on_LoadROI_clicked() {
	QString filename = QFileDialog::getOpenFileName(
		this,
		tr("Open Image"),
		"D:\\Desktop\\RedHawkEffect\\Parameters\\surfingBackground.png",
		tr("Image files (*.jpg *.png );;All files (*.*)"));
	Mat ROImask;
	if (filename.length() != 0) {
		std::string file = filename.toUtf8().constData();
		ROImask = cv::imread(file, 1);
		//PrintMatData(ROImask);
		FD.SetBoundryConstraints(ROImask, 0);// set animatingROI
		FD.SetMattingMask(ROImask);
	}
}

void scissor::PrintMatData(Mat img) {
	printf("===============================================\n");
	printf("isempty= %d\n", img.empty());
	printf("h, w= %d, %d\n", img.rows, img.cols);
	printf("channels= %d\n", img.channels());
	printf("type= %d\n", img.type());
	printf("===============================================\n");
}

void scissor::on_actionOutput_Long_Video_triggered() {
	int cycle{ 90 };
	Point pa(FD.GetOutputVideoParameter());

	double totalSecond{ (double)pa.x / pa.y * cycle };

	string terminalCommand{ "ffmpeg -y -loop 1 -framerate " + to_string(pa.x)
		+ " -i " + outputVideoPath + "finalResult_%d.jpg -t " + to_string(totalSecond) + " " + outputVideoPath + "longVideo.mp4" };
	cout << terminalCommand << endl;
	system(terminalCommand.c_str());

	printf("============================================================\n");
	printf("MKvideo done\n");
	printf("============================================================\n");
}

void scissor::SaveParameters() {
	static unsigned int stepCnt = 1;
	string parameterPath = saveParameterPath + "step_" + to_string(stepCnt);

	// _mkdir(path)	Returns 0 if successful, otherwise -1
	// Folders that already have the same filename will fail
	if (_mkdir(parameterPath.c_str()) == 0) {
		printf("Add new floder successfully\n");
	}
	else {
		printf("The folder already exists and will be overwritten\n");
	}

	WriteParameters(parameterPath);

	stepCnt++;
}
