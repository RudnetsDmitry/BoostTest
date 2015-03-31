#include <vector>
#include <iostream>
#include <string>

#include <windows.h>
#include <typeinfo>
#include <ctime>
#include <limits>
// for intrusive_ptr
#include <atlbase.h>

#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/chrono.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/optional.hpp>
#include <boost/intrusive_ptr.hpp>

//#define BOOST_SP_USE_QUICK_ALLOCATOR

namespace bcr = boost::chrono;

#ifdef max
#undef max
#endif

template <class T, size_t N> size_t array_size(T(&)[N]) { return N; }

struct DummyAddRef
{
	void AddRef(){}
	void Release(){}
};

// for intrusive_ptr
void intrusive_ptr_add_ref(IDispatch *p) { p->AddRef(); }
void intrusive_ptr_release(IDispatch *p) { p->Release(); }

void intrusive_ptr_add_ref(DummyAddRef *p) { p->AddRef(); }
void intrusive_ptr_release(DummyAddRef *p) { p->Release(); }

class CoInitializeGuard
{
public:
	CoInitializeGuard(){ CoInitialize(0); }
	~CoInitializeGuard(){ CoUninitialize(); }
};

namespace boost_test
{
	class Timer
	{
		boost::optional<bcr::system_clock::time_point> m_startTime;
		boost::optional<bcr::system_clock::duration> m_timeDuration; ///tick is 100 nanoseconds
	public:
		void Start()
		{
			m_startTime.reset(bcr::system_clock::now());
		}
		void Stop()
		{
			auto curTime = bcr::system_clock::now();
			assert(m_startTime);
			
			if (m_startTime)
				m_timeDuration.reset(curTime - *m_startTime);
			else
			{
				assert(false);
				m_timeDuration.reset();
			}
		}

		double DurationInInSec() const
		{
			auto res = DurationInInMicroSec();
			return res > 0 ? res / 1000000.0 : res;
		}

		__int64 DurationInInMicroSec() const
		{
			if (!m_timeDuration)
			{
				assert(false);
				return -1;
			}

			bcr::microseconds usec = bcr::duration_cast<bcr::microseconds>(*m_timeDuration);
			return (usec <= bcr::microseconds(0)) ? 0 : usec.count();
		}
	};
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

	void TestIntrusivePtr()
	{
		std::cout << "TestIntrusivePtr" << std::endl;

		CoInitializeGuard initialGuard;
		UNREFERENCED_PARAMETER(initialGuard);

		CLSID clsid;
		CLSIDFromProgID(CComBSTR{ "Scripting.FileSystemObject" }, &clsid);
		void *p;
		CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER, __uuidof(IDispatch), &p);
		boost::intrusive_ptr<IDispatch> disp{ static_cast<IDispatch*>(p), false };
		CComDispatchDriver dd{ disp.get() };
		CComVariant arg{ "C:\\Windows" };
		CComVariant ret{ false };
		dd.Invoke1(CComBSTR{ "FolderExists" }, &arg, &ret);
		std::cout << std::boolalpha << (ret.boolVal != 0) << std::endl;
	}
}


void TestBoost()
{
	using namespace boost_test;
	TestScopedPtr();
	TestScopedArray();
	TestSharedPtr();
	TestMakeSharedPtr();
	TestIntrusivePtr();
}

int main()
{
	TestBoost();

	std::cout << "Press any key to Exit" << std::endl;
	char ch;
	std::cin >> ch;
	return 0;
}
