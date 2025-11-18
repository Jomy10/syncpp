require "net/http"

Project(name: "sync++")

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

C::Library(
  name: "sync",
  language: :cpp,
  sources: "src/**/*.cpp",
  headers: "include",
  cflags: [
    "-std=c++#{cpp_version}"
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
