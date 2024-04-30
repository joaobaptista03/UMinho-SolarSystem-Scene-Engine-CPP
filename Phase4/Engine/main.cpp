#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <stack>
#include <math.h>
#include <algorithm>

#define _USE_MATH_DEFINES

int windowWidth = 0, windowHeight = 0;
int cameraX = 0, cameraY = 0, cameraZ = 0;
int lookAtX = 0, lookAtY = 0, lookAtZ = 0;
int upX = 0, upY = 1, upZ = 0;
int fov = 60, near = 1, far = 1000;
int startX, startY, tracking = 0;

float alfa = 0.0f, beta = 0.0f, radius = 5.0f;

bool drawCurve = false;

struct Point {
	float x = 0, y = 0, z = 0;
};

struct Translate {
	Point point = {0, 0, 0};
	bool isCatmullRom = false;
    float time = 0;
    bool alignDirection = false;
    std::vector<Point> path;
};

struct Scale {
	Point point{1, 1, 1};
};

struct Rotate {
	Point point;
    bool hasTime = false;
    float timeOrAngle;
};

struct Color{
	float diffuse[3];
	float ambient[3];
	float specular[3];
	float emissive[3];
	float shininess;
};

struct Group {
	Translate translate;
	Rotate rotate;
	Scale scale;
	char transformations[3] = {0, 0, 0};
    bool hasTranslate = false, hasRotate = false, hasScale = false, hasColor = false, hasTexture = false;
	std::vector<std::string> models;
	std::vector<Color> colors;
	std::vector<std::string> textures;
	std::vector<Group> subgroups;
	Group *parent;
	int transformcounter = 0;
	int modelcounter = 0;
};

struct Model {
    GLuint vboId;
    int numVertices;
};

std::vector<Group> sceneGraph;
std::stack<Group*> groupStack;
std::map<std::string, Model> modelCache;

void multiplyMatrices(GLfloat result[16], const GLfloat mat1[16], const GLfloat mat2[16]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i*4 + j] = 0;
            for (int k = 0; k < 4; ++k) {
                result[i*4 + j] += mat1[i*4 + k] * mat2[k*4 + j];
            }
        }
    }
}

void normalize(float v[3]) {
	float l = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if (l > 0) {
		v[0] /= l;
		v[1] /= l;
		v[2] /= l;
	}
}

void cross(float v1[3], float v2[3], float res[3]) {
	res[0] = v1[1]*v2[2] - v1[2]*v2[1];
	res[1] = v1[2]*v2[0] - v1[0]*v2[2];
	res[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

void alignObject(const Point& deriv) {
	float up[3] = {0, 1, 0};
	float x[3] = {deriv.x, deriv.y, deriv.z};
	normalize(x);
	float z[3];
	cross(x, up, z);
	normalize(z);
	float y[3];
	cross(z,x,y);
	normalize(y);
	memcpy(up, y, 3 * sizeof(float));

	float m[16] = {x[0], x[1], x[2], 0, y[0], y[1], y[2], 0, z[0], z[1], z[2], 0, 0, 0, 0, 1};

	glMultMatrixf(m);
}

void multMatrixVector(float m[4][4], float *v, float *res) {
	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j][k];
		}
	}
}

void getCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3, Point& pos, Point& deriv) {
    
	float m[4][4] = {{-0.5f, 1.5f, -1.5f, 0.5f},
                     {1.0f, -2.5f, 2.0f, -0.5f},
                     {-0.5f, 0.0f, 0.5f, 0.0f},
                     {0.0f, 1.0f, 0.0f, 0.0f}};

    float px[4] = {p0.x, p1.x, p2.x, p3.x};
    float py[4] = {p0.y, p1.y, p2.y, p3.y};
    float pz[4] = {p0.z, p1.z, p2.z, p3.z};

    float ax[4], ay[4], az[4];
    multMatrixVector(m, px, ax);
    multMatrixVector(m, py, ay);
    multMatrixVector(m, pz, az);

    float tv[4] = {t * t * t, t * t, t, 1};
    float tvd[4] = {3 * t * t, 2 * t, 1, 0};

    pos.x = pos.y = pos.z = 0;
    deriv.x = deriv.y = deriv.z = 0;

    for (int i = 0; i < 4; i++) {
        pos.x += tv[i] * ax[i];
        pos.y += tv[i] * ay[i];
        pos.z += tv[i] * az[i];

        deriv.x += tvd[i] * ax[i];
        deriv.y += tvd[i] * ay[i];
        deriv.z += tvd[i] * az[i];
    }
}

