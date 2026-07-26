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
$logPath = Resolve-Path "."
$logFile = Join-Path $logPath "cuda-install-$Version-$arch.log"

Write-Host "Downloading CUDA $Version from $url"
Invoke-WebRequest -Uri $url -OutFile $installer
$installerInfo = Get-Item $installer
Write-Host "Downloaded $($installerInfo.FullName) ($($installerInfo.Length) bytes)"

Write-Host "Installing CUDA $Version"
$argumentList = @("-s", "-loglevel:6", "-log:$logFile")
Write-Host "Installer arguments: $($argumentList -join ' ')"
$process = Start-Process -FilePath (Resolve-Path $installer) -ArgumentList $argumentList -PassThru
$timeout = [TimeSpan]::FromMinutes(25)

if (!$process.WaitForExit([int] $timeout.TotalMilliseconds)) {
	Write-Host "CUDA installer timed out after $($timeout.TotalMinutes) minutes."
	if (Test-Path $logFile) {
		Write-Host "Last CUDA installer log lines:"
		Get-Content $logFile -Tail 200
	}
	Stop-Process -Id $process.Id -Force
	throw "CUDA installer timed out"
}

if (Test-Path $logFile) {
	Write-Host "Last CUDA installer log lines:"
	Get-Content $logFile -Tail 120
}

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
