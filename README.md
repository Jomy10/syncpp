# Sync

High-level library offering synchronization objects for C++, based on Rust's `std::sync`.

## objects

### Mutex

Lock a variable when reading/writing.

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

Multiple read locks, only one write lock.

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

### Once

Initializes a variable exectly once in a thread-safe manner.

```c++
sync::Once<int> o = sync::Once<int>();

o.get(); // std::nullopt

std::function<int(int)> fn = [](int n) {
  return n;
};

o.init_once(fn, 1);

o.get().value(); // 1

o.init_once(fn, 10);

o.get().value(); // 1
```

### Lazy

Initialize a variable exactly once with a single initializer.

```c++
sync::Lazy<int> l = sync::Lazy<int>([]() { return 10 });
l.get(); // 10
```

## Building

This project uses the [beaver build system](https://github.com/Jomy10/beaver).

```sh
beaver -- --no-test
```

### Build options

Options can be specified after `beaver --`. For example: `beaver -- --no-test --rwlock-is-pthread`.

| Option | Description | default | Min C++ version |
|--------|-------------|---------|-----------------|
| `mutex-is-pthread` | Mutex implemented using `pthread_mutex_t` (default: `std::mutex`). | ON if C++ version < 11 and Target OS is posix | N/A |
| `mutex-extension` | Allow extra functions for Mutex based on the specified implementation (not portable) | off | N/A |
| `rwlock-is-pthread` | RwLock implemented using `pthread_rwlock_t` | ON if Target OS is posix | N/A |
| `rwlock-is-shared-mutex` | RwLock implemented using `std::shared_mutex` (currently not fully implemented) | off | 17 |
| `rwlock-extension` | Allow extra functions for RwLock based on the specified implementation (not portable) | off | N/A |
| `no-test` | Don't build the test target | off | N/A |

## Running tests

```sh
beaver test
```

## Feature list

| Feature | Implementation | Min C++ version | OS support | Default | Build option |
|---------|----------------|-----------------|------------|---------|--------------|
| Mutex | `std::mutex` | C++ 11 | any | if C++ >= 11 | default |
| Mutex | `pthread_mutex_t` | N/A | posix | if C++ < 11 and Target OS is posix | `mutex-is-pthread` |
| RwLock | `std::shared_mutex` | C++ 17 | any | NO (currently not complete) | `rwlock-is-shared-mutex` |
| RwLock | `pthread_rwlock_t` | N/A | posix | Target OS is posix | `rwlock-is-pthread` |
| Once | `std::once_flag` and `std::call_once` | C++ 17 | any | YES | None |
| Lazy | `Once` | C++ 17 | any | YES | None |

## Notes

In the Rust's std::sync, there are some types which will not be implemented in this library
as there are already alternatives in the standard library.

- `Once`: `std::call_once`
- `Arc`: `std::atomic<std::shared_ptr>`
