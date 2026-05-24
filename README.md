# In-Memory Cache Engine

A generic, policy-driven in-memory cache engine written in C++17.

## Features

- Generic `Cache<K, V>` template — works with any hashable key type and any value type
- Five eviction policies: LRU, LFU, FIFO, TTL, LRU+TTL
- Runtime policy switching without data loss
- Per-cache statistics (hit rate, evictions, insertions)
- Benchmark suite for comparing policies
- Named cache registry (CacheManager singleton)
- Config-driven defaults via `data/config.txt`

## Design Patterns

| Pattern   | Location                                      | Purpose                                      |
|-----------|-----------------------------------------------|----------------------------------------------|
| Strategy  | `EvictionPolicy<K>` + `Cache::switchPolicy()` | Swap eviction algorithm at runtime           |
| Factory   | `PolicyFactory<K>::create(name)`              | Create concrete policy objects by name       |
| Singleton | `CacheManager::getInstance()`                 | Single global registry for named caches      |

## Project Structure

```
include/   — all header files (.h)
src/       — all source files (.cpp)
data/      — configuration and workload files
build/     — CMake build output (gitignored)
```

## Configuration

Edit `data/config.txt` to set defaults:

```
capacity=100
default_ttl=60
policy=LRU
```

## Compilation

Requires CMake >= 3.14 and a C++17-capable compiler.

```bash
mkdir build && cd build
cmake ..
cmake --build .
./InMemoryCacheEngine
```

## Usage

On startup the application runs a template demo showing `Cache<int, string>` and `Cache<int, int>`, then launches an interactive menu where you can:

- Put / Get / Delete entries
- Switch eviction policy at runtime (Strategy + Factory patterns)
- Resize the cache
- View statistics
- Load entries from a file
- Run benchmarks comparing all policies
- Manage multiple named caches (Singleton pattern)
