#!/usr/bin/env bash
set -euo pipefail

version="${1:?CUDA version is required}"

if [[ "$version" != "12.9" ]]; then
	echo "Unsupported CUDA version for Linux CI: $version" >&2
	exit 1
fi

. /etc/os-release

ubuntu_version="${VERSION_ID//./}"
machine="$(uname -m)"

case "$machine" in
	x86_64)
		cuda_arch="x86_64"
		;;
	aarch64)
		cuda_arch="sbsa"
		;;
	*)
		echo "Unsupported CUDA Linux architecture: $machine" >&2
		exit 1
		;;
esac

repo_url="https://developer.download.nvidia.com/compute/cuda/repos/ubuntu${ubuntu_version}/${cuda_arch}"
toolkit_package="cuda-toolkit-${version/./-}"
cuda_path="/usr/local/cuda-${version}"

wget -q "${repo_url}/cuda-ubuntu${ubuntu_version}.pin"
sudo mv "cuda-ubuntu${ubuntu_version}.pin" /etc/apt/preferences.d/cuda-repository-pin-600
wget -q "${repo_url}/cuda-keyring_1.1-1_all.deb"
sudo dpkg -i cuda-keyring_1.1-1_all.deb
sudo apt-get update -qq
sudo apt-get install -y "${toolkit_package}"

if [[ ! -x "${cuda_path}/bin/nvcc" ]]; then
	echo "CUDA install did not create ${cuda_path}/bin/nvcc" >&2
	exit 1
fi

{
	echo "CUDA_PATH=${cuda_path}"
	echo "CUDA_HOME=${cuda_path}"
} >> "${GITHUB_ENV}"

echo "${cuda_path}/bin" >> "${GITHUB_PATH}"
export CUDA_PATH="${cuda_path}"
export CUDA_HOME="${cuda_path}"
export PATH="${cuda_path}/bin:${PATH}"

nvcc --version
