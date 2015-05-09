/**
Course: CS536 - Computer Graphics
Student: Zhichao Cao
Email: zc77@drexel.edu
Title: CS536 - Homework 3
*/

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <math.h>
using namespace std ;

string readFile(string strPath);
double calSlope(int qx, int qy, int rx, int ry);
void bresenham(int qx, int qy, int rx, int ry, int sign, int index, double scal, int rota, int xTran, int yTran);
void writePixel(int x, int y, int sign, int index, int length);
void transform(double& x, double& y, int sign, int index, double scal, int rota, int xTran, int yTran, int umin, int vmin, int umax, int vmax, int xl, int yl, int xu, int yu);
int calXpoint(int qx, int rx);
void drawPic(string & strOutput, int countIndex, int xLowerBound, int xUpperBound, int yLowerBound, int yUpperBound);
void scanLineFill(int viewXLowerBound, int viewYLowerBound, int viewXUpperBound, int viewYUpperBound, int xyLnIndex);

double xyOp[200][4];  //Set of pairs of points
double xyOpSlope[200];  //Set of slope of lines
double xOpPath[200][2000];  //Set x-coop of points on line
double yOpPath[200][2000];  //Set y-coop of points on line
double typeOpPath[200];  //Type of slope of lines
double lenOpPath[200];  //Number of points on line
int realWorldCoor[2000][2000];  //For cooperation of real world
int viewWorldCoor[2000][2000];  //For viewport
int scanLine[501][501];
int scanCount[501];
int scanLineCount[501];
int scanLineSet[501][501];
int sortedInter[501][501];
int yMax = -1;
int yMin = 2001;  //For scan-line filling algorithm

const double PI = 3.14159265358979323846;

