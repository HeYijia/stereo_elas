#include "elas.h"
#include "slamviewer.h"
#include "thread"
#include "iomanip"

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
    viewer_thread_->detach();

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

        Mat dest;
        stereo_->operator ()(leftImg,rightImg,dest,100);

        Mat show;
        dest.convertTo(show,CV_8U,1.0/8);
        imshow("disp",show);
        waitKey();
    }

}

int main()
{
    TestSystemNode TestSystem;
    TestSystem.runFromFolder();

    return 0;
}
