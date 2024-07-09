#include "previewwindow.h"
#include "ui_previewwindow.h"

previewWindow::previewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::previewWindow)
{
    ui->setupUi(this);

    //setStyleSheet("background:transparent;");
    setStyleSheet("background:#22222222;");

    setAttribute(Qt::WA_TranslucentBackground);

    ui->frameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->frameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->frameView->setScene(&framePreview);
}

previewWindow::~previewWindow()
{
    delete ui;
}

void previewWindow::setFrameSet( qt2gfm * set ){
    frameSet = set ;
    set->log();
    ui->frameSelector->setMaximum( set->frmn ) ;
    int current = ui->frameSelector->value() ;
    if( current > set->frmn ) current = 0 ;
    ui->frameSelector->setValue(current) ; // full set

    updateDisplay( current );
}

void previewWindow::updateDisplay( int value ){
/*
    ui->frameView->setFixedHeight(320);
    ui->frameView->setFixedWidth(320);
*/

    const QPixmap& frame = frameSet->getFrame(value) ;//value ? frameSet->getFrame(value) : frameSet->img ;

    QRect frmRect( 0, 0, frame.width(), frame.height() ) ;

    ui->frameView->setSceneRect( frmRect );

    framePreview.clear();
    framePreview.addPixmap( frame );
    //framePreview.setBackgroundBrush(QBrush(QColor(255,0,255,127)));

    ui->frameView->setScene(&framePreview);

    ui->frameView->fitInView( frmRect, Qt::KeepAspectRatio ) ;
    this->setWindowTitle( *frameSet->getNfo(value) ) ;
}

void previewWindow::on_frameSelector_valueChanged(int value){
    updateDisplay( value );
}

void previewWindow::resizeEvent(QResizeEvent*){
    updateDisplay( ui->frameSelector->value() );
}

