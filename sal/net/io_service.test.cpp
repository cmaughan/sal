#include <sal/net/io_service.hpp>
#include <sal/common.test.hpp>


#if __sal_os_windows


namespace {


using net_io_service = sal_test::fixture;


TEST_F(net_io_service, ctor)
{
}


} // namespace


#endif // __sal_os_windows
