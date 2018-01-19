#include "pdfviewer_mainwindow.h"

#include <QScrollArea>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QScrollBar>
#include <QInputDialog>
#include <cmath>
#include<QMessageBox>


static inline void imageCleanupHandler(void *data)
{
    unsigned char *samples = static_cast<unsigned char *>(data);

    if (samples) {
        delete []samples;
    }
}


PdfViewer_MainWindow::PdfViewer_MainWindow(QWidget *parent)
{
    filename="";
    search_count=0;
    m_index=0;
    m_scale=1.0f;
    m_rotate=0.0f;
    pix_height=0;
    pix_width=0;

    scrollArea = new QScrollArea;
    scrollArea->setAlignment(Qt::AlignCenter);

    label = new QLabel(this);
    label->setStyleSheet("QLabel { background-color : white;}");
    scrollArea->setWidget(label);

    this->setCentralWidget(scrollArea);
    this->setWindowState(Qt::WindowMaximized);

    //initilizing ctx
    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    fz_try(ctx){
        fz_register_document_handlers(ctx);
    }
    fz_catch(ctx)
    {
        qDebug()<<"cannot register document handlers: %s\n", fz_caught_message(ctx);
        fz_drop_context(ctx);
        exit(0);
    }

    //setting max length for the search string
    searchTextEdit.setMaxLength(20);

    createActions();
    createToolBars();
    open();
}

PdfViewer_MainWindow::~PdfViewer_MainWindow()
{
    //clean up
    openAction->deleteLater();
    previousPageAction->deleteLater();
    nextPageAction->deleteLater();
    zoomInAction->deleteLater();
    zoomOutAction->deleteLater();
    rotateLeftAction->deleteLater();
    rotateRightAction->deleteLater();
    DefalutModeAction->deleteLater();
    fitToHightAction->deleteLater();
    fitToWidthAction->deleteLater();
    fitToPageAction->deleteLater();
    toolBar->deleteLater();
    label->deleteLater();
    scrollArea->deleteLater();
    qDebug()<<"Object deleted sucessfully";
}

void PdfViewer_MainWindow::open()
{
    QString name=filename;
    filename = QFileDialog::getOpenFileName(this,tr("Open PDF/XPS file"), ".", "PDF (*.pdf);;XPS (*.xps)");
    if(filename.isEmpty())
    {
        if(!name.isEmpty()){filename=name; return;}
        qDebug()<<"Closing.No file name chosen.";
        exit(0);
    }
    else
        searchTextEdit.setText("");

    getTitle();

    // Open the PDF, XPS or CBZ document.
    //doc= fz_open_document(ctx, filename.toLatin1().data());

    // Open the PDF, XPS or CBZ document.
    fz_try(ctx){
        doc= fz_open_document(ctx, filename.toLatin1().data());
    }
    fz_catch(ctx)
    {
        qDebug()<<"cannot open document. %s\n", fz_caught_message(ctx);
        fz_drop_context(ctx);
        exit(0);
    }

    if(doc==NULL)
    {
        qDebug()<<"can not open document";
        return;
    }

    m_numPages=fz_count_pages(ctx, doc);
//    qDebug()<<"no of pages"<<m_numPages<<"\n";

    //resetting all variable to default while opening new pdf/doc.
    search_count=0;
    m_index=0;
    m_scale=1.0f;
    pix_height=0;
    pix_width=0;
    m_rotate=0;

    //disabling previous button when first page is dispalyed
    previousPageAction->setEnabled(false);

    //disabling next button if pdf has only one page
    if(m_numPages==1)
    {
        nextPageAction->setEnabled(false);
    }
    else
        nextPageAction->setEnabled(true);

    zoomInAction->setEnabled(true);
    zoomOutAction->setEnabled(true);

    showPage(0);

}

void PdfViewer_MainWindow::previousPage()
{


    if (m_index > 0) {
        --m_index;

        //checking if previous page had any string to search and then searching same string on the current page.
        QString input=searchTextEdit.text();
        if(input.isEmpty())
        {
            showPage(m_index);
            search_count=0;
        }
        else
        {
            showPage(m_index);
            searchForText(input);
        }
        nextPageAction->setEnabled(true);

    }

    //enabling and disablign the next/previous button based on the current page index
    if(m_index==0)
    {
        previousPageAction->setEnabled(false);
    }
    if(m_index!=m_numPages-1 && !(m_numPages))
    {
        nextPageAction->setEnabled(true);
    }
}