void getGlobalCatmullRomPoint(float gt, const std::vector<Point>& controlPoints, Point& pos, Point& deriv) {
    int pointCount = controlPoints.size();
    float t = gt * pointCount;
    int index = floor(t);
    t = t - index;
    
    int indices[4];
    indices[0] = (index + pointCount - 1) % pointCount;
    indices[1] = (indices[0] + 1) % pointCount;
    indices[2] = (indices[1] + 1) % pointCount;
    indices[3] = (indices[2] + 1) % pointCount;

    getCatmullRomPoint(t, controlPoints[indices[0]], controlPoints[indices[1]],
                          controlPoints[indices[2]], controlPoints[indices[3]], pos, deriv);
}

void drawCatmullRomCurve(const std::vector<Point>& controlPoints) {
	Point pos, deriv;
	glBegin(GL_LINE_LOOP);
	for (float gt = 0; gt < 1; gt += 0.01) {
		getGlobalCatmullRomPoint(gt, controlPoints, pos, deriv);
		glVertex3f(pos.x, pos.y, pos.z);
	}
	glEnd();
}

GLuint loadModelToVBO(const std::vector<float>& vertices) {
    GLuint vboId;
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vboId;
}

void drawModel(const std::string& file, const Color& color) {
    if (modelCache.find(file) == modelCache.end()) {
        std::ifstream inputFile(file);
        if (!inputFile.is_open()) {
            std::cerr << "Failed to open file: " << file << "\n";
            return;
        }
        
        std::string line;
        std::vector<float> vertices;
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
			std::string token;
			
            while (iss >> token) {
				std::istringstream subIss(token);
				std::string subToken;

				while (std::getline(subIss, subToken, ',')) {
					float subTokenFloat = std::stof(subToken);
					vertices.push_back(subTokenFloat);
				}
            }
        }
        
        inputFile.close();
        Model model;
        model.numVertices = vertices.size() / 3;
        model.vboId = loadModelToVBO(vertices);
        modelCache[file] = model;
    }

    Model& model = modelCache[file];
    glBindBuffer(GL_ARRAY_BUFFER, model.vboId);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLES, 0, model.numVertices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawGroup(const Group& group, float currentTime) {
    glPushMatrix();

	for (int i = 0; i < group.transformcounter; i++) {
		if (group.transformations[i] == 't') {
			if (!group.translate.isCatmullRom)
				glTranslatef(group.translate.point.x, group.translate.point.y, group.translate.point.z);
			else {
				if (drawCurve) drawCatmullRomCurve(group.translate.path);
				Point pos, deriv;
				float gt = fmod(currentTime / group.translate.time, 1.0);
				getGlobalCatmullRomPoint(gt, group.translate.path, pos, deriv);
				glTranslatef(pos.x, pos.y, pos.z);
	
				if (group.translate.alignDirection) alignObject(deriv);
			}
		} else if (group.transformations[i] == 'r') {
			if (group.rotate.hasTime) {
				float anglePerSecond = 360.0f / group.rotate.timeOrAngle;
				float currentAngle = fmod(currentTime * anglePerSecond, 360.0f);
				glRotatef(currentAngle, group.rotate.point.x, group.rotate.point.y, group.rotate.point.z);
			} else {
				glRotatef(group.rotate.timeOrAngle, group.rotate.point.x, group.rotate.point.y, group.rotate.point.z);
			}
		} else if (group.transformations[i] == 's') {
			glScalef(group.scale.point.x, group.scale.point.y, group.scale.point.z);
		}
	}

	int modelCounter = 0;

    for (const std::string& modelFile : group.models) drawModel(modelFile, group.colors[modelCounter++]);

    for (const Group& subgroup : group.subgroups) drawGroup(subgroup, currentTime);

    glPopMatrix();
}

