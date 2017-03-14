#ifndef SLAMVIEWER_H
#define SLAMVIEWER_H
#include "opencv2/core/core.hpp"
#include "elas.h"

#include "pangolin/pangolin.h"
#include "pangolin/gl/gltext.h"

#include <mutex>

namespace Stereo_Viewer {

class Viewer
{

public:
  Viewer(StereoEfficientLargeScale* stereo );
  void run();
  bool CheckFinish();
  void SetFinish();
  void Finished();
  bool IsFinished();
  void DrawCurrentCamera(pangolin::OpenGlMatrix &Twc);
  void DrawPoints();
  void DrawSparsePoints();

  Vec3b makeDepthColor(float id);

private:
  StereoEfficientLargeScale *stereo_;

  int _drawedframeID=0;

  bool mbFinished;
  bool mbSetFinished;
  std::mutex mMutexFinish;

  float mKeyFrameSize;
  float mKeyFrameLineWidth;
  float mGraphLineWidth;
  float mPointSize;
  float mCameraSize;
  float mCameraLineWidth;

  float mViewpointX, mViewpointY, mViewpointZ, mViewpointF;

};


}
#endif // SLAMVIEWER_H
