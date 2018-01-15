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


void PdfViewer_MainWindow::startup()
{
    filename="";
    search_count=-1;
    m_index=0, m_scale=1.0f;
    scrollArea = new QScrollArea;
    scrollArea->setAlignment(Qt::AlignCenter);

    label = new QLabel(this);
    label->setStyleSheet("QLabel { background-color : white;}");
    scrollArea->setWidget(label);

    this->setCentralWidget(scrollArea);
    this->setWindowState(Qt::WindowMaximized);


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

    createActions();
    createToolBars();
    open();


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


    qDebug()<<filename<<endl;
    // Open the PDF, XPS or CBZ document.
    doc= fz_open_document(ctx, filename.toLatin1().data());
    if(doc==NULL)
    {
        qDebug()<<"can not open document";
        return;
    }
    // m_title =
    m_numPages=fz_count_pages(ctx, doc);
    qDebug()<<"no of pages"<<m_numPages<<"\n";

    search_count=-1;
    m_index=0, m_scale=1.0f;
    previousPageAction->setEnabled(false);
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
        //        showPage(m_index);
        //        search_count=0;
        QString input=searchTextEdit.text();
        qDebug()<<"search text for prev page"<<input;
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
        //showPage(m_index);

    }
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
        QString input=searchTextEdit.text();
        qDebug()<<"search text for next page"<<input;
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
        //showPage(m_index);
        //search_count=0;
        previousPageAction->setEnabled(true);
    }
    if(m_index==m_numPages-1)
    {
        nextPageAction->setEnabled(false);
    }
    if(m_index!=0 && !(m_numPages))
    {
        previousPageAction->setEnabled(true);
    }

}

void PdfViewer_MainWindow::zoomIn()
{
    if (m_scale >= 3.0f) {//if (m_scale >= 10.0f) {
        zoomInAction->setEnabled(false);
        return;
    }
    zoomOutAction->setEnabled(true);
    m_scale += 0.3f;
    qDebug()<<"ZoomIn : "<<m_scale;
    showPage(m_index);
    if(search_count)
    {
        QString searchText=searchTextEdit.text();
        if(searchText.isEmpty()){return;}
        searchForText(searchText);
    }
}

void PdfViewer_MainWindow::zoomOut()
{
    if (m_scale < 0.3f) {
        zoomOutAction->setEnabled(false);
        return;
    }
    zoomInAction->setEnabled(true);
    m_scale -= 0.3f;
    qDebug()<<"ZoomOut : "<<m_scale;
    showPage(m_index);
    if(search_count)
    {
        QString searchText=searchTextEdit.text();
        if(searchText.isEmpty()){return;}
        searchForText(searchText);
    }
}

void PdfViewer_MainWindow::search()
{
    QString searchText=searchTextEdit.text();
    if(searchText.isEmpty())
    {
        qDebug()<<"Nothing to search.";
        QMessageBox msgBox(this);
        msgBox.setText("Enter text to search.");
        msgBox.setStandardButtons(QMessageBox::Close);
        msgBox.show();
        msgBox.exec();
    }
    else
    {
        searchForText(searchText);
    }
}

void PdfViewer_MainWindow::goTo()
{
    QString input=gotoLineEdit.text();

    int index_to_jump=(input).toInt();
    qDebug()<<index_to_jump;
    if(index_to_jump<=m_numPages && index_to_jump>0)
    {

//        showPage(index_to_jump-1);
//        search_count=0;
        m_index=index_to_jump-1;
        QString input=searchTextEdit.text();
        qDebug()<<"search text for goto page"<<input;
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

        if(index_to_jump==0 && m_numPages){qDebug()<<"prevpage false";
            previousPageAction->setEnabled(false);}
        else{qDebug()<<"prev page true";
            previousPageAction->setEnabled(true);}
    }
}

void PdfViewer_MainWindow::clearTextFromSearch()
{
    if(searchTextEdit.text().isEmpty()){
        searchTextEdit.setText("");
        search_count=0;
        showPage(m_index);
    }
    //qDebug()<<"clearing";

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

}

void PdfViewer_MainWindow::createToolBars()
{
    toolBar = this->addToolBar(tr("ToolBar"));
    toolBar->addAction(openAction);
    toolBar->addSeparator();
    toolBar->addAction(previousPageAction);
    toolBar->addAction(nextPageAction);
    toolBar->addAction(zoomInAction);
    toolBar->addAction(zoomOutAction);
    toolBar->addSeparator();
    //****************************
    gotoLineEdit.setMaximumWidth(30);
    toolBar->addWidget(&gotoLineEdit);
    toolBar->addAction(gotoAction);
    toolBar->addSeparator();
    //*************************

    toolBar->addWidget(&searchTextEdit);
    toolBar->addAction(searchAction);
    toolBar->setFixedHeight(30);

}


void PdfViewer_MainWindow::showPage(int index)
{
    QImage image = render(index,m_scale,m_scale);
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
    int size = 0;
    //fz_display_list *display_list;

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
    size = width * height * 4;

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
    qDebug()<<"searching for "<<needle;
    qDebug()<<"Search in page no"<<m_index;
    int hit_max=HIT_MAX_COUNT;
    fz_rect hit_box[HIT_MAX_COUNT];

    showPage(m_index);
    search_count= fz_search_page_number(ctx, doc, m_index, needle.toLatin1().data(), hit_box, hit_max);
    if(search_count)
    {
        qDebug()<<"search found : count = "<<search_count;

        for(int i=0;i<search_count;i++)
        {
            QPainter painter(&gimage);
            double wid,hight;
            QPen pen;
            pen.setStyle(Qt::NoPen);
            painter.setPen(pen);
            wid=static_cast<double>(hit_box[i].x1)-static_cast<double>(hit_box[i].x0);
            hight=static_cast<double>(hit_box[i].y1)-static_cast<double>(hit_box[i].y0);
            QRectF rect(static_cast<double>(hit_box[i].x0*m_scale),static_cast<double>(hit_box[i].y0*m_scale),wid*m_scale,hight*m_scale);
            qDebug()<<"Hit box :"<<hit_box[i].x0<<"-"<<hit_box[i].y0;
            qDebug()<<"Label x,y : "<<label->x()<<"-"<<label->y();
            qDebug()<<"Label w,h: "<<label->width()<<"-"<<label->height();

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
//        QMessageBox mb;
//        mb.setText("No search result");
//        mb.setIcon(QMessageBox::Information);
//        mb.setDefaultButton(QMessageBox::Ok);
//        mb.exec();
    }
}