void PdfViewer_MainWindow::nextPage()
{
    if (m_index < m_numPages - 1)
    {
        ++m_index;

        //checking if previous page had any string to search and then searching same string on the current page.
        QString input=searchTextEdit.text();
        if(input.isEmpty())
        {
            showPage(m_index);
            search_count=0;
        }
        else
        {
            showPage(m_index);
            searchForText(input);
        }
        previousPageAction->setEnabled(true);
    }

    //enabling and disablign the next/previous button based on the current page index
    if(m_index==m_numPages-1)
    {
        nextPageAction->setEnabled(false);
    }
    if(m_index!=0 && !(m_numPages))
    {
        previousPageAction->setEnabled(true);
    }

    qDebug()<<"NextPage : m_index "<<m_index;

}

void PdfViewer_MainWindow::zoomIn()
{

    if (m_scale >= 3.0f) {
        zoomInAction->setEnabled(false);
        return;
    }
    m_scale += 0.3f;

    zoomOutAction->setEnabled(true);


    //re-highlighting searched text on the newly rendered page, if there is any
    QString input=searchTextEdit.text();
    if(input.isEmpty())
    {
        showPage(m_index);
        search_count=0;
    }
    else
    {
        showPage(m_index);
        searchForText(input);
    }

    qDebug()<<"ZoomIn : m_scale "<<m_scale;
}

void PdfViewer_MainWindow::zoomOut()
{


    if (m_scale < 0.3f) {
        zoomOutAction->setEnabled(false);
        return;
    }
    m_scale -= 0.3f;

    zoomInAction->setEnabled(true);


    //re-highlighting searched text on the newly rendered page, if there is any
    QString input=searchTextEdit.text();
    if(input.isEmpty())
    {
        showPage(m_index);
        search_count=0;
    }
    else
    {
        showPage(m_index);
        searchForText(input);
    }
}

void PdfViewer_MainWindow::search()
{
    QString searchText=searchTextEdit.text();
    if(searchText.isEmpty())
    {
        //qDebug()<<"Nothing to search.";
        QMessageBox msgBox(this);
        msgBox.setText("Enter text to search.");
        msgBox.setStandardButtons(QMessageBox::Close);
        msgBox.show();
        msgBox.exec();
    }
    else
    {
        showPage(m_index);
        searchForText(searchText);
    }
}

void PdfViewer_MainWindow::goTo()
{
    QString input=gotoLineEdit.text();

    int index_to_jump=(input).toInt();
    if(index_to_jump<=m_numPages && index_to_jump>0)
    {
        m_index=index_to_jump-1;
        QString input=searchTextEdit.text();

        //checking if previous page had any string to search and then searching same string on the current page.
        if(input.isEmpty())
        {
            showPage(m_index);
            search_count=0;
        }
        else
        {
            showPage(m_index);
            searchForText(input);
        }

        gotoLineEdit.setText("");

        //enable or disable next and prev button based on m_index value
        if(index_to_jump==m_numPages && m_numPages){qDebug()<<"nextpage false";
            nextPageAction->setEnabled(false);}
        else{qDebug()<<"nextpage true";
            nextPageAction->setEnabled(true);}

        if(index_to_jump==1 && m_numPages){qDebug()<<"prevpage false";
            previousPageAction->setEnabled(false);}
        else{qDebug()<<"prev page true";
            previousPageAction->setEnabled(true);}
    }
}

void PdfViewer_MainWindow::clearTextFromSearch()
{
    //clear all the highlighted text (search text) from the page when search text box is cleared
    if(searchTextEdit.text().isEmpty()){
        searchTextEdit.setText("");
        search_count=0;
        showPage(m_index);
    }

}

void PdfViewer_MainWindow::rotate_left()
{
    m_rotate -= 90.0f;


    m_rotate=(float)((int)m_rotate % 360); //m_value < 360 && m_value > -360

    //checking if previous page had any string to search and then searching same string on the current page.
    QString input=searchTextEdit.text();
    if(input.isEmpty())
    {
        showPage(m_index);
        search_count=0;
    }
    else
    {
        showPage(m_index);
        searchForText(input);
    }

}