void changeSize(int w, int h) {
	if(h == 0)
		h = 1;

	float ratio = w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0, 0, w, h);

	gluPerspective(fov, ratio, near, far);

	glMatrixMode(GL_MODELVIEW);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(cameraX, cameraY, cameraZ,
			  lookAtX, lookAtY, lookAtZ,
			  upX, upY, upZ);

	glBegin(GL_LINES);
	
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1000.0f, 0.0f, 0.0f);
		glVertex3f(1000.0f, 0.0f, 0.0f);
		
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -1000.0f, 0.0f);
		glVertex3f(0.0f, 1000.0f, 0.0f);
		
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -1000.0f);
		glVertex3f(0.0f, 0.0f, 1000.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);

	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	for (const Group& group : sceneGraph) {
		drawGroup(group, currentTime);
	}
	
	glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy) {
	switch (c) {
		case 'd':
			alfa += 0.1; break;

		case 'a':
			alfa -= 0.1; break;

		case 'w':
			beta += 0.1f;
			if (beta > 1.5f)
				beta = 1.5f;
			break;

		case 's':
			beta -= 0.1f;
			if (beta < -1.5f)
				beta = -1.5f;
			break;

		case 'e': radius -= 1.0f;
			if (radius < 0.1f)
				radius = 0.1f;
			break;

		case 'q': radius += 1.0f; break;

		case ' ': drawCurve = !drawCurve; break;
	}

	cameraX = radius * cos(beta) * sin(alfa);
	cameraY = radius * sin(beta);
	cameraZ = radius * cos(beta) * cos(alfa);

	glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy) {
	switch (key) {
		case GLUT_KEY_RIGHT:
			alfa += 0.1; break;

		case GLUT_KEY_LEFT:
			alfa -= 0.1; break;

		case GLUT_KEY_UP:
			beta += 0.1f;
			if (beta > 1.5f)
				beta = 1.5f;
			break;

		case GLUT_KEY_DOWN:
			beta -= 0.1f;
			if (beta < -1.5f)
				beta = -1.5f;
			break;

		case GLUT_KEY_PAGE_UP: radius -= 1.0f;
			if (radius < 0.1f)
				radius = 0.1f;
			break;

		case GLUT_KEY_PAGE_DOWN: radius += 1.0f; break;
	}
	
	cameraX = radius * cos(beta) * sin(alfa);
	cameraY = radius * sin(beta);
	cameraZ = radius * cos(beta) * cos(alfa);

	glutPostRedisplay();
}

void processMouseButtons(int button, int state, int xx, int yy) {
    if (state == GLUT_DOWN) {
        startX = xx;
        startY = yy;
        switch(button) {
            case GLUT_LEFT_BUTTON:
                tracking = 1;
                break;
            case GLUT_RIGHT_BUTTON:
                tracking = 2;
                break;
            default:
                tracking = 0;
                break;
        }
    } else if (state == GLUT_UP) {
        tracking = 0;
    }
}

void processMouseMotion(int xx, int yy) {
    if (tracking == 0) return;

    int deltaX = xx - startX;
    int deltaY = yy - startY;

    if (tracking == 1) {
        alfa -= deltaX * 0.005f;
        beta += deltaY * 0.005f;

        if (beta > 1.5f) beta = 1.5f;
        else if (beta < -1.5f) beta = -1.5f;
    } else if (tracking == 2) {
        radius -= deltaY * 0.1f;
        if (radius < 3.0f) radius = 3.0f;
    }

    cameraX = radius * cos(beta) * sin(alfa);
    cameraY = radius * sin(beta);
    cameraZ = radius * cos(beta) * cos(alfa);

    startX = xx;
    startY = yy;
    glutPostRedisplay();
}