int main(int argc, char* argv[])
{
	string strOutput = "";  //String for output xpm file
	//string strOutputFile = "out.xpm";  //Output file name
	string strInputFile = "hw3_split.ps";  //-f
	double scalingFactor = 1.0;  //-s
	int cClockwiseRotation = 0;  //-r
	int xTranslation = 0;  //-m
	int yTranslation = 0;  //-n
	int xLowerBound = 0;  //-a
	int yLowerBound = 0;  //-b
	int xUpperBound = 250;  //-c
	int yUpperBound = 250;  //-d
	int viewXLowerBound = 0;  //-j
	int viewYLowerBound = 0;  //-k
	int viewXUpperBound = 200;  //-o
	int viewYUpperBound = 200;  //-p
	string strPS = "";  //Content string in the .ps file
	string strSetPS[200];  //Split strPS string
	int stIndex = 0;  
	int edIndex = 0;
	int stIndexOp = 0;
	string tempStrOp = "";
	int xyLnIndex = 0;  //Number of lines
	int xyOpIndex = 0;  //Index for x,y-coop, for line command
	int fstPointIndex = 0;  //Temp index for first point of moveto and lineto command
	int sndPointIndex = 0;  //Temp index for second point of moveto and lineto command
	int countIndex = 0;  //Number of lines in .ps file
	string strLength = "";  //Length of pic
	string strWidth = "";  //Width of pic
	int lineType = 0;  //Type of command in .ps file. 1 for line, 2 for moveto, 3 for lineto

	if(argv[1] == ">" && argv[2] == "out.xpm")
	{
		//./CG_hw1 -f hw1.ps -a 0 -b 0 -c 499 -d 499 -s 1.0 -m 0 -n 0 -r 0 > out.xpm
		strInputFile = "hw3_split.ps";
		scalingFactor = 1.0;
		cClockwiseRotation = 0;
		xTranslation = 0;
		yTranslation = 0;
		xLowerBound = 0;
		yLowerBound = 0;
		xUpperBound = 250;
		yUpperBound = 250;
		viewXLowerBound = 0;
		viewYLowerBound = 0;
		viewXUpperBound = 200;
		viewYUpperBound = 200;
		//strOutputFile = "out.xpm";
	}
	else
	{
		for(int i = 0; i < argc; i++)
		{
			string tempStr = "";
			tempStr = argv[i];
			if(tempStr == "-f")
			{
				strInputFile = argv[i+1];
			}
			else if(tempStr == "-s")
			{
				scalingFactor = atof(argv[i+1]);
			}
			else if(tempStr == "-r")
			{
				cClockwiseRotation = atoi(argv[i+1]);
			}
			else if(tempStr == "-m")
			{
				xTranslation = atoi(argv[i+1]);
			}
			else if(tempStr == "-n")
			{
				yTranslation = atoi(argv[i+1]);
			}
			else if(tempStr == "-a")
			{
				xLowerBound = atoi(argv[i+1]);
			}
			else if(tempStr == "-b")
			{
				yLowerBound = atoi(argv[i+1]);
			}
			else if(tempStr == "-c")
			{
				xUpperBound = atoi(argv[i+1]);
			}
			else if(tempStr == "-d")
			{
				yUpperBound = atoi(argv[i+1]);
			}
			else if(tempStr == "-j")
			{
				viewXLowerBound = atoi(argv[i+1]);
			}
			else if(tempStr == "-k")
			{
				viewYLowerBound = atoi(argv[i+1]);
			}
			else if(tempStr == "-o")
			{
				viewXUpperBound = atoi(argv[i+1]);
			}
			else if(tempStr == "-p")
			{
				viewYUpperBound = atoi(argv[i+1]);
			}
			else if(tempStr == ">")
			{
				//strOutputFile = argv[i+1];
			}
		}
		//cout << strInputFile << " " << scalingFactor << " "  << cClockwiseRotation << " "  << xTranslation << " "  << yTranslation << " "  << xLowerBound << " "  << yLowerBound << " "  << xUpperBound << " "  << yUpperBound << " "  << endl;
	}

	strPS = readFile(strInputFile);
	//cout << strPS << endl;

	for(int i = 0; i < strPS.length(); i++)
	{
		if(strPS[i] == '\n')
		{
			strSetPS[countIndex] = strPS.substr(stIndex, i - stIndex);
			stIndex = i + 1;
			countIndex++;
		}
	}

	for (int i = 0; i < countIndex; i++)
	{
		//cout << strSetPS[i] << endl;
		if(strSetPS[i].substr(0, 8) == "%%%BEGIN")
		{
			stIndex = i + 1;
		}
		else if(strSetPS[i].substr(0, 6) == "%%%END")
		{
			edIndex = i - 1;
		}
		else
		{
			continue;
		}
	}

	for (int i = stIndex; i <= edIndex; i++)
	{
		std::size_t posLineLow = strSetPS[i].find("line");
		std::size_t posLineUp = strSetPS[i].find("Line");
		std::size_t posLineToLow = strSetPS[i].find("lineto");
		std::size_t posLineToUp = strSetPS[i].find("Lineto");
		std::size_t posLineToUpAll = strSetPS[i].find("LineTo");
		std::size_t posMoveToLow = strSetPS[i].find("moveto");
		std::size_t posMoveToUp = strSetPS[i].find("Moveto");
		std::size_t posMoveToUpAll = strSetPS[i].find("MoveTo");
		std::size_t posStrokeLow = strSetPS[i].find("stroke");
		std::size_t posStrokeUp = strSetPS[i].find("Stroke");
		if(posLineToLow != std::string::npos || posLineToLow != std::string::npos || posLineToUpAll != std::string::npos)
		{
			lineType = 3;
		}
		else if(posMoveToLow != std::string::npos || posMoveToUp != std::string::npos || posMoveToUpAll != std::string::npos)
		{
			lineType = 2;
		}
		else if((posLineLow != std::string::npos || posLineUp != std::string::npos) && (posLineToLow == std::string::npos || posLineToLow == std::string::npos || posLineToUpAll == std::string::npos))
		{
			lineType = 1;
		}
		//else if(posStrokeLow != std::string::npos || posStrokeUp != std::string::npos)
		else
		{
			fstPointIndex = 0;
			sndPointIndex = 2;
			if(xyLnIndex > 0)
			{
				//xyLnIndex--;
				xyOp[xyLnIndex - 1][0] = 0;
				xyOp[xyLnIndex - 1][1] = 0;
			}
			continue;
		}
		for (int j = 0; j < strSetPS[i].length(); j++)
		{			
			if(lineType == 2)
			{
				if(strSetPS[i][j] == ' ' && (j == 0 || strSetPS[i][j - 1] == ' '))
				{
					stIndexOp = j + 1;
				}
				else if(strSetPS[i][j] == ' '&& strSetPS[i][j + 1] == ' ')
				{
					tempStrOp = strSetPS[i].substr(stIndexOp, j - stIndexOp);
					xyOp[xyLnIndex][fstPointIndex] = atof(tempStrOp.c_str());
					stIndexOp = j + 1;
					fstPointIndex++;
				}
				else if(strSetPS[i][j] == ' ')
				{
					tempStrOp = strSetPS[i].substr(stIndexOp, j - stIndexOp);
					xyOp[xyLnIndex][fstPointIndex] = atof(tempStrOp.c_str());
					stIndexOp = j + 1;
					fstPointIndex++;
				}
			}
			else if(lineType == 3)
			{
				if(strSetPS[i][j] == ' ' && (j == 0 || strSetPS[i][j - 1] == ' '))
				{
					stIndexOp = j + 1;
				}
				else if(strSetPS[i][j] == ' ' && strSetPS[i][j + 1] == ' ')
				{
					tempStrOp = strSetPS[i].substr(stIndexOp, j - stIndexOp);
					xyOp[xyLnIndex - 1][sndPointIndex] = atof(tempStrOp.c_str());
					stIndexOp = j + 1;
					xyOp[xyLnIndex][fstPointIndex] = atof(tempStrOp.c_str());
					fstPointIndex++;
					sndPointIndex++;
				}
				else if(strSetPS[i][j] == ' ')
				{
					tempStrOp = strSetPS[i].substr(stIndexOp, j - stIndexOp);
					xyOp[xyLnIndex - 1][sndPointIndex] = atof(tempStrOp.c_str());
					stIndexOp = j + 1;
					xyOp[xyLnIndex][fstPointIndex] = atof(tempStrOp.c_str());
					fstPointIndex++;
					sndPointIndex++;
				}
			}
			else if(lineType == 1)
			{
				if(strSetPS[i][j] == ' ' && (j == 0 || strSetPS[i][j - 1] == ' '))
				{
					stIndexOp = j + 1;
				}
				else if(strSetPS[i][j] == ' ' && strSetPS[i][j + 1] == ' ')
				{
					tempStrOp = strSetPS[i].substr(stIndexOp, j - stIndexOp);
					xyOp[xyLnIndex][xyOpIndex] = atof(tempStrOp.c_str());
					stIndexOp = j + 1;
					xyOpIndex++;
				}
				else if(strSetPS[i][j] == ' ')
				{
					tempStrOp = strSetPS[i].substr(stIndexOp, j - stIndexOp);
					xyOp[xyLnIndex][xyOpIndex] = atof(tempStrOp.c_str());
					stIndexOp = j + 1;
					xyOpIndex++;
				}
			}
			/*cout << xyLnIndex << endl;
			cout << xyOp[xyLnIndex][xyOpIndex] << endl;*/
		}
		//cout << xyOp[0][0] << " " << xyOp[0][1] << " "  << xyOp[0][2] << " "  << xyOp[0][3] << endl;
		//cout << xyLnIndex << endl;
		//cout << xyOp[xyLnIndex][0] << " " << xyOp[xyLnIndex][1] << " "  << xyOp[xyLnIndex][2] << " "  << xyOp[xyLnIndex][3] << endl;
		xyOpIndex = 0;
		fstPointIndex = 0;
		sndPointIndex = 2;
		stIndexOp = 0;
		xyLnIndex++;
	}
	/*for(int i = 0; i < xyLnIndex; i++)
	{
		cout << xyOp[i][0] << " " << xyOp[i][1] << " "  << xyOp[i][2] << " "  << xyOp[i][3] << endl;
	}*/
	/*xScale = static_cast<double>(viewXUpperBound - viewXLowerBound) / (xUpperBound - xLowerBound);
	yScale = static_cast<double>(viewYUpperBound - viewYLowerBound) / (yUpperBound - yLowerBound);*/
	for (int i = 0; i < xyLnIndex; i++)
	{
		double tempX, tempY = 0;			
		transform(xyOp[i][0], xyOp[i][1], 0, i, scalingFactor, cClockwiseRotation, xTranslation, yTranslation, viewXLowerBound, viewYLowerBound, viewXUpperBound, viewYUpperBound, xLowerBound, yLowerBound, xUpperBound, yUpperBound);
		transform(xyOp[i][2], xyOp[i][3], 0, i, scalingFactor, cClockwiseRotation, xTranslation, yTranslation, viewXLowerBound, viewYLowerBound, viewXUpperBound, viewYUpperBound, xLowerBound, yLowerBound, xUpperBound, yUpperBound);
		if(calXpoint(xyOp[i][0], xyOp[i][2]) == -1)
		{
			//xyOpSlope[i] = calSlope(xyOp[i][2], xyOp[i][3], xyOp[i][0], xyOp[i][1]);
			tempX = xyOp[i][0];
			tempY = xyOp[i][1];
			xyOp[i][0] = xyOp[i][2];
			xyOp[i][1] = xyOp[i][3];
			xyOp[i][2] = tempX;
			xyOp[i][3] = tempY;
		}
		//else
		//{
		//	xyOpSlope[i] = calSlope(xyOp[i][0], xyOp[i][1], xyOp[i][2], xyOp[i][3]);
		//}
		xyOpSlope[i] = calSlope(xyOp[i][0], xyOp[i][1], xyOp[i][2], xyOp[i][3]);
		
		//cout << xyOp[i][0] << " " << xyOp[i][1] << " "  << xyOp[i][2] << " "  << xyOp[i][3] << " " << xyOpSlope[i] << endl;
		
		if(xyOpSlope[i] >= 0 && xyOpSlope[i] <= 1)
		{
			typeOpPath[i] = 0;
			//No need for modification
			bresenham(xyOp[i][0], xyOp[i][1], xyOp[i][2], xyOp[i][3], typeOpPath[i], i, scalingFactor, cClockwiseRotation, xTranslation, yTranslation);
		}
		else if(xyOpSlope[i] >= -1 && xyOpSlope[i] < 0)
		{
			typeOpPath[i] = 1;
			//Switch y1 and y2 to make 0<=slope<=1
			bresenham(xyOp[i][0], -xyOp[i][1], xyOp[i][2], -xyOp[i][3], typeOpPath[i], i, scalingFactor, cClockwiseRotation, xTranslation, yTranslation);
		}
		else if(xyOpSlope[i] > 1)
		{
			typeOpPath[i] = 2;
			//(y1, x1)(y2, x2) to make 0<=slope<=1
			bresenham(xyOp[i][1], xyOp[i][0], xyOp[i][3], xyOp[i][2], typeOpPath[i], i, scalingFactor, cClockwiseRotation, xTranslation, yTranslation);
		}
		else if(xyOpSlope[i] < -1)
		{
			typeOpPath[i] = 3;
			//(y2, -x2) and (y1, -x1) to make 0<=slope<=1
			bresenham(xyOp[i][3], -xyOp[i][2], xyOp[i][1], -xyOp[i][0], typeOpPath[i], i, scalingFactor, cClockwiseRotation, xTranslation, yTranslation);
		}
		//else if(xyOpSlope[i] == 1000)
		//{
		//	for (int j = xyOp[i][1]; j <= xyOp[i][3]; j++)
		//	{
		//		writePixel(xyOp[i][0], j, 4, i, (j - xyOp[i][1]));
		//	}
		//	//bresenham(xyOp[i][0], xyOp[i][1], xyOp[i][2], xyOp[i][3], 4, i);
		//}
		//else if(xyOpSlope[i] == 1000)
		//{
		//	for (int j = xyOp[i][3]; j <= xyOp[i][1]; j++)
		//	{
		//		writePixel(xyOp[i][0], j, 5, i, (j - xyOp[i][3]));
		//	}
		//	//bresenham(xyOp[i][0], xyOp[i][1], xyOp[i][2], xyOp[i][3], 5, i);
		//}
	}
	for(int k = 0; k < countIndex; k++)
	{
		int xCoop = 0;
		int yCoop = 0;
		for (int l = 0; l < lenOpPath[k]; l++)
		{					
			xCoop = xOpPath[k][l];
			yCoop = yOpPath[k][l];
			realWorldCoor[xCoop][yCoop] = 1;
			if((xCoop > viewXLowerBound && xCoop <= viewXUpperBound) && (yCoop > viewYLowerBound && yCoop <= viewYUpperBound))
			{
				viewWorldCoor[xCoop][yCoop] = 1;
				if(yCoop > yMax)
				{
					yMax = yCoop;
				}
				else if(yCoop < yMin)
				{
					yMin = yCoop;
				}
			}
		}
	}

	scanLineFill(viewXLowerBound, viewYLowerBound, viewXUpperBound, viewYUpperBound, xyLnIndex);

	std::stringstream sl;
	sl << (xUpperBound - xLowerBound + 1);
	strLength = sl.str();
	std::stringstream sw;
	sw << (yUpperBound - yLowerBound + 1);
	strWidth = sw.str();
	
	strOutput = "/* XPM */\n";
	strOutput += "static char *quad_bw[] = {\n";
	strOutput += "/* columns rows colors chars-per-pixel */\n";
	//strOutput += "\"" + strLength + " " + strWidth + " 2 1\",\n";
	strOutput += "\" 501 501 2 1\",\n";
	strOutput += "/* pixels */\n";
	strOutput += "\"@ c #000000\",\n";
	strOutput += "\"  c #FFFFFF\",\n";

	drawPic(strOutput, countIndex, xLowerBound, xUpperBound, yLowerBound, yUpperBound);

	cout << strOutput;

	return 0;
}

