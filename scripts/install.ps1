# FlexonDB Installation Script for Windows
# Run this script as Administrator to install FlexonDB system-wide
# Or run as normal user to install to user directory

param(
    [string]$InstallDir = "",
    [switch]$UserInstall = $false,
    [switch]$LibraryInstall = $false,
    [switch]$Force = $false,
    [switch]$Help = $false
)

# Colors for console output
$Global:ErrorColor = "Red"
$Global:SuccessColor = "Green"
$Global:WarningColor = "Yellow"
$Global:InfoColor = "Cyan"

function Write-ColorText {
    param([string]$Text, [string]$Color = "White")
    Write-Host $Text -ForegroundColor $Color
}

function Show-Usage {
    Write-ColorText "FlexonDB Installation Script for Windows" $Global:InfoColor
    Write-Host ""
    Write-Host "Usage: .\install.ps1 [OPTIONS]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -InstallDir <path>     Custom installation directory"
    Write-Host "  -UserInstall           Install to user directory (no admin required)"
    Write-Host "  -LibraryInstall        Also install libraries and headers"
    Write-Host "  -Force                 Force installation without confirmation"
    Write-Host "  -Help                  Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\install.ps1                    # Install system-wide (requires admin)"
    Write-Host "  .\install.ps1 -UserInstall       # Install to user directory"
    Write-Host "  .\install.ps1 -LibraryInstall    # Install binaries and libraries"
    Write-Host "  .\install.ps1 -Help              # Show this help"
}

if ($Help) {
    Show-Usage
    exit 0
}

Write-ColorText "🚀 FlexonDB Installation Script for Windows" $Global:InfoColor
Write-ColorText "=============================================" $Global:InfoColor
Write-Host ""

# Check if we're in the FlexonDB project directory
if (-not (Test-Path "CMakeLists.txt") -or -not (Test-Path "src")) {
    Write-ColorText "❌ Error: This script must be run from the FlexonDB project root directory" $Global:ErrorColor
    exit 1
}

# Determine installation directories
if ($UserInstall) {
    $DefaultInstallDir = "$env:USERPROFILE\AppData\Local\Programs\FlexonDB"
    $DefaultLibDir = "$env:USERPROFILE\AppData\Local\Programs\FlexonDB\lib"
    $DefaultIncludeDir = "$env:USERPROFILE\AppData\Local\Programs\FlexonDB\include"
    $IsAdmin = $false
} else {
    $DefaultInstallDir = "$env:ProgramFiles\FlexonDB"
    $DefaultLibDir = "$env:ProgramFiles\FlexonDB\lib"
    $DefaultIncludeDir = "$env:ProgramFiles\FlexonDB\include"
    
    # Check if running as administrator
    $IsAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
    
    if (-not $IsAdmin) {
        Write-ColorText "❌ Error: System-wide installation requires administrator privileges" $Global:ErrorColor
        Write-ColorText "💡 Please run PowerShell as Administrator, or use -UserInstall for user installation" $Global:WarningColor
        exit 1
    }
}

if ($InstallDir -eq "") {
    $InstallDir = $DefaultInstallDir
}

$LibDir = "$InstallDir\lib"
$IncludeDir = "$InstallDir\include"

# Check if build exists
$DistDir = "dist\windows"
if (-not (Test-Path $DistDir)) {
    Write-ColorText "❌ Error: FlexonDB has not been built for Windows" $Global:ErrorColor
    Write-ColorText "💡 Please build first with:" $Global:WarningColor
    Write-Host "   mkdir build\windows"
    Write-Host "   cd build\windows"
    Write-Host "   cmake ..\..\"
    Write-Host "   cmake --build . --config Release"
    exit 1
}

# Check if main binary exists
$MainBinary = "$DistDir\bin\flexon.exe"
if (-not (Test-Path $MainBinary)) {
    Write-ColorText "❌ Error: FlexonDB binary not found at $MainBinary" $Global:ErrorColor
    Write-ColorText "💡 Please build FlexonDB first" $Global:WarningColor
    exit 1
}

Write-ColorText "✓ Found FlexonDB build for Windows" $Global:SuccessColor
Write-ColorText "✓ Installation directory: $InstallDir" $Global:SuccessColor

if ($LibraryInstall) {
    Write-ColorText "✓ Library directory: $LibDir" $Global:SuccessColor
    Write-ColorText "✓ Include directory: $IncludeDir" $Global:SuccessColor
}

Write-Host ""

