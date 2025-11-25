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

auto guard = m.acquire();
int value = *guard; // value = 2
```

### RwLock

```c++
sync::RwLock<int> m(1);

{
  auto guard = m.write();
  *guard = 5;
}

auto guard = m.read();
int value = *guard; // value = 5

auto wrguard = m.write(); // error
```

## Building

This project uses the [beaver build system](https://github.com/Jomy10/beaver).

```sh
beaver
```
