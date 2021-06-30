```
g++ -c main.cpp hello.cpp
g++ -o a.out main.o hello.o
```

```
g++ main.cpp hello.cpp
```

```
mkdir build
cd build
cmake ..
make
```

```
g++ -c hello.cpp good_morning.cpp
ar rvs libgreetings.a hello.o good_morning.o
g++ main.cpp libgreetings.a
```

```
g++ -fPIC -c hello.cpp good_morning.cpp
g++ -shared hello.o good_morning.o -o libgreetings.so
g++ main.cpp -L. -lgreetings -Xlinker -rpath -Xlinker
```

```
cmake -DBUILD_SHARED_LIBS=ON ..
cmake -DBUILD_SHARED_LIBS=OFF ..
```

```
cmake -DGREETINGS_BUILD_SHARED_LIBS=ON ..
cmake -DGREETINGS_BUILD_SHARED_LIBS=OFF ..
```

```
cd src
g++ -fPIC -c hello.cpp good_morning.cpp -I../include
g++ -shared *.o -o libgreetings.so
cd ../test
g++ main.cpp -I../include -L../src -lgreetings -Xlinker -rpath -Xlinker ../src

```