void parseWindow(std::string line) {
	std::size_t widthPos = line.find("width=");
	std::size_t heightPos = line.find("height=");

	if (widthPos != std::string::npos && heightPos != std::string::npos) {
		std::size_t widthStart = line.find("\"", widthPos) + 1;
		std::size_t widthEnd = line.find("\"", widthStart);
		std::size_t heightStart = line.find("\"", heightPos) + 1;
		std::size_t heightEnd = line.find("\"", heightStart);

		std::string widthStr = line.substr(widthStart, widthEnd - widthStart);
		std::string heightStr = line.substr(heightStart, heightEnd - heightStart);

		windowWidth = std::stoi(widthStr);
		windowHeight = std::stoi(heightStr);
	}
}

void parsePosition(std::string line) {
	std::size_t xPos = line.find("x=");
	std::size_t yPos = line.find("y=");
	std::size_t zPos = line.find("z=");

	if (xPos != std::string::npos && yPos != std::string::npos && zPos != std::string::npos) {
		std::size_t xStart = line.find("\"", xPos) + 1;
		std::size_t xEnd = line.find("\"", xStart);
		std::size_t yStart = line.find("\"", yPos) + 1;
		std::size_t yEnd = line.find("\"", yStart);
		std::size_t zStart = line.find("\"", zPos) + 1;
		std::size_t zEnd = line.find("\"", zStart);

		std::string xStr = line.substr(xStart, xEnd - xStart);
		std::string yStr = line.substr(yStart, yEnd - yStart);
		std::string zStr = line.substr(zStart, zEnd - zStart);

		cameraX = std::stoi(xStr);
		cameraY = std::stoi(yStr);
		cameraZ = std::stoi(zStr);
	}

	radius = sqrt(cameraX*cameraX + cameraY*cameraY + cameraZ*cameraZ);
	alfa = atan2(cameraX, cameraZ);
	beta = atan2(cameraY, sqrt(cameraX*cameraX + cameraZ*cameraZ));
}

void parseLookAt(std::string line) {
	std::size_t xPos = line.find("x=");
	std::size_t yPos = line.find("y=");
	std::size_t zPos = line.find("z=");

	if (xPos != std::string::npos && yPos != std::string::npos && zPos != std::string::npos) {
		std::size_t xStart = line.find("\"", xPos) + 1;
		std::size_t xEnd = line.find("\"", xStart);
		std::size_t yStart = line.find("\"", yPos) + 1;
		std::size_t yEnd = line.find("\"", yStart);
		std::size_t zStart = line.find("\"", zPos) + 1;
		std::size_t zEnd = line.find("\"", zStart);

		std::string xStr = line.substr(xStart, xEnd - xStart);
		std::string yStr = line.substr(yStart, yEnd - yStart);
		std::string zStr = line.substr(zStart, zEnd - zStart);

		lookAtX = std::stoi(xStr);
		lookAtY = std::stoi(yStr);
		lookAtZ = std::stoi(zStr);
	}
}

void parseUp(std::string line) {
	std::size_t xPos = line.find("x=");
	std::size_t yPos = line.find("y=");
	std::size_t zPos = line.find("z=");

	if (xPos != std::string::npos && yPos != std::string::npos && zPos != std::string::npos) {
		std::size_t xStart = line.find("\"", xPos) + 1;
		std::size_t xEnd = line.find("\"", xStart);
		std::size_t yStart = line.find("\"", yPos) + 1;
		std::size_t yEnd = line.find("\"", yStart);
		std::size_t zStart = line.find("\"", zPos) + 1;
		std::size_t zEnd = line.find("\"", zStart);

		std::string xStr = line.substr(xStart, xEnd - xStart);
		std::string yStr = line.substr(yStart, yEnd - yStart);
		std::string zStr = line.substr(zStart, zEnd - zStart);

		upX = std::stoi(xStr);
		upY = std::stoi(yStr);
		upZ = std::stoi(zStr);

	}
}

