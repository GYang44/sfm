#define CERES_FOUND true

#include <opencv2/sfm.hpp>
#include <opencv2/viz.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include "hc.hpp"

int getdir(const std::string _filename, std::vector<std::string> &files)
{
    std::ifstream myfile(_filename.c_str());
    if (!myfile.is_open()) {
        std::cout << "Unable to read file: " << _filename << std::endl;
        exit(0);
    } else {;
        size_t found = _filename.find_last_of("/\\");
        std::string line_str, path_to_file = _filename.substr(0, found);
        while ( getline(myfile, line_str) )
            files.push_back(path_to_file+std::string("/")+line_str);
    }
    return 1;
}


int main(int argc, char* argv[])
{
    // Read input parameters
    //sfmEnviroment wkEnv(std::string("../setting.xml"));

    sfmEnv workEnv("../setting.xml");
    if (!workEnv.sample(5))
    {
        std::cout << "cannot read configure file" << std::endl; 
        return -1;
    }

    // Parse the image paths
    std::vector<std::string> images_paths;
    getdir( workEnv.samplePath + "IMGList.txt", images_paths );


    // Build instrinsics
    //double f = atof(argv[2]),
    //            cx = atof(argv[3]), cy = atof(argv[4]);

    //cv::Matx33d K = cv::Matx33d( f, 0, cx,
    //                             0, f, cy,
    //                             0, 0, 1);
    cv::Matx33d K = workEnv.cameraMatrix;

    /// Reconstruct the scene using the 2d images

    bool is_projective = true;
    std::vector<cv::Mat> Rs_est, ts_est, points3d_estimated;
    cv::sfm::reconstruct(images_paths, Rs_est, ts_est, K, points3d_estimated, is_projective);


    // Print output

    std::cout << "\n----------------------------\n" << std::endl;
    std::cout << "Reconstruction: " << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "Estimated 3D points: " << points3d_estimated.size() << std::endl;
    std::cout << "Estimated cameras: " << Rs_est.size() << std::endl;
    std::cout << "Refined intrinsics: " << std::endl << K << std::endl << std::endl;
    std::cout << "3D Visualization: " << std::endl;
    std::cout << "============================" << std::endl;


    /// Create 3D windows

    cv::viz::Viz3d window("Coordinate Frame");
                    window.setWindowSize(cv::Size(500,500));
                    window.setWindowPosition(cv::Point(150,150));
                    window.setBackgroundColor(); // black by default

    // Create the pointcloud
    std::cout << "Recovering points ... ";

    // recover estimated points3d
    std::vector<cv::Vec3f> point_cloud_est;
    for (int i = 0; i < points3d_estimated.size(); ++i)
        point_cloud_est.push_back(cv::Vec3f(points3d_estimated[i]));

    std::cout << "[DONE]" << std::endl;


    /// Recovering cameras
    std::cout << "Recovering cameras ... ";

    std::vector<cv::Affine3d> path;
    for (size_t i = 0; i < Rs_est.size(); ++i)
        path.push_back(cv::Affine3d(Rs_est[i],ts_est[i]));

    std::cout << "[DONE]" << std::endl;


    /// Add the pointcloud
    if ( point_cloud_est.size() > 0 )
    {
        std::cout << "Rendering points ... ";

        cv::viz::WCloud cloud_widget(point_cloud_est, cv::viz::Color::green());
        window.showWidget("point_cloud", cloud_widget);

        std::cout << "[DONE]" << std::endl;
    }
    else
    {
        std::cout << "Cannot render points: Empty pointcloud" << std::endl;
    }


    /// Add cameras
    if ( path.size() > 0 )
    {
        std::cout << "Rendering Cameras ... ";

        window.showWidget("cameras_frames_and_lines", cv::viz::WTrajectory(path, cv::viz::WTrajectory::BOTH, 0.1, cv::viz::Color::green()));
        window.showWidget("cameras_frustums", cv::viz::WTrajectoryFrustums(path, K, 0.1, cv::viz::Color::yellow()));

        window.setViewerPose(path[0]);

        std::cout << "[DONE]" << std::endl;
    }
    else
    {
        std::cout << "Cannot render the cameras: Empty path" << std::endl;
    }

    /// Wait for key 'q' to close the window
    std::cout << std::endl << "Press 'q' to close each windows ... " << std::endl;

    window.spin();

    return 0;
}
