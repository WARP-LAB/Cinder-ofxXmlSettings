// xmlSettingsExample
// this is just copy paste from https://github.com/openframeworks/openFrameworks/tree/master/examples/addons/xmlSettingsExample/src
// all credits to oF
// note that there is XML in libcinder http://libcinder.org/docs/v0.8.3/guide___xml_tree.html, this port is only intended so that some oF code can be reused
// ported by kroko

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Texture.h"
#include "cinder/Text.h"

#include "ofxXmlSettings.h"

#define NUM_PTS 800

using namespace ci;
using namespace ci::app;
using namespace std;

class xmlSettingsExampleApp : public AppNative {
public:
	void setup();
    void prepareSettings( Settings *settings );
	void update();
	void draw();
    
    void mouseDown( MouseEvent event );
    void mouseUp( MouseEvent event );
    void mouseDrag (MouseEvent event);
    void keyDown( KeyEvent event );
    
private:
    ofxXmlSettings XML;
    Font TTF;//ofTrueTypeFont TTF;
    
    string xmlStructure;
    string message;
    
    Vec2f dragPts[NUM_PTS];//ofPoint dragPts[NUM_PTS];
    
    int pointCount;
    int lineCount;
    int lastTagNumber;
    
    float red;
    float green;
    float blue;
    
    // additional from original
    gl::Texture	mTextTexture;
    
};

// ----------------------------------------------------------------
void xmlSettingsExampleApp::prepareSettings( Settings *settings )
{
    settings->setResizable( false );
    settings->setWindowSize( 1024, 768 );
    settings->setFullScreen( false );
    settings->setFrameRate( 120.f );
    settings->setTitle( "xmlSettingsExample" );
    settings->enablePowerManagement( false );
}


// ----------------------------------------------------------------
void xmlSettingsExampleApp::setup()
{
    
    try {
        if( XML.loadFile(loadAsset( "mySettings.xml" ))) {
            message = "mySettings.xml loaded!";
        }else{
            message = "unable to load mySettings.xml check data/ folder";
        }
    }
    catch( AssetLoadExc::AssetLoadExc &exc ) {
		std::cout << "ERROR: loading asset: " << std::endl;
		std::cout << exc.what();
        quit();
	}
	catch( ... ) {
		std::cout << "ERROR: Unable to load asset" << std::endl;
        quit();
	}
    
    /*
     // for resources
     try {
     if( XML.loadFile(loadResource( "mySettings.xml" ))) {
     message = "mySettings.xml loaded!";
     }else{
     message = "unable to load mySettings.xml check data/ folder";
     }
     }
     }
     catch( ResourceLoadExc::ResourceLoadExc &exc ) {
     std::cout << "ERROR: loading resource: " << std::endl;
     std::cout << exc.what();
     quit();
     }
     catch( ... ) {
     std::cout << "ERROR: Unable to load asset" << std::endl;
     quit();
     }
     */
    
    //read the colors from XML
	//if the settings file doesn't exist we assigns default values (170, 190, 240)
	red		= XML.getValue("BACKGROUND:COLOR:RED", 170);
	green	= XML.getValue("BACKGROUND:COLOR:GREEN", 190);
	blue	= XML.getValue("BACKGROUND:COLOR:BLUE", 240);
    
    /*
     "BACKGROUND:COLOR:RED" referes to a structure like this:
     
     <BACKGROUND>
     <COLOR>
     <RED>101.103516</RED>
     </COLOR>
     </BACKGROUND>
     */
    
	//we initalize some of our variables
	lastTagNumber	= 0;
	pointCount		= 0;
	lineCount		= 0;
    
    //-------
	//this is a more advanced use of ofXMLSettings
	//we are going to be reading multiple tags with the same name
    
	//lets see how many <STROKE> </STROKE> tags there are in the xml file
	int numDragTags = XML.getNumTags("STROKE:PT");
    
	//if there is at least one <STROKE> tag we can read the list of points
	//and then try and draw it as a line on the screen
	if(numDragTags > 0){
        
		//we push into the last STROKE tag
		//this temporarirly treats the tag as
		//the document root.
		XML.pushTag("STROKE", numDragTags-1);
        
        //we see how many points we have stored in <PT> tags
        int numPtTags = XML.getNumTags("PT");
        
        if(numPtTags > 0){
            
            //We then read those x y values into our
            //array - so that we can then draw the points as
            //a line on the screen
            
            //we have only allocated a certan amount of space for our array
            //so we don't want to read more than that amount of points
            int totalToRead = MIN(numPtTags, NUM_PTS);
            
            for(int i = 0; i < totalToRead; i++){
                //the last argument of getValue can be used to specify
                //which tag out of multiple tags you are refering to.
                int x = XML.getValue("PT:X", 0, i);
                int y = XML.getValue("PT:Y", 0, i);
                dragPts[i].set(x, y);
                pointCount++;
            }
        }
        
		//this pops us out of the STROKE tag
		//sets the root back to the xml document
		XML.popTag();
	}
    
    //load a monospaced font
	//which we will use to show part of the xml structure
    TTF = Font( loadAsset( "mono.ttf" ), 10 );//TTF.loadFont("mono.ttf", 7);
    
}

