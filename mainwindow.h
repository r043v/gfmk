#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QList>
#include <QPixmap>

#include <QWindow>

//#include <QMimeDatabase>
#include <QGraphicsScene>
#include <QCloseEvent>

#include "./qt2gfm.h"

#include "./previewwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    const bool& previewShowed = previewShow ;
    void showPreview( void ) ;
    void hidePreview( void ) ;
    void togglePreview( void ) ;

private slots:
    void on_convertBtn_clicked();
    void on_closeBtn_clicked();
    void on_transparentOpt_currentIndexChanged(int index);
    void on_frameNbInput_valueChanged(int v);
    void on_formatOpt_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QGraphicsScene preview ;
    qt2gfm frameSet;
    previewWindow previewWnd ;
    bool previewShow ;

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent* event);
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);
    void updateFrameSizeLabel( void );
};
#endif // MAINWINDOW_H