void PdfViewer_MainWindow::rotate_right()
{
    m_rotate += 90.0f;
    m_rotate=(float)((int)m_rotate % 360);//m_value < 360 && m_value > -360

    //checking if previous page had any string to search and then searching same string on the current page.
    QString input=searchTextEdit.text();
    if(input.isEmpty() && search_count==0)
    {
        showPage(m_index);
        search_count=0;
    }
    else
    {
        showPage(m_index);
        searchForText(input);
    }
}

void PdfViewer_MainWindow::DefalutMode()
{
    m_rotate=0.0f;
    m_scale=1.0f;

    //checking if previous page had any string to search and then searching same string on the current page.
    QString input=searchTextEdit.text();
    if(input.isEmpty() && search_count==0)
    {
        showPage(m_index);
        search_count=0;
    }
    else
    {
        showPage(m_index);
        searchForText(input);
    }
}

void PdfViewer_MainWindow::fitToHight()
{
    /*swaping pix_width and pix_width when pdf is rotated either 90,-90,-270,270 degrees
     * in this case, as the pdf is rotated,the pdf hight and widht will interchange
     */
    float tem_pix_height=0.0f,tem_pix_width=0.0f;
    if(m_rotate==90 || m_rotate==-270 || m_rotate==270||m_rotate==-90)
    {
        tem_pix_height=pix_width;
        tem_pix_width=pix_height;

        pix_height=tem_pix_height;
        pix_width=tem_pix_width;

    }

    //calculating the m_scale value for which page hight fits the window hight
    float window_height=this->height();
    float scale=window_height-toolBar->height()-10;
    scale=scale/pix_height;
    m_scale=scale;

    //checking if previous page had any string to search and then searching same string on the current page.
    QString input=searchTextEdit.text();
    if(input.isEmpty() && search_count==0)
    {
        showPage(m_index);
        search_count=0;
    }
    else
    {
        showPage(m_index);
        searchForText(input);
    }

    /*restoring the pix_height and pix_width value back to the original as the fitting is done now.*/
    if(m_rotate==90 || m_rotate==-270 || m_rotate==270||m_rotate==-90)
    {
        pix_height=tem_pix_width;
        pix_width=tem_pix_height;
    }

}

void PdfViewer_MainWindow::fitToWidth()

{
    /*swaping pix_width and pix_width when pdf is rotated either 90,-90,-270,270 degrees
     * in this case, as the pdf is rotated,the pdf hight and widht will interchange
     */
    float tem_pix_height=0.0f,tem_pix_width=0.0f;
    if(m_rotate==90 || m_rotate==-270 || m_rotate==270||m_rotate==-90)
    {
        tem_pix_height=pix_width;
        tem_pix_width=pix_height;

        pix_height=tem_pix_height;
        pix_width=tem_pix_width;

    }

    //calculating the m_scale value for which page width fits the window hight
    float window_width=this->width();
    float scale=window_width-10;
    scale=scale/pix_width;
    m_scale=scale;

    //checking if previous page had any string to search and then searching same string on the current page.
    QString input=searchTextEdit.text();
    if(input.isEmpty() && search_count==0)
    {
        showPage(m_index);
        search_count=0;
    }
    else
    {
        showPage(m_index);
        searchForText(input);
    }

    /*restoring the pix_height and pix_width value back to the original as the fitting is done now.*/
    if(m_rotate==90 || m_rotate==-270 || m_rotate==270||m_rotate==-90)
    {
        pix_height=tem_pix_width;
        pix_width=tem_pix_height;
    }

}

void PdfViewer_MainWindow::fitToPage()
{
    float window_width=this->width();
    float window_height=this->height();
    if(window_height>=window_width)
    {
        fitToHight();
    }
    else
    {
        fitToWidth();
    }

}


