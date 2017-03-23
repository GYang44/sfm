#include <assert.h>
#include <armadillo>
#include <opencv2/core.hpp>

#ifndef OBJECT_3D_h
#define OBJECT_3D_h

class object3D
{
public:
	bool isInitialed = false;
	int counter = 0;
	arma::Mat<double> r = arma::Mat<double>(3,3,arma::fill::zeros);
	arma::Mat<double> t = arma::Mat<double>(3,1,arma::fill::zeros);
	arma::Mat<double> wr = arma::Mat<double>(3,3,arma::fill::zeros);//rotation matrix to world
	arma::Mat<double> p = arma::Mat<double>(3,1,arma::fill::zeros);//position relative to world
	std::vector<arma::Mat<double>> trajactory;
	//with known
	void calWrP(const object3D & refObject)
	{
		//wr = refAruco.wr * (refAruco.r)^(-1) * r
		wr = refObject.wr * refObject.r.i() * r;
		//p = refAruco.wr^-1 ( (refAruco.r)^(-1) * (t - refAruco.t) - refAruco.p)
		p = refObject.wr * (refObject.r.i() * (t - refObject.t)) + refObject.p;

		isInitialed = true;
		return;
	}

	static void cvMatToArmaMat(const cv::Mat & cvMat, arma::Mat<double> & armaMat)
	{
		cv::MatConstIterator_<double> cvMatIt = cvMat.begin<double>();
		for (uint row(0); row < cvMat.size().height; row++)
		{
			for (uint col(0); col < cvMat.size().width; col++)
			{
				armaMat(row,col) = *cvMatIt++;
			}
		}
		return;
	}

	//Set r and t of camera to the rotation and translation of current camera relative to previous
	void updateRT(const cv::Mat & R, const cv::Mat & T)
	{
		assert(R.depth() == CV_64F);
		assert(T.depth() == CV_64F);
		cvMatToArmaMat(R, r);
		cvMatToArmaMat(T, t);
		return;
	}

	//Calculate new position and orientation relative to world coordinate system
	void updateWrP(const object3D refObject)
	{
		wr = refObject.wr * r;
		p = refObject.wr * t + refObject.p;
		record();
		return;
	}
	void iniWr()
	{
		wr(0,2) = 1;
		wr(1,0) = 1;
		wr(2,1) = 1;
		record();
	}
	void record()
	{
		trajactory.push_back(p);
	}
};

#endif