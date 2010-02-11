/*******************************************************************************

                              moOpenCV.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
  *                                                                          *
  *   This code is distributed in the hope that it will be useful, but       *
  *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
  *   General Public License for more details.                               *
  *                                                                          *
  *   A copy of the GNU General Public License is available on the World     *
  *   Wide Web at <http://www.gnu.org/copyleft/gpl.html>. You can also       *
  *   obtain it by writing to the Free Software Foundation,                  *
  *   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.         *
  *                                                                          *
  ****************************************************************************

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa

*******************************************************************************/

#include "moOpenCV.h"

/*********************

  NOTAS:

  el tracker podrá funcionar como un thread que vaya calculando en funcion de q va llegando la info,
  o mejor aun, que trate de calcular, y cuando llega a un resultado el efecto en cuestion tome ese valor.

  //para que el tracker funcione sin shaders, debemos hacer el calculo antes de que se pase la informacion a la textura,
  para ello quizas no sea conveniente trabajar con threads, sino bloquear la ejecucion hasta terminar de tomar los valores q nos interesan.




***********************/


//========================
//  Factory
//========================

moOpenCVFactory *m_OpenCVFactory = NULL;

MO_PLG_API moResourceFactory* CreateResourceFactory(){
	if (m_OpenCVFactory==NULL)
		m_OpenCVFactory = new moOpenCVFactory();
	return (moResourceFactory*) m_OpenCVFactory;
}

MO_PLG_API void DestroyResourceFactory(){
	delete m_OpenCVFactory;
	m_OpenCVFactory = NULL;
}

moResource*  moOpenCVFactory::Create() {
	return new moOpenCV();
}

void moOpenCVFactory::Destroy(moResource* fx) {
	delete fx;
}



//===========================================
//
//     Class: moOpenCV
//
//===========================================


moOpenCV::moOpenCV()
{
	SetName("opencv");

}

moOpenCV::~moOpenCV()
{
    Finish();
}

moConfigDefinition * moOpenCV::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("threshold"), MO_PARAM_NUMERIC, OPENCV_THRESHOLD, moValue( "50", "INT").Ref() );
	p_configdefinition->Add( moText("threshold_max"), MO_PARAM_NUMERIC, OPENCV_THRESHOLD_MAX, moValue( "255", "INT").Ref() );

	return p_configdefinition;

}

MOboolean moOpenCV::Init()
{

    moText configname;
	MOint nvalues;
	MOint trackersystems;

	configname = m_pResourceManager->GetDataMan()->GetDataPath();
	configname +=  moSlash + (moText)GetConfigName();
    configname +=  moText(".cfg");

	if (m_Config.LoadConfig(configname) != MO_CONFIG_OK ) {
		moText text = moText("Couldn't load opencv config:") + (moText)configname;
		MODebug2->Error( text );
		return false;
	}

	moMoldeoObject::Init();


	moDefineParamIndex( OPENCV_THRESHOLD, moText("threshold") );
	moDefineParamIndex( OPENCV_THRESHOLD_MAX, moText("threshold_max") );


	img = cvCreateImage( cvSize(w,w), 8, 1 );
	cvZero( img );

    levels = 0;
    threshold = 150;
    threshold_max = 3;
    contours = NULL;

    storage = cvCreateMemStorage(0);


    idopencvout = -1;


    //contours = cvApproxPoly( contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1 );

	return true;

}

MOswitch moOpenCV::SetStatus(MOdevcode devcode, MOswitch state)
{
    return true;
}

void moOpenCV::SetValue( MOdevcode cd, MOfloat vl ) {

}

void moOpenCV::SetValue( MOdevcode cd, MOint vl ) {

}


//0: false    1  -1 :true
MOswitch moOpenCV::GetStatus(MOdevcode devcode)
{

	return 0;
}

MOint moOpenCV::GetValue(MOdevcode devcode)
{


    return(-1);
}

MOpointer moOpenCV::GetPointer(MOdevcode devcode ) {

	return (NULL);

}

//return devicecode index of corresponding code string
MOdevcode moOpenCV::GetCode(moText strcod)
{

    return(-1);
}

