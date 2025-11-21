# Sync

High-level library offering synchronization objects for C++, based on Rust's `std::sync`.

## objects

### Mutex

```c++
sync::Mutex<int> m(1);

{
  auto guard = m.acquire();
  int value = guard; // value = 1

  *guard = 2;
  value = guard; // value = 2
}

auto guard = guard.acquire();
int value = *guard; // value = 2
```