void PdfViewer_MainWindow::createActions()
{

    openAction = new QAction(tr("Open"), this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    previousPageAction = new QAction(tr("Previous"), this);
    connect(previousPageAction, SIGNAL(triggered()), this, SLOT(previousPage()));

    nextPageAction = new QAction(tr("Next"), this);
    connect(nextPageAction, SIGNAL(triggered()), this, SLOT(nextPage()));

    zoomInAction = new QAction(tr("ZoomIn"), this);
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAction = new QAction(tr("ZoomOut"), this);
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    searchAction=new QAction(tr("Search"), this);
    connect(searchAction,SIGNAL(triggered()), this, SLOT(search()));

    gotoAction=new QAction(tr("Goto"),this);
    connect(gotoAction, SIGNAL(triggered()), this, SLOT(goTo()));

    connect(&searchTextEdit,SIGNAL(textChanged(QString)),this,SLOT(clearTextFromSearch()));

    rotateLeftAction=new QAction(tr("Rotate Left"), this);
    connect(rotateLeftAction, SIGNAL(triggered()), this, SLOT(rotate_left()));

    rotateRightAction=new QAction(tr("Rotate Right"), this);
    connect(rotateRightAction, SIGNAL(triggered()), this, SLOT(rotate_right()));

    DefalutModeAction=new QAction(tr("Default settings"), this);
    connect(DefalutModeAction, SIGNAL(triggered()), this, SLOT(DefalutMode()));

    fitToHightAction= new QAction(tr("Fit Height"),this);
    connect(fitToHightAction, SIGNAL(triggered()),this, SLOT(fitToHight()));

    fitToWidthAction= new QAction(tr("Fit Width"),this);
    connect(fitToWidthAction, SIGNAL(triggered()),this, SLOT(fitToWidth()));

    fitToPageAction=new QAction(tr("Fit to Page"),this);
    connect(fitToPageAction, SIGNAL(triggered()),this, SLOT(fitToPage()));

}

void PdfViewer_MainWindow::createToolBars()
{
    toolBar = this->addToolBar(tr("ToolBar"));
    toolBar->addAction(openAction);

    toolBar->addSeparator();

    toolBar->addAction(previousPageAction);
    toolBar->addAction(nextPageAction);

    toolBar->addSeparator();

    toolBar->addAction(zoomInAction);
    toolBar->addAction(zoomOutAction);

    toolBar->addSeparator();

    toolBar->addAction(rotateLeftAction);
    toolBar->addAction(rotateRightAction);

    toolBar->addSeparator();

    toolBar->addAction(DefalutModeAction);
    toolBar->addAction(fitToPageAction);
    toolBar->addAction(fitToWidthAction);
    toolBar->addAction(fitToHightAction);

    toolBar->addSeparator();

    toolBar->addAction(DefalutModeAction);

    toolBar->addSeparator();


    toolBar->addWidget(&gotoLineEdit);
    toolBar->addAction(gotoAction);

    toolBar->addSeparator();

    toolBar->addWidget(&searchTextEdit);
    toolBar->addAction(searchAction);

    toolBar->setFixedHeight(30);

    gotoLineEdit.setMaximumWidth(30);

}


void PdfViewer_MainWindow::showPage(int index)
{
    QImage image =render(index,m_scale,m_scale,m_rotate);

    gimage=image;

    label->setPixmap(QPixmap::fromImage(image));
    label->resize(label->sizeHint());

    scrollArea->verticalScrollBar()->setValue(0);
    setTitle(m_title,(index+1));
}

QImage PdfViewer_MainWindow::render(int pagenumber,float scaleX, float scaleY, float rotation)
{
    QImage image;
    unsigned char *samples = NULL;
    unsigned char  *copyed_samples = NULL;
    int width = 0;
    int height = 0;


    fz_page *page = fz_load_page(ctx, doc, pagenumber);
    if(!page)
    {
        qDebug()<<"Cant load page.Droping page & exiting.";
        fz_drop_page(ctx,page);
        exit(0);
    }

    // build transform matrix
    fz_matrix transform = fz_identity;
    fz_rotate(&transform, rotation);
    fz_pre_scale(&transform, scaleX, scaleY);

    // get transformed page size
    fz_rect bounds;
    fz_irect bbox;
    fz_bound_page(ctx, page, &bounds);
    fz_transform_rect(&bounds, &transform);
    fz_round_rect(&bbox, &bounds);

    fz_pixmap *pix =fz_new_pixmap_from_page_number(ctx,doc,pagenumber,&transform,fz_device_bgr(ctx),1);

    samples = fz_pixmap_samples(ctx, pix);



    width = fz_pixmap_width(ctx, pix);
    height = fz_pixmap_height(ctx, pix);

    /*
    * set pix_height and pix_width to 0 when pdf is opened for the first time or new pdf is open for the first time
    * used while calculating m_scale for rotate_left and rotate_right
    */
    if(pix_height==0 && pix_width==0){

        pix_height=height;
        pix_width=width;
    }

//    qDebug()<<"pix width "<<width<<" pix height "<<height;
//    qDebug()<<"this width"<<this->width()<<" this height"<<this->height()<<endl;



    copyed_samples=samples;

#if QT_VERSION < 0x050200
    // most computers use little endian, so Format_ARGB32 means bgra order
    // note: this is not correct for computers with big endian architecture
    image = QImage(copyed_samples,
                   width, height, QImage::Format_ARGB32, imageCleanupHandler, copyed_samples);
#else
    // with Qt 5.2, Format_RGBA8888 is correct for any architecture
    image = QImage(copyed_samples,
                   width, height, QImage::Format_RGBA8888, NULL, copyed_samples);
#endif

    fz_drop_page(ctx,page);
    return image;
}

void PdfViewer_MainWindow::getTitle()
{
    int last_index=filename.lastIndexOf("/");
    m_title=filename.mid(last_index+1,filename.size());
}

void PdfViewer_MainWindow::setTitle(QString title,int p)
{
    QString end_title=QString::number(p)+"/"+QString::number(m_numPages);
    this->setWindowTitle(m_title+" - "+end_title);

}

void PdfViewer_MainWindow::searchForText(QString needle)
{
    int hit_max=HIT_MAX_COUNT;
    fz_rect hit_box[HIT_MAX_COUNT];

    search_count= fz_search_page_number(ctx, doc, m_index, needle.toLatin1().data(), hit_box, hit_max);

    //if hit occurs
    if(search_count)
    {
        qDebug()<<"search found : count = "<<search_count;

        for(int i=0;i<search_count;i++)
        {
            QPainter painter(&gimage);
            double width,hight;
            double label_x,label_y;
            double label_width,label_heigth;

            //using pen to hide the border of the rect to used with the qpainter
            QPen pen;
            pen.setStyle(Qt::NoPen);
            painter.setPen(pen);

            //calculating the width and heigth or the search text
            width=static_cast<double>(hit_box[i].x1)-static_cast<double>(hit_box[i].x0);
            hight=static_cast<double>(hit_box[i].y1)-static_cast<double>(hit_box[i].y0);

            //calculation the x and y cordinate where qpainter will be set
            if(m_rotate==0)
            {
                label_x=hit_box[i].x0*m_scale;
                label_y=hit_box[i].y0*m_scale;
                label_width=width*m_scale;
                label_heigth=hight*m_scale;

            }
            else if(m_rotate==90 || m_rotate==-270)//if rotated clockwise or anti clockwise
            {
                label_width=hight*m_scale;
                label_heigth=width*m_scale;

                label_x=label->width()-hit_box[i].y0*m_scale-label_width;
                label_y=hit_box[i].x0*m_scale;

            }
            else if(m_rotate==180||m_rotate==-180)//if rotated clockwise or anti clockwise
            {
                label_x=label->width()-hit_box[i].x0*m_scale-width*m_scale;
                label_y=label->height()-hit_box[i].y0*m_scale-hight*m_scale;
                label_width=width*m_scale;
                label_heigth=hight*m_scale;
            }
            else if(m_rotate==270 || m_rotate==-90)//if rotated clockwise or anti clockwise
            {
                label_width=hight*m_scale;
                label_heigth=width*m_scale;
                label_x=hit_box[i].y0*m_scale;
                label_y=label->height()-hit_box[i].x0*m_scale-label_heigth;
            }

            QRectF rect(static_cast<double>(label_x),static_cast<double>(label_y),label_width,label_heigth);

            painter.begin(this);
            painter.drawRect(rect);
            painter.fillRect(rect, QBrush(QColor(255, 255, 0, 128)));

            painter.end();

        }
        label->setPixmap(QPixmap::fromImage(gimage));
        label->show();

    }
    else{
        qDebug()<<"No search result";
    }
}