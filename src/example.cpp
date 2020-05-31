#include "example.h"

Dummy::Dummy() {

}

bool Dummy::doSomething() {
    const int digits[2] = {0, 1};
    return digits[0] + digits[1];
}


#ifdef ENABLE_DOCTEST_IN_LIBRARY
#include "doctest.h"
TEST_CASE("we can have tests written here, to test impl. details")
{
    CHECK(true);
}
#endif
