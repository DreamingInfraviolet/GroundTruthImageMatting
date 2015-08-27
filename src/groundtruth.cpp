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
		matCharF[i] = Mat(std::get<1>(foreground[i]), std::get<0>(foreground[i]), CV_16UC3, &std::get<2>(foreground[i])[0]);
		matCharB[i] = Mat(std::get<1>(background[i]), std::get<0>(background[i]), CV_16UC3, &std::get<2>(background[i])[0]);

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

	for(int i = 0; i < 5; ++i)
	{
		matCharF[i].convertTo(matFloatF[i], CV_32FC3, 1.0 / 65535);
		std::get<2>(foreground[i]).swap(std::vector<uint16_t> ());
		matCharB[i].convertTo(matFloatB[i], CV_32FC3, 1.0 / 65535);
		std::get<2>(background[i]).swap(std::vector<uint16_t>());
	}

	f.create(std::get<1>(foreground[0]), std::get<0>(foreground[0]), CV_32FC3);
	af.create(std::get<1>(foreground[0]), std::get<0>(foreground[0]), CV_32FC3);

	//Compute:
	Inform("Generating ground truth");
	a = GG::groundTruthAlpha2(matFloatF[0], matFloatF[1], matFloatF[2], matFloatF[3], matFloatF[4],
							  matFloatB[0], matFloatB[1], matFloatB[2], matFloatB[3], matFloatB[4],
							  f, af);

		//Convert results to rgb:
	a.convertTo(a, CV_16UC1, 65535);
	cvtColor(a, a, CV_GRAY2RGB);
	f.convertTo(f, CV_16UC3, 65535);
	af.convertTo(af, CV_16UC3, 65535);

	return{ a, f, af };
}