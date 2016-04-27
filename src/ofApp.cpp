#include "ofApp.h"

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // If you want to set any non-default parameters like size, format7, blocking
    // capture, etc., you can do it here before setup. They'll be applied to the
    // camera during setup().
    
    // setup() will pick the first camera. Or if there are multiple cameras you
    // can use the number, or the GUID of the camera.
    
    //camera.resetBus();
    /*width = 1600;
    height = 1200;*/
    
    //width = 640/2.0; // 1624
    //height = 480/2.0; // 1224
    //width = 1624/640;
    //height = 1224/480;
    raw_width = 1624;
    raw_height = 1224;
    width = 1600;
    height = 1200;
    //camera.resetBus();
    camera.set1394b(true);
    //camera.setSize(raw_width,raw_height);
    //camera.setSize(1600,1200);
    
    camera.setFormat7(true);
    //camera.setSize(raw_width,raw_height);
    
    camera.setBayerMode(DC1394_COLOR_FILTER_RGGB, DC1394_BAYER_METHOD_NEAREST);
    //camera.setImageType(OF_IMAGE_COLOR);
    camera.setImageType(OF_IMAGE_GRAYSCALE);
    camera.setup();
    
    
    /*
     typedef enum {
     DC1394_BAYER_METHOD_NEAREST=0,
     DC1394_BAYER_METHOD_SIMPLE,
     DC1394_BAYER_METHOD_BILINEAR,
     DC1394_BAYER_METHOD_HQLINEAR,
     DC1394_BAYER_METHOD_DOWNSAMPLE,
     DC1394_BAYER_METHOD_EDGESENSE,
     DC1394_BAYER_METHOD_VNG,
     DC1394_BAYER_METHOD_AHD
     } dc1394bayer_method_t;
     
    
    /*
     typedef enum {
     DC1394_COLOR_FILTER_RGGB = 512,
     DC1394_COLOR_FILTER_GBRG,
     DC1394_COLOR_FILTER_GRBG,
     DC1394_COLOR_FILTER_BGGR
     }
     */
    
    //camera.disableBayer();
    // After setup it's still possible to change a lot of parameters. If you want
    // to change a pre-setup parameter, the camera will auto-restart
    
    // Set the window size to the camera size.
    //ofSetWindowShape(camera.getWidth(), camera.getHeight());
    /*rawImage.allocate(camera.getWidth(), camera.getHeight());
    colorImage.allocate(camera.getWidth(), camera.getHeight());
    processed.allocate(camera.getWidth(), camera.getHeight(), OF_IMAGE_COLOR);*/
    
    //ofSetWindowShape(width*0.5, height*0.5);
    //ofSetWindowShape(640,480);
    ofSetWindowShape(1624, 1224);
    rawImage.allocate(width, height);
    colorImage.allocate(width, height);
    processed.allocate(width, height, OF_IMAGE_COLOR);
}

