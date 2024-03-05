#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

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
std::vector<std::string> files;

float alfa = 0.0f, beta = 0.0f, radius = 5.0f;

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
		glVertex3f(5.0f, 0.0f, 0.0f);
		// Y Axis in green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -5.0f, 0.0f);
		glVertex3f(0.0f, 5.0f, 0.0f);
		// Z Axis in blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -5.0f);
		glVertex3f(0.0f, 0.0f, 5.0f);
	glEnd();

	for (int i = 0; i < files.size(); i++)
		drawModel(files[i]);

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
		} else if (line.find("<model") != std::string::npos) {
			std::size_t filePos = line.find("file=");

			if (filePos != std::string::npos) {
				std::size_t fileStart = line.find("\"", filePos) + 1;
				std::size_t fileEnd = line.find("\"", fileStart);

				std::string fileStr = line.substr(fileStart, fileEnd - fileStart);

				files.push_back("../Output/" + fileStr);
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
		glutSpecialFunc(processSpecialKeys);

	//  OpenGL settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
	// enter GLUT's main cycle
		glutMainLoop();

	inputFile.close();

	return 0;
}