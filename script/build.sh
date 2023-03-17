WORKSPACE=$1
BUILD_TYPE=$2
preset=$(echo "$BUILD_TYPE" | awk '{print tolower($0)}')


echo "【KIE】Create Build Environment"
cmake -E make_directory ${WORKSPACE}/build


echo "【KIE】Install Conan"
sudo apt update
sudo apt install -y python3 python3-pip lcov curl
pip install conan


echo "【KIE】Configure Conan"
conan profile detect

echo "【KIE】Install Dependency"
cd ${WORKSPACE}/build
conan install .. --build=missing -s build_type=${BUILD_TYPE}

echo "【KIE】Configure CMake"
cd ${WORKSPACE}/build
cmake ${WORKSPACE} --preset ${preset}

echo "【KIE】Build"
cd ${WORKSPACE}
cmake --build --preset ${preset}