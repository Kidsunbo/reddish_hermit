WORKSPACE=$1
BUILD_TYPE=$2
declare -A PresetMap
PresetMap["Debug"] = "debug"
PresetMap["Release"] = "release"


echo "【KIE】Create Build Environment"
cmake -E make_directory ${WORKSPACE}/build


echo "【KIE】Install Conan"
sudo apt update
sudo apt install -y python3 python3-pip lcov curl
pip install conan


echo "【KIE】Configure Conan"
conan profile new default --detect &&
conan profile update settings.compiler.libcxx=libstdc++11 default &&
conan profile update settings.build_type=${BUILD_TYPE} default &&
conan remote add kie-kies https://conan.cloudsmith.io/kie/kies/

echo "【KIE】Install Dependency"
cd ${WORKSPACE}/build
conan install .. --build=missing -pr:b=default -pr:h=default

echo "【KIE】Configure CMake"
cd ${WORKSPACE}/build
cmake ${WORKSPACE} --preset ${PresetMap[$BUILD_TYPE]}

echo "【KIE】Build"
cd ${WORKSPACE}/build
cmake --build . --config ${BUILD_TYPE}