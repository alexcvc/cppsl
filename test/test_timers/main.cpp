#include <thread>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include "cppsl/time/functions.hpp"
#include "cppsl/time/watch.hpp"
#include "cppsl/time/stopTimer.hpp"
#include "gtest/gtest.h"

TEST(TestTimeFunctions, BoolTest) {   // 12/2/2020 -> 737761

  using std::chrono::system_clock;
  auto now = system_clock::now();

  std::clog << "==== " << cppsl::time::toString(now) << '\n';

  using DD = std::chrono::duration<std::size_t, std::ratio<2, 3>>;
  using TP = std::chrono::time_point<std::chrono::system_clock, DD>;

  std::array<std::string, 4> expected{"1970-Jan-01 01:00:00.000000000000000", "1970-Jan-01 01:00:00.666666666666667",
                                      "1970-Jan-01 01:00:01.333333333333333", "1970-Jan-01 01:00:02.000000000000000"};
  for (int i = 0; i <= 3; ++i) {
    auto answer{cppsl::time::toString(TP(DD(i)))};
    EXPECT_EQ(expected[i], answer);
    std::clog << "==== " << cppsl::time::toString(TP(DD(i))) << std::endl;
  }
}

TEST(TestTimeFunctions, ComparePositiveTest) {   // 12/2/2020 -> 737761

  using std::chrono::system_clock;

  for (std::pair<std::string, std::string> s : {
          std::pair("2017-May-01 00:10:15", "2017-May-01 01:10:15.000000000000000"),
          std::pair("2017-May-01 00:10:15.25", "2017-May-01 01:10:15.249999761581421"),
          std::pair("2017-Mar-01", "2017-Mar-01 00:00:00.000000000000000"),
          std::pair("1969-Dec-31 19:00:00.66666666", "1969-Dec-31 19:00:00.666666659999464"),
       }) {
    try {
      auto answer = cppsl::time::toString(cppsl::time::fromString<system_clock::time_point>(s.first));
      EXPECT_EQ(answer, s.second);

      std::clog << "--> input    >-- " << s.first << std::endl;
      std::clog << "--< expected >-- " << s.second << std::endl;
      std::clog << "--< answer   >-- " << answer << std::endl;

    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }
}

TEST(TestTimeFunctions, CompareNegativeTest) {   // 12/2/2020 -> 737761

  using std::chrono::system_clock;

  for (std::pair<std::string, std::string> s : {
          std::pair("2018", "2017-Dec-31 00:00:00.000000000000000"),   // underspecified - but succeeds as 2017-12-31
          std::pair("1752-May-5", "1752-May-05 00:00:00.000000000000000"),   // out of range
          std::pair("not a date", "get_time"),
       }) {
    try {
      auto answer = cppsl::time::toString(cppsl::time::fromString<system_clock::time_point>(s.first));
      EXPECT_EQ(answer, s.second);

      //      std::clog << "-->>>-- " << s.first << std::endl;
      //      std::clog << "--<<<-- " << answer << std::endl;

    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }
}

TEST(TestTimers, StopWatch) {
  using namespace std::chrono_literals;

  cppsl::time::StopWatch<std::chrono::seconds> timer1;
  cppsl::time::StopWatch<std::chrono::seconds> timer2;

  auto tp1 = timer1.Go();
  std::this_thread::sleep_for(1s);
  auto tp2 = timer2.Go();

  EXPECT_GE(tp2, tp1);

  auto el1 = timer1.ElapsedTime();
  auto el2 = timer2.ElapsedTime();

  EXPECT_GE(el1 - 1s, el2);

  timer1.Stop();
  tp1 = timer1.Go();

  EXPECT_GT(tp2, tp1);
}

TEST(TestTimers, TimerWatch) {
  using namespace std::chrono_literals;

  cppsl::time::TimerWatch timer1;

  timer1.SetTimeout(2s);
  timer1.Go();

  while (!timer1.IsElapsed()) {
    std::this_thread::sleep_for(500ms);
  }

  EXPECT_GT(timer1.ElapsedTime(), 2000ms);
}

TEST(TestTimers, RoundWatch) {
  using namespace std::chrono_literals;

  cppsl::time::RoundWatch timer;

  timer.Go();
  std::this_thread::sleep_for(1s);
  timer.StoreLap();
  std::this_thread::sleep_for(1s);
  timer.StoreLap();
  std::this_thread::sleep_for(1s);
  timer.StoreLap();
  std::this_thread::sleep_for(1s);
  timer.StoreLap();
  std::this_thread::sleep_for(1s);
  timer.StoreLap();
  timer.Stop();

  for (auto& iter : timer.Laps()) {
    auto st1 = fmt::format("{}", static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(iter.total_time).count()));
    auto st2 = fmt::format("{}", static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(iter.split_time).count()));
    std::clog << st1 << " with split time: " << st2 << std::endl;
  }
}

TEST(TestTimers, TesStopTimer) {
  using namespace std::chrono_literals;

  cppsl::time::StopTimer timer;

  timer.SetTimeout(1050ms);

  timer.Start();
  for (auto i = 0; i < 10; ++i) {
    std::this_thread::sleep_for(100ms);
    auto res = timer.IsElapsed();
    EXPECT_TRUE( res.has_value() && !res.value() );
  }
  std::this_thread::sleep_for(200ms);
  auto res = timer.IsElapsed();
  EXPECT_TRUE( res.has_value() && res.value() );
}

