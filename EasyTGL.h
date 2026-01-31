#pragma once

/* Platforms Supported : Windows, Linux*/
#define EASYTGL_PLATFORM "Windows"
#ifndef EASYTGL_THREAD_NUM
#define EASYTGL_THREAD_NUM 16
#endif
#define EASYTGL_USE_OUTPUT_BUFFER true
#define EASYTGL_DEFAULT_MAIN_BUFFER_SIZE 65536*2048
#define EASYTGL_DEFAULT_frame_buffer_SIZE 65536*1024

#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <concepts>
#include <iostream>
#include <algorithm>

namespace EasyTGL {
	#pragma region Vector2
	template <typename T>
	struct vector2
	{
		T x, y;
		vector2() = default;
		vector2(T _x, T _y)
		{
			this->x = _x, this->y = _y;
			return;
		}
		// 判断点是否在给出的矩形内
		bool in_area(vector2<T> _min, vector2<T> _max) const
		{
			T _x = this->x;
			T _y = this->y;
			if (_x < _min.x or _x > _max.x or _y < _min.y or _y > _max.y)
				return false;
			return true;
		}
	};
	template <typename T>
	vector2<T> operator - (const vector2<T>& vec)
	{
		return vector2<T>(-vec.x, -vec.y);
	}
	template <typename T>
	vector2<T> operator + (const vector2<T>& a, const vector2<T>& b)
	{
		return vector2<T>(a.x + b.x, a.y + b.y);
	}
	template <typename T>
	vector2<T> operator - (const vector2<T>& a, const vector2<T>& b)
	{
		return a + (-b);
	}

	using vec2i = vector2<int>;
	using vec2 = vector2<float>;
#pragma endregion
	#pragma region Vector3
	template <typename T>
	struct vector3
	{
		T x, y, z;
		vector3() = default;
		vector3(T _x, T _y, T _z)
		{
			this->x = _x, this->y = _y, this->z = _z;
			return;
		}
		// 判断点是否在给出的矩形内
		bool in_area(vector3<T> _min, vector3<T> _max) const
		{
			T _x = this->x;
			T _y = this->y;
			if (_x < _min.x or _x > _max.x or _y < _min.y or _y > _max.y)
				return false;
			return true;
		}
	};
	template <typename T>
	vector3<T> operator - (const vector3<T>& vec)
	{
		return vector3<T>(-vec.x, -vec.y);
	}
	template <typename T>
	vector3<T> operator + (const vector3<T>& a, const vector3<T>& b)
	{
		return vector3<T>(a.x + b.x, a.y + b.y);
	}
	template <typename T>
	vector3<T> operator - (const vector3<T>& a, const vector3<T>& b)
	{
		return a + (-b);
	}

	using vec3i = vector3<int>;
	using vec3 = vector3<float>;
#pragma endregion
	#pragma region RGB
	struct rgb
	{
		uint8_t r, g, b, a;
		rgb() = default;
		rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0) : r(r), g(g), b(b), a(a) { return; }
	};
	bool operator == (const rgb& a, const rgb& b)
	{
		return a.r == b.r and a.g == b.g and a.b == b.b;
	}
	bool operator != (const rgb& a, const rgb& b)
	{
		return !(a == b);
	}
#pragma endregion
	#pragma region Image2D
	struct image
	{
		int width, height;	// 窗口的宽和高
		std::vector<rgb> data;

		image(int _width, int _height, const std::vector<rgb>& _data)
		{
			width = _width;
			height = _height;
			memcpy(data.data(), _data.data(), sizeof(rgb) * _data.size());
			return;
		}
	};