void parseProjection(std::string line) {
	std::size_t fovPos = line.find("fov=");
	std::size_t nearPos = line.find("near=");
	std::size_t farPos = line.find("far=");

	if (fovPos != std::string::npos && nearPos != std::string::npos && farPos != std::string::npos) {
		std::size_t fovStart = line.find("\"", fovPos) + 1;
		std::size_t fovEnd = line.find("\"", fovStart);
		std::size_t nearStart = line.find("\"", nearPos) + 1;
		std::size_t nearEnd = line.find("\"", nearStart);
		std::size_t farStart = line.find("\"", farPos) + 1;
		std::size_t farEnd = line.find("\"", farStart);

		std::string fovStr = line.substr(fovStart, fovEnd - fovStart);
		std::string nearStr = line.substr(nearStart, nearEnd - nearStart);
		std::string farStr = line.substr(farStart, farEnd - farStart);

		fov = std::stoi(fovStr);
		near = std::stoi(nearStr);
		far = std::stoi(farStr);
	}
}

void parseGroup(std::string line, bool open) {
	if (open) {
		if (groupStack.empty()) {
			sceneGraph.emplace_back();
			groupStack.push(&sceneGraph.back()); 
		} else {
			groupStack.top()->subgroups.emplace_back();
			groupStack.push(&groupStack.top()->subgroups.back()); 
		}	
	} else {
		if (!groupStack.empty()) {
			groupStack.pop(); 
		} else {
			std::cerr << "Error: Group end tag without matching start tag.\n";
		}
	}
}

void parseTranslate(std::string line) {
	Group* group = groupStack.top();

	group->hasTranslate = true;
	std::size_t xPos = line.find("x=");
	std::size_t yPos = line.find("y=");
	std::size_t zPos = line.find("z=");
	std::size_t timePos = line.find("time=");
	std::size_t alignPos = line.find("align=");


	if (xPos != std::string::npos && yPos != std::string::npos && zPos != std::string::npos) {
		std::size_t xStart = line.find("\"", xPos) + 1;
		std::size_t xEnd = line.find("\"", xStart);
		std::size_t yStart = line.find("\"", yPos) + 1;
		std::size_t yEnd = line.find("\"", yStart);
		std::size_t zStart = line.find("\"", zPos) + 1;
		std::size_t zEnd = line.find("\"", zStart);

		std::string xStr = line.substr(xStart, xEnd - xStart);
		std::string yStr = line.substr(yStart, yEnd - yStart);
		std::string zStr = line.substr(zStart, zEnd - zStart);

		group->translate.point.x = std::stof(xStr);
		group->translate.point.y = std::stof(yStr);
		group->translate.point.z = std::stof(zStr);
		group->transformations[group->transformcounter++] = 't';
	}

	if (timePos != std::string::npos) {
		std::size_t timeStart = line.find("\"", timePos) + 1;
		std::size_t timeEnd = line.find("\"", timeStart);

		std::string timeStr = line.substr(timeStart, timeEnd - timeStart);
		group->translate.time = std::stof(timeStr);
		group->transformations[group->transformcounter++] = 't';
		group->translate.isCatmullRom = true;
	}

	if (alignPos != std::string::npos) {
		std::size_t alignStart = line.find("\"", alignPos) + 1;
		std::size_t alignEnd = line.find("\"", alignStart);

		std::string alignStr = line.substr(alignStart, alignEnd - alignStart);
		group->translate.alignDirection = (alignStr == "true");
	}
}

