#include <iostream>
#include <vector>
using namespace std;
double* read_text(const char* fileName, int sizeR, int sizeC);
void write_pgm(const char* filename, double* data, int sizeR, int sizeC, int Q);

class BaseImage {
	
	public:
		int largeImageRowSize = 1024;
		int largeImageCollumnSize = 768;
		int smallImageCollumnSize = 49;
		int smallImageRowSize = 36;

		int getLargeRowSize() {
			return largeImageRowSize;
		}

		int getLargeCollumnSize() {
			return largeImageCollumnSize;
		}

		int getSmallCollumnSize() {
			return smallImageCollumnSize;
		}

		int getSmallRowSize() {
			return smallImageRowSize;
		}

};

class MatchImage : BaseImage {
	private:
		int NNS_score;
		int location_coords[2];

	public:
		int get_NNS_Score() {
			return NNS_score;
		};

		int getXCoordinate() {
			return location_coords[0];
		}
		int getYCoordinate() {
			return location_coords[1];
		}

		MatchImage(int x, int y, int score) {
			location_coords[0] = x;
			location_coords[1] = y;
			NNS_score = score;
		}
};

class LargeImage : BaseImage {
	private:
		//2D array containing arrays which is the data of the whole image
		double** a2dArray = new double* [getLargeRowSize()];

		//2D array containing arrays which is the data of the Small image
		double** a2dArrayForWally = new double* [getSmallRowSize()];


		vector<MatchImage*> matchImages;
		MatchImage* closestMatch = new MatchImage(0, 0, 1000000000);


	public:	

		int NNS(int xStartPosition, int yStartPosition) {
			vector<double>differenceContainer(0);
			int totalNumberOfItems = 0;
			for (int y = 0; y < getSmallCollumnSize(); y++) {
				for (int x = 0; x < getSmallRowSize(); x++) {
					double test = a2dArray[y + yStartPosition][x + xStartPosition];
					if (a2dArrayForWally[x][y] < 255) {
						//Get the difference between the two points in the images and square it
						double difference = a2dArrayForWally[x][y] - a2dArray[y + yStartPosition][x + xStartPosition];
						double squaredDifference = difference * difference;
						differenceContainer.push_back(squaredDifference);
						totalNumberOfItems++;
					}
				}
			}
			double sumOfVector = 0;
			//Add all the values together
			for (int i = 0; i < totalNumberOfItems; i++) {
				sumOfVector += differenceContainer[i];
			}

			//RETURN THE SCORE
			return sumOfVector;
		}

		void findWally() {
			
			for (int y = 0; y < getLargeCollumnSize() - getSmallCollumnSize(); y++) {
				for (int x = 0; x < getLargeRowSize() - getSmallRowSize(); x++) {
					int tempScore = NNS(x, y);
					MatchImage* tempMatchImage = new MatchImage(x, y, tempScore);
					matchImages.push_back(tempMatchImage);
					//cout << "The score is: " << tempMatchImage->get_NNS_Score() << endl;

					if (tempMatchImage->get_NNS_Score() < closestMatch->get_NNS_Score())
						closestMatch = tempMatchImage;
				}
			}
			
			cout << "The Closest Match is: " << closestMatch->getXCoordinate() << "x" << closestMatch->getYCoordinate() << " With a score of " << closestMatch->get_NNS_Score() << endl;
			drawOutlineOnLargeImage(closestMatch->getXCoordinate(), closestMatch->getYCoordinate(), getSmallCollumnSize(), getSmallRowSize());
			
			//Convert 2d Array With Outline Into 1d Array
			double* wallyFoundImage = new double[largeImageRowSize * largeImageCollumnSize];
			int	imageCounter = 0;
			for (int y = 0; y < getLargeCollumnSize(); y++) {
				for (int x = 0; x < getLargeRowSize(); x++) {
					wallyFoundImage[imageCounter] = a2dArray[y][x];
					imageCounter++;
				}
			}

			//OutPut Image
			write_pgm("wallyIsFound.pgm", wallyFoundImage, getLargeCollumnSize(), getLargeRowSize(), 255);

			cout << "Image has been outputted" << endl;
		}

		

		void drawOutlineOnLargeImage(int xStartPos, int yStartPos, int xSize, int ySize) {
			//Draw The Far Left Outline
			for (int i = 0; i < xSize; i++) {
				a2dArray[yStartPos + i][xStartPos] = 1;
			}
			//Draw The Far Right Outline
			for (int i = 0; i < xSize; i++) {
				a2dArray[yStartPos + i][xStartPos + xSize] = 1;
			}

			//Draw The Top Outline
			for (int i = 0; i < ySize; i++) {
				a2dArray[yStartPos][xStartPos + i] = 1;
			}

			//Draw The Bottom Outline
			for (int i = 0; i < ySize; i++) {
				a2dArray[yStartPos + ySize][xStartPos + i] = 1;
			}
		}



		LargeImage() {
			//Array Containing Initial Values Of The Base Image
			double* baseImage = new double[largeImageRowSize * largeImageCollumnSize];
			//Array Containing Values Of The Small Image
			double* wallyImage = new double[smallImageRowSize * smallImageCollumnSize];

			
			//Fill BaseImage with all values from the text file
			baseImage = read_text("Cluttered_scene.txt", 1024, 768);



			//Allocate 36 arrays for each row (so 49x36 arrays)
			for (int i = 0; i < getLargeCollumnSize(); i++)
				a2dArray[i] = new double[getLargeRowSize()];

			//Put data of image into 2d array
			int largeImageCounter = 0;
			for (int y = 0; y < getLargeCollumnSize(); y++) {
				for (int x = 0; x < getLargeRowSize(); x++) {
				
					a2dArray[y][x] = baseImage[largeImageCounter];
					largeImageCounter++;
//					cout << a2dArray[y][x] << endl;
				}
				//cout <<"NEW LINE " << endl;
			}
			
			
			
			//Fill wallyImage array with all values of the small wally text file
			wallyImage = read_text("Wally_grey.txt", 36, 49);

			//Allocate 36 arrays for each row (so 49x36 arrays)
			for (int i = 0; i < getSmallRowSize(); i++)
				a2dArrayForWally[i] = new double[getSmallCollumnSize()];

			//Put data of image into 2d array
			int smallImageCounter = 0;
			for (int y = 0; y < getSmallCollumnSize(); y++) {
				for (int x = 0; x < getSmallRowSize(); x++) {

					a2dArrayForWally[x][y] = wallyImage[smallImageCounter];
					smallImageCounter++;
				}
			}
		}


};
int main()
{
	LargeImage testLargeImage;
	testLargeImage.findWally();
}
