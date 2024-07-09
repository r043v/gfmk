#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

#include "./qt2gfm.h"

namespace Ui {
class previewWindow;
}

class previewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit previewWindow(QWidget *parent = nullptr);
    ~previewWindow();
    void updateDisplay( int value );
    void setFrameSet( qt2gfm * ) ;

private slots:
    void on_frameSelector_valueChanged(int value);

private:
    Ui::previewWindow *ui;
    qt2gfm * frameSet ;
    QGraphicsScene framePreview ;
    void resizeEvent(QResizeEvent*);
};

#endif // PREVIEWWINDOW_H
