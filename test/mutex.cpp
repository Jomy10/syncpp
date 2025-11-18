#include <bandit/bandit.h>

#include <sync/mutex.hpp>

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
});

int main(int argc, char* argv[]) {
  return bandit::run(argc, argv);
}
