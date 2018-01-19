#ifndef PDFVIEWER_MAINWINDOW_H

#define PDFVIEWER_MAINWINDOW_H



#include <QMainWindow>



#include <limits.h>

#include <stdlib.h>

#include <stdio.h>



#include "mupdf/fitz.h"

#include "mupdf/pdf.h"



#include<QLabel>

#include<QMessageBox>

#include <QHBoxLayout>

#include <QVBoxLayout>

#include <QLineEdit>

#include<QPainter>

#include<QFileDialog>

#include<string>

#include<QDebug>



class QScrollArea;

class QLabel;

class QToolBar;

class QAction;



#define HIT_MAX_COUNT 1000



class PdfViewer_MainWindow : public QMainWindow

{

    Q_OBJECT



public:

    explicit PdfViewer_MainWindow(QWidget *parent = nullptr);

    ~PdfViewer_MainWindow();



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

     * search for a given text in  page

     * and highlight it

    */

    void search();



    /*goTo() :

     * go to the entered page no

     *and if any string is searched(if search box is not empty),highlight all occurance of search string on that page.

    */

    void goTo();



    /*clearTextFromSearch:

     * clear all highlighted searched string when the search box is cleared

    */

    void clearTextFromSearch();



    /*rotate_left:

     * roatet page to 90 degree to left

     */

    void rotate_left();



    /*rotate_right:

     * roatet page to 90 degree to right

     */

    void rotate_right();



    /*DefalutMode:

     *set page to all default setting

     */

    void DefalutMode();



    /*fitToHight:

     *fit page to the window height

     */

    void fitToHight();



    /*fitToWidth:

     *fit page to the window right

     */

    void fitToWidth();



    /*fitToPage:

     *fit page to the window height(if height > width) or width ((if height < width))

     */

    void fitToPage();

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



    /*render(int pagenumber,float scaleX = 1.0f, float scaleY = 1.0f, float rotation = 0.0f);:

     *pagenumber : page index which have be be rendered

     * scaleX : scale factor for X

     * scaleY : scale factor for Y

     * roation : degree to which page have to be rotated (rotation<360 && roatation <-360)

    */

    QImage render(int pagenumber,float scaleX = 1.0f, float scaleY = 1.0f, float rotation = 0.0f);



    /*getTitle:

     * get the title of the page from the filename

     */

    void getTitle();



    /*setTitle(QString title,int p):

     *set the title of the page to the widget title

     * title : filename

     * p : page number

     */

    void setTitle(QString title,int p);



    /*searchForText:

     * search for a 'needle' in the current page and highlight it

     * needle : text to be searched

     */

    void searchForText(QString needle);





    QImage gimage;  //to maintain copy of the image use showpage(), used while highligting during searchForText

    fz_rect hit_box[HIT_MAX_COUNT]; //stored the codinates of hit during searching text

    int search_count;   //stored hit count during searching text



    QScrollArea *scrollArea;

    QString filename;

    QLabel *label;

    QToolBar *toolBar;

    QAction *openAction;

    QAction *previousPageAction;

    QAction *nextPageAction;

    QAction *zoomInAction;

    QAction *zoomOutAction;

    QAction *rotateLeftAction;

    QAction *rotateRightAction;

    QAction *DefalutModeAction;

    QAction *gotoAction;

    QAction *searchAction;

    QAction *fitToHightAction;

    QAction *fitToWidthAction;

    QAction *fitToPageAction;



    QLineEdit searchTextEdit;

    QLineEdit gotoLineEdit;



    QString m_title;

    int m_numPages;

    int m_index;

    float m_scale;

    float m_rotate;

    fz_context *ctx;

    fz_document *doc;

    float pix_height;

    float pix_width;
};



#endif // PDFVIEWER_MAINWINDOW_H