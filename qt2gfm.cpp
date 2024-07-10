#include "qt2gfm.h"

qt2gfm::qt2gfm(){
    width = height = frameWidth = frameHeight = frameNb ;
    transparency = 3 ; // pink
    deep = 4 ; // 32b
}

int qt2gfm::load( QString fileName ){
    if( !mimedb.mimeTypeForFile( fileName ).name().startsWith("image/") ) return 0 ; // only pictures please
    if( !original.load( fileName ) ) return 0 ; // load ok ?
    if( original.isNull() ) return 0 ; // empty picture ..

    width = original.width() ; // get sizes
    height = original.height() ;

    if( !width || !height ) return 0 ; // bad picture ?

    guessFrameNb();
    //log();

    generateFrames();

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

struct frame * qt2gfm::getFrame( int frame ){
    if( frame < 0 || frame > frameNb ) return &frames[ 0 ] ; // fullSet ..
    return &frames[ frame ] ;
}

void qt2gfm::setDeep( int deep ){ this->deep = deep ; }

void qt2gfm::applyDeep( struct frame * f ){

    QImage i = f->frame.toImage().convertToFormat( QImage::Format_RGBA8888 )  ; // force 32b

    int sy = i.height(), sx = i.width() ;

    QSet<uint32_t> ucolors32 ; // a set who contain picture colors

    // fill our set
    for( int y=0; y<sy ; y++ ){
        uint8_t * line = i.scanLine(y) ;
        uint32_t *line32 = (uint32_t*)line, *line32end = &line32[sx] ;
        while( line32 != line32end ) ucolors32.insert(*line32++) ;
    }

    u_int32_t nbColors = ucolors32.count(), maxColors ;
    bool isTransparent = ucolors32.contains( 0 ) ;

    f->nbColors = nbColors ;

    if( deep == 32 ) return ;

    if( deep == 16 ){ // 16b deep, can be converted to 565 rgb
        i = i.convertToFormat( QImage::Format_ARGB8565_Premultiplied )  ; // force 16b, 565
        f->frame.convertFromImage( i ) ;
        return ;
    }

    // if here, we need 4b or 8b color deep
    // let's count colors to see if we need reduce them

    if( deep == 4 ){ // 4b/pixel ~ 16 colors + transparent
        maxColors = 16 ;
    } else { // 8b/pixel ~ 256 colors + transparent
        maxColors = 256 ;
    }

    if( isTransparent ) maxColors++ ; // gfm format doesn't count transparent colors in our palette

    //printf("%u/%u colors, %s\n", nbColors, maxColors, isTransparent ? "transparent" : "opaque" ) ;

    if( nbColors <= maxColors ) return ; // nothing to do, wanted deep can handle our picture

    // we need reduce picture colors

    // fill frame colors info to update nfo string
    f->originalNbColors = nbColors ;
    f->nbColors = maxColors ;

    /* to do */

    return ;

    // =>> let's try a step by step channel's precision bits downgrade <<=


    // convert to 444 and retry ?
    // colors stats and get best n colors to generate a palette
    // convert all picture to best match palette color

    printf("try reduce colors ... %u on %u\n", nbColors, maxColors);

    QImage i444 = i.convertToFormat( QImage::Format_RGB444 ); // reduce precision
    QSet<uint16_t> ucolors444 ; // a set who contain picture colors

    // fill our set
    for( int y=0; y<sy ; y++ ){
        uint8_t * line = i444.scanLine(y) ;
        uint16_t *line16 = (uint16_t*)line, *line16end = &line16[sx] ;
        while( line16 != line16end ) ucolors444.insert(*line16++) ;
    }

    u_int32_t nbColors444 = ucolors444.count() ;

    printf("reduced to 12b precision ... now have %u colors .. ", nbColors444 ) ;

    if( nbColors444 <= maxColors ){ // success \o/
        f->frame.convertFromImage(i444) ;
        f->nbColors = nbColors444 ;
        printf("success!\n") ;
        return ;
    }

    printf("omg :(\n") ;

    // plan B on the way :S

}

/*

bool reduceUntilNBColors( QImage *i, u_int32_t nb ){
    QImage i32 = i->convertToFormat( QImage::Format_RGB888 );
    uint8_t quality = 24 ; // full rgb

    QSet<uint32_t> colors ; // a set who contain picture colors
    u_int32_t nbcolors = 0 ;



    do {

    } while( nbcolors > nb ) ;

    // fill our set
    for( int y=0; y<sy ; y++ ){
        uint8_t * line = i444.scanLine(y) ;
        uint16_t *line16 = (uint16_t*)line, *line16end = &line16[sx] ;
        while( line16 != line16end ) ucolors444.insert(*line16++) ;
    }

    u_int32_t nbColors444 = ucolors444.count() ;

    printf("reduced to 12b precision ... now have %u colors .. ", nbColors444 ) ;

    if( nbColors444 <= maxColors ){ // success \o/
        f->frame.convertFromImage(i444) ;
        f->nbColors = nbColors444 ;
        printf("success!\n") ;
        return ;
    }

    printf("omg :(\n") ;
}

*/

union qualityBits {
    u_int32_t rgb ;
    struct {
        u_int8_t r ;
        u_int8_t g ;
        u_int8_t b ;
        u_int8_t junk ;
    };
};

void qualityReduce( void ){
    u_int8_t q = 24 ;

    union qualityBits rgb ;
    rgb.rgb = 0 ;

    //u_int32_t rgb = 0 ;
    //u_int8_t *rgb8 = (u_int8_t*)&rgb ;
    //u_int8_t *r = rgb8, *g = &rgb8[1], *b = &rgb8[2] ;

    while(q){
        rgb.rgb = 0 ;
        printf("quality %u\n",q) ;

        u_int8_t v = q/3 ;

        //*r = v ; *g = v ; *b = v ;
        rgb.r = v ;
        rgb.g = v ;
        rgb.b = v ;

        v *= 3 ;

        //printf("step 1 r%u g%u b%u v%u\n",*r,*g,*b,v);

        if( v != q ){
            u_int8_t chn_start = 0 ; // red
            u_int32_t save_rgb = rgb.rgb ;
            u_int8_t save_v = v ;

            while( chn_start != 3 ){
                rgb.rgb = save_rgb ;
                v = save_v ;
                u_int8_t *i = &rgb.r ; // &rgb8[ chn_start ] ;
                while( v != q ){
                    *i += 1 ;
                    v++ ;
                    i++ ;
                    if( i > &rgb.b ) i = &rgb.r ;
                };
                //printf("r%u g%u b%u\n",*r,*g,*b);
                printf("r%u g%u b%u\n",rgb.r,rgb.g,rgb.b) ;
                chn_start++ ;
            }
        } else printf("r%u g%u b%u\n",rgb.r,rgb.g,rgb.b) ; //printf("r%u g%u b%u\n",*r,*g,*b);

        q--;
    }
}

u_int32_t qt2gfm::applyTransparency( QPixmap &p ){
    // convert as a qimage to force 32b & access raw data..
    QImage i = p.toImage().convertToFormat( QImage::Format_RGBA8888 )  ;

    u_int32_t pTransparent = 0 ;

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

    return pTransparent ;
}

QString * qt2gfm::getNfo( int frame ){
    if( frame < 0 || frame > frameNb ) return &frames[0].nfo ; // fullset one
    return &frames[frame].nfo ;
}

void qt2gfm::generateNfo( void ){
    // 0 is full set
    frames[0].nfo = QString("%1 frame%2 of %3x%4").arg(frameNb).arg( frameNb > 1 ? "s" : "" ).arg( frmw ).arg( frmh ) ;

    for( int n=1 ; n<=frameNb; n++ ){
        struct frame * f = &frames[n] ;
        f->nfo = QString("%1 of %2 ~ %3 colors").arg(n).arg( frameNb ).arg( f->nbColors ) ;
        if( f->originalNbColors ){
            f->nfo += QString(" on %1").arg( f->originalNbColors ) ;
        }
    }
}

void zeroFrame( struct frame * f ){ // wtf qt, let me memset what i want :(
    f->nbColors = f->originalNbColors = f->transparent = 0 ;
}

void qt2gfm::generateFrames( void ){
    clearFrames() ; // fresh list

    struct frame fullSet ;
    zeroFrame( &fullSet ) ;
    fullSet.frame = original.copy() ;
    fullSet.transparent = applyTransparency( fullSet.frame ) ;
    frames.append( fullSet ) ;

    int x = 0, y = 0 ;
    for( int n=0; n<frameNb; n++ ){
        struct frame f ;
        zeroFrame( &f ) ;

        //printf("copy frame %i %i,%i %ix%i\n", n, x, y, frameWidth, frameHeight );
        f.frame = original.copy( x, y, frameWidth, frameHeight ) ;

        f.transparent = applyTransparency( f.frame );
        applyDeep( &f ) ;

        if( vertical ) y += frameHeight ; else x += frameWidth ;
        frames.append( f ) ;
    }

    generateNfo();
    //qualityReduce();
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
