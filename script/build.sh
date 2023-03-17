WORKSPACE=$1
BUILD_TYPE=$2
preset=$(echo "$BUILD_TYPE" | awk 'conan_{print tolower($0)}')


echo "【KIE】Create Build Environment"
cmake -E make_directory ${WORKSPACE}/build


echo "【KIE】Install Conan"
sudo apt update
sudo apt install -y python3 python3-pip lcov curl
pip install conan


echo "【KIE】Configure Conan"
conan profile detect

echo "【KIE】Check CMake Version"
cmake --version

echo "【KIE】Install Dependency"
conan install . --build=missing -s build_type=${BUILD_TYPE}

echo "【KIE】Configure CMake"
cmake . --preset ${preset}

echo "【KIE】Build"
cmake --build --preset ${preset}