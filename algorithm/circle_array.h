#ifndef CIRCLE_ARRAY_H_
#define CIRCLE_ARRAY_H_

#include <cstddef>
#include "../stock_base_info/stock_info.h"

template <class T>
class circle_array {
	size_t array_size;
	size_t cur_posi;
	size_t total_push_count;
	T* _array;
	circle_array(circle_array&);	//TODO--拷贝构造函数私有化，有时间再实现
public:
	explicit circle_array(size_t _size);
	~circle_array();

	void push_back(const T& value);

	T& operator[] (size_t index);

	size_t size() { return total_push_count > array_size ? array_size : total_push_count; };

	size_t find(T& value);
};

#endif
