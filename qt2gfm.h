#ifndef QT2GFM_H
#define QT2GFM_H

#include <QList>
#include <QSet>
#include <QPixmap>
#include <QMimeDatabase>

struct frame {
    QPixmap frame ;
    u_int32_t transparent ;
    u_int32_t originalNbColors ;
    u_int32_t nbColors ;
    QString nfo ;
} ;

union qualityBits {
    u_int32_t rgb ;
    struct {
        u_int8_t r ;
        u_int8_t g ;
        u_int8_t b ;
        u_int8_t junk ;
    };
};

class qt2gfm
{
private:
    QPixmap original ;
    QList<struct frame> frames ;

    void deleteFrames( void );
    void guessFrameNb( void );
    void clearFrames( void ) ;
    u_int32_t applyTransparency( QPixmap &p );
    void applyDeep( struct frame * f );
    void generateNfo( void );

    int width, height ; // original picture size
    bool vertical ;
    int frameWidth, frameHeight, frameNb = 0 ;
    int transparency ; /* 0:no, 1:upleft, 2:downleft, 3:pink, 4:white, 5:black */
    int deep ; /* 4 / 8 / 16 / 32 */
    QString folder, name ;
    QMimeDatabase mimedb ;

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
    void setDeep( int d, bool update = false ) ;
    struct frame * getFrame( int n ) ;
    QPixmap * getFrameset( void ) ;
    int toHeader( void ) ;
    void log(void);
    QString * getNfo( int frame );
};

#endif // QT2GFM_H
