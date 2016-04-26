#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // If you want to set any non-default parameters like size, format7, blocking
    // capture, etc., you can do it here before setup. They'll be applied to the
    // camera during setup().
    
    // setup() will pick the first camera. Or if there are multiple cameras you
    // can use the number, or the GUID of the camera.
    camera.setup();
    camera.set1394b(true);
    
    
    camera.setExposure(0.8);
    //camera.setBayerMode(DC1394_COLOR_FILTER_BGGR);
    //camera.setFormat7(true);
    bayerType = 0;
    /*
     typedef enum {
     DC1394_COLOR_FILTER_RGGB = 512,
     DC1394_COLOR_FILTER_GBRG,
     DC1394_COLOR_FILTER_GRBG,
     DC1394_COLOR_FILTER_BGGR
     } dc1394color_filter_t;
     */
    
    
    //camera.setImageType( OF_IMAGE_COLOR );
    //camera.setBayerMode( CV_Bayer
    
    // After setup it's still possible to change a lot of parameters. If you want
    // to change a pre-setup parameter, the camera will auto-restart
    
    // Set the window size to the camera size.
    // ofSetWindowShape(camera.getWidth(), camera.getHeight());
    
    rawImage.allocate(camera.getWidth(), camera.getHeight());
    colorImage.allocate(camera.getWidth(), camera.getHeight());
    processed.allocate(camera.getWidth(), camera.getHeight(), OF_IMAGE_COLOR);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    /*if ( true ) {
        if(camera.grabVideo(curFrame)) {
            curFrame.update();
        }
    }
    return;*/
    
    
    if(camera.grabVideo(curFrame)) {
        curFrame.update();
        rawImage.setFromPixels(curFrame.getPixels().getData(), camera.getWidth(), camera.getHeight());
        
        
        
        
        int code;
        switch(bayerType) {
            case 0:
                code = CV_BayerBG2RGB;
                bayerName = "CV_BayerBG2RGB";
                break;
            case 1:
                code = CV_BayerGB2RGB;
                bayerName = "CV_BayerGB2RGB";
                break;
            case 2:
                code = CV_BayerRG2RGB;
                bayerName = "CV_BayerRG2RGB";
                break;
            case 3:
                code = CV_BayerGR2RGB;
                bayerName = "CV_BayerGR2RGB";
                break;
        }
        
        cvCvtColor(rawImage.getCvImage(), colorImage.getCvImage(), CV_BayerRG2BGR); // this seems to best match the colour mode of the camera
        
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
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(camera.isReady()) {
        // Camera doesn't draw itself, curFrame does.
        curFrame.draw(0, 0);
        //processed.draw(200,200);
        //processed.draw(800,0);
        rawImage.draw(800,0);
        rgb.draw(800, 300);
    }
    
    ofDrawBitmapStringHighlight("use the up/down keys: " + bayerName, 10, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_UP) {
        bayerType--;
    }
    if(key == OF_KEY_DOWN) {
        bayerType++;
    }
    bayerType = ofClamp(bayerType, 0, 3);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
