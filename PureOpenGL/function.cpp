#include "function.h"
#include <sstream>
#include <chrono>
#include <random>
using namespace std;

namespace PureOpenGL
{
	std::ostream & operator<<(std::ostream & onfs, const glm::vec3 & v)
	{
		onfs << v.x << " " << v.y << " " << v.z;
		return onfs;
	}

	std::ostream & operator<<(std::ostream & onfs, const glm::ivec3 & v)
	{
		onfs << v.x << " " << v.y << " " << v.z;
		return onfs;
	}

	std::istream & operator>>(std::istream & infs, glm::vec3 & v)
	{
		infs >> v.x >> v.y >> v.z;
		return infs;
	}

	std::istream & operator>>(std::istream & infs, glm::ivec3 & v)
	{
		infs >> v.x >> v.x >> v.z;
		return infs;
	}

	std::string treatSourcePath(std::string path)
	{
		extern std::string recource_path;
		return (recource_path + path);
	}

	std::string treatShaderPath(std::string path)
	{
		extern std::string shader_path;
		return (shader_path + path);
	}

	std::ostream & operator<< (std::ostream & onfs, const std::vector<char> & v)
	{
		for (auto & i : v)
			onfs << i;
		return onfs;
	}

	void transfer()
	{
		using namespace std;
		ifstream infs;
		infs.open(treatSourcePath("light_pos.dat"));
		infs.seekg(0, ios::end);
		long long size = infs.tellg();
		vector<char> vc(size);
		infs.seekg(0, ios::beg);
		infs.read(&vc[0], size);
		stringstream ss;
		ss << vc;

		int light_cnt;
		ss >> light_cnt;
		vector<glm::vec3> v1(light_cnt);
		for (int i = 0; i < light_cnt; ++i)
			ss >> v1[i].x >> v1[i].y >> v1[i].z;
		infs.close();
		ofstream onfs(treatSourcePath("light_pos.txt"));
		onfs << light_cnt << endl;
		for (int i = 0; i < light_cnt; ++i)
		{
			onfs << v1[i].x << " " << v1[i].y << " " << v1[i].z << " ";
			onfs << 1 << " " << 1 << " " << 1 << " " << 1 << endl;
		}
	}

	// Error´¦Àíº¯Êý
	void Error(std::string msg)
	{
		printf("ERROR: %s\n", msg.c_str());
		getchar();
		exit(0);
	}

	int randomNumber(int range)
	{
		std::random_device rd;
		return rd() % range;
	}

	glm::vec3 randomColor()
	{
		return glm::vec3(randomNumber(256) / 255.0f, randomNumber(256) / 255.0f, randomNumber(256) / 255.0f);
	}

	std::string getImgName()
	{
		using namespace std::chrono;
		using namespace std;
		steady_clock::duration d = steady_clock::now().time_since_epoch();
		milliseconds mil = duration_cast<milliseconds>(d);
		return "img/" + to_string(mil.count()) + ".png";
	}

	std::pair<float, float> screen2NDC(int posx, int posy, int width, int height)
	{
		posx = posx - width / 2;
		posy = posy - height / 2;
		float px = (float)posx / (width / 2);
		float py = (float)posy / (height / 2);
		return make_pair(px, py);
	}
}