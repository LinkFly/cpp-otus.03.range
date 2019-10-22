#pragma once

#include <vector>
#include <functional>

//// Types
using vecstr = std::vector<std::string>;
using vecint = std::vector<int>;
using vec_vecint = std::vector<vecint>;
using func_str = std::function<void(std::string)>;

//// Utils
std::vector<std::string> split(const std::string& str, char d);

void read_lines(std::istream& stream, func_str fn_line_handler);

template<class ForwardIt, class T, class Func>
ForwardIt lower_bound_with_convertor2(ForwardIt start, ForwardIt end, const T& el, Func convertor);

template<class ForwardIt, class T, class Func>
ForwardIt lower_bound_with_convertor(ForwardIt start, ForwardIt end, const T& el, Func convertor);

// TODO!!! Generalize lower_bound_with_convertor and lower_bound_with_convertor2

template<class ForwardIt, class T, class Func>
ForwardIt lower_bound_with_convertor(ForwardIt start, ForwardIt end, const T& el, Func convertor)
{
	ForwardIt idx_finded = end;
	typename std::iterator_traits<ForwardIt>::difference_type len, mid;
	while (true) {
		ForwardIt idx = start;
		len = std::distance(start, end);
		if (len == 0) {
			break; // return idx_finded
		}
		else if (len == 1) {
			if (el == convertor(*idx)) {
				idx_finded = idx;
				break;
			}
		}
		mid = len / 2;
		std::advance(idx, mid);
		T val = convertor(*idx);
		if (el > val) {
			end = idx;
		}
		else if (el < val) {
			start = idx;
			std::advance(start, 1);
		}
		else if (el == val) {
			idx_finded = idx;
			end = idx;
		}
	}
	return idx_finded;
}

template<class ForwardIt, class T, class Func>
ForwardIt lower_bound_with_convertor2(ForwardIt start, ForwardIt end, const T& el, Func convertor)
{
	ForwardIt idx_finded = end;
	typename std::iterator_traits<ForwardIt>::difference_type len, mid;
	while (true) {
		ForwardIt idx = start;
		len = std::distance(start, end);
		if (len == 0) {
			break; // return idx_finded
		}
		else if (len == 1) {
			if (el == convertor(*idx)) {
				idx_finded = idx;
				break;
			}
		}
		mid = len / 2;
		std::advance(idx, mid);
		T val = convertor(*idx);
		// new
		if (el != val) {
			idx_finded = idx;
		}
		////
		if (el > val) {
			end = idx;
		}
		else if (el < val) {
			/*start = idx;
			std::advance(start, 1);*/
			idx_finded = end;
			break;
		}
		else if (el == val) {
			/*idx_finded = idx;
			end = idx;*/
			start = idx;
			std::advance(start, 1);
		}
	}
	return idx_finded;
}