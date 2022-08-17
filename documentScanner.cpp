
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


Mat imgOriginal, imgGray, imgBlur, imgCanny, imgDil,imgErode,imgThre,imgWarp,imgCrop;
vector<Point> intialPoints, docPoints;
float w = 420, h = 596;

Mat preProcessing(Mat img)
{
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(7, 7), 5, 0);
	Canny(imgBlur, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);
	return imgDil;
}

	vector<Point> getContours(Mat image) {


		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;


		findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		vector<vector<Point>> conPoly(contours.size());
		vector<Rect> boundRect(contours.size());

		vector<Point> biggest;
		int maxArea=0;


		Point myPoint(0, 0);

		for (int i = 0; i < contours.size(); i++)
		{
			int area = contourArea(contours[i]);
			cout << area << endl;

			string objectType;

			if (area > 1000)
			{
				float peri = arcLength(contours[i], true);
				approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

				if (area > maxArea && conPoly[i].size()==4) {
					biggest = { conPoly[i][0],conPoly[i][1],conPoly[i][2],conPoly[i][3] };
					maxArea = area;
				}
			}
		}
		return biggest;
	}



void drawPoints(vector<Point> points, Scalar color)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(imgOriginal,points[i], 10, color, FILLED);
		putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
	}
}
vector < Point> reorder(vector<Point>points)
{
	vector<Point> newPoints;
	vector<int> sumPoints, subPoints;

	for (int i = 0; i < 4; i++)
	{
		sumPoints.push_back(points[i].x + points[i].y);
		subPoints.push_back(points[i].x - points[i].y);
	}
	newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); 
	newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); 
	newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); 
	newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); 

	return newPoints;
}   

Mat getWarp(Mat img, vector<Point> points, float w, float h)
{
	Point2f src[4] = { points[0],points[1],points[2],points[3] };
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };
	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, imgWarp, matrix, Point(w, h));

	return imgWarp;
}


void main() {


	string path = "Resources/paper.jpg";
	imgOriginal = imread(path);
	resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5);
	imgThre = preProcessing(imgOriginal);
	intialPoints=getContours(imgThre);
	docPoints=reorder(intialPoints);
	imgWarp = getWarp(imgOriginal, docPoints, w, h); 
		int cropVal = 5;
	 Rect roi(cropVal, cropVal, w - (2 * cropVal), h - (2 * cropVal));
	 imgCrop = imgWarp(roi);


	imshow("Image", imgOriginal);
	imshow("Image Dilation", imgThre);
	imshow("Image wrap", imgWarp);
	imshow("Image Crop", imgCrop);
	waitKey(0);

}