// ----------------------------------------------------------------
void xmlSettingsExampleApp::update()
{
    //we change the background color here based on the values
	//affected by the mouse position
    gl::clear( Color( (float)red/255,(float)green/255,(float)blue/255 ) ); //ofBackground((int)red,(int)green,(int)blue);
}

// ----------------------------------------------------------------
void xmlSettingsExampleApp::draw()
{
    //---------
	//Lets draw the stroke as a continous line
    gl::color(.13f,.13f,.13f); //ofSetHexColor(0x222222);
    /*
     ofNoFill();
     ofBeginShape();
     for(int i = 0; i < pointCount; i++){
     ofVertex(dragPts[i].x, dragPts[i].y);
     }
     ofEndShape(false);
     
     ofFill();
     */
    GLfloat *verts = new float[pointCount*2];
	for( int v = 0; v < pointCount; v++ ) {
		verts[v*2+0] = dragPts[v].x;
		verts[v*2+1] = dragPts[v].y;
	}
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glDrawArrays( GL_LINE_STRIP, 0, pointCount );
	glDisableClientState( GL_VERTEX_ARRAY );
	delete [] verts;
    
	//--------
	//we make a black area on the left
	//which we can print the xml text on
    gl::enableAlphaBlending();// ofEnableAlphaBlending();
	gl::color( ColorAf(.0f,.0f,.0f,.78f) );//ofSetColor(0, 0, 0, 200);
    gl::drawSolidRect( Rectf(.0f,.0f,160.0f,getWindowHeight()) ); //ofRect(0, 0, 160, ofGetHeight());
	gl::disableAlphaBlending();//ofDisableAlphaBlending();
    
	//our text that shows how the <STROKE> data looks in the xml file
	gl::color(.94f,.94f,.94f);//ofSetColor(240, 240, 240);
    
	string drawString = "How the data is stored:\n\n";
	if(xmlStructure.size() > 0) drawString += xmlStructure+"</STROKE>";
	//gl::drawString(drawString, Vec2f(5,40), ColorA(.94f,.94f,.94f,1),TTF);//TTF.drawString(drawString, 5, 40);
#warning line breaks won't work as on oF, needs text boxing (but hey, this is xml loading example :))
    // textbox
    TextBox tbox = TextBox().alignment( TextBox::LEFT ).font( TTF ).size( Vec2i( 160.0f, getWindowHeight() ) ).text( drawString );
    tbox.setColor( ColorA(.94f,.94f,.94f,1) );
    tbox.setBackgroundColor( ColorA( 0, 0, 0, 0 ) );
    mTextTexture = gl::Texture( tbox.render() );
    gl::enableAlphaBlending();
    if( mTextTexture )
		gl::draw( mTextTexture );
    // --textbox end
    
	//the message bars at the top and bottom of the app
	//ofSetHexColor(0xDDDDDD);
	gl::enableAlphaBlending();// ofEnableAlphaBlending();
	gl::color( ColorAf(.0f,.0f,.0f,.78f) );//ofSetColor(0, 0, 0, 200);
    
	gl::drawSolidRect( Rectf(160.f,.0f,getWindowWidth(),20.f) );//ofRect(160, 0, ofGetWidth()-160, 20);
	gl::drawSolidRect( Rectf(160.f,getWindowHeight()-20.0f,getWindowWidth(),getWindowHeight()));//ofRect(160, ofGetHeight()-20, ofGetWidth()-160, 20);
    
    //we draw our status message at the top
	//ofSetColor(210, 90, 100);
	gl::color(.94f,.94f,.94f);//ofSetColor(240, 240, 240);
	gl::drawString("Save settings to XML hit 's' key     status: "+message, Vec2f(170,12), ColorA(.94f,.94f,.94f,1),TTF);//TTF.drawString("Save settings to XML hit 's' key     status: "+message, 170, 12);
    
	//instructions at the bottom
	gl::drawString("mouse drag changes background color and records stroke", Vec2f(168,getWindowHeight()-9), ColorA(.94f,.94f,.94f,1),TTF);//TTF.drawString("mouse drag changes background color and records stroke", 168, ofGetHeight() - 9);
    
}