void parseRotate(std::string line) {
	Group* group = groupStack.top();

	group->hasRotate = true;
	
	std::size_t anglePos = line.find("angle=");
	std::size_t xPos = line.find("x=");
	std::size_t yPos = line.find("y=");
	std::size_t zPos = line.find("z=");
	std::size_t timePos = line.find("time=");

	if (xPos != std::string::npos && yPos != std::string::npos && zPos != std::string::npos) {
		std::size_t xStart = line.find("\"", xPos) + 1;
		std::size_t xEnd = line.find("\"", xStart);
		std::size_t yStart = line.find("\"", yPos) + 1;
		std::size_t yEnd = line.find("\"", yStart);
		std::size_t zStart = line.find("\"", zPos) + 1;
		std::size_t zEnd = line.find("\"", zStart);

		std::string xStr = line.substr(xStart, xEnd - xStart);
		std::string yStr = line.substr(yStart, yEnd - yStart);
		std::string zStr = line.substr(zStart, zEnd - zStart);

		group->rotate.point.x = std::stof(xStr);
		group->rotate.point.y = std::stof(yStr);
		group->rotate.point.z = std::stof(zStr);
		group->transformations[group->transformcounter++] = 'r';
	}

	if (anglePos != std::string::npos) {
		std::size_t angleStart = line.find("\"", anglePos) + 1;
		std::size_t angleEnd = line.find("\"", angleStart);

		std::string angleStr = line.substr(angleStart, angleEnd - angleStart);
		group->rotate.timeOrAngle = std::stof(angleStr);
	}

	if (timePos != std::string::npos) {
		std::size_t timeStart = line.find("\"", timePos) + 1;
		std::size_t timeEnd = line.find("\"", timeStart);

		std::string timeStr = line.substr(timeStart, timeEnd - timeStart);
		group->rotate.hasTime = true;
		group->rotate.timeOrAngle = std::stof(timeStr);
	}
}

void parsePoint(std::string line) {
	Group* group = groupStack.top();

	Point point;
	std::size_t xPos = line.find("x=");
	std::size_t yPos = line.find("y=");
	std::size_t zPos = line.find("z=");
	if (xPos != std::string::npos && yPos != std::string::npos && zPos != std::string::npos) {
		std::size_t xStart = line.find("\"", xPos) + 1;
		std::size_t xEnd = line.find("\"", xStart);
		std::size_t yStart = line.find("\"", yPos) + 1;
		std::size_t yEnd = line.find("\"", yStart);
		std::size_t zStart = line.find("\"", zPos) + 1;
		std::size_t zEnd = line.find("\"", zStart);

		std::string xStr = line.substr(xStart, xEnd - xStart);
		std::string yStr = line.substr(yStart, yEnd - yStart);
		std::string zStr = line.substr(zStart, zEnd - zStart);

		point.x = std::stof(xStr);
		point.y = std::stof(yStr);
		point.z = std::stof(zStr);
		group->translate.path.push_back(point);
	}
}

void parseScale(std::string line) {
	Group *group = groupStack.top();

	group->hasScale = true;
	std::size_t xPos = line.find("x=");
	std::size_t yPos = line.find("y=");
	std::size_t zPos = line.find("z=");

	if (xPos != std::string::npos && yPos != std::string::npos && zPos != std::string::npos) {
		std::size_t xStart = line.find("\"", xPos) + 1;
		std::size_t xEnd = line.find("\"", xStart);
		std::size_t yStart = line.find("\"", yPos) + 1;
		std::size_t yEnd = line.find("\"", yStart);
		std::size_t zStart = line.find("\"", zPos) + 1;
		std::size_t zEnd = line.find("\"", zStart);

		std::string xStr = line.substr(xStart, xEnd - xStart);
		std::string yStr = line.substr(yStart, yEnd - yStart);
		std::string zStr = line.substr(zStart, zEnd - zStart);

		group->scale.point.x = std::stof(xStr);
		group->scale.point.y = std::stof(yStr);
		group->scale.point.z = std::stof(zStr);
		group->transformations[group->transformcounter++] = 's';
	}
}

void parseModel(std::string line) {
	std::size_t filePos = line.find("file=");

	if (filePos != std::string::npos) {
		std::size_t fileStart = line.find("\"", filePos) + 1;
		std::size_t fileEnd = line.find("\"", fileStart);

		std::string fileStr = line.substr(fileStart, fileEnd - fileStart);
		groupStack.top()->models.push_back("../Output/" + fileStr);
	}
}

