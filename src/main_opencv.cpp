#include "elas.h"
#include "slamviewer.h"
#include "thread"
#include "iomanip"

#include <opencv2/core/core.hpp>

class TestSystemNode
{
      StereoEfficientLargeScale* stereo_;

      std::thread * viewer_thread_;
      Stereo_Viewer::Viewer* viewer_;

public:
    TestSystemNode();
    ~TestSystemNode();
    void runFromFolder();
};

TestSystemNode::~TestSystemNode()
{
      delete stereo_;
      delete viewer_;
      delete viewer_thread_;
}
TestSystemNode::TestSystemNode()
{

    stereo_ = new StereoEfficientLargeScale(0, 128);

    viewer_ = new Stereo_Viewer::Viewer(stereo_);
    viewer_thread_ = new std::thread(&Stereo_Viewer::Viewer::run,viewer_);
    //viewer_thread_->detach();

}

Vec3b makeDepthColor(float id)
{
        if(id <= 0) return Vec3b(128,0,0);
        if(id >= 1) return Vec3b(0,0,128);

        int icP = (id*8);
        float ifP = (id*8)-icP;

        if(icP == 0) return Vec3b(255*(0.5+0.5*ifP), 		    		  0,     					0);
        if(icP == 1) return Vec3b(255, 					  255*(0.5*ifP),     					0);
        if(icP == 2) return Vec3b(255, 				  255*(0.5+0.5*ifP),     					0);
        if(icP == 3) return Vec3b(255*(1-0.5*ifP), 					255,     					255*(0.5*ifP));
        if(icP == 4) return Vec3b(255*(0.5-0.5*ifP), 					255,     					255*(0.5+0.5*ifP));
        if(icP == 5) return Vec3b(0, 						255*(1-0.5*ifP),     					255);
        if(icP == 6) return Vec3b(0, 						255*(0.5-0.5*ifP),     					255);
        if(icP == 7) return Vec3b(0, 					  				  0,     					255*(1-0.5*ifP));

}

void TestSystemNode::runFromFolder()
{
    cv::Mat leftImg, rightImg;

    for (int i=10; ; i++)
    {
        std::stringstream leftFile_ss;
        std::stringstream rightFile_ss;
        leftFile_ss<<"/home/hyj/bagfiles/stereo/08/image_0/"<<std::setw(6) << std::setfill('0') <<i<< ".png";
        rightFile_ss<<"/home/hyj/bagfiles/stereo/08/image_1/"<<std::setw(6) << std::setfill('0') <<i<< ".png";
        //leftFile_ss<<"/home/hyj/bagfiles/stereo/new_1/left_img/"<<std::setw(6) << std::setfill('0') <<i<< ".jpg";
        //rightFile_ss<<"/home/hyj/bagfiles/stereo/new_1/right_img/"<<std::setw(6) << std::setfill('0') <<i<< ".jpg";

        std::cout << "reading image " << leftFile_ss.str() << std::endl;
        leftImg = imread(leftFile_ss.str().c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        rightImg = imread(rightFile_ss.str().c_str(), CV_LOAD_IMAGE_GRAYSCALE);

        if(leftImg.data == NULL || rightImg.data == NULL) {
            break;
        }

        double t = (double)cvGetTickCount();

        cv::Mat gradx = cv::Mat::zeros(leftImg.rows, leftImg.cols, CV_32F);
        cv::Mat grady = cv::Mat::zeros(leftImg.rows, leftImg.cols, CV_32F);
        cv::Mat mag =  cv::Mat::zeros(leftImg.rows, leftImg.cols, CV_32F);
        cv::Mat img;
        cv::GaussianBlur( leftImg, img, cv::Size( 3, 3 ), 0, 0 );
        cv::Scharr(img, gradx, CV_32F, 1, 0, 1/32.0);
        cv::Scharr(img, grady, CV_32F, 0, 1, 1/32.0);
        cv::magnitude(gradx,grady,mag);
        cv::Mat ipOut;
        cv::threshold(mag, ipOut, 15, 0, THRESH_TOZERO);

        Mat dest;

        stereo_->operator ()(leftImg,rightImg,dest,100,mag);

        t=(double)cvGetTickCount()-t;
        printf("used time is %gms\n",(t/(cvGetTickFrequency()*1000)));

        Mat show;
        dest.convertTo(show,CV_8U,1.0/8);
        imshow("disp",show);
        imshow("left",leftImg);
        imshow("mag",mag);

        cv::Mat result;
        cv::cvtColor(leftImg,result,CV_GRAY2BGR);
        cv::Mat map = stereo_->GetDenseMap();
        for(size_t v = 2; v< map.rows -2; v += 1)
          for(size_t u = 2; u< map.cols -2 ; u+=1)
        {
            //if(mag.at<float>(v,u) < 10) continue;
            if(map.at<short>(v, u) > 0)
            {
              float z =379.8145 / map.at<short>(v,u);
              cv::Vec3b color = makeDepthColor(1/z);
              result.at<Vec3b>(v,u)[0]=color[2];
              result.at<Vec3b>(v,u)[1]=color[1];
              result.at<Vec3b>(v,u)[2]=color[0];
            }

          }
        cv::imshow("result",result);
        char c = waitKey(1);
        if(c == 27)break;


    }

    if(viewer_->IsFinished())
        std::cout<<"finished"<<std::endl;
    //usleep(500000);

}

int main()
{
    TestSystemNode TestSystem;
    TestSystem.runFromFolder();

    return 0;
}
