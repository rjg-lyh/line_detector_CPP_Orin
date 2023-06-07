#include "postprocess.hpp"

float sigmoid(float x)
{
    return (1 / (1 + exp(-x)));
}

pair<Point2i, Point2i> computeEndDots(Mat& image, cv::Scalar dot_color, vector<Point2i> v){
    for (int i = 0; i < v.size(); i++)
	{
		cv::circle(image, v[i], 1, dot_color, 1, 8, 0);
	}
 
	cv::Vec4f line_para;
	cv::fitLine(v, line_para, cv::DIST_L1, 0, 1e-2, 1e-2);
 
	std::cout << "line_para = " << line_para << std::endl;
 
	//获取点斜式的点和斜率
	cv::Point point0;
	point0.x = line_para[2];
	point0.y = line_para[3];
 
	double k = line_para[1] / line_para[0];
 
	//计算直线的端点(y = k(x - x0) + y0)
	cv::Point point1, point2;
	point1.x = 0;
	point1.y = k * (0 - point0.x) + point0.y;
	point2.x = 640;
	point2.y = k * (640 - point0.x) + point0.y;
    return pair<Point2i, Point2i>(point1, point2);
}

Point2i convertDot(const Point2i& point, int w, int h){
    // (dot21[0]*(w//256),dot21[1]*(h//256)), (dot22[0]*(w//256),dot22[1]*(h//256))
    int x = point.x*(w/256);
    int y = point.y*(h/256);
    return Point2i(x, y);
}

Mat drawLine(const Mat& src, float* pdata){
    vector<Point2i> v1;
    vector<Point2i> v2;
    vector<Point2i> v3;
    vector<Point2i> v4;
    
    //求出mask
    for(int i=0; i<256*256; ++i){
        int dx = i%256;
        int dy = i/256;
        int area = 256*256;
        float* p_c1 = pdata;
        float* p_c2 = p_c1 + area;
        float* p_c3 = p_c2 + area;
        float* p_c4 = p_c3 + area;
        if(sigmoid(*p_c1) > 0.5){
            v1.emplace_back(Point2i(dx, dy));
        }
        if(sigmoid(*p_c2) > 0.5){
            v2.emplace_back(Point2i(dx, dy));
        }
        if(sigmoid(*p_c3) > 0.5){
            v3.emplace_back(Point2i(dx, dy));
        }
        if(sigmoid(*p_c4) > 0.5){
            v4.emplace_back(Point2i(dx, dy));
        }
        ++pdata;
    }
    
    Mat mask = cv::Mat::zeros(256, 256, CV_8UC3);
	//将拟合点绘制到空白图上

    auto pair2 = computeEndDots(mask, cv::Scalar(0, 255, 0), v2);
    auto pair3 = computeEndDots(mask, cv::Scalar(255, 0, 0), v3);
    auto pair4 = computeEndDots(mask, cv::Scalar(0, 0, 255), v4);
	
    cv::line(mask, pair2.first, pair2.second, cv::Scalar(255, 255, 0), 2, 8, 0);
    cv::line(mask, pair3.first, pair3.second, cv::Scalar(0, 255, 255), 2, 8, 0);
	cv::line(mask, pair4.first, pair4.second, cv::Scalar(255, 0, 255), 2, 8, 0);

    // cv::imshow("mask 256✖256", mask);
    // cv::waitKey(0);
    
    Mat image(src);
    int w = image.cols;
    int h = image.rows;
    cv::line(src, convertDot(pair2.first, w, h), convertDot(pair2.second, w, h), cv::Scalar(255, 0, 0), 13, 8, 0); //右主作物行
    cv::line(src, convertDot(pair3.first, w, h), convertDot(pair3.second, w, h), cv::Scalar(0, 0, 255), 13, 8, 0); //左主作物行
    cv::line(src, convertDot(pair4.first, w, h), convertDot(pair4.second, w, h), cv::Scalar(0, 255, 0), 13, 8, 0); //导航线

	return image;

}