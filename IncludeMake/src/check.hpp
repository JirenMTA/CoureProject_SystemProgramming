#ifndef CHECK_HPP
#define CHECK_HPP 1

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <type_traits>
#include <chrono>
#include <string>
namespace DO_NOT_USE_DIRECTLY {
    [[noreturn]]
	inline void error(const char* file, int line) {
		auto tmp = errno;//fprintf may fail, so we preserve errno
		fprintf(stderr, "%s (line %d) :", file, line);
        fflush(stderr);
		errno = tmp;
		perror(nullptr);
		exit(EXIT_FAILURE);
	}

	inline int xcheck(int p, const char* file, int line) {
		if (p < 0) error(file, line);
		return p;
	}

	template<typename T>
	inline T* xcheck(T* p, const char* file, int line) {
		if (p == nullptr)  error(file, line);
		return p;
	}

    inline bool is_error_allowed(int allowed_code){
        return errno==allowed_code;
    }

    template <typename... TErrors>
    inline bool is_error_allowed(int allowed_code, TErrors... allowed_codes){
        if(errno != allowed_code)
            return false;
        return is_error_allowed(allowed_codes...);
    }

    template <typename... TErrors>
    inline int xcheck_except(int p,const char* file, int line, TErrors... allowed_codes){
        if(p>= 0 || is_error_allowed(allowed_codes...))
            return p;
        error(file, line);
    }

    template <typename T, typename... TErrors>
    inline T xcheck_except(T* p,const char* file, int line, TErrors... allowed_codes){
        if(p|| is_error_allowed(allowed_codes...))
            return p;
        error(file, line);
    }

}

//USE ONLY THIS MACRO
//Example: int fd = check(open("file", O_CREAT|O_RDWR, S_IRWXU));
#define check(x) DO_NOT_USE_DIRECTLY::xcheck(x, __FILE__, __LINE__ )
#define check_except(x,...) DO_NOT_USE_DIRECTLY::xcheck_except(x, __FILE__, __LINE__, __VA_ARGS__)

//https://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros

// if you wish to avoid the macro (set C++20 in CMakeLists or in Project settings in VS)
//https://en.cppreference.com/w/cpp/utility/source_location

struct scope_timer{
    using clock = std::chrono::high_resolution_clock;
    decltype(clock::now()) start_time;
    const std::string name;
    explicit scope_timer(std::string name = "") : name(std::move(name)) {start_time = clock::now();}
    ~scope_timer(){
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start_time);
        printf("%s Time: %lf\n",name.c_str(),(double)duration.count() / 1000);
    }
};

inline void msleep(int ms){
    timespec t{.tv_sec = ms/1000, .tv_nsec=ms%1000*1000000};
    nanosleep(&t, nullptr);
}

#endif // !CHECK_HPP