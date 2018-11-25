#include "circle_array.h"

template <class T>
circle_array<T>::circle_array(size_t _size) {
	array_size = _size;
	total_push_count = cur_posi = 0;
	_array = new T[array_size];
}

template <class T>
circle_array<T>::~circle_array() {
	delete[] _array;
}

template <class T>
void circle_array<T>::push_back(const T& value) {
	total_push_count++;
	_array[cur_posi++] = value;
	cur_posi = cur_posi % array_size;
}

template <class T>
T& circle_array<T>::operator[](size_t index) {
	size_t index_posi = index;
	if(total_push_count >= array_size) {
		index_posi = index + cur_posi;
	}
	index_posi %= array_size;
	return _array[index_posi];
}

template <class T>
size_t circle_array<T>::find(T& value) {
	size_t start_index = total_push_count >= array_size ? cur_posi + 1 : 0;
	size_t tmp_size = 0;
	for(size_t i = 0;i < array_size;i++) {
		tmp_size = start_index + i;
		tmp_size %= array_size;
		if(_array[tmp_size] == value) {
			return i;
		}
	}

	return -1;
}

/**
 * C++有很充足的原因不支持模板分离编译，有两种解决方式:
 * 1.模板类都写入到头文件当中
 * 2.通过下面的方式，声明式实例化模板类，但是其他模块用这个模板类的话就会受限制，只能用下面显示声明的类型
 */
template class circle_array<stock_info*>;

template class circle_array<int>; //TODO -- 测试用，等下需要删掉
