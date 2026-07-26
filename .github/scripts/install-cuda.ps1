param(
	[Parameter(Mandatory = $true)]
	[string] $Version
)

$ErrorActionPreference = "Stop"

$arch = [System.Runtime.InteropServices.RuntimeInformation]::ProcessArchitecture.ToString().ToLowerInvariant()

if ($Version -eq "12.9") {
	$url = "https://developer.download.nvidia.com/compute/cuda/12.9.0/network_installers/cuda_12.9.0_windows_network.exe"
} elseif ($Version -eq "13.4" -and $arch -eq "arm64") {
	$url = "https://packages.nvidia.com/prerelease/cuda/13.4.0/local_installers/cuda_13.4.0_windows_arm64.exe"
} elseif ($Version -eq "13.4") {
	$url = "https://packages.nvidia.com/prerelease/cuda/13.4.0/local_installers/cuda_13.4.0_windows_x86_64.exe"
} else {
	throw "Unsupported CUDA version: $Version"
}

$cudaPath = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v$Version"
$installer = "cuda-$Version-$arch.exe"
$logDir = Join-Path (Resolve-Path ".") "cuda-install-$Version-$arch"

function Get-CudaComponents {
	param(
		[Parameter(Mandatory = $true)]
		[string] $CudaVersion
	)

	if ($CudaVersion -eq "12.9") {
		return @(
			"cudart_12.9",
			"nvcc_12.9",
			"nvrtc_12.9",
			"nvrtc_dev_12.9",
			"nvvm_12.9",
			"thrust_12.9"
		)
	}

	if ($CudaVersion -eq "13.4") {
		return @(
			"crt_13.4",
			"cudart_13.4",
			"nvcc_13.4",
			"nvrtc_13.4",
			"nvrtc_dev_13.4",
			"nvvm_13.4",
			"thrust_13.4"
		)
	}

	throw "Unsupported CUDA component version: $CudaVersion"
}

function Show-InstallerLogs {
	param(
		[Parameter(Mandatory = $true)]
		[string] $Path,
		[int] $Tail = 80
	)

	if (!(Test-Path $Path)) {
		Write-Host "CUDA installer log path does not exist yet: $Path"
		return
	}

	$item = Get-Item $Path
	if (!$item.PSIsContainer) {
		Write-Host "CUDA installer log: $Path"
		Get-Content $Path -Tail $Tail
		return
	}

	$logs = Get-ChildItem $Path -File -Recurse | Sort-Object LastWriteTime -Descending
	if (!$logs) {
		Write-Host "CUDA installer log directory is empty: $Path"
		return
	}

	foreach ($log in ($logs | Select-Object -First 3)) {
		Write-Host "CUDA installer log: $($log.FullName)"
		Get-Content $log.FullName -Tail $Tail
	}
}

Write-Host "Downloading CUDA $Version from $url"
Invoke-WebRequest -Uri $url -OutFile $installer
$installerInfo = Get-Item $installer
Write-Host "Downloaded $($installerInfo.FullName) ($($installerInfo.Length) bytes)"

if (Test-Path $logDir) {
	Remove-Item $logDir -Recurse -Force
}
New-Item -ItemType Directory -Path $logDir | Out-Null

Write-Host "Installing CUDA $Version"
$argumentList = @("-s", "-loglevel:6", "-log:$logDir")
$components = Get-CudaComponents -CudaVersion $Version

Write-Host "Using selective CUDA Toolkit packages: $($components -join ', ')"
$argumentList += $components

Write-Host "Installer arguments: $($argumentList -join ' ')"
$process = Start-Process -FilePath (Resolve-Path $installer) -ArgumentList $argumentList -PassThru
$timeout = [TimeSpan]::FromMinutes(20)
$start = Get-Date

while (!$process.WaitForExit(60000)) {
	$elapsed = (Get-Date) - $start
	Write-Host "CUDA installer is still running after $([int] $elapsed.TotalMinutes) minutes."
	Show-InstallerLogs -Path $logDir -Tail 25

	if ($elapsed -ge $timeout) {
		Write-Host "CUDA installer timed out after $($timeout.TotalMinutes) minutes."
		Show-InstallerLogs -Path $logDir -Tail 200
		Stop-Process -Id $process.Id -Force
		throw "CUDA installer timed out"
	}
}

$elapsed = (Get-Date) - $start
Write-Host "CUDA installer exited after $([int] $elapsed.TotalMinutes) minutes with code $($process.ExitCode)."

Show-InstallerLogs -Path $logDir -Tail 120

if ($process.ExitCode -ne 0) {
	throw "CUDA installer failed with exit code $($process.ExitCode)"
}

if (!(Test-Path "$cudaPath\bin\nvcc.exe")) {
	throw "CUDA install did not create $cudaPath\bin\nvcc.exe"
}

"CUDA_PATH=$cudaPath" >> $env:GITHUB_ENV
"$cudaPath\bin" >> $env:GITHUB_PATH
$env:CUDA_PATH = $cudaPath
$env:Path = "$cudaPath\bin;$env:Path"

nvcc --version