string readFile(string strPath)
{
	//Read the ps file
	string strPS = "";
	char chStrPath[20];
	strcpy(chStrPath, strPath.c_str());
	char buffer[256];
	ifstream hsfile;
	hsfile.open(chStrPath);
	if(!hsfile){
        cout << "Unable to open the file";
        exit(1);  // terminate with error
	}
	while (!hsfile.eof())
	{
		//strPS = hsfile.get();
		hsfile.getline(buffer,50);
		//cout << buffer << endl;
		strPS += buffer;
		strPS += "\n";
	}
	hsfile.close();
	return strPS;
}

void bresenham(int qx, int qy, int rx, int ry, int sign, int index, double scal, int rota, int xTran, int yTran)
{
	//Bresenham's algorithm
	int length_line = 0;
	int dx, dy, D, x, y = 0;
	//cout<<qx<<","<<qy<<" "<<rx<<","<<ry<<endl;
	dx = rx - qx;
	dy = ry - qy;
	D = 2 * dy - dx;
	y = qy;
	for (x = qx; x <= rx; x++)
	{
		writePixel(x, y, sign, index, length_line);
		//cout << x << "," << y << endl;
		if(D <= 0)
		{
			D += 2 * dy;
		}
		else
		{
			D += 2 * (dy -dx) ;
			y++;
		}
		length_line++;
	}
	lenOpPath[index] = length_line;
}

