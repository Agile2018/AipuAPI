#include "ToolsImage.h"

ToolsImage::ToolsImage()
{
}

ToolsImage::~ToolsImage()
{
}

std::vector<uchar> ToolsImage::WriteMatOnBuffer(cv::Mat frame) {
	std::vector<uchar> bufferImage;
	int params[3] = { 0 };
	params[0] = cv::IMWRITE_JPEG_QUALITY;
	params[1] = QUALITY_IMAGE;
	if (!frame.empty())
	{
		bool code = cv::imencode(".jpg", frame,
			bufferImage, std::vector<int>(params, params + 2));

	}
	return bufferImage;
}