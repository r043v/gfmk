#ifndef QT2GFM_H
#define QT2GFM_H

#include <QList>
#include <QPixmap>
#include <QMimeDatabase>

class qt2gfm
{
private:
    QPixmap original ;
    QPixmap fullSet ;
    QList<QPixmap> frames ;
    void deleteFrames( void );
    QMimeDatabase mimedb ;
    void guessFrameNb( void );
    void clearFrames( void ) ;
    void applyTransparency( QPixmap &p );
    void generateNfo( void );

    int width, height ; // original picture size
    bool vertical ;
    int frameWidth, frameHeight, frameNb = 0 ;
    int transparency ; /* 0:no, 1:upleft, 2:downleft, 3:pink, 4:white, 5:black */
    QString folder, name ;
    QList<QString> nfo ;

public:
    const int& imgw = width;
    const int& imgh = height;
    const int& frmw = frameWidth;
    const int& frmh = frameHeight;
    const int& frmn = frameNb;
    const bool& imgv = vertical;
    const QPixmap& img = original;

    qt2gfm();
    //~qt2gfm();
    int load( QString fileName ) ;
    void generateFrames( void );
    void forceFrameNb( int nb ) ;
    void forceWay( bool vertical ) ;
    void setTransparency( int t ) ;
    QPixmap & getFrame( int n ) ;
    QPixmap * getFrameset( void ) ;
    int toHeader( void ) ;
    void log(void);
    QString * getNfo( int frame );
};

#endif // QT2GFM_H