//esto se ejecuta por cada CICLO PRINCIPAL DE moConsole(frame)
//por ahora no nos sirve de nada, porque no mandamos events....ni procesamos events...
//al menos que Ligia...o algun device especial(nada mas el hecho de que se haya
//enchufado la camara por ejemplo
//podriamos poner una funcion aqui de reconocimiento de DV....
void moOpenCV::Update(moEventList *Events)
{
	//get the pointer from the Moldeo Object sending it...

	moBucket* pBucket = NULL;
	moVideoSample* pSample = NULL;

    ///Always delete
	moEvent *actual,*tmp;


	actual = Events->First;
	while(actual!=NULL) {
		tmp = actual->next;
		if (actual->deviceid == GetId() && actual->reservedvalue3!=MO_MESSAGE) {
			Events->Delete(actual);
		}
		actual = tmp;
	}


    threshold = m_Config[moR(OPENCV_THRESHOLD)][MO_SELECTED][0].Int();
    //threshold_max = m_Config[moR(OPENCV_THRESHOLD_MAX)][MO_SELECTED][0].Int();

    MODebug2->Push( moText("threshold") + IntToStr(threshold)  );
    //MODebug2->Push( moText("threshold_max") + IntToStr(threshold_max)  );

    int test = threshold + 10;

    MODebug2->Push( moText("test") + IntToStr(test)  );

	actual = Events->First;
	//Buscamos los eventos del VideoManager...samples... obsoleto, seran datos de Outlets e Inlets...
	//Outlet de videomanager LIVEOUT0 a inlet de tracker
	///Updating OpenCV Outlets
	while(actual!=NULL) {
		//solo nos interesan los del VideoManager por ahora
		if(actual->deviceid == m_pResourceManager->GetVideoMan()->GetId() ) {

			pSample = (moVideoSample*)actual->pointer;

			pBucket = (moBucket*)pSample->m_pSampleBuffer;

			//atencion! el devicecode corresponde al  iesimo dispositivo de captura...
			if (  pSample && pBucket ) {

                int m_sizebuffer = pBucket->GetSize();
                MOubyte * m_buffer = pBucket->GetBuffer();

			    //(MOpointer)pTS->GetData();

			    if (img) {
                    cvReleaseImage( &img );
                }

			    img = cvCreateImage( cvSize( pSample->m_VideoFormat.m_Width, pSample->m_VideoFormat.m_Height), IPL_DEPTH_8U, 3 );

			    //img = cvCreateImageHeader( cvSize( pSample->m_VideoFormat.m_Width, pSample->m_VideoFormat.m_Height), IPL_DEPTH_8U, 3 );
			    //cvInitImageHeader( img, cvSize( pSample->m_VideoFormat.m_Width, pSample->m_VideoFormat.m_Height), IPL_DEPTH_8U, 3, IPL_ORIGIN_TL);

                cvZero( img );
                ///cvSet( img, cvScalar( 255, 0, 0 ),0);
                //img->imageData = (char*)pSample->m_pSampleBuffer;
/*
                MODebug2->Push(moText("moOpenCV::Update:: videosample buffersize") + IntToStr(pSample->m_VideoFormat.m_BufferSize));
                int cc = 0;
                uchar * ddata = (uchar *)img->imageData;
                uchar * mdata = (uchar *) m_buffer;
                for( int jj=0; jj<pSample->m_VideoFormat.m_Height; jj++) {
                    for( int ii=0; ii<pSample->m_VideoFormat.m_Width; ii++) {
                        //cvSet2D( img, ii, jj, cvScalar(mdata[cc],mdata[cc+1],mdata[cc+2]) );
                        //cvSet2D( img, ii, jj, cvScalar(0,255,255) );
                        ddata[cc] = mdata[cc];
                        ddata[cc+1] = mdata[cc+1];
                        mdata[cc+2] = mdata[cc+2];
                        cc+=3;
                    }
                }
                */
                cvSetData( img, (void*)m_buffer, pSample->m_VideoFormat.m_Width*3);
//                (bh.biWdith*3 + 3) & -4

                if (idopencvout==-1) {
                    ///create texture in moldeo to see it
                    moTexParam tparam = MODefTex2DParams;
                    tparam.internal_format = GL_RGB;
                    CvSize size = cvGetSize(img);
                    idopencvout = m_pResourceManager->GetTextureMan()->AddTexture( moText("opencvout"), size.width, size.height, tparam );
                } else {

                    IplImage* img_gray = cvCreateImage( cvGetSize(img), 8, 1 );
                    cvCvtColor( img, img_gray, CV_BGR2GRAY );
                    //cvSobel( img_gray, img_gray, 1, 1, 3);
                    double highthresh;
                    highthresh = threshold;
                    int aperturesize;
                    aperturesize = (int)threshold_max;

                    //aperturesize = threshold * threshold_max;
                    double t = 150;

                    t = (double) threshold;

                    cvCanny( img_gray, img_gray, 0, t, 3 );
                    //cvDilate( img_gray, img_gray, 0, 1 );
                    //cvFindContours( img_gray, storage, &contours, sizeof(CvContour),CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
                    //cvDrawContours( img_gray, contours, CV_RGB(255,0,0), CV_RGB(0,255,0), 0, 3, CV_AA, cvPoint(0,0) );

                    cvCvtColor( img_gray, img, CV_GRAY2BGR );



                    ///FINDIG SQUARES!!!
                    //CvSeq* polys = findSquares4( img, storage );
                    //drawSquares( img, polys );
                    ///END

                    /*
                    IplImage* img_gray = cvCreateImage( cvGetSize(img), 8, 1 );
                    cvCvtColor( img, img_gray, CV_BGR2GRAY );
                    //cvThreshold( img_gray, img_gray, 100, 255, CV_THRESH_BINARY );
                    cvCvtColor( img_gray, img, CV_BGR2GRAY );
*/
                    // object that will contain blobs of inputImage
                    /* // BlobsLib 0.8
                        CBlobResult blobs;
                        IplImage* img_gray = cvCreateImage( cvGetSize(img), 8, 1 );
                        cvCvtColor( img, img_gray, CV_BGR2GRAY );

                        //IplImage *imgThr;
                        //cvThreshold( img_gray, imgThr, 100, 255, CV_THRESH_BINARY );

                        // Extract the blobs using a threshold of 100 in the image
                        blobs = CBlobResult( img_gray, NULL, 100 );

                        // create a file with some of the extracted features
                        //blobs.PrintBlobs( "c:\\tmp\\blobs.txt" );

                        // discard the blobs with less area than 5000 pixels
                        // ( the criteria to filter can be any class derived from COperadorBlob )
                        blobs.Filter( blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, 100 );
                        // create a file with filtered results
                        //blobs.PrintBlobs( "c:\\tmp\\filteredBlobs.txt" );

                        // object with the blob with most perimeter in the image
                        // ( the criteria to select can be any class derived from COperadorBlob )
                        CBlob blobWithBiggestPerimeter;
                        CBlob blobWithLessArea;

                        // from the filtered blobs, get the blob with biggest perimeter
                        blobs.GetNthBlob( CBlobGetPerimeter(), 0, blobWithBiggestPerimeter );
                        // get the blob with less area
                        blobs.GetNthBlob( CBlobGetArea(), blobs.GetNumBlobs() - 1, blobWithLessArea );

                        CBlob* currentBlob;

                        for (int i = 0; i < blobs.GetNumBlobs(); i++ )
                        {
                                currentBlob = blobs.GetBlob(i);
                                currentBlob->FillBlob( img, cvScalar(255,0,0));
                        }
                        */




                        /// cvblob
                        /*

                        CvSize size = cvGetSize(img);

                        cvThreshold(img, img, 100, 200, CV_THRESH_BINARY);

                        cvSetImageROI(img, cvRect(0, 0, size.width, size.height));

                        IplImage *chB=cvCreateImage(cvGetSize(img),8,1);

                        cvSplit(img,chB,NULL,NULL,NULL);

                        IplImage *labelImg = cvCreateImage(cvGetSize(chB),IPL_DEPTH_LABEL,1);

                        CvBlobs blobs;
                        unsigned int result = cvLabel(chB, labelImg, blobs);

                        cvRenderBlobs( labelImg, blobs, img, img);
                        */

                        /*

                        for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
                          {
                            CvContourChainCode *contour = cvGetContour(it->second, labelImg);
                            cvRenderContourChainCode(contour, img);

                            CvContourPolygon *polygon = cvConvertChainCodesToPolygon(contour);

                            CvContourPolygon *sPolygon = cvSimplifyPolygon(polygon, 10.);
                            CvContourPolygon *cPolygon = cvPolygonContourConvexHull(sPolygon);

                            cvRenderContourPolygon(sPolygon, img, CV_RGB(0, 0, 255));
                            cvRenderContourPolygon(cPolygon, img, CV_RGB(0, 255, 0));

                            delete cPolygon;
                            delete sPolygon;
                            delete polygon;
                            delete contour;
                          }
*/





/*
                    IplImage *imgThr;
                    CBlob* currentBlob;
                    int param2 = 20;
                    CBlobResult blobs;
                    cvThreshold( img, imgThr, 100, 255, CV_THRESH_BINARY );
                    // find non-white blobs in thresholded image
                    blobs = CBlobResult( imgThr, NULL, 255 );
                    // exclude the ones smaller than param2 value
                    blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, param2 );
                    // get mean gray color of biggest blob
                    CBlob biggestBlob;
                    CBlobGetMean getMeanColor( img );
                    double meanGray;

                    blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );
                    meanGray = getMeanColor( biggestBlob );

                    // display filtered blobs
                    cvMerge( imgThr, imgThr, imgThr, NULL, img );

                    for (int i = 0; i < blobs.GetNumBlobs(); i++ )
                    {
                            currentBlob = blobs.GetBlob(i);
                            currentBlob->FillBlob( img, CV_RGB(255,0,0));
                    }
                    //FIN BLOB
                    */

                    /*

                    ///Convert to gray
                    IplImage* img_gray = cvCreateImage( cvGetSize(img), 8, 1 );
                    cvCvtColor( img, img_gray, CV_BGR2GRAY );

                    //IplImage* img_gray_dst = cvCreateImage( cvGetSize(img_gray), 8, 1 );
                    IplImage* img_gray_dst = cvCloneImage( img_gray );
                    //cvEqualizeHist( img_gray, img_gray_dst );
                    //cvThreshold( img_gray, img_gray_dst, 128, 255, CV_THRESH_BINARY );

                    //cvFindContours( img_gray_dst, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );




                    ///convert the gray to rgb gray

                    IplImage* img_grayrgb = cvCreateImage( cvGetSize(img_gray_dst), 8, 3 );
                    cvCvtColor( img_gray_dst, img_grayrgb, CV_GRAY2BGR );

                    //cvDrawContours( img_grayrgb, contours, CV_RGB(255,0,0), CV_RGB(0,255,0), 0, 3, CV_AA, cvPoint(0,0) );
                    */

                    ///now set the moldeo texture buffer with the data
                    void* data;
                    int step;
                    CvSize size2;


                    //cvGetRawData( img_grayrgb, (uchar**)&data, &step, &size );
                    cvGetRawData( img, (uchar**)&data, &step, &size2 );
                    //cvGetRawData( img, (uchar**)&data, &step, &size );

                    //MODebug2->Push(moText("moOpenCV::Update:: step from img:") + IntToStr(step));


                    ///get the moldeo texture and set it
                    moTexture* pTex;
                    pTex = m_pResourceManager->GetTextureMan()->GetTexture(idopencvout);

                    pTex->SetBuffer( data, GL_BGR, GL_UNSIGNED_BYTE );
                }

			    //

                /*
			    MODebug2->Push(moText("moOpenCV::Update "));
			    MODebug2->Push(
                                moText(" width: ")
                                + IntToStr(pSample->m_VideoFormat.m_Width)
                                + moText(" height: ")
                                + IntToStr(pSample->m_VideoFormat.m_Height)
                            );
                */
			    //cvFindContours( img, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
			    //if (contours->




                //m_Outlets[actual->devicecode]->GetData()->SetPointer( NULL, sizeof(moTrackerSystemData) );
                //m_Outlets[actual->devicecode]->Update();

			}
			tmp = actual->next;
			actual = tmp;
		} else actual = actual->next;//no es el que necesitamos...
	}


	moMoldeoObject::Update(Events);
}


