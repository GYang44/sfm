#include <math.h>
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

	void composeRotation(const double & roll, const double & pitch, const double & yaw, arma::Mat<double> & outRotation)
	{
		arma::Mat<double> matRoll(3,3,arma::fill::zeros);
		arma::Mat<double> matPitch(3,3,arma::fill::zeros);
		arma::Mat<double> matYaw(3,3,arma::fill::zeros);

		matRoll(0,0) = 1;
		matRoll(1,1) = cos(roll);
		matRoll(1,2) = -sin(roll);
		matRoll(2,1) = sin(roll);
		matRoll(2,2) = cos(roll);
		
		matPitch(0,0) = cos(pitch);
		matPitch(0,2) = sin(pitch);
		matPitch(1,1) = 1;
		matPitch(2,0) = -sin(pitch);
		matPitch(2,2) = cos(pitch);
		
		matYaw(0,0) = cos(yaw);
		matYaw(0,1) = -sin(yaw);
		matYaw(1,0) = sin(yaw);
		matYaw(1,1) = cos(yaw);
		matYaw(2,2) = 1;

		outRotation = matRoll * matPitch * matYaw;

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

	void decomposeRotaiton(const arma::Mat<double> & inMat, arma::Mat<double> & outX, arma::Mat<double> & outY, arma::Mat<double> & outZ)
	{
		outX.eye(3,3);
		outY.eye(3,3);
		outZ.eye(3,3);

		double thetaX = atan2(inMat(2,1), inMat(2,2));
		double thetaY = atan2(-inMat(2,0), sqrt(inMat(2,1)*inMat(2,1) + inMat(2,2)*inMat(2,2)));
		double thetaZ = atan2(inMat(1,0), inMat(0,0));

		outX(1,1) = cos(thetaX);
		outX(1,2) = -sin(thetaX);
		outX(2,1) = sin(thetaX);
		outX(2,2) = cos(thetaX);

		outY(0,0) = cos(thetaY);
		outY(0,2) = sin(thetaY);
		outY(2,0) = -sin(thetaY);
		outY(2,2) = cos(thetaY);

		outZ(0,0) = cos(thetaZ);
		outZ(0,1) = -sin(thetaZ);
		outZ(1,0) = sin(thetaZ);
		outZ(1,1) = cos(thetaZ);

	}

	//mode 000 ~ 111(binary) flag for xyz respectively
	arma::Mat<double> revertRotation(const arma::Mat<double> & inMat, const ushort & mode)
	{
		arma::Mat<double> X, Y, Z;
		decomposeRotaiton(inMat, X, Y, Z);
		if(mode & 0x0001) X = X.t();
		if(mode & 0x0002) Y = Y.t();
		if(mode & 0x0004) Z = Z.t();
		return Z*X*Y;
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

	void iniWr(const double & roll=0, const double & pitch=0, const double & yaw=0)
	{
		composeRotation(roll, pitch, yaw, this -> wr);
		record();
	}

};

#endif
