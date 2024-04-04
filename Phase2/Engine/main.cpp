#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include <stack>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

int windowWidth = 0, windowHeight = 0;
int cameraX = 0, cameraY = 0, cameraZ = 0;
int lookAtX = 0, lookAtY = 0, lookAtZ = 0;
int upX = 0, upY = 0, upZ = 0;
int fov = 0, near = 0, far = 0;
int startX, startY, tracking = 0;

float alfa = 0.0f, beta = 0.0f, radius = 5.0f;


struct Translate{
	float x;
	float y;
	float z;
};

struct Scale{
	float x;
	float y;
	float z;
};

struct Rotate{
	float angle;
	float x;
	float y;
	float z;
};

struct Group{
	Translate translate;
	Rotate rotate;
	Scale scale;
    bool hasTranslate = false, hasRotate = false, hasScale = false;
	std::vector<std::string> models;
	std::vector<Group> subgroups;
	Group *parent;
};

std::vector<Group> sceneGraph; // The root of the scene graph
std::stack<Group*> groupStack; // For keeping track of the current group in the hierarchy

void openGroup() {
    if (groupStack.empty()) {
        sceneGraph.emplace_back();
        groupStack.push(&sceneGraph.back()); 
    } else {
        groupStack.top()->subgroups.emplace_back();
        groupStack.push(&groupStack.top()->subgroups.back()); 
    }
}

void closeGroup() {
    if (!groupStack.empty()) {
        groupStack.pop(); 
    } else {
        std::cerr << "Error: Group end tag without matching start tag.\n";
    }
}

void drawModel(std::string file) {
	std::ifstream inputFile;
	inputFile.open(file);

	if (!inputFile.is_open())
	{
		std::cerr << "Failed to open file: " << file << "\n";
		return;
	}

	std::string line;
	while (std::getline(inputFile, line))
	{
		std::istringstream iss(line);
		std::vector<std::string> tokens;
		std::string token;
		

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);

		while (iss >> token) {
			std::istringstream subIss(token);
			std::string subToken;
			while (std::getline(subIss, subToken, ','))
				tokens.push_back(subToken);
			
			if (tokens.size() == 3) {
				glVertex3f(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]));
				tokens.clear();
			}
		}

		glEnd();
	}

	inputFile.close();
}


void drawGroup(const Group& group) {
    // Push the current matrix to preserve the GL state
    glPushMatrix();

    // Apply transformations
    if (group.hasRotate) {
        glRotatef(group.rotate.angle, group.rotate.x, group.rotate.y, group.rotate.z);
    }
    if (group.hasTranslate) {
        glTranslatef(group.translate.x, group.translate.y, group.translate.z);
    }
    if (group.hasScale) {
        glScalef(group.scale.x, group.scale.y, group.scale.z);
    }

    // Draw each model in this group
    for (const std::string& modelFile : group.models) {
        drawModel(modelFile); // Assuming drawModel takes a file path and draws the model
    }

    // Recursively draw subgroups with their transformations
    for (const Group& subgroup : group.subgroups) {
        drawGroup(subgroup);
    }

    // Pop the matrix after drawing this group and its children to revert to the previous state
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
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-5.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(5.0f, 0.0f, 0.0f);
		// Y Axis in green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -5.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 5.0f, 0.0f);
		// Z Axis in blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -5.0f);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 5.0f);
	glEnd();

	for (const Group& group : sceneGraph) {
		drawGroup(group);
	}
	
	glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy) {

	switch (c) {
		case 'd':
			alfa -= 0.1; break;

		case 'a':
			alfa += 0.1; break;

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

		case 'q': radius -= 0.1f;
			if (radius < 0.1f)
				radius = 0.1f;
			break;

		case 'e': radius += 0.1f; break;
	}

	cameraX = radius * cos(beta) * sin(alfa);
	cameraY = radius * sin(beta);
	cameraZ = radius * cos(beta) * cos(alfa);

	glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) {
	switch (key) {
		case GLUT_KEY_RIGHT:
			alfa -= 0.1; break;

		case GLUT_KEY_LEFT:
			alfa += 0.1; break;

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

		case GLUT_KEY_PAGE_DOWN: radius -= 0.1f;
			if (radius < 0.1f)
				radius = 0.1f;
			break;

		case GLUT_KEY_PAGE_UP: radius += 0.1f; break;
	}
	
	cameraX = radius * cos(beta) * sin(alfa);
	cameraY = radius * sin(beta);
	cameraZ = radius * cos(beta) * cos(alfa);

	glutPostRedisplay();
}


void processMouseButtons(int button, int state, int xx, int yy) {
	
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alfa += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			
			radius -= yy - startY;
			if (radius < 3)
				radius = 3.0;
		}
		tracking = 0;
	}
}


void processMouseMotion(int xx, int yy) {
    int deltaX, deltaY;

    if (!tracking) return;

    deltaX = xx - startX; // How much has the mouse moved?
    deltaY = yy - startY;

    if (tracking == 1) {
        // Adjust angles based on movement
        alfa += deltaX * 0.001; // Adjust rotation speed if necessary
        beta += deltaY * 0.1;

        // Clamp beta to prevent flipping
        if (beta > 85.0) beta = 85.0;
        else if (beta < -85.0) beta = -85.0;
    } else if (tracking == 2) {
        // Zoom in/out
        radius -= deltaY * 0.1; // Adjust zoom speed if necessary
        if (radius < 3) radius = 3;
    }

    // Update camera position based on new angles and radius
    cameraX = radius * cos(beta * M_PI / 180.0) * sin(alfa * M_PI / 180.0);
    cameraY = radius * sin(beta * M_PI / 180.0);
    cameraZ = radius * cos(beta * M_PI / 180.0) * cos(alfa * M_PI / 180.0);

    glutPostRedisplay(); // Mark the current window as needing to be redisplayed
}


