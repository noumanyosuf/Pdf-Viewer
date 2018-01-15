#ifndef PDFVIEWER_MAINWINDOW_H
#define PDFVIEWER_MAINWINDOW_H

#include <QMainWindow>

#include<QFileDialog>
#include<string>
#include<QDebug>



#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include<QLabel>
#include<QMessageBox>




#include <QHBoxLayout>
#include <QVBoxLayout>
#include<QPushButton>
#include <QLineEdit>
#include<QPainter>

class QScrollArea;
class QLabel;
class QToolBar;
class QAction;

#define HIT_MAX_COUNT 1000

class PdfViewer_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //explicit PdfViewer_MainWindow(QWidget *parent = nullptr);
    void startup();
//    ~PdfViewer_MainWindow();

private slots:
    /*open():
     * open QFileDialog to allow user to choose file from system
     *initilize doc using fetched filename
     *calls showpage to render first ie.0th page.
    */
    void open();

    /*previousPage():
     * render previous page
    */
    void previousPage();

    /*nextPage():
     * render next page
    */
    void nextPage();

    /*zoomIn():
     * zoom in page with .1f incremeneted m_scale value
    */
    void zoomIn();

    /*zoomOut():
     * zoom out page with .1f decremeneted m_scale value
    */
    void zoomOut();

    /*search:
     * seatch for a given text in current page
    */
    void search();
    /*****/
    void goTo();
    void clearTextFromSearch();
    /*****/
private:
    /*createActions:
    * initilize diffrrent Gui elements ie.previousPageAction,nextPageAction,zoomInAction,zoomOutAction,searchAction
    * and setup their connection to respective slots
    */
    void createActions();

    /*createToolBars:
     * add toolbar element to the window
    */
    void createToolBars();

    /*showPage:
     * render a page on the screen using index no of page
     * index: index of page which have to be rendered on screeen
    */
    void showPage(int index);

    /*render:
     *
    */
    QImage render(int pagenumber,float scaleX = 1.0f, float scaleY = 1.0f, float rotation = 0.0f);
    void getTitle();
    void setTitle(QString title,int p);
    void searchForText(QString needle);
    void highlight_searched_multitext();


    QImage gimage;
    fz_rect hit_box[100];
    int search_count;
    /*******/
    QAction *gotoAction;
    QLineEdit gotoLineEdit;
    /*******/

    QScrollArea *scrollArea;
    QString filename;
    QLabel *label;
    QToolBar *toolBar;
    QAction *openAction;
    QAction *previousPageAction;
    QAction *nextPageAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;

    QAction *searchAction;
    QLineEdit searchTextEdit;

    QString m_title;
    int m_numPages;
    int m_index;
    float m_scale;
    fz_context *ctx;
    fz_document *doc;


};

#endif // PDFVIEWER_MAINWINDOW_H
