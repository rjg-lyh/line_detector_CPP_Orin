#include "postprocess.hpp"

float sigmoid(float x)
{
    return (1 / (1 + exp(-x)));
}

FitInfo computeEndDots(Mat& mask, cv::Scalar dot_color, vector<Point2i> v){
    // 判断是否合法
    if(v.size() < 10){
        return FitInfo(false);
    }

    for (int i = 0; i < v.size(); i++)
	{
		cv::circle(mask, v[i], 1, dot_color, 1, 8, 0);
	}
 
	cv::Vec4f line_para;
	cv::fitLine(v, line_para, cv::DIST_L1, 0, 1e-2, 1e-2);
 
    // 垂直
    if(line_para[0] == 0){
        return FitInfo(Point2i(line_para[2], 0), Point2i(line_para[2], 10), true);
    }

	// 正常情况
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
    return FitInfo(point1, point2, true);
}


Point2i invertDot(const Point2i& point, int w, int h){
    // (dot21[0]*(w//256),dot21[1]*(h//256)), (dot22[0]*(w//256),dot22[1]*(h//256))
    int x = point.x*(w/256);
    int y = point.y*(h/256);
    return Point2i(x, y);
}

OutInfo* postprocess(Mat& src, float* pdata){
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

    FitInfo fitinfo_2 = computeEndDots(mask, cv::Scalar(0, 255, 0), v2);
    FitInfo fitinfo_3 = computeEndDots(mask, cv::Scalar(255, 0, 0), v3);
    FitInfo fitinfo_4 = computeEndDots(mask, cv::Scalar(0, 0, 255), v4);
	
    // cv::line(mask, pair2.first, pair2.second, cv::Scalar(255, 255, 0), 2, 8, 0);
    // cv::line(mask, pair3.first, pair3.second, cv::Scalar(0, 255, 255), 2, 8, 0);
	// cv::line(mask, pair4.first, pair4.second, cv::Scalar(255, 0, 255), 2, 8, 0);

    // cv::imshow("mask 256✖256", mask);
    // cv::waitKey(0);
    
    OutInfo* outinfo = new OutInfo;
    int w = src.cols;
    int h = src.rows;

    if(fitinfo_2.valid){
        Point2i pair2_inv_1 = invertDot(fitinfo_2.point1, w, h);
        Point2i pair2_inv_2 = invertDot(fitinfo_2.point2, w, h);
        // cv::line(src, pair2_inv_1, pair2_inv_2, cv::Scalar(0, 0, 255), 13, 8, 0); //右主作物行
        draw_dotted_line2(src, pair2_inv_1, pair2_inv_2, cv::Scalar(0, 0, 255), 13, 45);
    }

    if(fitinfo_3.valid){
        Point2i pair3_inv_1 = invertDot(fitinfo_3.point1, w, h);
        Point2i pair3_inv_2 = invertDot(fitinfo_3.point2, w, h);
        // cv::line(src, pair3_inv_1, pair3_inv_2, cv::Scalar(0, 0, 255), 13, 8, 0); //左主作物行
        draw_dotted_line2(src, pair3_inv_1, pair3_inv_2, cv::Scalar(0, 0, 255), 13, 45);
    }

    cv::arrowedLine(src, Point2i(w/2, h-2), Point2i(w/2, h*4/5), Scalar(0,255,0),10); // 中心箭头可视化

    // 导航线拟合失败，直接返回 无效
    if(! fitinfo_4.valid){
        return outinfo;
    }
    outinfo->valid = true;

    Point2i pair4_inv_1 = invertDot(fitinfo_4.point1, w, h);
    Point2i pair4_inv_2 = invertDot(fitinfo_4.point2, w, h);
    // cv::line(src, pair4_inv_1, pair4_inv_2, cv::Scalar(255, 0, 0), 13, 8, 0); //导航线
    draw_dotted_line2(src, pair4_inv_1, pair4_inv_2, cv::Scalar(255, 0, 0), 6, 45);

    int x1 = pair4_inv_1.x; 
    int y1 = pair4_inv_1.y;
    int x2 = pair4_inv_2.x; 
    int y2 = pair4_inv_2.y;

    int x_center = w/2;
    int x0, x_t;
    int y_t = h*4/5;
    float angle;
    double k = 9999999;

    if(x1 == x2){
        x0 = x1;
        x_t = x1;
        angle = 90;
    }
    else{
        k = (y1 - y2)/(x1 - x2);
        x0 = (h - y1)/k + x1;
        x_t = (y_t - y1)/k + x1;
        angle = -rad2deg(atan(k)); //-90 ~ 90，由于y轴是倒着的，所以加个负号
    }
    outinfo->ex = x0 - x_center;
    outinfo->e_angle = angle >= 0 ? (90 - angle):-(90 + angle);

    if(0 < x0 < w){
        cv::arrowedLine(src, Point2i(x0, h-2), Point2i(x_t, y_t), Scalar(255,0,0),10); // 导航线箭头可视化
    }

    cv::putText(src,"lateral_deviation: " + to_string(outinfo->ex),Point(40,30),FONT_HERSHEY_SIMPLEX,2,Scalar(0,0,255),3,8);
    cv::putText(src,"course_deviation: " + to_string(outinfo->e_angle),Point(40,80),FONT_HERSHEY_SIMPLEX,2,Scalar(0,0,255),3,8);
    cv::putText(src,"x0: " + to_string(x0),Point(40,130),FONT_HERSHEY_SIMPLEX,2,Scalar(0,0,255),3,8);
    cv::putText(src,"angle: " + to_string(angle),Point(40,180),FONT_HERSHEY_SIMPLEX,2,Scalar(0,0,255),3,8);
    // cv::putText(src,"k: " + to_string(k),Point(40,230),FONT_HERSHEY_SIMPLEX,2,Scalar(0,0,255),3,8);


    return outinfo;

}