#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <boost/make_shared.hpp>

#include <opencv2/opencv.hpp>
#include <Eigen/Core>  
#include <Eigen/Geometry> 
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/common/transforms.h>
#include <pcl/common/projection_matrix.h>




#include <octomap/octomap.h>
#include <octomap/ColorOcTree.h>


#include "KeyFrame.h"
#include "Converter.h"

#ifndef POINTCLOUDMAPPING_H
#define POINTCLOUDMAPPING_H


typedef Eigen::Matrix<double, 6, 1> Vector6d;
typedef pcl::PointXYZRGBA PointT; // A point structure representing Euclidean xyz coordinates, and the RGB color.
typedef pcl::PointCloud<PointT> PointCloud;

namespace ORB_SLAM2 {

class Converter;
class KeyFrame;

class PointCloudMapping {
    public:
        PointCloudMapping(double resolution=0.01);
        ~PointCloudMapping();
        void insertKeyFrame(KeyFrame* kf, const cv::Mat& color, const cv::Mat& depth); // 传入的深度图像的深度值单位已经是m
        void requestFinish();
        bool isFinished();
        void getGlobalCloudMap(PointCloud::Ptr &outputMap);

    private:
        void showPointCloud();
        void generatePointCloud(const cv::Mat& imRGB, const cv::Mat& imD, const cv::Mat& pose, int nId); 

        double mCx, mCy, mFx, mFy, mResolution;
        
        std::shared_ptr<std::thread>  viewerThread;
  
        std::mutex mKeyFrameMtx;
        
        //数据队列  kf color depth
        std::condition_variable mKeyFrameUpdatedCond;
        std::queue<KeyFrame*> mvKeyFrames;
        std::queue<cv::Mat> mvColorImgs, mvDepthImgs;

        bool mbShutdown;
        bool mbFinish;

        std::mutex mPointCloudMtx;
        
        //pointmap` & octomap
        PointCloud::Ptr mPointCloud;
        octomap::ColorOcTree *tree;
       

        // filter
        pcl::VoxelGrid<PointT> voxel;
        pcl::StatisticalOutlierRemoval<PointT> statistical_filter;
};

}
#endif
