#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include "Vec.h"
#include "rawrgbchar.h"

namespace GG
{
	/**

	* where b is the image with the blue background, bb the image of the blue background, g is the image with the green background, bg is the image of the green background...
	* Remark: Input format is expected to be 32 bits floats and pixel values are normalised between [0;1] (instead of [0;255] for 8 bits pixels for example)
	* @param b Image with blue background
	* @param bb Image OF the blue background... etc.
	*
	* */
	static cv::Mat groundTruthAlpha2(cv::Mat &b, cv::Mat &g, cv::Mat &k, cv::Mat &y, cv::Mat &r,
		cv::Mat &bb, cv::Mat &bg, cv::Mat &bk, cv::Mat &by, cv::Mat &br,
		cv::Mat &F, cv::Mat &AF){


		cv::Mat A = cv::Mat::zeros(b.rows, b.cols, CV_32FC1);


		cv::Mat mA = cv::Mat::zeros(15, 4, CV_64FC1);
		cv::Mat mb = cv::Mat::zeros(15, 1, CV_64FC1);
		cv::Mat mx = cv::Mat::zeros(4, 1, CV_64FC1);


		for (int i = 0; i < b.rows; i++){


			float *pb = (float*)(b.data + i*b.step);
			float *pg = (float*)(g.data + i*g.step);
			float *pk = (float*)(k.data + i*k.step);
			float *py = (float*)(y.data + i*y.step);
			float *pr = (float*)(r.data + i*r.step);
			float *pbb = (float*)(bb.data + i*bb.step);
			float *pbg = (float*)(bg.data + i*bg.step);
			float *pbk = (float*)(bk.data + i*bk.step);
			float *pby = (float*)(by.data + i*by.step);
			float *pbr = (float*)(br.data + i*br.step);



			float *pA = (float*)(A.data + i*A.step);
			float *pF = (float*)(F.data + i*F.step);
			float *pAF = (float*)(AF.data + i*AF.step);


			for (int j = 0; j < b.cols; j++){


				GG::Vec B(pb, j);
				GG::Vec G(pg, j);
				GG::Vec K(pk, j);
				GG::Vec Y(py, j);
				GG::Vec R(pr, j);
				GG::Vec BB(pbb, j);
				GG::Vec BG(pbg, j);
				GG::Vec BK(pbk, j);
				GG::Vec BY(pby, j);
				GG::Vec BR(pbr, j);


				//mA
				mA.at<double>(0, 0) = -BB.x; mA.at<double>(0, 1) = 1.0;
				mA.at<double>(1, 0) = -BB.y; mA.at<double>(1, 2) = 1.0;
				mA.at<double>(2, 0) = -BB.z; mA.at<double>(2, 3) = 1.0;
				mA.at<double>(3, 0) = -BG.x; mA.at<double>(3, 1) = 1.0;
				mA.at<double>(4, 0) = -BG.y; mA.at<double>(4, 2) = 1.0;
				mA.at<double>(5, 0) = -BG.z; mA.at<double>(5, 3) = 1.0;
				mA.at<double>(6, 0) = -BK.x; mA.at<double>(6, 1) = 1.0;
				mA.at<double>(7, 0) = -BK.y; mA.at<double>(7, 2) = 1.0;
				mA.at<double>(8, 0) = -BK.z; mA.at<double>(8, 3) = 1.0;
				mA.at<double>(9, 0) = -BY.x; mA.at<double>(9, 1) = 1.0;
				mA.at<double>(10, 0) = -BY.y; mA.at<double>(10, 2) = 1.0;
				mA.at<double>(11, 0) = -BY.z; mA.at<double>(11, 3) = 1.0;
				mA.at<double>(12, 0) = -BR.x; mA.at<double>(12, 1) = 1.0;
				mA.at<double>(13, 0) = -BR.y; mA.at<double>(13, 2) = 1.0;
				mA.at<double>(14, 0) = -BR.z; mA.at<double>(14, 3) = 1.0;


				//mb
				mb.at<double>(0, 0) = B.x - BB.x;
				mb.at<double>(1, 0) = B.y - BB.y;
				mb.at<double>(2, 0) = B.z - BB.z;
				mb.at<double>(3, 0) = G.x - BG.x;
				mb.at<double>(4, 0) = G.y - BG.y;
				mb.at<double>(5, 0) = G.z - BG.z;
				mb.at<double>(6, 0) = K.x - BK.x;
				mb.at<double>(7, 0) = K.y - BK.y;
				mb.at<double>(8, 0) = K.z - BK.z;
				mb.at<double>(9, 0) = Y.x - BY.x;
				mb.at<double>(10, 0) = Y.y - BY.y;
				mb.at<double>(11, 0) = Y.z - BY.z;
				mb.at<double>(12, 0) = R.x - BR.x;
				mb.at<double>(13, 0) = R.y - BR.y;
				mb.at<double>(14, 0) = R.z - BR.z;


				//mx
				cv::solve(mA, mb, mx, cv::DECOMP_QR);


				double alpha = mx.at<double>(0, 0);
				GG::Vec f(mx.at<double>(1, 0), mx.at<double>(2, 0), mx.at<double>(3, 0));

				pA[j] = (float)alpha;
				pAF[j * 3] = (float)f.x;
				pAF[j * 3 + 1] = (float)f.y;
				pAF[j * 3 + 2] = (float)f.z;
				pF[j * 3] = (float)(f.x / alpha);
				pF[j * 3 + 1] = (float)(f.y / alpha);
				pF[j * 3 + 2] = (float)(f.z / alpha);

			}
		}


		return A;


	}

}

/**
* Generates the ground truth for the given images.
* @param foreground A pointer to 5 RawRgbChar objects. These objects are DESTROYED inside the function.
* @param background A pointer to 5 RawRgbChar objects. These objects are DESTROYED inside the function.
* @return empty upon failure, or 3 images upon success, corresponding to A, F and AF respectively.
* */
std::vector<cv::Mat> GenerateGroundTruth(RawRgbChar* foreground, RawRgbChar* background);