MOboolean moOpenCV::Finish()
{

	return true;
}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 )
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

CvSeq* moOpenCV::findSquares4( IplImage* img, CvMemStorage* storage )
{
    CvSeq* contours;
    int thresh = 50;
    int i, c, l, N = 1;
    CvSize sz = cvSize( img->width & -2, img->height & -2 );
    //IplImage* timg = cvCloneImage( img ); // make a copy of input image
    IplImage* timg = img;
    IplImage* gray = cvCreateImage( sz, 8, 1 );
    IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 1 );
    IplImage* tgray;
    CvSeq* result;
    double s, t;
    // create empty sequence that will contain points -
    // 4 points per square (the square's vertices)
    CvSeq* squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );

    // select the maximum ROI in the image
    // with the width and height divisible by 2
    cvSetImageROI( timg, cvRect( 0, 0, sz.width, sz.height ));

    // down-scale and upscale the image to filter out the noise

    tgray = cvCreateImage( sz, 8, 1 );
    cvCvtColor( timg, tgray, CV_BGR2GRAY );

    cvPyrDown( tgray, pyr, CV_GAUSSIAN_5x5 );
    cvPyrUp( pyr, tgray, CV_GAUSSIAN_5x5 );

    // find squares in every color plane of the image
    //for( c = 0; c < 3; c++ )
    //{
        // extract the c-th color plane
        //cvSetImageCOI( timg, c+1 );
        //cvCopy( timg, tgray, 0 );

        // try several threshold levels
        for( l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cvCanny( tgray, gray, 0, thresh, 5 );
                // dilate canny output to remove potential
                // holes between edge segments
                cvDilate( gray, gray, 0, 1 );
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                cvThreshold( tgray, gray, (l+1)*255/N, 255, CV_THRESH_BINARY );
            }

            // find contours and store them all as a list
            cvFindContours( gray, storage, &contours, sizeof(CvContour),
                CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

            // test each contour
            while( contours )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                result = cvApproxPoly( contours, sizeof(CvContour), storage,
                    CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( result->total == 4 &&
                    fabs(cvContourArea(result,CV_WHOLE_SEQ)) > 100 &&
                    cvCheckContourConvexity(result) )
                {
                    s = 0;

                    for( i = 0; i < 5; i++ )
                    {
                        // find minimum angle between joint
                        // edges (maximum of cosine)
                        if( i >= 2 )
                        {
                            t = fabs(angle(
                            (CvPoint*)cvGetSeqElem( result, i ),
                            (CvPoint*)cvGetSeqElem( result, i-2 ),
                            (CvPoint*)cvGetSeqElem( result, i-1 )));
                            s = s > t ? s : t;
                        }
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( s < 0.3 )
                        for( i = 0; i < 4; i++ )
                            cvSeqPush( squares,
                                (CvPoint*)cvGetSeqElem( result, i ));
                }

                // take the next contour
                contours = contours->h_next;
            }
        }
    //}

    // release all the temporary images
    cvReleaseImage( &gray );
    //cvReleaseImage( &pyr );
    //cvReleaseImage( &tgray );
    //cvReleaseImage( &timg );

    return squares;
}


void moOpenCV::drawSquares( IplImage* cpy, CvSeq* squares )
{
    CvSeqReader reader;

    int i;

    // initialize reader of the sequence
    cvStartReadSeq( squares, &reader, 0 );

    // read 4 sequence elements at a time (all vertices of a square)
    for( i = 0; i < squares->total; i += 4 )
    {
        CvPoint pt[4], *rect = pt;
        int count = 4;

        // read 4 vertices
        CV_READ_SEQ_ELEM( pt[0], reader );
        CV_READ_SEQ_ELEM( pt[1], reader );
        CV_READ_SEQ_ELEM( pt[2], reader );
        CV_READ_SEQ_ELEM( pt[3], reader );

        // draw the square as a closed polyline
        cvPolyLine( cpy, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );
    }

}

//===========================================
//
//     Class: moOpenCVSystem
//
//===========================================



moOpenCVSystem::moOpenCVSystem() {

}

moOpenCVSystem::~moOpenCVSystem() {

}
