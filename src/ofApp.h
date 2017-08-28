#pragma once

#include "ofMain.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

inline void drawFrustom(ofMatrix4x4 projMat, ofMatrix4x4 viewMat) {
	glm::mat4 projmat;
	memcpy(glm::value_ptr(projmat), projMat.getPtr(), sizeof(glm::mat4));
	glm::mat4 viewmat;
	memcpy(glm::value_ptr(viewmat), viewMat.getPtr(), sizeof(glm::mat4));

	/*
	  3+------+4
	   |      |
	   |      |
	  1+------+2
	*/
	glm::vec3 p1 = glm::unProject(glm::vec3(0, 0, -1), viewmat, projmat, glm::vec4(0, 0, 1, 1));
	glm::vec3 p2 = glm::unProject(glm::vec3(1, 0, -1), viewmat, projmat, glm::vec4(0, 0, 1, 1));
	glm::vec3 p3 = glm::unProject(glm::vec3(0, 1, -1), viewmat, projmat, glm::vec4(0, 0, 1, 1));
	glm::vec3 p4 = glm::unProject(glm::vec3(1, 1, -1), viewmat, projmat, glm::vec4(0, 0, 1, 1));

	glm::vec3 pf1 = glm::unProject(glm::vec3(0, 0, 1), viewmat, projmat, glm::vec4(0, 0, 1, 1));
	glm::vec3 pf2 = glm::unProject(glm::vec3(1, 0, 1), viewmat, projmat, glm::vec4(0, 0, 1, 1));
	glm::vec3 pf3 = glm::unProject(glm::vec3(0, 1, 1), viewmat, projmat, glm::vec4(0, 0, 1, 1));
	glm::vec3 pf4 = glm::unProject(glm::vec3(1, 1, 1), viewmat, projmat, glm::vec4(0, 0, 1, 1));

	auto drawLine = [](const glm::vec3 &a, const glm::vec3 &b) {
		ofDrawLine(a.x, a.y, a.z, b.x, b.y, b.z);
	};

	drawLine(p1, p2);
	drawLine(p2, p4);
	drawLine(p4, p3);
	drawLine(p3, p1);

	drawLine(p1, pf1);
	drawLine(p2, pf2);
	drawLine(p3, pf3);
	drawLine(p4, pf4);

	drawLine(pf1, pf2);
	drawLine(pf2, pf4);
	drawLine(pf4, pf3);
	drawLine(pf3, pf1);
}

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void renderScene();

	ofEasyCam _camera;
	ofMesh _mesh;

	ofFbo _fbos[4];

	ofShader _shader;

	bool _showSplitRendering = false;
};
