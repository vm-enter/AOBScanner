#include "stdafx.h"

#include "CMainWindow.hpp"

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(aobscanner);

	QCoreApplication::setOrganizationName("Japanese1337hackers");
	QCoreApplication::setOrganizationDomain("jpn.hackers.com");
	QCoreApplication::setApplicationName("Scanner");

	QApplication app(argc, argv);
	CMainWindow mainwindow;
	mainwindow.show();
	return app.exec();
}
