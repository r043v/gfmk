#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    setFixedHeight( 200 ) ; // hide our controls until a drop

    ui->setupUi(this);

    setAcceptDrops(true);

    setStyleSheet("background:#aaffffff;");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);

    //ui->graphicsView->setGeometry(4,4,192,192); // to auto center the picture ..

    ui->graphicsView->installEventFilter( this ) ;
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setScene(&preview);
    ui->graphicsView->setAlignment( Qt::AlignVCenter ) ;

    //QPixmap defaultImg(":/data/drop192.png") ;
    preview.addPixmap( QPixmap(":/data/drop192.png") ) ;

    ui->graphicsView->fitInView(QRect(0,0,192,192),Qt::KeepAspectRatio);

    previewShow = false ;

    ui->framesNfo->setStyleSheet("background:none;");

    //printf("window of %ix%i\n",width(),height());
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e){
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e){

    QList filelist = e->mimeData()->urls() ; // dropped files list
    int nb = filelist.length() ;
    if( !nb || nb > 1 ) return ; // only one file at a time please

    //MainWindow * t = this ;

    QString fileName = filelist[0].toLocalFile();

    if( !frameSet.load( fileName ) ) return ; // load picture

    preview.clear();
    preview.addPixmap( frameSet.img );

    QRect frmRect(0,0,frameSet.imgw,frameSet.imgh) ;

    ui->graphicsView->setSceneRect( frmRect );
    ui->graphicsView->fitInView(frmRect,Qt::KeepAspectRatio);

    printf("vertical ? %i\n", frameSet.imgv);

    ui->horizontalCheck->setChecked( !frameSet.imgv ) ; // is horizontal ?
    ui->frameNbInput->setValue( frameSet.frmn ) ;

    ui->framesNfo->setText( QString("frame%1 of %2x%3").arg( frameSet.frmn > 1 ? "s" : "" ).arg( frameSet.frmw ).arg( frameSet.frmh ) ) ;

    // extend our main window with controls
    setFixedHeight( 260 ) ;

    // open preview window
    showPreview();
    showPreview(); // to do, drop me

    qApp->processEvents(); // force refresh ?
}

void MainWindow::showPreview( void ){
    if( !frameSet.frmn ) return ; // show only if it's loaded
    previewShow = true ;

    previewWnd.setFrameSet( &frameSet ) ;
    previewWnd.show();
}

void MainWindow::hidePreview( void ){
    previewShow = false ;
    previewWnd.hide();
}

void MainWindow::togglePreview( void ){
    if( previewShow ) hidePreview(); else showPreview() ;
}

void MainWindow::on_convertBtn_clicked(){

}

void MainWindow::on_closeBtn_clicked(){
    qApp->closeAllWindows();
}

// event filter to catch click on graphics view ..
bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    if( obj == ui->graphicsView
    &&  event->type() == QEvent::MouseButtonPress
    ){ // click on graphic view!
        togglePreview();
        windowHandle()->startSystemMove();
        return true ; // stop propagation
    }

    return false ; // not our event ..
}

void MainWindow::closeEvent(QCloseEvent *event){
    previewWnd.close();
}

void MainWindow::on_transparentOpt_currentIndexChanged(int index){
    frameSet.setTransparency( index ) ;
    showPreview();
}


void MainWindow::on_frameNbInput_valueChanged(int v){
    frameSet.forceFrameNb( v );
    showPreview();
    //previewWnd.updateDisplay( 0 );
}