// ----------------------------------------------------------------
void xmlSettingsExampleApp::keyDown( KeyEvent event )
{
#warning save is good for me, but saving should be implemented also if file does not exist
    switch (event.getChar()) {
        case 's':
            try {
                XML.saveFile(loadAsset("mySettings.xml")); // TODO: save file also if no asset is found in thelocation
            } catch (...) {
                //
            }
			message ="settings saved to xml!";
            break;
        default:
            break;
    }
}

// ----------------------------------------------------------------
void xmlSettingsExampleApp::mouseDrag( MouseEvent event )
{
    int x = event.getX();
    int y = event.getY();
    
    //-------
	//we change the background color based on
	//the two mouse coords coming in
	float xpct = (float)x / getWindowWidth();// ofGetWidth();
	float ypct = (float)y / getWindowHeight();// ofGetHeight();
	red			= xpct * 255.0;
	green		= ypct * 255.0;
	blue		= (int)(red - green) % 255;
    
    
	//-------------
	//we also want to record the stroke
    
	//lets store the drag of the user.
	//we will push into the most recent
	//<STROKE> tag - add the mouse points
	//then pop out
	if( XML.pushTag("STROKE", lastTagNumber) ){
        
		//now we will add a pt tag - with two
		//children - X and Y
        
		int tagNum = XML.addTag("PT");
		XML.setValue("PT:X", x, tagNum);
		XML.setValue("PT:Y", y, tagNum);
        
		XML.popTag();
	}
    
	//-------------
	//here we make a string of text that looks like how the xml data
	//is stored in the settings file - this is just so people can
	//visually see how the data is stored.
    
	//if the text is about to go off screen
	if(lineCount > 64){
		//we find the first <PT> tag with the
		//x and y data and we remove it from the begining
		//this way the displayed text always shows the newest data
		//without going offscreen.
		int pos = xmlStructure.find("</PT>");
		xmlStructure = xmlStructure.substr(pos+6);
	}
    
	//here we add the most recent point to our fake xml string
    xmlStructure += "    <PT>\n        <X>"+std::to_string(x)+"</X>\n        <Y>"+std::to_string(y)+"</Y>\n    </PT>\n";//xmlStructure += "    <PT>\n        <X>"+ofToString(x)+"</X>\n        <Y>"+ofToString(y)+"</Y>\n    </PT>\n";
	lineCount+=4; //we have added 4 lines so increment by 4
    
	//------------
	//we also record the x y points into an array - so we can draw it
	if(pointCount < NUM_PTS -1){
		dragPts[pointCount].set(x, y);
		pointCount++;
	}
}

// ----------------------------------------------------------------
void xmlSettingsExampleApp::mouseDown( MouseEvent event )
{
    //reset so we don't store extra tags
    XML.clear();
    
	//we can also add tags with the same name.
	//here we are just adding an empty tag
	//and when the user drags their mouse
	//we will store the pts in this tag
	lastTagNumber	= XML.addTag("STROKE");
	xmlStructure	= "<STROKE>\n";
    
	//We start a new stroke
	lineCount		= 0;
	pointCount		= 0;
}

// ----------------------------------------------------------------
void xmlSettingsExampleApp::mouseUp( MouseEvent event )
{
    //update the colors to the XML structure when the mouse is released
	XML.setValue("BACKGROUND:COLOR:RED", red);
	XML.setValue("BACKGROUND:COLOR:GREEN", green);
	XML.setValue("BACKGROUND:COLOR:BLUE", blue);
    
}

CINDER_APP_NATIVE( xmlSettingsExampleApp, RendererGl )
