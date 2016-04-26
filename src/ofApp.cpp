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
    camera.set1394b(true);
    camera.setFormat7(true);
    camera.setBayerMode(DC1394_COLOR_FILTER_RGGB, DC1394_BAYER_METHOD_NEAREST);
    camera.setup();
    
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
    rawImage.allocate(camera.getWidth(), camera.getHeight());
    colorImage.allocate(camera.getWidth(), camera.getHeight());
    processed.allocate(camera.getWidth(), camera.getHeight(), OF_IMAGE_COLOR);
}

void ofApp::update() {
    // grabVideo() will place the most recent frame into curFrame. If it's a new
    // frame, grabFrame returns true. If there are multiple frames available, it
    // will drop old frames and only give you the newest. This guarantees the
    // lowest latency. If you prefer to not drop frames, set the second argument
    // (dropFrames) to false. By default, capture is non-blocking.
    auto v = camera.grabVideo(curFrame);
    
    if(v) {
        curFrame.update();
        rawImage.setFromPixels(curFrame.getPixels().getData(), camera.getWidth(), camera.getHeight());
        
        
        
        
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
    } else {
        
    }
}

void ofApp::draw() {
    // If the camera isn't ready, the curFrame will be empty.
    if(camera.isReady()) {
        // Camera doesn't draw itself, curFrame does.
        curFrame.draw(0, 0);
        rawImage.draw(640,0);
        rgb.draw(1280, 0);
    }
    
    ofDrawBitmapStringHighlight("use the up/down keys: " + bayerName, 10, 20);
}

void ofApp::keyPressed(int key){
    if(key == OF_KEY_UP) {
        bayerType--;
    }
    if(key == OF_KEY_DOWN) {
        bayerType++;
    }
    bayerType = ofClamp(bayerType, 0, 3);
}
