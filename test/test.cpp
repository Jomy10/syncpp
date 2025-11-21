#include <bandit/bandit.h>

#include <sync/mutex.hpp>
#include <sync/rwlock.hpp>
#include <thread>

using namespace snowhouse;
using namespace bandit;

go_bandit([]() {
  describe("mutex", []() {
    it("works", []() {
      sync::Mutex<int> m(1);

      auto guard = m.acquire();

      AssertThat(m.try_acquire(), Is().EqualTo(std::nullopt));

      AssertThat((int)guard, Is().EqualTo(1));

      *guard = 2;

      AssertThat((int)guard, Is().EqualTo(2));

      guard.~MutexGuard<int>();

      auto guard2 = m.try_acquire();
      AssertThat(guard2.has_value(), Is().EqualTo(true));

      AssertThat((int)guard2.value(), Is().EqualTo(2));
    });
  });

  describe("rwlock", []() {
    #if __cplusplus >= 201710L && defined(SYNC_RWLOCK_IS_SHARED_MUTEX)
    it("works", []() {
      sync::RwLock<int> l(1);

      {
        auto h1 = std::thread([&l] {
          AssertThat(l.try_read().has_value(), Is().EqualTo(true));
        });
        auto h2 = std::thread([&l] {
          AssertThat(l.try_read().has_value(), Is().EqualTo(true));
        });

        auto h3 = std::thread([&l] {
          auto guard = l.try_read();
          AssertThat(guard.has_value(), Is().EqualTo(true));

          auto h4 = std::thread([&l] {
            AssertThat(l.try_read().has_value(), Is().EqualTo(true));
          });

          auto h5 = std::thread([&l] {
            AssertThat(l.try_write().has_value(), Is().EqualTo(false));
          });

          h4.join();
          h5.join();

          AssertThat(guard.value().value(), Is().EqualTo(1));
        });

        h1.join();
        h2.join();
        h3.join();
      }

      {
        auto guard = l.try_write();

        AssertThat(guard.has_value(), Is().EqualTo(true));

        AssertThat(l.try_read().has_value(), Is().EqualTo(false));

        auto h1 = std::thread([&l] {
          AssertThat(l.try_read().has_value(), Is().EqualTo(false));
        });

        *guard.value() = 2;

        AssertThat(guard.value().value(), Is().EqualTo(2));

        h1.join();
      }

      auto guard = l.try_read();

      AssertThat(guard.has_value(), Is().EqualTo(true));
      AssertThat(guard.value().value(), Is().EqualTo(2));
    });
    #else
    it("works", []() {
      sync::RwLock<int> l(1);

      {
        AssertThat(l.try_read() == nullptr, Is().EqualTo(false));

        auto guard = l.read();

        AssertThat(l.try_read() == nullptr, Is().EqualTo(false));

        AssertThat(l.try_write() == nullptr, Is().EqualTo(true));

        AssertThat(guard.value(), Is().EqualTo(1));
      }

      {
        auto guard = l.try_write();

        AssertThat(guard == nullptr, Is().EqualTo(false));

        AssertThat(l.try_read() == nullptr, Is().EqualTo(true));

        **guard = 2;

        AssertThat(**guard, Is().EqualTo(2));
      }

      auto guard = l.try_read();

      AssertThat(guard == nullptr, Is().EqualTo(false));
      AssertThat(guard->value(), Is().EqualTo(2));
    });
    #endif
  });
});

int main(int argc, char* argv[]) {
  return bandit::run(argc, argv);
}
