// ImageProcessing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace cv;
using namespace std;

const static string PoutFileName = "Resources\\pout.bmp";
const static string LenaFileName = "Resources\\lena.bmp";
const static string MapFileName = "Resources\\map.bmp";
const static string RectFileName = "Resources\\Rect1.bmp";

void greyscaleStretch(const Mat& source, Mat& dist, function<uchar(uchar)> transform)
{
	Mat table(1, 256, CV_8U);
	auto p = table.data;
	for (size_t c = 0; c < 256; ++c)
	{
		p[c] = transform(c);
	}
	//cout << table << endl;
	LUT(source, table, dist);
}

void addSaltPepperNoise(const Mat& source, Mat& dist, double p)
{
	RNG rng;
	CV_Assert(source.depth() != sizeof(uchar));
	source.copyTo(dist);

	if (source.channels() == 1)
	{
		auto end = dist.end<uchar>();
		for (auto itr = dist.begin<uchar>(); itr != end; ++itr)
		{
			if (rng.uniform(0.0, 1.0) < p)
			{
				*itr = rng.uniform(0.0, 1.0) < 0.5 ? 0 : 255;
			}
		}
	}
	else
	{
		auto end = dist.end<Vec3b>();
		for (auto itr = dist.begin<Vec3b>(); itr != end; ++itr)
		{
			if (rng.uniform(0.0, 1.0) < p)
			{
				uchar val = rng.uniform(0.0, 1.0) < 0.5 ? 0 : 255;
				(*itr)[0] = val;
				(*itr)[1] = val;
				(*itr)[2] = val;
			}
		}
	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2)
	{
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}

	string fileName = PoutFileName;
	Mat source;
	source = imread(fileName, IMREAD_COLOR); // Read the file
	if (!source.data) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	int c = source.channels();

	namedWindow("Original", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Original", source); // Show our image inside it.

	Mat output = source.clone();

	//medianBlur(source, output, 5);
	float A = 2.0, B= 15;
	output = source * A + B;

	namedWindow("LinearTransform", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("LinearTransform", output); // Show our image inside it.

	float x1 = 0.25 * 255, x2 = 0.75 * 255, y1 = 0.1 * 255, y2 = 0.9 * 255;
	greyscaleStretch(source, output, [=](uchar x) -> uchar{
		if (x <= x1)
			return static_cast<uchar>( x * (y1 / x1));
		else if (x <= x2)
			return static_cast<uchar>((y2 - y1) / (x2 - x1) * (x - x1) + y1);
		else
			return static_cast<uchar>((255 - y2) / (255 - x2) * (x - x2) + y2);
	});

	namedWindow("GreyScaleStretch", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("GreyScaleStretch", output); // Show our image inside it.

	int histSize = 256;
	int channels = 0;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	Mat histgram;
	calcHist(&source, 1, &channels, Mat(), histgram, 1, &histSize, &histRange);
	int hist_w = 512; int hist_h = 200;
	int bin_w = cvRound((double)hist_w / histSize);
	Mat histImage(hist_h, hist_w, CV_8U, Scalar(0));
	/// Normalize the result to [ 0, histImage.rows ]
	normalize(histgram, histgram, 0, histImage.rows, NORM_MINMAX);
	/// Draw for each channel
	for (int i = 0; i < histSize; i++)
	{
			line(histImage,
				Point(bin_w*(i), hist_h - cvRound(histgram.at<float>(i))),
				Point(bin_w*(i), hist_h),
				Scalar(255), bin_w);
	}
	namedWindow("Histogram", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Histogram", histImage); // Show our image inside it.

	{
		double I = sum(histgram)[0];
		histgram = histgram / (I / 255.0f);
		Mat lutable(1, 256, CV_8U);
		uchar* p = lutable.data;
		float acc = 0.0f;
		for (size_t i = 0; i < 256; i++)
		{
			acc += histgram.at<float>(i);
			p[i] = cvRound(acc);
		}
		//cout << lutable << endl;
		LUT(source, lutable, output);
	}
	namedWindow("Histogram Equalized (Custom)", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Histogram Equalized (Custom)", output); // Show our image inside it.

	waitKey(0); // Wait for a keystroke in the window

	source = imread(LenaFileName);

	Mat noisedLena;
	addSaltPepperNoise(source, noisedLena, 0.03);
	namedWindow("Noised", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Noised", noisedLena); // Show our image inside it.

	unsigned kernalSize = 3;
	blur(noisedLena, output, Size(kernalSize, kernalSize), Point(-1, -1));
	namedWindow("Homogeneous Blur", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Homogeneous Blur", output); // Show our image inside it.

	medianBlur(noisedLena, output, kernalSize);
	namedWindow("Median Blur", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Median Blur", output); // Show our image inside it.

	//try{
	//	equalizeHist(source, output);
	//}
	//catch (cv::Exception excp)
	//{
	//	cout << excp.err << endl;
	//}

	waitKey(0); // Wait for a keystroke in the window

	source = imread(MapFileName);
	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(source, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(source, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);

	/// Total Gradient (approximate)
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, output);
	namedWindow("Sobel", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Sobel", output); // Show our image inside it.

	Laplacian(source, output, ddepth, kernalSize, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(output, output);
	namedWindow("Laplacian", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Laplacian", output); // Show our image inside it.

	waitKey(0); // Wait for a keystroke in the window


	// DFT & IDFT

	source = imread(LenaFileName, CV_LOAD_IMAGE_GRAYSCALE);
	if (source.data == 0)
	{
		cout << " Can't open file Rect1.bmp" << endl;
		return -1;
	}
	//namedWindow("Original", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Original", source);
	Mat padded;                            //expand input image to optimal size
	int m = getOptimalDFTSize(source.rows);
	int n = getOptimalDFTSize(source.cols); // on the border add zero values
	copyMakeBorder(source, padded, 0, m - source.rows, 0, n - source.cols, BORDER_CONSTANT, Scalar::all(0));

	padded.convertTo(padded, CV_32F);
	Mat planes[] = { padded, Mat::zeros(padded.size(), CV_32F) };

	Mat Spectrum;
	merge(planes, 2, Spectrum);         // Add to the expanded another plane with zeros

	dft(Spectrum, Spectrum);            // this way the result may fit in the source matrix

	// compute the magnitude and switch to logarithmic scale
	// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	//split(Spectrum, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	//Mat mag,magnitudeDisp,phase;
	//magnitude(planes[0], planes[1], mag);// planes[0] = magnitude
	//mag.copyTo(magnitudeDisp);

	//Mat magRecoverd;
	//Mat xplans[2] = { mag, Mat::zeros(mag.size(),CV_32F) };
	//merge(xplans, 2, magRecoverd);

	//idft(magRecoverd, magRecoverd);
	//split(magRecoverd, xplans);
	//namedWindow("Magnitude Recover", WINDOW_AUTOSIZE); // Create a window for display.
	//imshow("Magnitude Recover", xplans[0]);

	//divide(planes[0], mag, xplans[0],CV_32F);
	//divide(planes[1], mag, xplans[1], CV_32F);
	//merge(xplans, 2, magRecoverd);
	dft(Spectrum, Spectrum, DFT_INVERSE | DFT_SCALE);
	//split(magRecoverd, xplans);
	split(Spectrum, planes);
	magnitude(planes[0], planes[1], output);// planes[0] = magnitude
	namedWindow("Phase Recover", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Phase Recover", planes[0]);


	//magnitudeDisp += Scalar::all(1);                    // switch to logarithmic scale
	//log(magnitudeDisp, magnitudeDisp);

	//// crop the spectrum, if it has an odd number of rows or columns
	//magnitudeDisp = magnitudeDisp(Rect(0, 0, magnitudeDisp.cols & -2, magnitudeDisp.rows & -2));

	//// rearrange the quadrants of Fourier image  so that the origin is at the image center
	//int cx = magnitudeDisp.cols / 2;
	//int cy = magnitudeDisp.rows / 2;

	//Mat q0(magnitudeDisp, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	//Mat q1(magnitudeDisp, Rect(cx, 0, cx, cy));  // Top-Right
	//Mat q2(magnitudeDisp, Rect(0, cy, cx, cy));  // Bottom-Left
	//Mat q3(magnitudeDisp, Rect(cx, cy, cx, cy)); // Bottom-Right

	//Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	//q0.copyTo(tmp);
	//q3.copyTo(q0);
	//tmp.copyTo(q3);

	//q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	//q2.copyTo(q1);
	//tmp.copyTo(q2);

	//normalize(magnitudeDisp, magnitudeDisp, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
	//// viewable image form (float between values 0 and 1).

	//namedWindow("Spectrum Magnitude", WINDOW_AUTOSIZE); // Create a window for display.
	//imshow("Spectrum Magnitude", magnitudeDisp);

	waitKey(0);
	return 0;
}

