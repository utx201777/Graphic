#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <vector>
#include <set>
#include <algorithm>
#include <functional>
using namespace cv;
using namespace std;

int random(int number)
{
	srand(time(NULL));
	return rand() % number;
}
const int Height = 512;
const int Width = 512;
const int OverLapSize = 8;
const int TileSize = 32;
const int DefaultSize = 64;
using point = pair<int, int>;
bool operator<(const point v1, const point v2)
{
	if (v1.first == v2.first)
		return v1.second < v2.second;
	return v1.first < v2.first;
}

void generateInit(Mat &dest, Mat &source)
{	
	Mat roi_d(dest, Rect(0, 0, TileSize, TileSize));
	Mat roi_s(source, Rect(random(DefaultSize - TileSize), random(DefaultSize - TileSize), TileSize, TileSize));
	roi_s.copyTo(roi_d);
}

Rect getRect(int x,int y,int height,int width)
{
	return Rect(y, x, width, height);
}

void solve(set<point> & points, vector<vector<bool>> &mask, int x, int y, vector<vector<bool>> &tags)
{
	if (x < 0 || y < 0)
		return;
	mask[x][y] = true;
	tags[x][y] = true;
	int xn[][2] = { -1,0,0,-1 };
	for (int i = 0; i < 2; ++i)
	{
		int tmp_x = x + xn[i][0];
		int tmp_y = y + xn[i][1];
		if (tmp_x < 0 || tmp_y < 0)
			continue;
		point p(tmp_x, tmp_y);
		if (points.count(p) || tags[tmp_x][tmp_y])
			continue;
		solve(points, mask, tmp_x, tmp_y, tags);
	}
}


// 1是竖着的
void generateMask(Mat &area_1, Mat &area_2, Mat &c_area_1, Mat &c_area_2, vector<vector<bool>> &mask)
{
	vector<point> line_v;
	vector<point> line_h;
	// 计算竖着的第一行
	int idx;
	int dis = 1e9;
	for (int i = 0; i < OverLapSize; ++i)
	{
		int tmp = abs(area_1.at<uchar>(0, i) - c_area_1.at<uchar>(0, i));
		if (tmp < dis)
		{
			dis = tmp;
			idx = i;
		}
	}
	line_v.push_back(point(0, idx));
	for (int i = 1; i < TileSize; ++i)
	{
		int tmp_idx;
		dis = 1e9;
		for (int j = idx - 1; j <= idx + 1; ++j)
		{
			if (j < 0 || j >= OverLapSize)
				continue;
			int tmp = abs(area_1.at<uchar>(i,j) - c_area_1.at<uchar>(i, j));
			if (tmp < dis)
			{
				dis = tmp;
				tmp_idx = j;
			}
		}
		idx = tmp_idx;
		line_v.push_back(point(i, idx));
	}
	// 横着的第一行
	dis = 1e9;
	for (int i = 0; i < OverLapSize; ++i)
	{
		int tmp = abs(area_2.at<uchar>(i, 0) - c_area_2.at<uchar>(i, 0));
		if (tmp < dis)
		{
			dis = tmp;
			idx = i;
		}
	}
	line_h.push_back(point(idx, 0));
	for (int i = 1; i < TileSize; ++i)
	{
		int tmp_idx;
		dis = 1e9;
		for (int j = idx - 1; j <= idx + 1; ++j)
		{
			if (j < 0 || j >= OverLapSize)
				continue;
			int tmp = abs(area_2.at<uchar>(j, i) - c_area_2.at<uchar>(j,i));
			if (tmp < dis)
			{
				dis = tmp;
				tmp_idx = j;
			}
		}
		idx = tmp_idx;
		line_h.push_back(point(idx, i));
	}
	// 寻找链子
	int pos_h, pos_v;
	for(int i=0;i<TileSize;++i)
		for (int j = 0; j < TileSize; ++j)
		{
			if (line_v[i].first == line_h[j].first && line_v[i].second == line_h[j].second)
			{
				pos_v = i;
				pos_h = j;
				break;
			}
		}
	printf("insert: %d %d\n", pos_v, pos_h);
	set<point> points;
	points.insert(line_v[pos_v]);
	for (int i = pos_v + 1; i < TileSize; ++i)
		points.insert(line_v[i]);
	for (int i = pos_h + 1; i < TileSize; ++i)
		points.insert(line_h[i]);
	for (auto it : points);
		//printf("points: %d %d\n", it.first, it.second);
	// 搜寻mask
	vector<vector<bool>> tags(TileSize, vector<bool>(TileSize, 0));
	solve(points, mask, TileSize-1, TileSize - 1, tags);
}

