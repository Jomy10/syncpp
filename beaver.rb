require "net/http"

Project(name: "syncpp")

cpp_version = opt("cpp-version", default: 23)

with_test = !flag("no-test", default: false)

pre "build" do
  if with_test
    Dir.mkdir "deps" unless Dir.exist? "deps"

    # GitHub is down
    unless Dir.exist? "deps/bandit"
      sh "curl -L https://github.com/banditcpp/bandit/archive/refs/heads/main.zip -o deps/main.zip"
      sh "unzip deps/main.zip -d deps"
      sh "mv deps/bandit-main deps/bandit"
      File.delete "deps/main.zip"

      sh "curl -L https://github.com/banditcpp/snowhouse/archive/89ac7cd0baf2c411671a3169b9364acb1e5cddfd.zip -o deps/snowhouse.zip"
      sh "unzip deps/snowhouse.zip -d deps/bandit/bandit/assertion_frameworks/"
      sh "rm -r deps/bandit/bandit/assertion_frameworks/snowhouse"
      sh "mv deps/bandit/bandit/assertion_frameworks/snowhouse* deps/bandit/bandit/assertion_frameworks/snowhouse"
      File.delete "deps/snowhouse.zip"
    end

    # sh "git clone --recursive git@github.com:banditcpp/bandit.git deps/bandit"
  end
end

# RwLock implemented with C++17 shared mutex. This does not allow for multiple read locks on the same thread
rwlock_is_shared_mutex = flag("rwlock-is-shared-mutex", default: false)
if rwlock_is_shared_mutex
  STDERR << "[WARNING] option rwlock-is-shared-mutex is currently not working properly (test cases are failing). Pull requests are welcome\n"
end
throw "Shared mutex requires c++ >= 17" if rwlock_is_shared_mutex && cpp_version < 17

# RwLock implemented with pthread
rwlock_is_pthread = flag("rwlock-is-pthread", default: TARGET.posix?)

lflags = [
  rwlock_is_pthread ? "-lpthread" : nil
].reject { |v| v.nil? }

defines = [
  rwlock_is_shared_mutex ? "-DSYNC_RWLOCK_IS_SHARED_MUTEX" : nil,
  rwlock_is_pthread ? "-DSYNC_RWLOCK_IS_PTHREAD" : nil,
  # Allow extensions for RwLock that are not compliant with other implementations
  flag("rwlock-extension", default: false) ? "-DSYNC_RWLOCK_EXTENSION" : nil,
  # RwLock is implemented using pthread
].reject { |v| v.nil? }

if cpp_version >= 11
  defines << "-DSYNC_HAVE_SYSTEM_ERROR"
end

if cpp_version >= 17
  defines << "-DSYNC_HAVE_OPTIONAL"
end

C::Library(
  name: "sync",
  language: :cpp,
  sources: "src/**/*.cpp",
  headers: "include",
  cflags: [
    "-std=c++#{cpp_version}",
    *defines
  ],
)

if with_test
  C::Executable(
    name: "test",
    language: :cpp,
    sources: "test/**/*.cpp",
    headers: {
      private: "deps/bandit"
    },
    lflags: lflags,
    dependencies: [
      "sync"
    ],
  )
end


cmd "test" do
  project("sync++")
    .target("test")
    .run([])
end
