#include <vector>
#include <iostream>
#include <string>

#include <windows.h>
#include <typeinfo>
#include <ctime>

#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

template <class T, size_t N> size_t array_size(T(&)[N]) { return N; }

namespace boost_test
{
	void TestScopedPtr()
	{
		std::cout << "TestScopedPtr" << std::endl;

		boost::scoped_ptr<int> p{ new int{ 1 } };
		std::cout << *p << std::endl;
		p.reset(new int{ 2 });
		std::cout << *p.get() << std::endl;
		p.reset();
		std::cout << std::boolalpha << static_cast<bool>(p) << std::endl;
	}

	void TestScopedArray()
	{
		std::cout << "TestScopedArray" << std::endl;
		enum e_ar_count
		{
			ar_count = 2
		};

		boost::scoped_array<int> ar_p{ new int[ar_count] };
		*ar_p.get() = 2;
		ar_p[1] = 3;

		for (auto i = 0; i < ar_count; ++i)
			std::cout << ar_p[i] << std::endl;
	}

	void CloseWindowHandle(HANDLE handle)
	{
		CloseHandle(handle);
	}

	void TestSharedPtr()
	{
		std::cout << "TestSharedPtr" << std::endl;

		typedef boost::shared_ptr<int> int_ptr;
		int_ptr p1{ new int{ 1 } };
		int_ptr p2 = p1;
		std::cout << *p1 << std::endl << *p2 << std::endl;
		p1.reset();
		std::cout << std::boolalpha << static_cast<bool>(p1) << std::endl;
		std::cout << std::boolalpha << static_cast<bool>(p2) << std::endl;
		p2.reset();
		std::cout << std::boolalpha << static_cast<bool>(p2) << std::endl;

		{
			boost::shared_ptr<void> processPtr(OpenProcess(PROCESS_SET_INFORMATION, FALSE, GetCurrentProcessId()), CloseWindowHandle);
			UNREFERENCED_PARAMETER(processPtr);
		}
	}

	void TestMakeSharedPtr()
	{
		std::cout << "TestMakeSharedPtr" << std::endl;

		auto p1 = boost::make_shared<int>(22);
		std::cout << typeid(p1).name() << std::endl;
		auto p2 = boost::make_shared<int[10]>(10);
		std::cout << typeid(p2).name() << std::endl;
	}

	void TestBoostQuickAllocator()
	{
		std::cout << "TestBoostQuickAllocator";
		boost::shared_ptr<int> p;
		std::time_t then = std::time(nullptr);
		for (int i = 0; i < 1000000; ++i)
			p.reset(new int{ i });
		std::time_t now = std::time(nullptr);
		std::cout << now - then << '\n';
	}
}


void TestBoost()
{
	using namespace boost_test;
	TestScopedPtr();
	TestScopedArray();
	TestSharedPtr();
	TestMakeSharedPtr();
	TestBoostQuickAllocator();
}

int main()
{
	TestBoost();

	char ch;
	std::cin >> ch;
	return 0;
}