int main()
{
	Mat source = imread("texture_1.jpg",0);
	printf("channels: %d\n", source.channels());
	printf("size: %d %d\n", source.rows,source.cols);
	
	Mat dest(Height, Width, CV_8UC1);
	generateInit(dest, source);

	// 第一行
	for (int i = TileSize - OverLapSize; i <= Width - TileSize; i+=(TileSize-OverLapSize))
	{
		Mat area(dest, getRect(0, i, TileSize, OverLapSize));
		double dis = 1e10;
		int x, y;
		for(int j = 0; j < DefaultSize -TileSize;++j)
			for (int k = 0; k < DefaultSize - TileSize; ++k)
			{
				Mat area_t(source, getRect(j, k, TileSize,OverLapSize));
				Mat res;
				absdiff(area, area_t, res);
				auto tmp_dis = sum(res)[0];
				if (tmp_dis < dis)
				{
					dis = tmp_dis;
					x = j;
					y = k;
				}
			}
		printf("dis %lf\n", dis);
		Mat copy_d(dest, getRect(0, i, TileSize, TileSize));
		Mat copy_s(source, getRect(x, y, TileSize, TileSize));
		copy_s.copyTo(copy_d);
	}	

	// 第一列
	for (int i = TileSize - OverLapSize; i <= Height - TileSize; i += (TileSize - OverLapSize))
	{
		Mat area(dest, getRect(i, 0, OverLapSize, TileSize));
		double dis = 1e10;
		int x, y;
		for (int m = 0; m <= DefaultSize - TileSize; ++m)
			for (int n = 0; n <= DefaultSize - TileSize; ++n)
			{
				Mat area_t(source, getRect(m, n, OverLapSize , TileSize));
				Mat res;
				absdiff(area, area_t, res);
				auto tmp_dis = sum(res)[0];
				if (tmp_dis < dis)
				{
					dis = tmp_dis;
					x = m;
					y = n;
				}
			}
		printf("dis %lf\n", dis);
		Mat copy_d(dest, getRect(i, 0, TileSize, TileSize));
		Mat copy_s(source, getRect(x, y, TileSize, TileSize));
		copy_s.copyTo(copy_d);
	}
	// 中间部分
	for (int i = TileSize - OverLapSize; i <= Height - TileSize; i += (TileSize - OverLapSize))
		for (int j = TileSize - OverLapSize; j <= Width - TileSize; j += (TileSize - OverLapSize))
		{
			Mat area_1(dest, getRect(i, j, TileSize, OverLapSize));// 竖着的
			Mat area_2(dest, getRect(i, j, OverLapSize, TileSize));// 横着的
			Mat area_0(dest, getRect(i, j, OverLapSize, OverLapSize));// 小方形
			double dis = 1e10;
			int x, y;
			for(int m = 0;m<=DefaultSize-TileSize;++m)
				for (int n = 0; n <= DefaultSize - TileSize; ++n)
				{
					Mat c_area_1(source, getRect(m, n, TileSize, OverLapSize));
					Mat c_area_2(source, getRect(m, n, OverLapSize, TileSize));
					Mat c_area_0(source, getRect(m, n, OverLapSize, OverLapSize));
					Mat res;
					absdiff(c_area_1, area_1, res);
					auto tmp_dis = sum(res)[0];
					absdiff(c_area_2, area_2, res);
					tmp_dis += sum(res)[0];
					absdiff(c_area_0, area_0, res);
					tmp_dis -= sum(res)[0];
					if (tmp_dis < dis)
					{
						dis = tmp_dis;
						x = m;
						y = n;
					}
				}
			printf("dis %lf\n", dis);
			Mat c_area_1(source, getRect(x, y, TileSize, OverLapSize));
			Mat c_area_2(source, getRect(x, y, OverLapSize, TileSize));			
			vector<vector<bool>> mask(TileSize, vector<bool>(TileSize, 0));
			generateMask(area_1, area_2, c_area_1, c_area_2, mask);// 计算掩码
			Mat copy_d(dest, getRect(i, j, TileSize, TileSize));
			Mat copy_s(source, getRect(x, y, TileSize, TileSize));
			for(int m=0;m<TileSize;++m)
				for (int n = 0; n < TileSize; ++n)
				{
					if (mask[m][n]) // 位于掩码的部分更新
						copy_d.at<uchar>(m, n) = copy_s.at<uchar>(m, n);
				}			
			//copy_s.copyTo(copy_d);
		}
	imshow("view", dest);
	waitKey(0);
	return 0;
}