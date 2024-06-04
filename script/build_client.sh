BUILD_TYPE=$1
preset=$(echo "$BUILD_TYPE" | awk '{print "conan-" tolower($0)}')


# echo "【KIE】Install Conan"
# sudo apt update
# sudo apt install -y python3 python3-pip lcov curl
# pip install conan

# echo "【KIE】Configure Conan"
# conan profile detect

echo "【KIE】Install Dependency"
conan install . --build=missing -s build_type=${BUILD_TYPE}

echo "【KIE】Configure CMake"
cmake . --preset ${preset} -DENABLE_CLIENT=ON

echo "【KIE】Build"
cmake --build --preset ${preset}