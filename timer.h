#pragma once

#include <chrono>
#include <thread>
#include <algorithm>
namespace EasyTime {
	float to_second(long long milliseconds) { return milliseconds * 0.001; }
	struct timer
	{
	public:
		timer() { restart(); }
		void restart() { start_stamp = getTime(); }
		long long elapsed() const { return getTime() - start_stamp; }
		static void sleep(double time/*秒*/) { sleep_milliseconds(time * 1e3); }
		static void sleep_milliseconds(long long time/*毫秒*/)
		{
			long long t = getTime() + time;
			std::this_thread::sleep_for(std::chrono::milliseconds(std::max(time - 50, 0LL)));
			while (getTime() < t) std::this_thread::yield();
		}
		static void wait_until(long long timestamp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(std::max(timestamp - getTime() - 50, 0LL)));
			while (getTime() < timestamp) std::this_thread::yield();
		}
		/*
		* 传入本地时间，不是UTC
		*/
		static void wait_until(int year, int month, int day, int hour = 0, int minute = 0, int second = 0)
		{
			std::tm time = {};
			time.tm_year = year - 1900;
			time.tm_mon = month - 1;
			time.tm_mday = day;
			time.tm_hour = hour;
			time.tm_min = minute;
			time.tm_sec = second;

			wait_until(
				std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::from_time_t(
						mktime(&time)
					).time_since_epoch()
				).count()
			);

			return;
		}
		/*
		* @param date 年月日 传入8位字符串 如"20110302"
		* @param time 时分秒 传入6位字符串 如"161905"
		* */
		static void wait_until(const std::string& date, const std::string& time)
		{
			wait_until(
				std::atoi(date.substr(0, 4).c_str()),
				std::atoi(date.substr(4, 2).c_str()),
				std::atoi(date.substr(6, 2).c_str()),
				std::atoi(time.substr(0, 2).c_str()),
				std::atoi(time.substr(2, 2).c_str()),
				std::atoi(time.substr(4, 2).c_str())
			);
		}
		/*
		* 获取系统毫秒时间戳
		*/
		static long long getTime()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()
			).count();
		}
	private:
		long long start_stamp;	// 计时器开始的时间
	};
}