#pragma endregion
	using std::shared_ptr, std::vector, std::is_same_v;
	vector<char> MAIN_BUFFER(EASYTGL_DEFAULT_MAIN_BUFFER_SIZE);

	void init(uint32_t main_buffer_size = EASYTGL_DEFAULT_MAIN_BUFFER_SIZE)
	{
		MAIN_BUFFER.reserve(main_buffer_size);
		MAIN_BUFFER.resize(main_buffer_size);
		setvbuf(stdout, MAIN_BUFFER.data(), _IOLBF, EASYTGL_DEFAULT_MAIN_BUFFER_SIZE);	// 设置终端的自定义输出缓冲区
		return;
	}

	#pragma region Screen
	// screen中像素坐标以(0,0)为最左上角
	class screen
	{
	public:
		screen(int _width, int _height, int pos_x = 1, int pos_y = 1, int _pixel_width = 2, uint32_t buffer_size = EASYTGL_DEFAULT_frame_buffer_SIZE)
		{
			//_text_buffer.resize(buffer_size);	// 自定义输出缓冲区大小
			//memset(_text_buffer.data(), 0, _text_buffer.size() * sizeof(char));
			//setvbuf(stdout, _text_buffer.data(), _IOLBF, _text_buffer.size());	// 设置终端的自定义输出缓冲区

			this->width = _width;
			this->height = _height;
			this->pixel_width = _pixel_width;
			this->basic_pixel = new char[pixel_width + 1];
			this->global_pos = vec2i(pos_x, pos_y);
			this->frame_buffer.resize(_width * _height);
			this->lastframe_frame_buffer.clear();
			for (int i = 0; i < pixel_width; i++) basic_pixel[i] = ' ';
			basic_pixel[pixel_width] = '\0';
			this->fast_clear();
			return;
		}
		~screen()
		{
			delete[] this->basic_pixel;
			setvbuf(stdout, NULL, _IONBF, 0);
			return;
		}
		#pragma region 2D Drawing Operation
		inline void draw_pixel(const vec2i& point, const rgb& color)
		{
			if (!in_screen(point)) return;
			frame_buffer[point.x + point.y * width] = color;
			return;
		}
		inline void draw_pixel_without_check(const vec2i& point, const rgb& color)
		{
			frame_buffer[point.x + point.y * width] = color;
			return;
		}
		void draw_line(const vec2i& _p1, const vec2i& _p2, const rgb& color)
		{
			vec2i p1 = _p1, p2 = _p2;
			if (!in_screen(p1)) {
				if (in_screen(p2)) std::swap(p1, p2);
			}
			
			int x1 = p1.x, y1 = p1.y;
			int x2 = p2.x, y2 = p2.y;

			int dx = abs(x2 - x1);
			int dy = abs(y2 - y1);
			int sx = (x1 < x2) ? 1 : -1;  // x方向步进
			int sy = (y1 < y2) ? 1 : -1;  // y方向步进
			int err = dx - dy;  // 误差项

			// 循环直到到达终点
			while (!in_screen(x1, y1)) {
				// 到达终点时退出循环
				if (x1 == x2 && y1 == y2) break;

				// 计算误差调整
				int e2 = 2 * err;
				if (e2 > -dy) {  // 误差累积到需要调整x方向
					err -= dy;
					x1 += sx;
				}
				if (e2 < dx) {   // 误差累积到需要调整y方向
					err += dx;
					y1 += sy;
				}
			}
			// 循环直到到达终点
			while (in_screen(x1,y1)) {
				draw_pixel(vec2i(x1, y1), color);  // 绘制当前像素

				// 到达终点时退出循环
				if (x1 == x2 && y1 == y2) break;

				// 计算误差调整
				int e2 = 2 * err;
				if (e2 > -dy) {  // 误差累积到需要调整x方向
					err -= dy;
					x1 += sx;
				}
				if (e2 < dx) {   // 误差累积到需要调整y方向
					err += dx;
					y1 += sy;
				}
			}
			return;
		}
		void draw_hline(const int& y, const int& _x1, const int& _x2, const rgb& color)
		{
			if (y < 0 or y >= height) return;
			int x1 = _x1, x2 = _x2;
			x1 = std::clamp(x1, 0, width - 1);
			x2 = std::clamp(x2, 0, width - 1);
			if (x1 > x2) std::swap(x1, x2);
			if (x1 == x2) return;
			for (; x1 <= x2; x1++) {
				draw_pixel_without_check(vec2i(x1, y), color);
			}
			return;
		}
		void draw_vline(const int& x, const int& _y1, const int& _y2, const rgb& color)
		{
			if (x < 0 or x >= width) return;
			int y1 = _y1, y2 = _y2;
			y1 = std::clamp(y1, 0, height - 1);
			y2 = std::clamp(y2, 0, height - 1);
			if (y1 > y2) std::swap(y1, y2);
			if (y1 == y2) return;
			for (; y1 <= y2; y1++) {
				draw_pixel_without_check(vec2i(x, y1), color);
			}
			return;
		}
		void draw_triangle(vec2i p1, vec2i p2, vec2i p3, rgb color, bool filled = true)
		{
			if (filled) draw_fill_triangle(p1, p2, p3, color);
			else draw_line_triangle(p1, p2, p3, color);
			return;
		}
		void display(bool all_redraw = false)
		{
			auto put_pixel = [&](const int& x, const int& y, const rgb& color) -> void
				{
					printf(
						"\033[%d;%dH\033[48;2;%d;%d;%dm%s",
						global_pos.y + y,
						global_pos.x + x,
						color.r, color.g, color.b,
						basic_pixel
					);
					return;
				};
			printf("\033[0m");
			if (all_redraw) {
				//printf("\033[%d;%dH", global_pos_y, global_pos_x);
				for (int idx = 0; idx < width * height; idx++) {
					//printf("\033[%d;%dH", global_pos.y + (idx / width), global_pos.x + (idx % width) * pixel_width);
					//printf("\033[48;2;%d;%d;%dm%s", frame_buffer[idx].r, frame_buffer[idx].g, frame_buffer[idx].b, basic_pixel);
					put_pixel(
						idx / width,
						(idx % width) * pixel_width,
						frame_buffer[idx]
					);
				}
				lastframe_frame_buffer = frame_buffer;
				//std::swap(lastframe_frame_buffer, frame_buffer);
			}
			else {
				std::vector<int> difference = get_difference();
				for (int i = 0; i < difference.size(); i++) {
					int idx = difference[i];
					int x = idx % width;
					int y = idx / width;
					rgb* color = &frame_buffer[idx];

					printf("\033[%d;%dH", global_pos.y + y, global_pos.x + x * pixel_width);
					if (i == 0 or frame_buffer[difference[i - 1]] != *color) {
						printf("\033[48;2;%d;%d;%dm", color->r, color->g, color->b);
					}
					printf("%s", basic_pixel);
					lastframe_frame_buffer[idx] = frame_buffer[idx];
				}
			}
			fflush(stdout);
			printf("\033[0m\033[38;2;255;255;255m");
			return;
		}
		inline bool in_screen(const vec2i& p)
		{
			return p.in_area(vec2i(0, 0), vec2i(width - 1, height - 1));
		}
		inline bool in_screen(int x, int y)
		{
			return (x >= 0 and x < width) and (y >= 0 and y < height);
		}
		void draw_image(vec2i _pos, image* _image)
		{
			/*
			* _pos 图像左上角坐标(从0开始)
			* _image 图片
			*/
			for (int i = 0; i < _image->width; i++) {
				for (int j = 0; j < _image->height; j++) {
					draw_pixel(vec2i(i + _pos.x, j + _pos.y), _image->data[j * _image->width + i]);
				}
			}
			return;
		}
#pragma endregion
		image capture()
		{
			return image(width, height, frame_buffer);
		}
		void clear(rgb color = rgb(0, 0, 0))
		{
			for (int i = 0; i < frame_buffer.size(); i++) {
				frame_buffer[i] = color;
			}
		}
		void fast_clear()
		{
			memset(frame_buffer.data(), 0x00, sizeof(rgb) * frame_buffer.size());
			return;
		}

	//private:
		std::vector<char> screen_buffer;
		vec2i global_pos;	// 窗口左上角在命令行中的全局位置从(1,1)开始
		int width, height;	// 窗口的宽和高
		int pixel_width;	// 像素的宽度(单位 字符) (像素高度固定为1个字符)
		char* basic_pixel;	// 一个逻辑像素的图元(字符串)
		std::vector<rgb> frame_buffer;	// 当前帧
		std::vector<rgb> lastframe_frame_buffer;	// 上一帧

		float calc_slope(float dx, float dy)
		{
			if (dy == 0) return 0;  // 水平线，斜率为0
			return dx / dy;
		}
		float calc_slope(const vec2i& a, const vec2i& b)
		{
			float dx = a.x - b.x;
			float dy = a.y - b.y;

			if (dy == 0) return 0;  // 水平线，斜率为0
			return dx / dy;
		}
		void sort_by_y(vec2i* a, vec2i* b, vec2i* c)
		{
			if (a->y > b->y) std::swap(*a, *b);
			if (a->y > c->y) std::swap(*a, *c);
			if (b->y > c->y) std::swap(*b, *c);
			return;
		}
		void draw_line_triangle(vec2i p1, vec2i p2, vec2i p3, rgb color)
		{
			draw_line(p1, p2, color);
			draw_line(p1, p3, color);
			draw_line(p2, p3, color);
			return;
		}
		void draw_fill_triangle(vec2i p1, vec2i p2, vec2i p3, rgb color)
		{
			sort_by_y(&p1, &p2, &p3);
			float k1 = calc_slope(p1, p3);
			float k2 = calc_slope(p1, p2);
			float k3 = calc_slope(p2, p3);

			//上半部分 从p1到p2 水平扫描线
			float x_left = p1.x, x_right = p1.x;
			for (int y = p1.y; y < p2.y; y++) {
				draw_hline(y, (int)std::floor(x_left), (int)std::floor(x_right), color);
				x_left += k2;  // 左边界
				x_right += k1; // 右边界
			}

			//下半部分 从p2到p3 水平扫描线
			x_left = p2.x;  // 重新开始
			for (int y = p2.y; y <= p3.y; y++) {
				draw_hline(y, (int)std::floor(x_left), (int)std::floor(x_right), color);
				x_left += k3;  // 左边界
				x_right += k1; // 右边界
			}
			return;
		}
		// 返回两帧之间变化(脏区域)像素索引
		std::vector<int> get_difference()
		{
			std::vector<int> result;
			result.reserve(width * height);
			//#pragma omp parallel for num_threads(a)
			for (int i = 0; i < frame_buffer.size(); i++) {
				if (i >= lastframe_frame_buffer.size()) {
					result.push_back(i);
					lastframe_frame_buffer.push_back(rgb());
					continue;
				}
				if (frame_buffer[i] != lastframe_frame_buffer[i]) {
					result.push_back(i);
					//printf("\033[0m1111111111111111");
				}
			}
			return result;
		}
		std::map<int, int> get_triangle_scan_line(vec2i p1, vec2i p2)
		{
			std::map<int, int> result;
			int x1 = p1.x, y1 = p1.y;
			int x2 = p2.x, y2 = p2.y;

			int dx = abs(x2 - x1);
			int dy = abs(y2 - y1);
			int sx = (x1 < x2) ? 1 : -1;  // x方向步进
			int sy = (y1 < y2) ? 1 : -1;  // y方向步进
			int err = dx - dy;  // 误差项

			// 循环直到到达终点
			while (true) {
				result[y1] = x1;

				// 到达终点时退出循环
				if (x1 == x2 && y1 == y2) break;

				// 计算误差调整
				int e2 = 2 * err;
				if (e2 > -dy) {  // 误差累积到需要调整x方向
					err -= dy;
					x1 += sx;
				}
				if (e2 < dx) {   // 误差累积到需要调整y方向
					err += dx;
					y1 += sy;
				}
			}
			return result;
		}
	};
#pragma endregion
	void clear_all() { printf("\033[2J"); }
}