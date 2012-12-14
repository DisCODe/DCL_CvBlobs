/*!
 * \file BlobExtractor_Processor.hpp
 * \brief
 */

#ifndef BLOBEXTRACTOR_PROCESSOR_HPP_
#define BLOBEXTRACTOR_PROCESSOR_HPP_

#define CV_NO_BACKWARD_COMPATIBILITY


#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Property.hpp"

#include <opencv2/core/core.hpp>

#include "Types/BlobResult.hpp"

namespace Processors {
namespace BlobExtractor {

/*!
 * \class BlobExtractor_Processor
 * \brief Example processor class.
 * \author tkornuta
 */
class BlobExtractor_Processor: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	BlobExtractor_Processor(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~BlobExtractor_Processor();

	/*!
	 * Prepare data streams and handlers
	 */
	void prepareInterface();

protected:

	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Retrieves data from device.
	 */
	bool onStep();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();

	/*!
	 * Event handler function.
	 */
	void onNewImage();

	/// Event handler for new image
	Base::EventHandler<BlobExtractor_Processor> h_onNewImage;

	/// Input data stream
	Base::DataStreamIn<cv::Mat> in_img;

	/// Event raised, when new image with blobs is ready
	Base::Event * newImage;

	/// Output data stream - image with drawn blobs
	Base::DataStreamOut<cv::Mat> out_img;

	/// Event raised, when set of blobs is extracted
	Base::Event * newBlobs;

	/// Output data stream - list of detected blobs
	Base::DataStreamOut<Types::Blobs::BlobResult> out_blobs;

private:
	cv::Mat img_uchar;
	Base::Property<int> min_size;
	Base::Property<int> background_color;

};

}//: namespace BlobExtractor
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("BlobExtractor", Processors::BlobExtractor::BlobExtractor_Processor, Common::Panel_Empty)

#endif /* BLOBEXTRACTOR_PROCESSOR_HPP_ */

