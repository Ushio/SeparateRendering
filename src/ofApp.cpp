#include "ofApp.h"

/*
 x, y, w, h: ÇÕÇ∑Ç◊Çƒ0~1ÇÃê≥ãKâªÇ≥ÇÍÇΩÇ‡ÇÃÇ∆ÇµÇƒéÛÇØéÊÇÈ
 +-------------------------
 |  
 |   + (x, y)-------+(w)
 |   |              |
 |   + -------------+
 |   (h)
 |
*/
inline ofMatrix4x4 trimProjectionMatrix(const ofMatrix4x4 &projectionMatrix, float x, float y, float w, float h) {
	glm::mat4 projmat;
	memcpy(glm::value_ptr(projmat), projectionMatrix.getPtr(), sizeof(glm::mat4));
	glm::mat4 viewmat;

	/*
	3+------+4
	|       |
	|       |
	1+------+2
	*/
	glm::vec3 p1 = glm::unProject(glm::vec3(0, 0, -1), viewmat, projmat, glm::vec4(0, 0, 1, 1));
	glm::vec3 p4 = glm::unProject(glm::vec3(1, 1, -1), viewmat, projmat, glm::vec4(0, 0, 1, 1));

	glm::vec3 pf1 = glm::unProject(glm::vec3(0, 0, 1), viewmat, projmat, glm::vec4(0, 0, 1, 1));
	glm::vec3 pf4 = glm::unProject(glm::vec3(1, 1, 1), viewmat, projmat, glm::vec4(0, 0, 1, 1));

	float L = ofLerp(p1.x, p4.x, x);
	float T = ofLerp(p4.y, p1.y, y);

	float R = ofLerp(p1.x, p4.x, x + w);
	float B = ofLerp(p4.y, p1.y, y + h);

	/*
	éãêçë‰ÇmakeFrustumMatrixÇ≈îCà”Ç…êÿÇËèoÇ∑

	 top, bottomÇ™ãtÇ…å©Ç¶ÇÈÇ™ÅAofÇ≈ÇÕÇ±ÇÍÇ™ê≥ÇµÇªÇ§	
	*/
	ofMatrix4x4 trim;
	trim.makeFrustumMatrix(
		L,
		R,
		T,
		B,
		-p1.z,
		-pf1.z
	);
	return trim;
}

//--------------------------------------------------------------
void ofApp::setup(){

	_camera.setNearClip(0.1f);
	_camera.setFarClip(200.0f);
	_camera.setDistance(5.0f);

	_mesh.load("bunny.ply");

	int w = ofGetWidth() / 2;
	int h = ofGetHeight() / 2;

	for (int i = 0; i < 4; ++i) {
		_fbos[i].allocate(w, h);
	}

	_shader.load("basic_vert.glsl", "basic_frag.glsl");
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofColor colors[] = {
		ofColor::red,
		ofColor::green,
		ofColor::blue,
		ofColor::yellow
	};
	ofVec4f views[] = {
		ofVec4f(0.0f, 0.0f, 0.5f, 0.5f),
		ofVec4f(0.5f, 0.0f, 0.5f, 0.5f),
		ofVec4f(0.0f, 0.5f, 0.5f, 0.5f),
		ofVec4f(0.5f, 0.5f, 0.5f, 0.5f)
	};

	ofCamera camera;
	camera.setPosition(1, 0.5, 2);
	camera.lookAt(ofVec3f(0, 0, 0));
	camera.setAspectRatio((float)ofGetWidth() / ofGetHeight());
	camera.setFarClip(80);
	camera.setNearClip(0.5);

	auto projectionMatrix = camera.getProjectionMatrix();
	auto modelMatrix = camera.getModelViewMatrix();

	ofEnableDepthTest();
	for (int i = 0; i < 4; ++i) {
		_fbos[i].begin();

		ofClear(colors[i]);

		auto v = views[i];
		auto trimmedProjection = trimProjectionMatrix(projectionMatrix, v.x, v.y, v.z, v.w);
		ofPushView();

		ofSetMatrixMode(OF_MATRIX_PROJECTION);
		ofLoadMatrix(trimmedProjection);
		ofSetMatrixMode(OF_MATRIX_MODELVIEW);
		ofLoadMatrix(modelMatrix);

		this->renderScene();

		ofPopView();

		_fbos[i].end();
	}
	ofDisableDepthTest();


	ofClear(0);
	ofEnableDepthTest();
	_camera.begin();

	ofPushMatrix();
	ofRotateZ(90.0f);
	ofSetColor(255);
	ofDrawGridPlane(1.0f);
	ofPopMatrix();


	ofPushMatrix();
	ofSetLineWidth(3);
	ofDrawAxis(1);
	ofSetLineWidth(1);
	ofPopMatrix();

	// drawFrustom(projectionMatrix, modelMatrix);
	for (int i = 0; i < 4; ++i) {
		auto v = views[i];
		auto trimmedProjection = trimProjectionMatrix(projectionMatrix, v.x, v.y, v.z, v.w);

		ofSetColor(colors[i]);
		drawFrustom(trimmedProjection, modelMatrix);
	}

	ofSetColor(255);
	this->renderScene();

	_camera.end();
	ofDisableDepthTest();

	if (_showSplitRendering) {
		int w = ofGetWidth() / 2;
		int h = ofGetHeight() / 2;
		ofVec2f dsts[] = {
			ofVec2f(0.0f, 0.0f),
			ofVec2f(w, 0.0f),
			ofVec2f(0.0f, h),
			ofVec2f(w, h)
		};
		for (int i = 0; i < 4; ++i) {
			ofSetColor(255);
			_fbos[i].draw(dsts[i]);

			char buffer[256];
			sprintf(buffer, "_fbos[%d].draw()", i);
			ofDrawBitmapString(buffer, dsts[i] + ofVec2f(30, 30));
		}
	}

	ofDrawBitmapString("toggle views 'space key'", 10, 10);
}

void ofApp::renderScene() {
	ofPushMatrix();
	ofTranslate(0.0, -0.5, 0.0);

	_shader.begin();
	_shader.setUniform3f("u_color", ofVec3f(1.0));
	_mesh.draw();
	_shader.end();

	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == ' ') {
		_showSplitRendering = !_showSplitRendering;
	}
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