int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <xml file>" << std::endl;
		return 1;
	}

	std::ifstream inputFile;
	inputFile.open(argv[1]);

	if (!inputFile.is_open())
	{
		std::cerr << "Failed to open file: " << argv[1] << "\n";
		return 2;
	}

	std::string line;
	Group* currentGroup = nullptr;
	while (std::getline(inputFile, line))
	{
		if (line.find("<window") != std::string::npos) {
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

		} else if (line.find("<position") != std::string::npos) {
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

		} else if (line.find("<lookAt") != std::string::npos) {
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

		} else if (line.find("<up") != std::string::npos) {
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
		} else if (line.find("<projection") != std::string::npos) {
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
		} else if (line.find("<group") != std::string::npos) {
			openGroup();
		} else if (line.find("</group") != std::string::npos) {
			closeGroup();
		} else if (line.find("<translate") != std::string::npos) {
			groupStack.top()->hasTranslate = true;
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

				groupStack.top()->translate.x = std::stof(xStr);
				groupStack.top()->translate.y = std::stof(yStr);
				groupStack.top()->translate.z = std::stof(zStr);
			}
		} else if (line.find("<rotate") != std::string::npos) {
			groupStack.top()->hasRotate = true;
			std::size_t anglePos = line.find("angle=");
			std::size_t xPos = line.find("x=");
			std::size_t yPos = line.find("y=");
			std::size_t zPos = line.find("z=");



			if (anglePos == std::string::npos) {
				anglePos = line.find("angle =");
			} else if (anglePos == std::string::npos) {
				anglePos = line.find("angle= ");
			} else if (anglePos == std::string::npos) {
				anglePos = line.find("angle = ");
			}
			
			if (xPos == std::string::npos) {
				xPos = line.find("x =");
			} else if (xPos == std::string::npos) {
				xPos = line.find("x= ");
			} else if (xPos == std::string::npos) {
				xPos = line.find("x = ");
			}

			if (yPos == std::string::npos) {
				yPos = line.find("y =");
			} else if (yPos == std::string::npos) {
				yPos = line.find("y= ");
			} else if (yPos == std::string::npos) {
				yPos = line.find("y = ");
			}

			if (zPos == std::string::npos) {
				zPos = line.find("z =");
			} else if (zPos == std::string::npos) {
				zPos = line.find("z= ");
			} else if (zPos == std::string::npos) {
				zPos = line.find("z = ");
			}

			if (anglePos != std::string::npos && xPos != std::string::npos && yPos != std::string::npos && zPos != std::string::npos) {
				std::size_t angleStart = line.find("\"", anglePos) + 1;
				std::size_t angleEnd = line.find("\"", angleStart);
				std::size_t xStart = line.find("\"", xPos) + 1;
				std::size_t xEnd = line.find("\"", xStart);
				std::size_t yStart = line.find("\"", yPos) + 1;
				std::size_t yEnd = line.find("\"", yStart);
				std::size_t zStart = line.find("\"", zPos) + 1;
				std::size_t zEnd = line.find("\"", zStart);

				std::string angleStr = line.substr(angleStart, angleEnd - angleStart);
				std::string xStr = line.substr(xStart, xEnd - xStart);
				std::string yStr = line.substr(yStart, yEnd - yStart);
				std::string zStr = line.substr(zStart, zEnd - zStart);

				groupStack.top()->rotate.angle = std::stof(angleStr);
				groupStack.top()->rotate.x = std::stof(xStr);
				groupStack.top()->rotate.y = std::stof(yStr);
				groupStack.top()->rotate.z = std::stof(zStr);
			}
		} else if (line.find("<scale") != std::string::npos) {
			groupStack.top()->hasScale = true;
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

				groupStack.top()->scale.x = std::stof(xStr);
				groupStack.top()->scale.y = std::stof(yStr);
				groupStack.top()->scale.z = std::stof(zStr);
			}
		} else if (line.find("<model") != std::string::npos) {
			std::size_t filePos = line.find("file=");

			if (filePos != std::string::npos) {
				std::size_t fileStart = line.find("\"", filePos) + 1;
				std::size_t fileEnd = line.find("\"", fileStart);

				std::string fileStr = line.substr(fileStart, fileEnd - fileStart);
				groupStack.top()->models.push_back("../Output/" + fileStr);
			}
		}
	}
	// init GLUT and the window
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
		glutInitWindowPosition(100,100);
		glutInitWindowSize(windowWidth, windowHeight);
		glutCreateWindow("CG@DI-UM");
			
	// Required callback registry 
		glutDisplayFunc(renderScene);
		glutReshapeFunc(changeSize);

	
	// Callback registration for keyboard processing
		glutKeyboardFunc(processKeys);
		glutMouseFunc(processMouseButtons);
		glutMotionFunc(processMouseMotion);
		
	// Callback registration for keyboard processing
		glutSpecialFunc(processSpecialKeys);

	//  OpenGL settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
	// enter GLUT's main cycle
		glutMainLoop();

	inputFile.close();

	return 0;
}