////////////////////////////////////////////////////////////////////////////////////////////////////
/// File:	main.cpp.
///
/// Summary:	Implements the main class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "scissor.h"
#include <QApplication>
#include <QTextStream>


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Fn:	int main(int argc, char *argv[])
///
/// Summary:	Main entry-point for this application.
///
/// Author:	Yc Lin
///
/// Date:	2019/10/4
///
/// Parameters:
/// argc - 	   	The number of command-line arguments provided. 
/// argv - 	   	An array of command-line argument strings. 
///
/// Returns:	Exit-code for the process - 0 for success, else an error code.
////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	// load qt theme
	string tmp_includePath = includePath + "qdarkstyle/Ubuntu.qss";
	QFile f(tmp_includePath.c_str());//:qdarkstyle/style.qss
	if (!f.exists()) {
		printf("Unable to set stylesheet, file not found\n");
		system("pause");
	}
	else {
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}

	scissor w;
	w.show();

	return a.exec();
}