void parseDiffuse(std::string line){
	Group* group = groupStack.top();

	group->hasColor = true;
	std::size_t rPos = line.find("R=");
	std::size_t gPos = line.find("G=");
	std::size_t bPos = line.find("B=");

	if (rPos != std::string::npos && gPos != std::string::npos && bPos != std::string::npos) {
		std::size_t rStart = line.find("\"", rPos) + 1;
		std::size_t rEnd = line.find("\"", rStart);
		std::size_t gStart = line.find("\"", gPos) + 1;
		std::size_t gEnd = line.find("\"", gStart);
		std::size_t bStart = line.find("\"", bPos) + 1;
		std::size_t bEnd = line.find("\"", bStart);

		std::string rStr = line.substr(rStart, rEnd - rStart);
		std::string gStr = line.substr(gStart, gEnd - gStart);
		std::string bStr = line.substr(bStart, bEnd - bStart);

		group->colors[group->modelcounter].diffuse[0] = std::stof(rStr);
		group->colors[group->modelcounter].diffuse[1] = std::stof(gStr);
		group->colors[group->modelcounter].diffuse[2] = std::stof(bStr);
	}
	group->modelcounter;
}

void parseAmbient(std::string line){
	Group* group = groupStack.top();

	group->hasColor = true;
	std::size_t rPos = line.find("R=");
	std::size_t gPos = line.find("G=");
	std::size_t bPos = line.find("B=");

	if (rPos != std::string::npos && gPos != std::string::npos && bPos != std::string::npos) {
		std::size_t rStart = line.find("\"", rPos) + 1;
		std::size_t rEnd = line.find("\"", rStart);
		std::size_t gStart = line.find("\"", gPos) + 1;
		std::size_t gEnd = line.find("\"", gStart);
		std::size_t bStart = line.find("\"", bPos) + 1;
		std::size_t bEnd = line.find("\"", bStart);

		std::string rStr = line.substr(rStart, rEnd - rStart);
		std::string gStr = line.substr(gStart, gEnd - gStart);
		std::string bStr = line.substr(bStart, bEnd - bStart);

		group->colors[group->modelcounter].ambient[0] = std::stof(rStr);
		group->colors[group->modelcounter].ambient[1] = std::stof(gStr);
		group->colors[group->modelcounter].ambient[2] = std::stof(bStr);
	}
}

void parseSpecular(std::string line){
	Group* group = groupStack.top();

	group->hasColor = true;
	std::size_t rPos = line.find("R=");
	std::size_t gPos = line.find("G=");
	std::size_t bPos = line.find("B=");

	if (rPos != std::string::npos && gPos != std::string::npos && bPos != std::string::npos) {
		std::size_t rStart = line.find("\"", rPos) + 1;
		std::size_t rEnd = line.find("\"", rStart);
		std::size_t gStart = line.find("\"", gPos) + 1;
		std::size_t gEnd = line.find("\"", gStart);
		std::size_t bStart = line.find("\"", bPos) + 1;
		std::size_t bEnd = line.find("\"", bStart);

		std::string rStr = line.substr(rStart, rEnd - rStart);
		std::string gStr = line.substr(gStart, gEnd - gStart);
		std::string bStr = line.substr(bStart, bEnd - bStart);

		group->colors[group->modelcounter].specular[0] = std::stof(rStr);
		group->colors[group->modelcounter].specular[1] = std::stof(gStr);
		group->colors[group->modelcounter].specular[2] = std::stof(bStr);
	}
}

