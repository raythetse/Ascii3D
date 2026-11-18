# Packages a Release x64 build into dist\Ascii3D-<version>-win-x64.zip, with every DLL it needs
# on a machine without Visual Studio or the Vulkan SDK. CI runs it; it also runs locally.
#
#   powershell -ExecutionPolicy Bypass -File Packaging\package.ps1
#
# Needs: the built x64\Release\Ascii3D.exe, VULKAN_SDK set, and Visual Studio (for dumpbin and the
# C++ runtime DLLs). Works in Windows PowerShell 5.1 and PowerShell 7.

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent

# The version comes from Version.h, the one place it is kept
$versionH = Get-Content (Join-Path $root 'Version.h') -Raw
$parts = foreach ($name in 'MAJOR', 'MINOR', 'PATCH') {
    if ($versionH -notmatch "#define ASCII3D_VERSION_$name (\d+)") { throw "Version.h has no ASCII3D_VERSION_$name" }
    $Matches[1]
}
$version = $parts -join '.'
$packageName = "Ascii3D-$version-win-x64"

$exe = Join-Path $root 'x64\Release\Ascii3D.exe'
if (-not (Test-Path $exe)) { throw "Build Release x64 first: $exe not found" }
if (-not $env:VULKAN_SDK) { throw 'VULKAN_SDK is not set' }

# Visual Studio's dumpbin lists a DLL's imports, and its redist folder has the C++ runtime DLLs
# that Microsoft allows apps to ship next to their exe ("app-local" deployment)
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products * -property installationPath
$dumpbin = & $vswhere -latest -products * -find 'VC\Tools\MSVC\**\bin\Hostx64\x64\dumpbin.exe' | Select-Object -First 1
$redistVersion = (Get-Content "$vs\VC\Auxiliary\Build\Microsoft.VCRedistVersion.default.txt").Trim()
$crtDir = Get-ChildItem "$vs\VC\Redist\MSVC\$redistVersion\x64" -Directory -Filter 'Microsoft.VC*.CRT' | Select-Object -First 1
if (-not $crtDir) { throw "No C++ runtime folder under $vs\VC\Redist\MSVC\$redistVersion\x64" }

# Where each DLL the package needs comes from. Anything else must be part of Windows itself.
$bundled = @{ 'shaderc_shared.dll' = "$env:VULKAN_SDK\Bin\shaderc_shared.dll" }
foreach ($dll in Get-ChildItem $crtDir.FullName -Filter '*.dll') { $bundled[$dll.Name.ToLower()] = $dll.FullName }
# vulkan-1.dll is the Vulkan loader. It comes with the GPU driver, and must match it, so it's not shipped.
$fromDriver = @('vulkan-1.dll')

function Get-Imports([string]$path) {
    $out = & $dumpbin /nologo /dependents $path
    if ($LASTEXITCODE -ne 0) { throw "dumpbin failed on $path" }
    # Import lines are indented DLL names; the delay-load list counts too
    $out | Where-Object { $_ -match '^\s+(\S+\.dll)$' } | ForEach-Object { $Matches[1].ToLower() }
}

$dist = Join-Path $root 'dist'
$stage = Join-Path $dist $packageName
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
New-Item -ItemType Directory $stage | Out-Null

# Follow the imports from the exe through every DLL that gets bundled
Copy-Item $exe $stage
$queue = [System.Collections.Generic.Queue[string]]::new()
$queue.Enqueue($exe)
$seen = @{}
while ($queue.Count -gt 0) {
    foreach ($dll in Get-Imports $queue.Dequeue()) {
        if ($seen[$dll]) { continue }
        $seen[$dll] = $true
        if ($bundled.ContainsKey($dll)) {
            Copy-Item $bundled[$dll] $stage
            $queue.Enqueue($bundled[$dll])
            Write-Host "  bundled  $dll"
        }
        elseif ($fromDriver -contains $dll) {
            Write-Host "  driver   $dll"
        }
        # api-ms-win-crt-*.dll is the Universal CRT, part of Windows 10 and later
        elseif ($dll -like 'api-ms-win-*' -or (Test-Path "$env:SystemRoot\System32\$dll")) {
            Write-Host "  windows  $dll"
        }
        else {
            throw "$dll is needed but isn't bundled or part of Windows"
        }
    }
}

# Scenes, minus the scratch files: Test.scn, and CustomScene.scn, which CTRL+M overwrites
$scenes = New-Item -ItemType Directory (Join-Path $stage 'CustomScenes')
Get-ChildItem (Join-Path $root 'CustomScenes') -Filter '*.scn' |
    Where-Object { $_.Name -notin 'Test.scn', 'CustomScene.scn' } |
    Copy-Item -Destination $scenes

# Docs. LICENSE gets a .txt extension so it opens in Notepad on double-click.
Copy-Item (Join-Path $root 'HOWTO.txt') $stage
Copy-Item (Join-Path $root 'LICENSE') (Join-Path $stage 'LICENSE.txt')
Copy-Item (Join-Path $PSScriptRoot 'README.txt') $stage
Copy-Item (Join-Path $PSScriptRoot 'THIRD-PARTY-NOTICES.txt') $stage
Copy-Item (Join-Path $PSScriptRoot 'licenses') $stage -Recurse

# Windows' own tar.exe (bsdtar) writes zip entries with forward slashes, which every unzip tool
# reads. Windows PowerShell 5.1's Compress-Archive writes backslashes.
$zip = Join-Path $dist "$packageName.zip"
if (Test-Path $zip) { Remove-Item $zip }
& "$env:SystemRoot\System32\tar.exe" -a -c -f $zip -C $dist $packageName
if ($LASTEXITCODE -ne 0) { throw 'tar failed' }

Write-Host "Packaged $zip"
# In GitHub Actions, hand the zip's path to the steps that follow
if ($env:GITHUB_OUTPUT) { "zip=$zip", "zip-name=$packageName.zip" | Out-File $env:GITHUB_OUTPUT -Append -Encoding utf8 }
