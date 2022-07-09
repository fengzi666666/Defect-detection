#include <iostream>
#include <OpenNI.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace std;

using namespace cv;

int main()
{
		
	cv::VideoWriter outputVideo;
	//cv::Size S = cv::Size((int)vid_capture.get(CAP_PROP_FRAME_WIDTH),
	//			(int)vid_capture.get(CAP_PROP_FRAME_HEIGHT));

	//打开视频路径，设置基本信息 open函数中你参数跟上面给出的VideoWriter函数是一样的
	string str = "E:/programs data/QT/Defect detection/detection_data/1.mp4";
	outputVideo.open(str, VideoWriter::fourcc('H', '2', '6', '4'), 25, Size(640,480), true);

	//定义oni文件中视频的总帧数以及得到的图片的保存目录
	int total = 0;
	string imagefile = "E:/programs data/QT/Defect detection/detection_data";

	//初始化OpenNI环境
	openni::OpenNI::initialize();

	//声明设备并打开oni文件
	openni::Device fromonifile;
	fromonifile.open("E:/programs data/QT/Defect detection/detection_data/1.oni");

	//声明控制对象，这对视频流的控制起到了关键作用
	openni::PlaybackControl* pController = fromonifile.getPlaybackControl();

	//声明视频流对象以及帧对象
	openni::VideoStream streamColor;
	openni::VideoFrameRef frameColor;

	//验证是否有彩色传感器（是否有彩色视频）和建立与设备想关联的视频流
	if (fromonifile.hasSensor(openni::SENSOR_COLOR))
	{
		if (streamColor.create(fromonifile, openni::SENSOR_COLOR) == openni::STATUS_OK)
		{
			cout << "建立视频流成功" << endl;
		}
		else
		{
			cerr << "ERROR: 建立视频流没有成功" << endl;
			system("pause");
			return -1;
		}
	}
	else
	{
		cerr << "ERROR: 该设备没有彩色传感器" << endl;
		system("pause");
		return -1;
	}

	//建立显示窗口
	cv::namedWindow("Image");

	//获取总的视频帧数并将该设备的速度设为-1以便能留出足够的时间对每一帧进行处理、显示和保存
	total = pController->getNumberOfFrames(streamColor);
	pController->setSpeed(-1);

	//开启视频流
	streamColor.start();
	for (int i = 1; i <= total; ++i)
	{
		//读取视频流的当前帧
		streamColor.readFrame(&frameColor);

		cout << "当前正在读的帧数是：" << frameColor.getFrameIndex() << endl;
		cout << "当前的循环次数是：  " << i << endl;

		//将帧保存到Mat中并且将其转换到BGR模式，因为在OpenCV中图片的模式是BGR
		cv::Mat rgbImg(frameColor.getHeight(), frameColor.getWidth(), CV_8UC3, (void*)frameColor.getData());
		cv::Mat bgrImg;
		cvtColor(rgbImg, bgrImg, cv::COLOR_RGB2BGR);

		//将每一帧按顺序帧保存到图片目录下
		char imagefullname[255];
		char imagenum[50];
		sprintf(imagenum, "\\%03d.png", i);
		strcpy(imagefullname, imagefile.data());
		strcat(imagefullname, imagenum);
		//cv::imwrite(imagefullname, bgrImg);
		outputVideo << rgbImg;

		//显示当前帧
		cv::imshow("Image", bgrImg);
		if (cv::waitKey(30) == 27)
		{
			break;
		}
	}

	outputVideo.release();

	//销毁显示窗口
	cv::destroyWindow("Image");

	//关闭视频流
	streamColor.destroy();

	//关闭设备
	fromonifile.close();

	//关闭OpenNI
	openni::OpenNI::shutdown();

	return 0;
}