# Confirmation prompt
if (-not $Force) {
    Write-ColorText "📦 Ready to install FlexonDB" $Global:WarningColor
    Write-Host ""
    Write-Host "Files to install:"
    Write-Host "  • flexon.exe          → $InstallDir\flexon.exe"
    
    if (Test-Path "$DistDir\bin\flexon-cli.exe") {
        Write-Host "  • flexon-cli.exe      → $InstallDir\flexon-cli.exe"
    }
    
    if (Test-Path "$DistDir\bin\flexon-shell.exe") {
        Write-Host "  • flexon-shell.exe    → $InstallDir\flexon-shell.exe"
    }
    
    if ($LibraryInstall) {
        Write-Host "  • Libraries           → $LibDir\"
        Write-Host "  • Headers             → $IncludeDir\"
    }
    
    Write-Host ""
    $confirmation = Read-Host "Continue with installation? [y/N]"
    if ($confirmation -notmatch "^[Yy]$") {
        Write-ColorText "📦 Installation cancelled" $Global:WarningColor
        exit 0
    }
}

Write-Host ""
Write-ColorText "🔧 Installing FlexonDB..." $Global:InfoColor

try {
    # Create installation directory
    if (-not (Test-Path $InstallDir)) {
        New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null
        Write-ColorText "  → Created directory: $InstallDir" $Global:SuccessColor
    }

    # Install main binary
    Write-ColorText "  → Installing flexon.exe..." $Global:SuccessColor
    Copy-Item "$DistDir\bin\flexon.exe" -Destination "$InstallDir\flexon.exe" -Force

    # Install additional binaries if they exist
    $AdditionalBinaries = @("flexon-cli.exe", "flexon-shell.exe")
    foreach ($Binary in $AdditionalBinaries) {
        $SourcePath = "$DistDir\bin\$Binary"
        if (Test-Path $SourcePath) {
            Write-ColorText "  → Installing $Binary..." $Global:SuccessColor
            Copy-Item $SourcePath -Destination "$InstallDir\$Binary" -Force
        }
    }

    # Install libraries and headers if requested
    if ($LibraryInstall) {
        if (Test-Path "$DistDir\lib") {
            Write-ColorText "  → Installing libraries..." $Global:SuccessColor
            if (-not (Test-Path $LibDir)) {
                New-Item -ItemType Directory -Force -Path $LibDir | Out-Null
            }
            Copy-Item "$DistDir\lib\*" -Destination $LibDir -Recurse -Force
        }
        
        if (Test-Path "include") {
            Write-ColorText "  → Installing headers..." $Global:SuccessColor
            if (-not (Test-Path $IncludeDir)) {
                New-Item -ItemType Directory -Force -Path $IncludeDir | Out-Null
            }
            Copy-Item "include\*" -Destination $IncludeDir -Recurse -Force
        }
    }

    # Add to PATH if not already there
    $PathToAdd = $InstallDir
    $CurrentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
    
    if ($UserInstall) {
        $Target = "User"
    } else {
        $Target = "Machine"
        $CurrentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
    }
    
    if ($CurrentPath -notlike "*$PathToAdd*") {
        Write-ColorText "  → Adding to PATH..." $Global:SuccessColor
        $NewPath = $CurrentPath + ";" + $PathToAdd
        [Environment]::SetEnvironmentVariable("PATH", $NewPath, $Target)
        
        # Update current session PATH
        $env:PATH = $NewPath
    }

    Write-Host ""
    Write-ColorText "✅ FlexonDB installed successfully!" $Global:SuccessColor
    Write-Host ""
    Write-ColorText "🎉 You can now run:" $Global:InfoColor
    Write-Host "   flexon --help"
    Write-Host ""
    
    if ($UserInstall) {
        Write-ColorText "💡 Note: Installed to user directory" $Global:WarningColor
        Write-Host "   Only available for current user: $env:USERNAME"
    } else {
        Write-ColorText "💡 Note: Installed system-wide" $Global:InfoColor
        Write-Host "   Available for all users on this system"
    }
    
    Write-Host ""
    Write-ColorText "📚 Documentation:" $Global:InfoColor
    Write-Host "   • README.md for usage examples"
    Write-Host "   • docs\ directory for detailed documentation"
    Write-Host ""
    Write-ColorText "🚀 Happy database management with FlexonDB!" $Global:SuccessColor

} catch {
    Write-ColorText "❌ Error during installation: $($_.Exception.Message)" $Global:ErrorColor
    exit 1
}