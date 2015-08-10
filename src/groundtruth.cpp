#include <vector>
#include <opencv2\opencv.hpp>
#include "rawrgbchar.h"
#include "io.h"
#include "groundtruth.h"

std::vector<cv::Mat> GenerateGroundTruth (RawRgbChar* foreground, RawRgbChar* background)
{
	//Prepare and wrap in Mat:
	Inform("Preparing ground truth");
	using namespace cv;

	Mat matCharF[5];
	Mat matCharB[5];

	size_t imageLen = std::get<2>(foreground[0]).size();

	for (int i = 0; i < 5; ++i)
	{
		matCharF[i] = Mat(std::get<1>(foreground[i]), std::get<0>(foreground[i]), CV_8UC3, &std::get<2>(foreground[i])[0]);
		matCharB[i] = Mat(std::get<1>(background[i]), std::get<0>(background[i]), CV_8UC3, &std::get<2>(background[i])[0]);

		if (imageLen != std::get<2>(foreground[i]).size() || imageLen != std::get<2>(background[i]).size())
		{
			::Error("Mismatched image sizes in ground truth");
			return{};
		}
	}

	//Convert to float:

	Mat matFloatF[5];
	Mat matFloatB[5];

	Mat a;
	Mat f;
	Mat af;

#ifdef HALVE_GT_IMAGE
	for (int i = 0; i < 5; ++i)
	{
		Mat matFloatFullF, matFloatFullB;
		matFloatF[i].convertTo(matFloatFullF, CV_32FC3);
		resize(matFloatFullF, matFloatF[i], Size(std::get<0>(foreground[i]) / 2, std::get<1>(foreground[i]) / 2));
		std::get<2>(foreground[i]).clear();
		matFloatB[i].convertTo(matFloatFullB, CV_32FC3);
		resize(matFloatFullB, matFloatB[i], Size(std::get<0>(background[i]) / 2, std::get<1>(background[i]) / 2));
		std::get<2>(background[i]).clear();
	}

	f.create(std::get<1>(foreground[0]) / 2, std::get<0>(foreground[0]) / 2, CV_32FC3);
	af.create(std::get<1>(foreground[0]) / 2, std::get<0>(foreground[0]) / 2, CV_32FC3);
#else

	for(int i = 0; i < 5; ++i)
	{
		matCharF[i].convertTo(matFloatF[i], CV_32FC3);
		std::get<2>(foreground[i]).swap(std::vector<unsigned char> ());
		matCharB[i].convertTo(matFloatB[i], CV_32FC3);
		std::get<2>(background[i]).swap(std::vector<unsigned char>());
	}

	f.create(std::get<1>(foreground[0]), std::get<0>(foreground[0]), CV_32FC3);
	af.create(std::get<1>(foreground[0]), std::get<0>(foreground[0]), CV_32FC3);

#endif

	//Compute:
	Inform("Generating ground truth");
	a = GG::groundTruthAlpha2(matFloatF[0], matFloatF[1], matFloatF[2], matFloatF[3], matFloatF[4],
							  matFloatB[0], matFloatB[1], matFloatB[2], matFloatB[3], matFloatB[4],
							  f, af);

	//Convert results to rgb:
	Mat aR, aRgb, fRgb, afRgb;
	a.convertTo(aR, CV_8UC1, 255);
	cvtColor(aR, aRgb, CV_GRAY2RGB);

	f.convertTo(fRgb, CV_8UC4);
	af.convertTo(afRgb, CV_8UC4);

	return{ aRgb, fRgb, afRgb };
}