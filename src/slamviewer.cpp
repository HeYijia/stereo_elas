#include "slamviewer.h"
namespace Stereo_Viewer {

bool Viewer::CheckFinish()
{
    std::unique_lock<std::mutex> lock(mMutexFinish);
    return mbSetFinished;
}

void Viewer::SetFinish()
{
    std::unique_lock<std::mutex> lock(mMutexFinish);
    mbSetFinished = true;
}

void Viewer::Finished()
{
    std::unique_lock<std::mutex> lock(mMutexFinish);
    mbFinished = true;
}

bool Viewer::IsFinished()
{
    return mbFinished;
}

Viewer::Viewer(StereoEfficientLargeScale *stereo):
  stereo_(stereo)
{

  mbFinished = false;
  mViewpointX =  0;
  mViewpointY = -0.7;
  mViewpointZ =  -1.8;
  mViewpointF =  500;

  mKeyFrameSize = 0.05;
  mKeyFrameLineWidth = 1.0;
  mCameraSize = 0.08;
  mCameraLineWidth = 3.0;

  mPointSize = 3.0;

}


void Viewer::DrawCurrentCamera(pangolin::OpenGlMatrix &Twc)
{
    const float &w = mCameraSize;
    const float h = w*0.75;
    const float z = w*0.6;

    glPushMatrix();

#ifdef HAVE_GLES
        glMultMatrixf(Twc.m);
#else
        glMultMatrixd(Twc.m);
#endif

    glLineWidth(mCameraLineWidth);
    glColor3f(0.0f,1.0f,0.0f);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(w,h,z);
    glVertex3f(0,0,0);
    glVertex3f(w,-h,z);
    glVertex3f(0,0,0);
    glVertex3f(-w,-h,z);
    glVertex3f(0,0,0);
    glVertex3f(-w,h,z);

    glVertex3f(w,h,z);
    glVertex3f(w,-h,z);

    glVertex3f(-w,h,z);
    glVertex3f(-w,-h,z);

    glVertex3f(-w,h,z);
    glVertex3f(w,h,z);

    glVertex3f(-w,-h,z);
    glVertex3f(w,-h,z);
    glEnd();

    glPopMatrix();
}


void Viewer::DrawSparsePoints()
{

    glPointSize(mPointSize);
    glBegin(GL_POINTS);

   cv::Mat map = stereo_->GetDenseMap();

   if( !map.empty())
   {
       for(size_t y = 2; y< map.rows -2; y++)
         for(size_t x = 2; x< map.cols -2 ; x++)
       {
           if(map.at<float>(y,3*x+2) > 0)
           {
                 float  g = 1/map.at<float>(y,3*x+2) * 1.5;
                 glColor3f(g , g , g );
                glVertex3f(  map.at<float>(y,3*x),map.at<float>(y,3*x+1),map.at<float>(y,3*x+2));
           }

       }
   }

   glEnd();

}

Vec3b Viewer::makeDepthColor(float id)
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

void Viewer::DrawPoints()
{

    glPointSize(mPointSize);
    glBegin(GL_POINTS);

   cv::Mat map = stereo_->GetDenseMap();
   cv::Mat gray = stereo_->GetImg();
   double min,max;
   cv::minMaxLoc(map,&min,&max);

   float fx = 707.0912000000;
   float fy = 707.0912000000;
   float cx = 601.8873;
   float cy = 183.11040 ;
   if( !map.empty())
   {
       for(size_t v = 2; v< map.rows -2; v += 1)
         for(size_t u = 2; u< map.cols -2 ; u+=1)
       {
           if(map.at<short>(v, u) > 0)
           {
               float z =379.8145 / map.at<short>(v,u);

               //cv::Vec3b color = makeDepthColor(1/z);
               //glColor3f(color(0)/255. , color(1)/255., color(2)/255. );
               glColor3f(gray.at< uchar >(v,u)/255. , gray.at< uchar >(v,u)/255.,gray.at< uchar >(v,u)/255.);

              // float z =379.8145 / map.at<short>(v,u);
               float x = z * (u-cx )/ fx;
               float y = z* (v-cy)/fy;

               glVertex3f(  x,y,z);

           }

       }
   }

   glEnd();

}


void Viewer::run()
{

  mbFinished = false;
  mbSetFinished = false;
  pangolin::CreateWindowAndBind("MapViewer: depth map viewer",1024,768);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  pangolin::CreatePanel("menu").SetBounds(0.0,1.0,0.0,pangolin::Attach::Pix(175));
  pangolin::Var<bool> menuFollowCamera("menu.Follow Camera",true,true);
  pangolin::Var<bool> menuShowKeyFrames("menu.Show KeyFrames",true,true);
  pangolin::Var<bool> menuShowPoints("menu.Show SemiDense",true,true);
  pangolin::Var<bool> menuShowSparsePoints("menu.Show SparsePoints",true,true);
  pangolin::Var<bool> menuClose("menu.Close",false,false);


  std::string ss = "Robot";
  pangolin::GlText txt = pangolin::GlFont::I().Text(ss.c_str());

  // Define Camera Render Object (for view / scene browsing)
  pangolin::OpenGlRenderState s_cam(
              pangolin::ProjectionMatrix(1024,768,mViewpointF,mViewpointF,512,389,0.1,1000),
              pangolin::ModelViewLookAt(mViewpointX,mViewpointY,mViewpointZ, 0,0,0,0.0,-1.0, 0.0)
              );

  // Add named OpenGL viewport to window and provide 3D Handler
  pangolin::View& d_cam = pangolin::CreateDisplay()
          .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f/768.0f)
          .SetHandler(new pangolin::Handler3D(s_cam));

  pangolin::OpenGlMatrix Twc;
  Twc.SetIdentity();

  bool bFollow = true;
  while(!CheckFinish())
  {

    usleep(10000);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //mCurrentPose = svo_msg_->getCurrentPose();
    //GetCurrentOpenGLCameraMatrix(Twc);
   // std::cout<<Twc<<std::endl;

    if(menuFollowCamera && bFollow)
    {
        s_cam.Follow(Twc);
    }
    else if(menuFollowCamera && !bFollow)
    {
        s_cam.SetModelViewMatrix(pangolin::ModelViewLookAt(mViewpointX,mViewpointY,mViewpointZ, 0,0,0,0.0,-1.0, 0.0));
        s_cam.Follow(Twc);
        bFollow = true;
    }
    else if(!menuFollowCamera && bFollow)
    {
        bFollow = false;
    }


    d_cam.Activate(s_cam);
    glClearColor(1.0f,1.0f,1.0f,1.0f);


    DrawCurrentCamera(Twc);

    txt.DrawWindow(200,700);

    if(menuShowKeyFrames)
    {
    //  DrawKeyFrames(menuShowKeyFrames);
    }

    if(menuShowPoints)
    {
      DrawPoints();
      //DrawMapRegionPoints();
      //DrawLastFramePoints();
    }
    if(menuShowSparsePoints)
    {
      //DrawSparsePoints();
    }

    pangolin::FinishFrame();

    if(menuClose)
    {
      SetFinish();
    }
  }

  Finished();
  //pangolin::Quit();

  //pangolin::BindToContext("MapViewer: trajactory viewer");

}


}
