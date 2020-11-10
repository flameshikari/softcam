#include <softcamcore/Misc.h>
#include <gtest/gtest.h>


namespace {
namespace sc = softcam;

const char SHMEM_NAME[] = "shmemtest";
const char ANOTHER_NAME[] = "shmemtest2";
const unsigned long SHMEM_SIZE = 888;


TEST(Timer, Basic1) {
    sc::Timer timer;
    auto t1 = timer.get();
    auto t2 = timer.get();
    auto t3 = timer.get();

    EXPECT_GE( t1, 0.0f );
    EXPECT_GE( t2, t1 );
    EXPECT_GE( t3, t2 );
}

TEST(Timer, Sleep) {
    sc::Timer timer;
    EXPECT_NO_THROW({ sc::Timer::sleep(0.2f); });
    auto t = timer.get();

    EXPECT_GT( t, 0.0f );
}

TEST(Timer, Rewind) {
    sc::Timer timer;
    EXPECT_NO_THROW({ timer.rewind(0.2f); });
    auto t = timer.get();

    EXPECT_LT( t, 0.0f );
}

TEST(Timer, Reset) {
    sc::Timer timer;
    timer.sleep(0.2f);
    auto t1 = timer.get();
    timer.reset();
    auto t2 = timer.get();

    EXPECT_GE( t2, 0.0f );
    EXPECT_LT( t2, t1 );
}


TEST(SharedMemory, Basic1) {
    auto shmem = sc::SharedMemory::create(SHMEM_NAME, SHMEM_SIZE);

    EXPECT_TRUE( shmem );
    EXPECT_NE( shmem.get(), nullptr );
    EXPECT_GE( shmem.size(), SHMEM_SIZE );
}

TEST(SharedMemory, Basic2) {
    auto view1 = sc::SharedMemory::create(SHMEM_NAME, SHMEM_SIZE);
    auto view2 = sc::SharedMemory::open(SHMEM_NAME);

    EXPECT_TRUE( view1 );
    EXPECT_TRUE( view2 );
    EXPECT_NE( view1.get(), nullptr );
    EXPECT_NE( view2.get(), nullptr );
    EXPECT_NE( view1.get(), view2.get() );
    EXPECT_GE( view1.size(), SHMEM_SIZE );
    EXPECT_GE( view2.size(), SHMEM_SIZE );
}

TEST(SharedMemory, InvalidArgs) {
    {
        auto shmem = sc::SharedMemory::create(SHMEM_NAME, 0);
        EXPECT_FALSE( shmem );
        EXPECT_EQ( shmem.get(), nullptr );
        EXPECT_EQ( shmem.size(), 0 );
    }{
        auto shmem = sc::SharedMemory::create("\\", SHMEM_SIZE);
        EXPECT_FALSE( shmem );
        EXPECT_EQ( shmem.get(), nullptr );
        EXPECT_EQ( shmem.size(), 0 );
    }{
        auto view1 = sc::SharedMemory::create(SHMEM_NAME, SHMEM_SIZE);
        auto view2 = sc::SharedMemory::open(ANOTHER_NAME);
        EXPECT_FALSE( view2 );
        EXPECT_EQ( view2.get(), nullptr );
        EXPECT_EQ( view2.size(), 0 );
    }
}

TEST(SharedMemory, OpenBeforeCreateFails) {
    auto view1 = sc::SharedMemory::open(SHMEM_NAME);
    auto view2 = sc::SharedMemory::create(SHMEM_NAME, SHMEM_SIZE);

    EXPECT_FALSE( view1 );
    EXPECT_TRUE( view2 );
    EXPECT_EQ( view1.get(), nullptr );
    EXPECT_NE( view2.get(), nullptr );
    EXPECT_EQ( view1.size(), 0 );
    EXPECT_GE( view2.size(), SHMEM_SIZE );
}

TEST(SharedMemory, MultipleCreateFails) {
    auto view1 = sc::SharedMemory::create(SHMEM_NAME, SHMEM_SIZE);
    auto view2 = sc::SharedMemory::create(SHMEM_NAME, SHMEM_SIZE);

    EXPECT_TRUE( view1 );
    EXPECT_FALSE( view2 );
    EXPECT_EQ( view2.get(), nullptr );
    EXPECT_EQ( view2.size(), 0 );
}

TEST(SharedMemory, MultipleOpenSucceeds) {
    auto view1 = sc::SharedMemory::create(SHMEM_NAME, SHMEM_SIZE);
    auto view2 = sc::SharedMemory::open(SHMEM_NAME);
    auto view3 = sc::SharedMemory::open(SHMEM_NAME);

    EXPECT_TRUE( view1 );
    EXPECT_TRUE( view2 );
    EXPECT_TRUE( view3 );
    EXPECT_NE( view1.get(), nullptr );
    EXPECT_NE( view2.get(), nullptr );
    EXPECT_NE( view3.get(), nullptr );
    EXPECT_NE( view1.get(), view2.get() );
    EXPECT_NE( view1.get(), view3.get() );
    EXPECT_NE( view2.get(), view3.get() );
    EXPECT_GE( view1.size(), SHMEM_SIZE );
    EXPECT_GE( view2.size(), SHMEM_SIZE );
    EXPECT_GE( view3.size(), SHMEM_SIZE );
}

} //namespace