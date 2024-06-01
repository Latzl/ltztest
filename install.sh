#/bin/env bash

target_dir='/usr/local/include'

echo "start copy ltz header files to ${target_dir}"
cp -rvf ./third_party/ltz/ ${target_dir}/
echo "start copy tcli header files to ${target_dir}"
cp -rvf ./tcli/include/ ${target_dir}/tcli/