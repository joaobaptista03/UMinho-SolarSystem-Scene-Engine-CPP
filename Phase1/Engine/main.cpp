#include <iostream>
#include <fstream>
#include <string> // Include the missing header

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

	int windowWidth = 0;
	int windowHeight = 0;
	int cameraX = 0;
	int cameraY = 0;
	int cameraZ = 0;
	int lookAtX = 0;
	int lookAtY = 0;
	int lookAtZ = 0;
	int upX = 0;
	int upY = 0;
	int upZ = 0;
	int fov = 0;
	int near = 0;
	int far = 0;

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
		}
	}



	return 0;
}