void parseEmissive(std::string line){
	Group* group = groupStack.top();

	group->hasColor = true;
	std::size_t rPos = line.find("R=");
	std::size_t gPos = line.find("G=");
	std::size_t bPos = line.find("B=");

	if (rPos != std::string::npos && gPos != std::string::npos && bPos != std::string::npos) {
		std::size_t rStart = line.find("\"", rPos) + 1;
		std::size_t rEnd = line.find("\"", rStart);
		std::size_t gStart = line.find("\"", gPos) + 1;
		std::size_t gEnd = line.find("\"", gStart);
		std::size_t bStart = line.find("\"", bPos) + 1;
		std::size_t bEnd = line.find("\"", bStart);

		std::string rStr = line.substr(rStart, rEnd - rStart);
		std::string gStr = line.substr(gStart, gEnd - gStart);
		std::string bStr = line.substr(bStart, bEnd - bStart);

		group->colors[group->modelcounter].emissive[0] = std::stof(rStr);
		group->colors[group->modelcounter].emissive[1] = std::stof(gStr);
		group->colors[group->modelcounter].emissive[2] = std::stof(bStr);
	}
}

void parseShininess(std::string line){
	Group* group = groupStack.top();

	group->hasColor = true;
	std::size_t shininessPos = line.find("value=");

	if (shininessPos != std::string::npos) {
		std::size_t shininessStart = line.find("\"", shininessPos) + 1;
		std::size_t shininessEnd = line.find("\"", shininessStart);

		std::string shininessStr = line.substr(shininessStart, shininessEnd - shininessStart);
		group->colors[group->modelcounter].shininess = std::stof(shininessStr);
	}
	group->modelcounter++;
}

void parseTexture(std::string line){
	Group* group = groupStack.top();

	group->hasTexture = true;
	std::size_t filePos = line.find("file=");

	if (filePos != std::string::npos) {
		std::size_t fileStart = line.find("\"", filePos) + 1;
		std::size_t fileEnd = line.find("\"", fileStart);

		std::string fileStr = line.substr(fileStart, fileEnd - fileStart);
		group->textures[group->modelcounter] = "../Output/" + fileStr;
	}
	group->modelcounter++;
}

void parseXML(std::ifstream& inputFile) {
	std::string line;
	while (std::getline(inputFile, line)) {
		std::regex cleanSpaces("\\s*=\\s*");
		line = std::regex_replace(line, cleanSpaces, "=");

		if (line.find("<window") != std::string::npos) parseWindow(line);

		else if (line.find("<position") != std::string::npos) parsePosition(line);
		
		else if (line.find("<lookAt") != std::string::npos) parseLookAt(line);

		else if (line.find("<up") != std::string::npos) parseUp(line);

		else if (line.find("<projection") != std::string::npos) parseProjection(line);
		
		else if (line.find("<group") != std::string::npos) parseGroup(line, true);

		else if (line.find("</group") != std::string::npos) parseGroup(line, false);

		else if (line.find("<translate") != std::string::npos) parseTranslate(line);

		else if (line.find("<rotate") != std::string::npos) parseRotate(line);

		else if (line.find("<point") != std::string::npos) parsePoint(line);

		else if (line.find("<scale") != std::string::npos) parseScale(line);

		else if (line.find("<model") != std::string::npos) parseModel(line);

		else if (line.find("<diffuse") != std::string::npos) parseDiffuse(line);

		else if (line.find("<ambient") != std::string::npos) parseAmbient(line);

		else if (line.find("<specular") != std::string::npos) parseSpecular(line);

		else if (line.find("<emissive") != std::string::npos) parseEmissive(line);

		else if (line.find("<shininess") != std::string::npos) parseShininess(line);

		else if (line.find("<texture") != std::string::npos) parseTexture(line);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <xml file>" << std::endl;
		return 1;
	}

	std::ifstream inputFile;
	inputFile.open(argv[1]);

	if (!inputFile.is_open()) {
		std::cerr << "Failed to open file: " << argv[1] << "\n";
		return 2;
	}

	parseXML(inputFile);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("CG@DI-UM");

	glewInit();

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	glutSpecialFunc(processSpecialKeys);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutIdleFunc(renderScene);

	glutMainLoop();

	inputFile.close();

	return 0;
}