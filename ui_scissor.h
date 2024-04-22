/********************************************************************************
** Form generated from reading UI file 'scissor.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCISSOR_H
#define UI_SCISSOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_scissor
{
public:
    QAction *actionOpen;
    QAction *actionZoom_In;
    QAction *actionZoom_Out;
    QAction *actionGradient_Map;
    QAction *actionOrigin_Map;
    QAction *actionStart;
    QAction *actionBack;
    QAction *actionRestart;
    QAction *actionDrawNodeVector;
    QAction *actionSave_Image;
    QAction *actionSave_Mask;
    QAction *actionGet_Mask;
    QAction *actionPixel_node;
    QAction *actionExit;
    QAction *actionOutput_Long_Video;
    QWidget *centralWidget;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_2;
    QPushButton *runMorphing;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QSpinBox *speedValue;
    QRadioButton *fixedROIFlag;
    QRadioButton *Layer_0;
    QRadioButton *animatingPictureFlag;
    QComboBox *colorComboBox;
    QRadioButton *animatingROIFlag;
    QRadioButton *flowFieldFlag;
    QRadioButton *mattingROIFlag;
    QCheckBox *SaveParametersFlag;
    QPushButton *LoadROI;
    QPushButton *LoadParameter;
    QRadioButton *Layer_1;
    QPushButton *Run;
    QPushButton *showConstraints;
    QPushButton *GetResult;
    QPushButton *combineLayer;
    QPushButton *ContentAware;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuAction;
    QMenu *menuDebug;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QButtonGroup *radioBtnGroup;
    QButtonGroup *buttonGroup;

    void setupUi(QMainWindow *scissor)
    {
        if (scissor->objectName().isEmpty())
            scissor->setObjectName(QStringLiteral("scissor"));
        scissor->resize(1765, 798);
        actionOpen = new QAction(scissor);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionZoom_In = new QAction(scissor);
        actionZoom_In->setObjectName(QStringLiteral("actionZoom_In"));
        actionZoom_Out = new QAction(scissor);
        actionZoom_Out->setObjectName(QStringLiteral("actionZoom_Out"));
        actionGradient_Map = new QAction(scissor);
        actionGradient_Map->setObjectName(QStringLiteral("actionGradient_Map"));
        actionOrigin_Map = new QAction(scissor);
        actionOrigin_Map->setObjectName(QStringLiteral("actionOrigin_Map"));
        actionStart = new QAction(scissor);
        actionStart->setObjectName(QStringLiteral("actionStart"));
        actionBack = new QAction(scissor);
        actionBack->setObjectName(QStringLiteral("actionBack"));
        actionRestart = new QAction(scissor);
        actionRestart->setObjectName(QStringLiteral("actionRestart"));
        actionDrawNodeVector = new QAction(scissor);
        actionDrawNodeVector->setObjectName(QStringLiteral("actionDrawNodeVector"));
        actionSave_Image = new QAction(scissor);
        actionSave_Image->setObjectName(QStringLiteral("actionSave_Image"));
        actionSave_Mask = new QAction(scissor);
        actionSave_Mask->setObjectName(QStringLiteral("actionSave_Mask"));
        actionGet_Mask = new QAction(scissor);
        actionGet_Mask->setObjectName(QStringLiteral("actionGet_Mask"));
        actionPixel_node = new QAction(scissor);
        actionPixel_node->setObjectName(QStringLiteral("actionPixel_node"));
        actionExit = new QAction(scissor);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionOutput_Long_Video = new QAction(scissor);
        actionOutput_Long_Video->setObjectName(QStringLiteral("actionOutput_Long_Video"));
        centralWidget = new QWidget(scissor);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        scrollArea = new QScrollArea(centralWidget);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setGeometry(QRect(0, 0, 1521, 731));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1502, 712));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(scrollAreaWidgetContents);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(scrollAreaWidgetContents);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(0, 0));
        label->setContextMenuPolicy(Qt::DefaultContextMenu);
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        horizontalLayout->addWidget(label);

        label_2 = new QLabel(scrollAreaWidgetContents);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        horizontalLayout->addWidget(label_2);

        scrollArea->setWidget(scrollAreaWidgetContents);
        runMorphing = new QPushButton(centralWidget);
        runMorphing->setObjectName(QStringLiteral("runMorphing"));
        runMorphing->setGeometry(QRect(1550, 690, 101, 41));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(1530, 10, 219, 571));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        groupBox = new QGroupBox(layoutWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMaximumSize(QSize(217, 81));
        QFont font;
        font.setPointSize(12);
        groupBox->setFont(font);
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(false);
        font1.setWeight(50);
        label_3->setFont(font1);

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        speedValue = new QSpinBox(groupBox);
        speedValue->setObjectName(QStringLiteral("speedValue"));
        speedValue->setMaximum(600);
        speedValue->setValue(30);

        gridLayout->addWidget(speedValue, 1, 0, 1, 1);


        gridLayout_2->addWidget(groupBox, 13, 0, 1, 1);

        fixedROIFlag = new QRadioButton(layoutWidget);
        radioBtnGroup = new QButtonGroup(scissor);
        radioBtnGroup->setObjectName(QStringLiteral("radioBtnGroup"));
        radioBtnGroup->addButton(fixedROIFlag);
        fixedROIFlag->setObjectName(QStringLiteral("fixedROIFlag"));
        fixedROIFlag->setEnabled(false);

        gridLayout_2->addWidget(fixedROIFlag, 3, 0, 1, 1);

        Layer_0 = new QRadioButton(layoutWidget);
        buttonGroup = new QButtonGroup(scissor);
        buttonGroup->setObjectName(QStringLiteral("buttonGroup"));
        buttonGroup->addButton(Layer_0);
        Layer_0->setObjectName(QStringLiteral("Layer_0"));
        Layer_0->setChecked(true);

        gridLayout_2->addWidget(Layer_0, 11, 0, 1, 1);

        animatingPictureFlag = new QRadioButton(layoutWidget);
        radioBtnGroup->addButton(animatingPictureFlag);
        animatingPictureFlag->setObjectName(QStringLiteral("animatingPictureFlag"));
        animatingPictureFlag->setEnabled(false);
        animatingPictureFlag->setChecked(true);

        gridLayout_2->addWidget(animatingPictureFlag, 5, 0, 1, 1);

        colorComboBox = new QComboBox(layoutWidget);
        colorComboBox->setObjectName(QStringLiteral("colorComboBox"));
        colorComboBox->setEnabled(false);

        gridLayout_2->addWidget(colorComboBox, 1, 0, 1, 1);

        animatingROIFlag = new QRadioButton(layoutWidget);
        radioBtnGroup->addButton(animatingROIFlag);
        animatingROIFlag->setObjectName(QStringLiteral("animatingROIFlag"));
        animatingROIFlag->setEnabled(false);

        gridLayout_2->addWidget(animatingROIFlag, 2, 0, 1, 1);

        flowFieldFlag = new QRadioButton(layoutWidget);
        radioBtnGroup->addButton(flowFieldFlag);
        flowFieldFlag->setObjectName(QStringLiteral("flowFieldFlag"));
        flowFieldFlag->setEnabled(false);

        gridLayout_2->addWidget(flowFieldFlag, 4, 0, 1, 1);

        mattingROIFlag = new QRadioButton(layoutWidget);
        radioBtnGroup->addButton(mattingROIFlag);
        mattingROIFlag->setObjectName(QStringLiteral("mattingROIFlag"));
        mattingROIFlag->setEnabled(false);

        gridLayout_2->addWidget(mattingROIFlag, 0, 0, 1, 1);

        SaveParametersFlag = new QCheckBox(layoutWidget);
        SaveParametersFlag->setObjectName(QStringLiteral("SaveParametersFlag"));
        SaveParametersFlag->setCheckable(true);
        SaveParametersFlag->setChecked(true);

        gridLayout_2->addWidget(SaveParametersFlag, 14, 0, 1, 1);

        LoadROI = new QPushButton(layoutWidget);
        LoadROI->setObjectName(QStringLiteral("LoadROI"));

        gridLayout_2->addWidget(LoadROI, 6, 0, 1, 1);

        LoadParameter = new QPushButton(layoutWidget);
        LoadParameter->setObjectName(QStringLiteral("LoadParameter"));

        gridLayout_2->addWidget(LoadParameter, 9, 0, 1, 1);

        Layer_1 = new QRadioButton(layoutWidget);
        buttonGroup->addButton(Layer_1);
        Layer_1->setObjectName(QStringLiteral("Layer_1"));

        gridLayout_2->addWidget(Layer_1, 12, 0, 1, 1);

        Run = new QPushButton(layoutWidget);
        Run->setObjectName(QStringLiteral("Run"));
        Run->setEnabled(true);
        Run->setCheckable(false);

        gridLayout_2->addWidget(Run, 15, 0, 1, 1);

        showConstraints = new QPushButton(layoutWidget);
        showConstraints->setObjectName(QStringLiteral("showConstraints"));

        gridLayout_2->addWidget(showConstraints, 7, 0, 1, 1);

        GetResult = new QPushButton(layoutWidget);
        GetResult->setObjectName(QStringLiteral("GetResult"));
        GetResult->setEnabled(false);

        gridLayout_2->addWidget(GetResult, 8, 0, 1, 1);

        combineLayer = new QPushButton(layoutWidget);
        combineLayer->setObjectName(QStringLiteral("combineLayer"));

        gridLayout_2->addWidget(combineLayer, 16, 0, 1, 1);

        ContentAware = new QPushButton(layoutWidget);
        ContentAware->setObjectName(QStringLiteral("ContentAware"));

        gridLayout_2->addWidget(ContentAware, 10, 0, 1, 1);

        scissor->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(scissor);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1765, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuAction = new QMenu(menuBar);
        menuAction->setObjectName(QStringLiteral("menuAction"));
        menuDebug = new QMenu(menuBar);
        menuDebug->setObjectName(QStringLiteral("menuDebug"));
        scissor->setMenuBar(menuBar);
        mainToolBar = new QToolBar(scissor);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        scissor->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(scissor);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        scissor->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuAction->menuAction());
        menuBar->addAction(menuDebug->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave_Image);
        menuFile->addAction(actionSave_Mask);
        menuFile->addAction(actionExit);
        menuFile->addAction(actionOutput_Long_Video);
        menuView->addAction(actionZoom_In);
        menuView->addAction(actionZoom_Out);
        menuView->addAction(actionOrigin_Map);
        menuView->addAction(actionGradient_Map);
        menuAction->addAction(actionStart);
        menuAction->addAction(actionBack);
        menuAction->addAction(actionRestart);
        menuAction->addAction(actionGet_Mask);
        menuDebug->addAction(actionDrawNodeVector);

        retranslateUi(scissor);

        QMetaObject::connectSlotsByName(scissor);
    } // setupUi

    void retranslateUi(QMainWindow *scissor)
    {
        scissor->setWindowTitle(QApplication::translate("scissor", "RedHawkEffect", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("scissor", "Open", Q_NULLPTR));
        actionZoom_In->setText(QApplication::translate("scissor", "Zoom In", Q_NULLPTR));
        actionZoom_Out->setText(QApplication::translate("scissor", "Zoom Out", Q_NULLPTR));
        actionGradient_Map->setText(QApplication::translate("scissor", "Gradient Map", Q_NULLPTR));
        actionOrigin_Map->setText(QApplication::translate("scissor", "Origin Map", Q_NULLPTR));
        actionStart->setText(QApplication::translate("scissor", "Start", Q_NULLPTR));
        actionBack->setText(QApplication::translate("scissor", "Back", Q_NULLPTR));
        actionRestart->setText(QApplication::translate("scissor", "Restart", Q_NULLPTR));
        actionDrawNodeVector->setText(QApplication::translate("scissor", "Cost graph", Q_NULLPTR));
        actionSave_Image->setText(QApplication::translate("scissor", "Save Image", Q_NULLPTR));
        actionSave_Mask->setText(QApplication::translate("scissor", "Save Mask", Q_NULLPTR));
        actionGet_Mask->setText(QApplication::translate("scissor", "Get Mask", Q_NULLPTR));
        actionPixel_node->setText(QApplication::translate("scissor", "Pixel node", Q_NULLPTR));
        actionExit->setText(QApplication::translate("scissor", "Exit", Q_NULLPTR));
        actionOutput_Long_Video->setText(QApplication::translate("scissor", "Output Long Video", Q_NULLPTR));
        label->setText(QString());
        label_2->setText(QString());
        runMorphing->setText(QApplication::translate("scissor", "runMorphing", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("scissor", "flow speed", Q_NULLPTR));
        label_3->setText(QApplication::translate("scissor", "Enter a value between 0 and 60", Q_NULLPTR));
        fixedROIFlag->setText(QApplication::translate("scissor", "fixedROI", Q_NULLPTR));
        Layer_0->setText(QApplication::translate("scissor", "Layer_0", Q_NULLPTR));
        animatingPictureFlag->setText(QApplication::translate("scissor", "animatingPicture", Q_NULLPTR));
        colorComboBox->clear();
        colorComboBox->insertItems(0, QStringList()
         << QApplication::translate("scissor", "ForeGround_fill", Q_NULLPTR)
         << QApplication::translate("scissor", "ForeGround_draw", Q_NULLPTR)
         << QApplication::translate("scissor", "Gray", Q_NULLPTR)
         << QApplication::translate("scissor", "BackGround", Q_NULLPTR)
        );
        animatingROIFlag->setText(QApplication::translate("scissor", "animatingROI", Q_NULLPTR));
        flowFieldFlag->setText(QApplication::translate("scissor", "flowField", Q_NULLPTR));
        mattingROIFlag->setText(QApplication::translate("scissor", "mattingROI", Q_NULLPTR));
        SaveParametersFlag->setText(QApplication::translate("scissor", "SaveParameters", Q_NULLPTR));
        LoadROI->setText(QApplication::translate("scissor", "LoadROI", Q_NULLPTR));
        LoadParameter->setText(QApplication::translate("scissor", "LoadParameter", Q_NULLPTR));
        Layer_1->setText(QApplication::translate("scissor", "Layer_1", Q_NULLPTR));
        Run->setText(QApplication::translate("scissor", "Run", Q_NULLPTR));
        showConstraints->setText(QApplication::translate("scissor", "showConstraints", Q_NULLPTR));
        GetResult->setText(QApplication::translate("scissor", "GetResult", Q_NULLPTR));
        combineLayer->setText(QApplication::translate("scissor", "combineLayer", Q_NULLPTR));
        ContentAware->setText(QApplication::translate("scissor", "ContentAware", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("scissor", "File", Q_NULLPTR));
        menuView->setTitle(QApplication::translate("scissor", "View", Q_NULLPTR));
        menuAction->setTitle(QApplication::translate("scissor", "Action", Q_NULLPTR));
        menuDebug->setTitle(QApplication::translate("scissor", "Debug", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class scissor: public Ui_scissor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCISSOR_H
