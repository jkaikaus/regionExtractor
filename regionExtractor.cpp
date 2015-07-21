#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <limits>
#include <iterator>

using namespace cv;

std::vector<vector<Point>> scaleHull(int scale, Point mean, std::vector<Point> specificHull, std::vector<vector<Point>> newcontours )
{
    int scalex, scaley;
    std::vector<Point> vector(specificHull.size());
    for (size_t i = 0; i < specificHull.size(); i++)
	{  
        int tempx = specificHull[i].x - mean.x;
        int tempy = specificHull[i].y - mean.y;
        scalex = scale*tempx;
        scaley = scale*tempy;
        scalex+= mean.x;
        scaley+= mean.y;
        vector.push_back(Point(scalex,scaley));
    }
    for (size_t j = 0; j < newcontours.size(); j++)
    {
        for (size_t k = 0; k < vector.size(); k++)
        {
            Point p = vector[k];
            if (p.x != 0 && p.y != 0 )
            {
                newcontours[j].push_back(p);
            }
        }
    }
    return newcontours;
} 

int main(int argc, char *argv[])
{
    Mat img = imread("/home/jkaikaus/Lab/regionExtractor/Samples/3.jpg");
    Mat box, fin;
    cvtColor(img, box, COLOR_BGR2GRAY);
    Size s = img.size(); //obtain image dimensions
	int rows = s.height;
	int cols = s.width;
    Mat mask = Mat::zeros(rows, cols, CV_8UC1); //make mask based off of image dimensions
    RNG rng(12345);
    MSER ms(5, 60, 14400, .25, .2, 200, 1.01, .003, 5);
    std::vector<vector<Point>> regions;
    std::vector<vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    ms(box, regions, Mat());
    Mat g;
    for (size_t i = 0; i < regions.size(); i++)
   	{
        drawContours(mask,regions,i,Scalar::all(255),1,8, vector<Vec4i>(), 0, Point());
        findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
        std::vector<vector<Point>> hull(contours.size());
        for (size_t j = 0; j < contours.size(); j++)
   	    {
            convexHull(Mat(contours[j]), hull[j], false);
            std::vector<Moments> mu(hull.size());
            mu[j]= moments(hull[j], false);
            std::vector<Point2f> mc(hull.size());
            mc[j] = Point2f( mu[j].m10/mu[j].m00 , mu[j].m01/mu[j].m00 );
            std::vector<Point> intmc(mc.size());
            intmc[j]= Point((int)mc[j].x,(int)mc[j].y);
            std::vector<vector<Point>> newcontours1(contours.size());
            std::vector<vector<Point>> newcontours2(contours.size());
            std::vector<vector<Point>> newcontours3(contours.size());
            newcontours1 = scaleHull(1.5, intmc[j], hull[j], newcontours1);
            newcontours2 = scaleHull(2, intmc[j], hull[j], newcontours2);
            newcontours3 = scaleHull(3, intmc[j], hull[j], newcontours3);
            img.copyTo(g);
            drawContours(g,hull,j,Scalar(0,255,255),2,8, vector<Vec4i>(), 0, Point());
            drawContours(g,newcontours1,j,Scalar(0,255,0),2,8, vector<Vec4i>(), 0, Point()); 
            drawContours(g,newcontours2,j,Scalar(0,0,255),2,8, vector<Vec4i>(), 0, Point());
            drawContours(g,newcontours3,j,Scalar(255,0,0),2,8, vector<Vec4i>(), 0, Point());
            circle( g, mc[j], 4, Scalar::all(255), -1, 8, 0 );
            imshow("/home/jkaikaus/Lab/regionExtractor/output/Samples/4.jpg", g);
            waitKey(0);
   	    }
   	}

    //imshow("/home/jkaikaus/Lab/regionExtractor/output/Samples/4.jpg", img);
    //waitKey(0);
   	return 0;
}



