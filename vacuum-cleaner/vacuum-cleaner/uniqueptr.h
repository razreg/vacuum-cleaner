#ifndef __MAKEUNIQUE__H_
#define __MAKEUNIQUE__H_

#ifndef __cpp_lib_make_unique
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif // __cpp_lib_make_unique

#endif // __MAKEUNIQUE__H_