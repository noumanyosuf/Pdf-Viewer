#include <QApplication>
#include<pdfviewer_mainwindow.h>


int main(int argcQT, char *argvQT[])
{
    QApplication a(argcQT, argvQT);

    PdfViewer_MainWindow win;
    win.startup();
    win.show();
    return a.exec();
}
