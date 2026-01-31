// Example Program For EasyTGL
/*

	termios old_settings, new_settings;
	tcgetattr(STDIN_FILENO, &old_settings);
	new_settings = old_settings;
	new_settings.c_lflag &= ~(ICANON | ECHO);	// 关闭规范模式和回显
	new_settings.c_cc[VMIN] = 0;	// 至少读取一个字符
	new_settings.c_cc[VTIME] = 0;	// 无超时
	tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);	// 应用新设置
找到这段代码在Windows上运行的平替，要求必须无第三方依赖，允许使用Windows的API，不强制

*/
#include "EasyTGL.h"
#include "timer.h"

using EasyTGL::vec2, EasyTGL::vec2i, EasyTGL::rgb;

int main()
{
	EasyTGL::init();
	EasyTime::timer timer;
	timer.restart();
	//(void)getchar();
	EasyTGL::screen scr(400, 320);
	EasyTGL::screen scr2(560, 320, 400*2);
	//EasyTGL::screen scr(960, 320);	// 超大吃配置
	//EasyTGL::screen scr(128, 74);	// 滚轮缩小四次，终端全屏
	//EasyTGL::screen scr(160, 90);	// 滚轮缩小四次，终端全屏
	EasyTGL::clear_all();
	int frame = 1, x = 0;
	//while (true) {
	//	x = sin(frame * 0.001) * 100;
	//	//scr.clear(rgb(0, 0, 0));
	//	scr.fast_clear();
	//	scr.display();
	//	frame++; frame %= 31416;
	//}
	while (frame >= 1) {
		x = sin(frame * 0.0005) * 100;
		//scr.clear(rgb(0, 0, 0));
		scr.fast_clear();
		scr.draw_line(vec2i(x * 1.5, x * 0.4), vec2i(x + 1000, x + 500), rgb(255, 170, 0));
		scr.draw_triangle(vec2i(100 + x, 20 - x * 0.5), vec2i(200 + x * 0.1, 150 + x), vec2i(340 - x * 1.2, 50), rgb(255, 255, 255));
		scr.display();

		scr2.fast_clear();
		scr2.draw_line(vec2i(x * 1.5, x * 0.4), vec2i(x + 1000, x + 500), rgb(255, 170, 0));
		scr2.draw_triangle(vec2i(100 + x, 20 - x * 0.5), vec2i(200 + x * 0.1, 150 + x), vec2i(340 - x * 1.2, 50), rgb(255, 255, 255));
		scr2.display();
		frame++; frame %= 31416;
	}
	EasyTGL::shut();
	printf("%f", EasyTime::to_second(timer.elapsed()));
}
//optimized : 13.45s
//without optimize : 43s









