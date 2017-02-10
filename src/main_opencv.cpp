#include "elas.h"
int main()
{
    Mat leftim=imread("urban4_left.pgm");
    Mat rightim=imread("urban4_right.pgm");
    Mat dest;
    StereoEfficientLargeScale elas(0,128);

    // we can set various parameter
        //elas.elas.param.ipol_gap_width=;
        //elas.elas.param.speckle_size=getParameter("speckle_size");
        //elas.elas.param.speckle_sim_threshold=getParameter("speckle_sim");

    elas(leftim,rightim,dest,100);


    Mat show;
    dest.convertTo(show,CV_8U,1.0/8);
    imshow("disp",show);
    waitKey();
    return 0;
}
