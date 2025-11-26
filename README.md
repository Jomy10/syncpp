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
beaver -- --no-test
```

### Build options

Options can be specified after `beaver --`. For example: `beaver -- --no-test --rwlock-is-pthread`.

| Option | Description | On if |
|--------|-------------|-------|
| `rwlock-is-pthread` | RwLock implemented using `pthread_rwlock_t` | Target OS is posix |
| `rwlock-is-shared-mutex` | RwLock implemented using `std::shared_mutex` (currently not fully implemented) | specified |
| `rwlock-extension` | Allow extra functions for RwLock based on the specified implementation (not portable) | specified |
| `mutex_is_pthread` | Mutex implemented using `pthread_mutex_t` (default: `std::mutex`). | C++ version < 11 and Target OS is posix |
| `mutex-extension` | Allow extra functions for Mutex based on the specified implementation (not portable) | specified |
