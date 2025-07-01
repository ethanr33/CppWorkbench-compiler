
if ! [ -d build ]; then
  mkdir build
fi

cd build
cmake ..
make
cd ..

build_exec=compiler
test_exec=test_suites

if ! [ -f $build_exec ]; then
  ln -s build/compiler $build_exec
fi

if ! [ -f $test_exec ]; then
  ln -s build/tests $test_exec
fi