#include "qt2gfm.h"

qt2gfm::qt2gfm(){
    width = height = frameWidth = frameHeight = frameNb = 0 ;
    transparency = 3 ; // pink
}

int qt2gfm::load( QString fileName ){
    if( !mimedb.mimeTypeForFile( fileName ).name().startsWith("image/") ) return 0 ; // only pictures please
    if( !original.load( fileName ) ) return 0 ; // load ok ?
    if( original.isNull() ) return 0 ; // empty picture ..

    width = original.width() ; // get sizes
    height = original.height() ;

    if( !width || !height ) return 0 ; // bad picture ?

    guessFrameNb();
    log();

    generateFrames();

    //float r = ( (float)w ) / ( (float)h ) ;

    return 1 ; // no error
}

void qt2gfm::log(void){
    printf("%s picture of %ix%i\n%i frames of %ix%i\n", vertical ? "vertical" : "horizontal", width, height, frameNb, frameWidth, frameHeight );
}

QPixmap * qt2gfm::getFrameset( void ){
    return &original ;
}

void qt2gfm::guessFrameNb( void ){
    vertical = height > width ;

    int w, h ; // vertical or not doesn't care, force horizontal for computation
    if( vertical ){
        w = height ; h = width ;
    } else {
        h = height ; w = width ;
    }

    // size check to detect if it's a set
    if( w > 2*h ){
        int size = h + 3, nb ;
        while( 1 ){ // check for an exact match
            nb = w / size ;
            if( nb * size == w ) break ; // maybe found
            if( --size < 4 ){ size = 0 ; break ; } // not found, maybe an unique frame
        };

        if( size ){
            if( vertical ){
                frameWidth = width ;
                frameHeight = size ;
            } else {
                frameHeight = height ;
                frameWidth = size ;
            }

            frameNb = nb ;
            return ;
        }
    }

    // probably a single frame picture
    frameHeight = height ;
    frameWidth = width ;
    frameNb = 1 ;
}

void qt2gfm::clearFrames( void ){
//    qDeleteAll( frames.begin(), frames.end() );
    frames.clear();
}

QPixmap& qt2gfm::getFrame( int frame ){
    if( !frame || frame > frameNb ) return fullSet ;
    return frames[ frame - 1 ] ;
}

void qt2gfm::applyTransparency( QPixmap &p ){
    // convert as a qimage to force 32b & access raw data..
    QImage i = p.toImage() ;

    u_int8_t deep = 4 ;

    if( deep == 4 ) i = i.convertToFormat(QImage::Format_RGB444) ;

    i = i.convertToFormat( QImage::Format_RGBA8888 )  ;

    //i.convertToFormat(QImage::Format_RGB444);

    if( transparency ){
        int sy = i.height(), sx = i.width() ;
        uint8_t * line ;

        QColor transparent ;

        switch( transparency ){
        case 1 : // up left
            line = i.scanLine(0) ;
            transparent = QColor(line[2],line[1],line[0]) ;
            break;
        case 2 : //down left
            line = i.scanLine(sy - 1) ;
            transparent = QColor(line[2],line[1],line[0]) ;
            break;
        case 3 : // pink
            transparent = QColor(255,0,255) ;
            break ;
        case 4 : // white
            transparent = QColor(0,0,0) ;
            break ;
        case 5 : // black
            transparent = QColor(255,255,255) ;
            break ;
        }

        QRgb transparent32 = transparent.rgb() ;

        for( int y=0; y<sy ; y++ ){
            line = i.scanLine(y) ; // finnaly a C style anywhere here !
            uint32_t *line32 = (uint32_t*)line, *line32end = &line32[sx] ;
            while( line32 != line32end ){
                if( *line32 == transparent32 ) *line32 = 0 ;
                line32++ ;
            }
        }
    }

    p.convertFromImage( i ) ;
}

QString * qt2gfm::getNfo( int frame ){
    static QString error = QString("wtf") ;
    if( frame < 0 || frame > frameNb ) return &error ;
    return &nfo[ frame ] ;
}

void qt2gfm::generateNfo( void ){
    nfo.clear();

    // 0 is full set
    nfo.append(
        QString("%1 frame%2 of %3x%4").arg(frameNb).arg( frameNb > 1 ? "s" : "" ).arg( frmw ).arg( frmh )
    ) ;

    for( int n=0 ; n<frameNb; n++ ){
        nfo.append(
            QString("frame %1 of %2").arg(n+1).arg( frameNb )
        ) ;
    }
}

void qt2gfm::generateFrames( void ){
    clearFrames() ; // fresh list
    int x = 0, y = 0 ;
    for( int n=0; n<frameNb; n++ ){
        printf("copy frame %i %i,%i %ix%i\n", n, x, y, frameWidth, frameHeight );
        QPixmap p = original.copy( x, y, frameWidth, frameHeight ) ;

        applyTransparency( p );

        if( vertical ) y += frameHeight ; else x += frameWidth ;
        frames.append( p ) ;
    }

    fullSet = original.copy() ;
    applyTransparency( fullSet ) ;

    generateNfo();
}

void qt2gfm::setTransparency( int t ){
    transparency = t ;
    generateFrames() ;
}

void qt2gfm::forceFrameNb( int n ){
    frameNb = n ;

    if( vertical ){
        frameHeight = height / n ;
    } else {
        frameWidth = width / n ;
    }

    generateFrames() ;
}
