# Setup MSYS2 environment and install required packages
$msysPath = "C:\msys64"

if (-not (Test-Path $msysPath)) {
    Write-Host "MSYS2 not found at C:\msys64. Please install from https://www.msys2.org/"
    exit 1
}

# Run pacman commands in MSYS2 environment
Write-Host "Updating package database..."
& "$msysPath\msys2_shell.cmd" -defterm -no-start -c "pacman -Sy"

Write-Host "Installing GCC compiler..."
& "$msysPath\msys2_shell.cmd" -defterm -no-start -c "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gcc"

Write-Host "Installing GTest..."
& "$msysPath\msys2_shell.cmd" -defterm -no-start -c "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gtest"

Write-Host "Setup complete! Compiler available at: C:\msys64\ucrt64\bin\g++.exe"