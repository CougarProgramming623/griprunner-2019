#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <curl/curl.h>

#include "ntcore_cpp.h"
#include "networktables/NetworkTableInstance.h"
#include "GripPipeline.h"

struct mem {
	uint8_t *mem;
	size_t used;
	size_t alloced;
};

static size_t
write_mem_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t reallocsize = 0, realsize = size * nmemb;
	struct mem *mem = (struct mem *)userp;

	reallocsize = mem->used + realsize + 1;
	if (reallocsize > mem->alloced) {
		mem->mem = (uint8_t *)realloc(mem->mem, reallocsize);
		if (NULL == mem->mem) {
			perror(__FUNCTION__);
			return 0;
		}
	}
	memcpy(mem->mem + mem->used, contents, realsize);
	mem->used += realsize;
	mem->mem[mem->used] = 0;

	return realsize;
}

std::shared_ptr<nt::NetworkTable>
start_networktables(void)
{
	auto inst = nt::NetworkTableInstance::GetDefault();
	inst.StartClient("roborio-623-frc.local");
	return inst.GetTable("vision");
}

int
main(int argc, char **argv)
{
	std::vector<uint8_t> rawimg;
	cv::Mat cvimg;
	grip::GripPipeline gp;
	struct mem mem;
	CURLcode res;
	CURL *h = NULL;
	std::shared_ptr<nt::NetworkTable> vision_table;
	std::vector<double> center_x, center_y, width, height;

	h = curl_easy_init();
	if (NULL == h) {
		fprintf(stderr, "curl_easy_init() failed\n");
		return -1;
	}
	mem.mem = NULL;
	mem.used = 0;
	mem.alloced = 0;

	curl_easy_setopt(h, CURLOPT_URL, "http://localhost:7777/?action=snapshot");
	curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, write_mem_callback);
	curl_easy_setopt(h, CURLOPT_WRITEDATA, (void *)&mem);

	vision_table = start_networktables();

	for (;;) {
		res = curl_easy_perform(h);
		if (CURLE_OK != res) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			  curl_easy_strerror(res));
			sleep(1);
			continue;
		}
		rawimg.clear();
		rawimg.insert(rawimg.end(), mem.mem, mem.mem + mem.used);
		mem.used = 0;
		cvimg = cv::imdecode(rawimg, CV_LOAD_IMAGE_COLOR);
		gp.Process(cvimg);
		std::vector<std::vector<cv::Point>> *out = gp.GetFilterContoursOutput();
		for (std::vector<cv::Point> contour: *out) {
			cv::Rect bb = boundingRect(contour);
			center_x.push_back(bb.x + bb.width / 2);
			center_y.push_back(bb.y + bb.height / 2);
			width.push_back(bb.width);
			height.push_back(bb.height);
			printf("x: %d, y: %d, h: %d, w: %d\n", bb.x + bb.width / 2,
			  bb.y + bb.height / 2, bb.height, bb.width);
		}
		vision_table.get()->PutNumberArray("centerX",
		  llvm::ArrayRef<double>(center_x));
		vision_table.get()->PutNumberArray("centerY",
		  llvm::ArrayRef<double>(center_y));
		vision_table.get()->PutNumberArray("width",
		  llvm::ArrayRef<double>(width));
		vision_table.get()->PutNumberArray("height",
		  llvm::ArrayRef<double>(height));
		vision_table.get()->GetInstance().Flush();
		center_x.clear();
		center_y.clear();
		width.clear();
		height.clear();
	}
}
