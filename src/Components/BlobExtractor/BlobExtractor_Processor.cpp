/*!
 * \file BlobExtractor_Processor.cpp
 * \brief
 */

#include <memory>
#include <string>
#include <iostream>

#include "BlobExtractor_Processor.hpp"
#include "ComponentLabeling.hpp"

#include "Logger.hpp"
#include "Common/Timer.hpp"
#include "Types/BlobOperators.hpp"

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Processors {
namespace BlobExtractor {

BlobExtractor_Processor::BlobExtractor_Processor(const std::string & name) : Base::Component(name),
	min_size("min_size", 10000, "range"),
	background_color("background_color", 0, "range")
{
	LOG(LTRACE)<<"Hello BlobExtractor_Processor\n";
	min_size.addConstraint("0");
	min_size.addConstraint("1000000");
	registerProperty(min_size);

	background_color.addConstraint("0");
	background_color.addConstraint("255");
	registerProperty(background_color);
}

BlobExtractor_Processor::~BlobExtractor_Processor() {
	LOG(LTRACE)<<"Good bye BlobExtractor_Processor\n";
}

void BlobExtractor_Processor::prepareInterface() {
	h_onNewImage.setup(this, &BlobExtractor_Processor::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);
	addDependency("onNewImage", &in_img);

	registerStream("in_img", &in_img);
	registerStream("out_img", &out_img);
	registerStream("out_blobs", &out_blobs);

	addDependency("onNewImage", &in_img);
}

bool BlobExtractor_Processor::onInit() {
	LOG(LTRACE) << "BlobExtractor_Processor::initialize\n";

	return true;
}

bool BlobExtractor_Processor::onFinish() {
	LOG(LTRACE) << "BlobExtractor_Processor::finish\n";

	return true;
}

bool BlobExtractor_Processor::onStop()
{
	return true;
}

bool BlobExtractor_Processor::onStart()
{
	return true;
}

void BlobExtractor_Processor::onNewImage() {
	LOG(LTRACE) << "BlobExtractor_Processor::onNewImage() called!\n";

	Common::Timer timer;
	timer.restart();

	cv::Mat in = in_img.read();
	in.convertTo(img_uchar, CV_8UC1);
	IplImage ipl_img = IplImage(img_uchar);
	cv::Mat mat_img = img_uchar;
 	cv::Mat out = cv::Mat::zeros(in.size(), CV_8UC3);

	Types::Blobs::Blob_vector res;
	bool success;

	try
	{
		success = ComponentLabeling( &ipl_img, NULL, background_color, res );
	}
	catch(...)
	{
		success = false;
		LOG(LWARNING) << "blob find error\n";
	}

	try {
		if( !success ) {
			LOG(LERROR) << "Blob find error\n";
		} else {
			LOG(LTRACE) << "blobs found";
			Types::Blobs::BlobResult result(res);

			result.Filter( result, B_EXCLUDE, Types::Blobs::BlobGetArea(), B_LESS, min_size );

			out_blobs.write(result);
			LOG(LTRACE) << "blobs written";

			LOG(LTRACE) << "blobs sent";
			result.draw(out, CV_RGB(255, 0, 0), 0, 0);
			out_img.write(in);

		}

		LOG(LINFO) << "Blobing took " << timer.elapsed() << " seconds\n";
	}
	catch(...)
	{
		LOG(LERROR) << "BlobExtractor onNewImage failure";
	}
}

}//: namespace BlobExtractor
}//: namespace Processors
