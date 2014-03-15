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
    min_size("min_size", 100, "range"),
	background_color("background_color", 0, "range")
{
	LOG(LTRACE)<<"Hello BlobExtractor_Processor\n";
	min_size.addConstraint("0");
    min_size.addConstraint("310000");
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
    CLOG(LTRACE) << "BlobExtractor_Processor::onNewImage() called!\n";

	Common::Timer timer;
	timer.restart();

    // Read image.
    cv::Mat in = in_img.read();

    cv::Mat out = cv::Mat::zeros(in.size(), CV_8UC3);
    cv::Mat img_uchar = cv::Mat(in.size(), CV_8UC1);

    if (in.channels()==1)
    {
        cvtColor(in, out, CV_GRAY2BGR);
        in.copyTo(img_uchar);
//        in.convertTo(img_uchar, CV_8UC1);
    }
    else
    {
        in.copyTo(out);
        cvtColor(in, img_uchar, CV_BGR2GRAY);
    }

    //in.convertTo(out, CV_8UC3);

    // Create a single channel ipl_image. :]
    IplImage ipl_img = IplImage(img_uchar);


	Types::Blobs::Blob_vector res;
	bool success;

	try
	{
		success = ComponentLabeling( &ipl_img, NULL, background_color, res );
        CLOG(LTRACE) << "Component labeling result: " <<success;
	}
	catch(...)
	{
		success = false;
        CLOG(LWARNING) << "blob find error\n";
	}

	try {
		if( !success ) {
            CLOG(LERROR) << "Blob find error\n";
		} else {
            Types::Blobs::BlobResult result(res);
            CLOG(LTRACE) << "blobs found: " << result.GetNumBlobs();

            result.Filter( result, B_EXCLUDE, Types::Blobs::BlobGetArea(), B_LESS, min_size );
            CLOG(LTRACE) << "blobs after filtering: " << result.GetNumBlobs();

			out_blobs.write(result);
            CLOG(LTRACE) << "blobs written to ";

            result.draw(out, CV_RGB(242, 0, 86), 0, 0);
            out_img.write(out);

		}

        CLOG(LINFO) << "Blobing took " << timer.elapsed() << " seconds\n";
	}
	catch(...)
	{
        CLOG(LERROR) << "BlobExtractor onNewImage failure";
    }
}

}//: namespace BlobExtractor
}//: namespace Processors