void ofApp::update() {
    // grabVideo() will place the most recent frame into curFrame. If it's a new
    // frame, grabFrame returns true. If there are multiple frames available, it
    // will drop old frames and only give you the newest. This guarantees the
    // lowest latency. If you prefer to not drop frames, set the second argument
    // (dropFrames) to false. By default, capture is non-blocking.
    // your buffer (in this case, an OF image) needs to be grayscale
    // assumes it's up in yourApp.h
    if (buffer.getImageType() != OF_IMAGE_GRAYSCALE ){
        //buffer.allocate(raw_width, raw_height, OF_IMAGE_GRAYSCALE);
        buffer.allocate(mouseX, mouseY, OF_IMAGE_GRAYSCALE);
        //cout << mouseX << "," << mouseY << endl;
        camera.setImageType(OF_IMAGE_GRAYSCALE);
    }
    
    //buffer.allocate(mouseX, mouseY, OF_IMAGE_GRAYSCALE);
    
    switch(bufMode){
        case 0:
            bufX = 1600;
            bufY = 1200;
            break;
        case 1:
            bufX = 1624;
            bufY = 1224;
            break;
        case 2:
        case 3:
            bufX = 320;
            bufY = 240;
            break;
        case 4:
            bufX = 640;
            bufY = 480;
            break;
    }
    
    buffer.allocate( bufX, bufY, OF_IMAGE_GRAYSCALE );
    
    bool remaining;
    int i = 0;
    auto * c = camera.getLibdcCamera();
    
    // this is a bit sketchy; it assumes you're in FMT 7 mode 0
    dc1394video_mode_t vm = (dc1394video_mode_t) ((int) DC1394_VIDEO_MODE_FORMAT7_0);
    auto capturePolicy = DC1394_CAPTURE_POLICY_POLL; //non-blocking
    
    // possibly not necessary, but ensure you're getting raw data
    dc1394_format7_set_color_coding(c, vm, DC1394_COLOR_CODING_RAW8);
    
    // start transmit
    dc1394switch_t cur, target;
    dc1394_video_get_transmission(c, &cur);
    target = DC1394_ON;
    if(cur != target){
        dc1394_video_set_transmission(c, target);
    }
    
    do {
        dc1394video_frame_t *frame;
        dc1394error_t err = dc1394_capture_dequeue(c, capturePolicy, &frame);
        
        if(frame != NULL) {
            unsigned char* src = frame->image;
            unsigned char* dst = buffer.getPixels().getData();
            auto width = buffer.getWidth();
            auto height = buffer.getHeight();
            
            memcpy(dst, src, width * height);
            
            dc1394_capture_enqueue(c, frame);
            remaining = true;
            i++;
        } else {
            // silencio
            remaining = false;
        }
        
    } while (remaining);
    
    
    // did we capture something?
    if ( i > 0 ){
        //cout << "yay"<< endl;
        if (!cvBuffer.isAllocated())
        {
            //cvBuffer.allocate(this->width, this->height, OF_IMAGE_COLOR);
            //cvBuffer.allocate( mouseX, mouseY, OF_IMAGE_COLOR);
            cvBuffer.allocate(1624, 1224, OF_IMAGE_COLOR);
        }
        cvBuffer.allocate(bufX, bufY, OF_IMAGE_COLOR);
        //ofxCv::convertColor(buffer, cvBuffer, CV_BayerRG2BGR);
        
        int code;
        switch(bayerType) {
            case 0:
                code = CV_BayerRG2BGR;
                ofxCv::convertColor(buffer, cvBuffer, CV_BayerRG2BGR);
                //cvCvtColor(buffer, colorImage.getCvImage(), CV_BayerBG2RGB); // this seems to best match the colour mode of the camera
                bayerName = "CV_BayerRG2BGR";
                
                break;
            case 1:
                code = CV_BayerGB2RGB;
                ofxCv::convertColor(buffer, cvBuffer, CV_BayerGB2RGB);
                bayerName = "CV_BayerGB2RGB";
                break;
            case 2:
                code = CV_BayerRG2RGB;
                ofxCv::convertColor(buffer, cvBuffer, CV_BayerRG2RGB);
                bayerName = "CV_BayerRG2RGB";
                break;
            case 3:
                code = CV_BayerGR2RGB;
                ofxCv::convertColor(buffer, cvBuffer, CV_BayerGR2RGB);
                bayerName = "CV_BayerGR2RGB";
                break;
            case 4:
                code = CV_BayerBG2RGB;
                ofxCv::convertColor(buffer, cvBuffer, CV_BayerBG2RGB);
                bayerName = "CV_BayerBG2RGB";
                break;
        }
        //rawImage.setFromPixels(buffer.getPixels());
        /*rawImage.setFromPixels(buffer.getPixels().getData(), bufX, bufY);
        
        cvCvtColor(rawImage.getCvImage(), colorImage.getCvImage(), code);
        processed.setFromPixels(colorImage.getPixels());
        processed.update();
        //bayer.setFromPixels( buffer.getPixels() );
        convertColor(bayer, rgb, code);
        //rgb.update();*/
        
        cvBuffer.update();
        return;
        
        //bayer.setFromPixels(cvBuffer.getPixels());
        
        //convertColor(bayer, rgb, code);
        //rgb.update();
        //bayer.setFromPixels(cvBuffer.getPixels().getData(), bufX, bufY, OF_IMAGE_COLOR);
        //rgb.setFromPixels(cvBuffer.getPixels().getData(), bufX, bufY, OF_IMAGE_COLOR);
        //convertColor(bayer, rgb, code);
        //rgb.update();
        rawImage.setFromPixels( cvBuffer.getPixels().getData(), bufX, bufY );
        colorImage.setFromPixels( cvBuffer.getPixels().getData(), bufX, bufY );
        cvCvtColor(rawImage.getCvImage(), colorImage.getCvImage(), code);
        //colorImage.updateTexture();
        
        processed.setFromPixels(colorImage.getPixels());
        
        
        //processed.setFromPixels(rawImage.getPixels(), camera.getWidth(), camera.getHeight() );
        processed.update();
        
        //cvBuffer.update();
        //cvBuffer.setFromPixels( buffer.getPixels() );
    }
    return;
    
        curFrame.update();
    //return;
        //rawImage.setFromPixels(curFrame.getPixels().getData(), camera.getWidth(), camera.getHeight());
    //rawImage.setFromPixels( cvBuffer.getPixels().getData(), camera.getWidth(), camera.getHeight());
        
    return;
    rawImage.setFromPixels( cvBuffer.getPixels() );
    
        int code;
        switch(bayerType) {
            case 0:
                code = CV_BayerBG2RGB;
                cvCvtColor(rawImage.getCvImage(), colorImage.getCvImage(), CV_BayerBG2RGB); // this seems to best match the colour mode of the camera
                bayerName = "CV_BayerBG2RGB";
                break;
            case 1:
                code = CV_BayerGB2RGB;
                cvCvtColor(rawImage.getCvImage(), colorImage.getCvImage(), CV_BayerGB2RGB); // this seems to best match the colour mode of the camera
                bayerName = "CV_BayerGB2RGB";
                break;
            case 2:
                code = CV_BayerRG2RGB;
                cvCvtColor(rawImage.getCvImage(), colorImage.getCvImage(), CV_BayerRG2RGB); // this seems to best match the colour mode of the camera
                bayerName = "CV_BayerRG2RGB";
                break;
            case 3:
                code = CV_BayerGR2RGB;
                cvCvtColor(rawImage.getCvImage(), colorImage.getCvImage(), CV_BayerGR2RGB); // this seems to best match the colour mode of the camera
                bayerName = "CV_BayerGR2RGB";
                break;
        }
        
        //cvCvtColor(rawImage.getCvImage(), colorImage.getCvImage(), CV_BayerRG2BGR); // this seems to best match the colour mode of the camera
        
        bayer.setFromPixels(rawImage.getPixels());
        
        convertColor(bayer, rgb, code);
        rgb.update();
        
        /*
         CV_BayerBG2RGB =CV_BayerRG2BGR,
         CV_BayerGB2RGB =CV_BayerGR2BGR,
         CV_BayerRG2RGB =CV_BayerBG2BGR,
         CV_BayerGR2RGB =CV_BayerGB2BGR,
         */
        //cvCvtColor(rawImage.getCvImage(), colorImage.getCvImage(), CV_BayerGR2RGB); // this seems to best match the colour
        
        // ofxLibdc doesn't seem to capture from this camera in colour mode - so Elliot used ofxOpenCV to extract the colour data from the raw image..
        // this is a cludge to adjust the blue level...
        
        
        processed.setFromPixels(colorImage.getPixels());
        
        /*unsigned char * blue = processed.getPixels().getData() + 2;
         for (int i=0; i<processed.getWidth()*processed.getHeight(); i++) {
         *blue = pow((float) *blue / 255.0f, 0.9f)  * 255.0f;
         blue += 3;
         }*/
        
        //processed.setFromPixels(rawImage.getPixels(), camera.getWidth(), camera.getHeight() );
        processed.update();
    
}