void writePixel(int x, int y, int sign, int index, int length)
{
	//Generating mapping point of the line
	switch (sign)
	{
		case 0:
			xOpPath[index][length] = x;
			yOpPath[index][length] = y;
			break;
		case 1:
			xOpPath[index][length] = x;
			yOpPath[index][length] = -y;
			break;
		case 2:	
			xOpPath[index][length] = y;
			yOpPath[index][length] = x;
			break;
		case 3:
			xOpPath[index][length] = -y;
			yOpPath[index][length] = x;
			break;
		/*case 4:
		case 5:
			xOpPath[index][length] = x;
			yOpPath[index][length] = y;
			break;*/
		default:
			xOpPath[index][length] = 0;
			yOpPath[index][length] = 0;
			cout << "Points fail" << endl;
			break;
	}
}

double calSlope(int qx, int qy, int rx, int ry)
{
	//Calculate the slope of the line
	double slope = 0.0;
	if(rx - qx != 0)
		slope = static_cast<double> (ry - qy) / (rx - qx);
	else
	{
		if(ry - qy < 0)
			slope = -1000;
		else if(ry - qy > 0)
			slope = 1000;
		else if (ry - qy == 0)
			slope = 0;
	}
	return slope;
}

int calXpoint(int qx, int rx)
{
	//Calculate the distance of points of x-coop
	if(rx - qx < 0)
	{
		return -1;
	}
	else if(rx - qx == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void drawPic(string & strOutput, int countIndex, int xLowerBound, int xUpperBound, int yLowerBound, int yUpperBound)
{
	//Building output string for xpm file
	int pExist = 0;
	int xCoop = 0;
	int yCoop = 0;
	yUpperBound = 500;
	yLowerBound = 0;
	xUpperBound = 500;
	xLowerBound = 0;
	for(int i = 0; i <= (yUpperBound - yLowerBound); i++)
	{
		//Clipping line by Sutherland-Hodgman Algorithm 
		strOutput += "\"";
		for (int j = xLowerBound; j <= xUpperBound; j++)
		{
			if(realWorldCoor[j][yUpperBound -i] == 1 && viewWorldCoor[j][yUpperBound -i] == 1)
			{
				strOutput += "@";
			}
			else
			{
				strOutput += " ";
			}
		}
		if(i == yUpperBound)
		{
			strOutput += "\"\n};";
		}
		else
		{
			strOutput += "\",\n";
		}
	}
}

void transform(double& x, double& y, int sign, int index, double scal, int rota, int xTran, int yTran, int umin, int vmin, int umax, int vmax, int xl, int yl, int xu, int yu)
{

	//Transformation of lines
	double xScale = static_cast<double>(umax - umin) / (xu - xl);
	double yScale = static_cast<double>(vmax - vmin) / (yu - yl);
	double tempX = x * scal;
	double tempY = y * scal;;
	double cosValue = cos((rota*PI)/180.0);
	double sinValue = sin((rota*PI)/180.0);
	x = tempX * cosValue - tempY * sinValue + xTran;
	y = tempX * sinValue + tempY * cosValue + yTran;
	x = (x - xl) * xScale + umin;
	y = (y - yl) * yScale + vmin;
}

void scanLineFill(int viewXLowerBound, int viewYLowerBound, int viewXUpperBound, int viewYUpperBound, int xyLnIndex)
{
	
	for(int i = 0; i < 501; i++)
	{
		//Initialization
		scanCount[i] = 0;
		scanLineCount[i] = 0;
		for(int j = 0; j < 501; j++)
		{
			scanLine[i][j] = 0;
			scanLineSet[i][j] = -1;
			sortedInter[i][j] = -1;
		}
	}
	for(int i = yMax; i >= yMin; i--)
	{
		int count = 0;
		for (int j = 0; j < xyLnIndex; j++)
		{
			if(xyOpSlope[j] == 0)
			{
				//Ignore
			}
			else if(xyOp[j][3] > xyOp[j][1] && i == xyOp[j][3])
			{
				//Ignore
			}
			else if(xyOp[j][3] < xyOp[j][1] && i == xyOp[j][1])
			{
				//Ignore
			}
			else if(xyOp[j][3] > xyOp[j][1] && (i < xyOp[j][3] && i >= xyOp[j][1]))
			{
				//Add to scan line set
				scanLine[count][i] = j;
				count++;
				scanLineCount[i]++;
			}
			else if(xyOp[j][3] < xyOp[j][1] && (i >= xyOp[j][3] && i < xyOp[j][1]))
			{
				//Add to scan line set
				scanLine[count][i] = j;
				count++;
				scanLineCount[i]++;
			}
		}
	}
	for(int i = yMax; i >= yMin; i--)
	{
		//Calculate intersections
		int count = 0;
		for(int j = 0; j < scanLineCount[i]; j++)
		{
			int lIndex = scanLine[j][i];
			for(int k = 0; k < 2000; k++)
			{
				if(yOpPath[lIndex][k] == i)
				{
					sortedInter[count][i] = xOpPath[lIndex][k];
					count++;
					scanCount[i]++;
					break;
				}
			}
		}
	}
	for(int i = yMax; i >= yMin; i--)
	{
		//Sort the intersections in x
		for(int j = 0; j < scanLineCount[i]; j++)
		{
			for(int k = 0; k < scanLineCount[i] - 1; k++)
			{
				int temp = 0;
				if(sortedInter[k][i] > sortedInter[k + 1][i])
				{
					temp = sortedInter[k][i];
					sortedInter[k][i] = sortedInter[k + 1][i];
					sortedInter[k + 1][i] = temp;
				}
			}
		}
	}
	/*for(int i = yMax; i >= yMin; i--)
	{
		cout<<i<<endl;
		for(int j = 0; j < scanLineCount[i]; j++)
		{
			cout<<sortedInter[j][i]<<" ";
		}
		cout<<scanLineCount[i]<<endl;
	}*/
	for(int i = yMax; i >= yMin; i--)
	{
		int count = 0;
		for (int j = 0; j < 501; j++)
		{
			if(realWorldCoor[j][i] == 0 && j > sortedInter[count][i] && j < sortedInter[count + 1][i])
			{
				realWorldCoor[j][i] = 1;
				if((j >= viewXLowerBound && j <= viewXUpperBound) && (i >= viewYLowerBound && i <= viewYUpperBound))
				{
					viewWorldCoor[j][i] = 1;
				}
			}
			else if(j == sortedInter[count + 1][i])
			{
				if(sortedInter[count][i] == sortedInter[count + 1][i])
				{
					j = j - 1;
				}
				if(scanLineCount[i] > (count + 2) || scanCount[i] > (count + 2))
				{					
					count = count + 2;
				}
				else
				{
					break;
				}
			}
		}
	}
}
