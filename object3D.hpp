#include <assert.h>
#include <armadillo>
#include <opencv2/core.hpp>

#ifndef OBJECT_3D_h
#define OBJECT_3D_h

class Object3D
{
public:
	bool isInitialed = false;
	int counter = 0;
	arma::Mat<double> r = arma::Mat<double>(3,3,arma::fill::zeros);
	arma::Mat<double> t = arma::Mat<double>(3,1,arma::fill::zeros);
	arma::Mat<double> wr = arma::Mat<double>(3,3,arma::fill::zeros);//rotation matrix to world
	arma::Mat<double> p = arma::Mat<double>(3,1,arma::fill::zeros);//position relative to world
	//with known
	void calWrP(const Object3D & refObject)
	{
		const arma::Mat<double> refWr (refObject.wr);
		const arma::Mat<double> refP (refObject.p);

		//wr = refAruco.wr * (refAruco.r)^(-1) * r
		wr = refWr * refObject.r.i() * r;
		//p = refAruco.wr^-1 ( (refAruco.r)^(-1) * (t - refAruco.t) - refAruco.p)
		p = refWr * (refObject.r.i() * (t - refObject.t)) + refP;

		isInitialed = true;
		return;
	}

	static void cvMatToArmaMat(const cv::Mat & cvMat, arma::Mat<double> & armaMat)
	{
		cv::MatConstIterator_<double> cvMatIt = cvMat.begin<double>();
		for (int row(0); row < cvMat.size().height; row++)
		{
			for (int col(0); col < cvMat.size().width; col++)
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

};

class CameraObj: public Object3D
{
public:
	std::vector<arma::Mat<double>> trajactory;

	void record()
	{
		trajactory.push_back(p);
	}

	arma::Mat<double> rectify(const arma::Mat<double> & r)
	{
		phi_x = 
	}

	//Calculate new position and orientation relative to world coordinate system
	void calWrP()
	{
		const arma::Mat<double> refWr(wr);
		const arma::Mat<double> refP(p);
		wr = refWr * r;
		p = refWr * t + refP;
		record();
		return;
	}

	void iniWr()
	{
		wr(0,0) = 1;
		wr(1,1) = 1;
		wr(2,2) = 1;
		record();
	}

};

#endif