void ofApp::draw() {
    // If the camera isn't ready, the curFrame will be empty.
    /*if(camera.isReady()) {
        // Camera doesn't draw itself, curFrame does.
        curFrame.draw(0, 0);
        cvBuffer.draw(640,0);
        
    }*/
    //cvBuffer.draw(0,0,640,480);
    //rawImage.draw(640,0);
    //rgb.draw(640, 0);
    cvBuffer.draw(0,0);
    //processed.draw(640,0);
    //rgb.draw(640,0);
    //colorImage.draw(640,0, 640,480);
    //processed.draw(640,480,640,480);
    ofDrawBitmapStringHighlight("use the up/down keys: " + bayerName, 10, 20);
    ofDrawBitmapStringHighlight("use the left/right keys: " + ofToString(bufX) + ", " + ofToString(bufY), 10, 80);
}

void ofApp::keyPressed(int key){
    if(key == OF_KEY_UP) {
        bayerType--;
    }
    if(key == OF_KEY_DOWN) {
        bayerType++;
    }
    
    if(key == OF_KEY_LEFT) {
        bufMode--;
    }
    if(key == OF_KEY_RIGHT) {
        bufMode++;
    }
    bayerType = ofClamp(bayerType, 0, 4);
        bufMode = ofClamp(bufMode, 0, 4);
}
