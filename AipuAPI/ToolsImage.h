#ifndef TOOLSIMAGE_H
#define TOOLSIMAGE_H

#define QUALITY_IMAGE	100

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class ToolsImage
{
public:
	ToolsImage();
	~ToolsImage();
	std::vector<uchar> WriteMatOnBuffer(cv::Mat frame);
private:

};


#endif // !TOOLSIMAGE_H

