# ===== Win32 API =====
Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

public class Win32Keyboard {
    [DllImport("user32.dll")]
    public static extern short GetAsyncKeyState(int vKey);

    public static bool IsKeyDown(int vKey) {
        return (GetAsyncKeyState(vKey) & 0x8000) != 0;
    }
}
"@

# ===== Config Parameters =====
$portName = "COM6"  
$baudRate = 9600
$commandRefreshMs = 100  # Command refresh interval in milliseconds

# ===== Virtual Key Codes =====
$VK_W = 0x57
$VK_A = 0x41
$VK_S = 0x53
$VK_D = 0x44
$VK_T = 0x54
$VK_ESC = 0x1B

# ===== Init Serial Port =====
$port = [System.IO.Ports.SerialPort]::new(
    $portName,
    $baudRate,
    [System.IO.Ports.Parity]::None,
    8,
    [System.IO.Ports.StopBits]::One
)

try {
    $port.Open()
} catch {
    Write-Host "Unable to open ${portName}: $($_.Exception.Message)" -ForegroundColor Red
    exit
}

Write-Host "==========================================" -ForegroundColor Green
Write-Host "   WASD Game Control (Direct Keyboard)" -ForegroundColor Green
Write-Host "   W: Forward | S: Backward | A: Left | D: Right" -ForegroundColor Yellow
Write-Host "   W+D (E): Right Forward | W+A (Q): Left Forward" -ForegroundColor Yellow
Write-Host "   S+D (C): Right Backward | S+A (Z): Left Backward" -ForegroundColor Yellow
Write-Host "   Hold keys to move, release to stop | Press ESC to exit" -ForegroundColor Yellow
Write-Host "   Press T to toggle between auto and manual mode" -ForegroundColor Yellow
Write-Host "==========================================" -ForegroundColor Green
Write-Host "[Mode] Waiting for Arduino status..." -ForegroundColor DarkGray

$lastState = "x"
$lastCommandTime = [DateTime]::MinValue
$manualMode = $null
$tWasDown = $false
$serialBuffer = ""

try {
    while ($true) {
        if ([Win32Keyboard]::IsKeyDown($VK_ESC)) { break }

        if ($port.BytesToRead -gt 0) {
            $serialBuffer += $port.ReadExisting()
        }

        $newlineIndex = $serialBuffer.IndexOf("`n")
        while ($newlineIndex -ge 0) {
            $line = $serialBuffer.Substring(0, $newlineIndex).Trim()
            $serialBuffer = $serialBuffer.Substring($newlineIndex + 1)

            if ($line -match '^MODE:(AUTO|MANUAL)$') {
                $manualMode = $matches[1] -eq "MANUAL"
                $modeName = if ($manualMode) { "Manual" } else { "Auto" }
                $modeColor = if ($manualMode) { "Cyan" } else { "Green" }
                Write-Host "`n[Mode] $modeName" -ForegroundColor $modeColor
            } elseif ($line -eq "EVENT:AUTO_STOP_TIMEOUT") {
                Write-Host "`n[Notice] Auto mode stayed stopped for 5 seconds; switched to Manual mode." -ForegroundColor Yellow
            }

            $newlineIndex = $serialBuffer.IndexOf("`n")
        }

        $tIsDown = [Win32Keyboard]::IsKeyDown($VK_T)
        $modeChanged = $false

        if ($tIsDown -and -not $tWasDown) {
            $port.Write("t")
            $lastState = "x"
            $lastCommandTime = [DateTime]::MinValue
            $modeChanged = $true
        }
        $tWasDown = $tIsDown

        if ($manualMode -eq $true -and -not $modeChanged) {
            $w = [Win32Keyboard]::IsKeyDown($VK_W)
            $a = [Win32Keyboard]::IsKeyDown($VK_A)
            $s = [Win32Keyboard]::IsKeyDown($VK_S)
            $d = [Win32Keyboard]::IsKeyDown($VK_D)

            # Combo key priority: W+D, W+A, S+D, S+A
            $cmd = "x"
            if ($w -and $d)     { $cmd = "e" } # 右前 (W+D)
            elseif ($w -and $a) { $cmd = "q" } # 左前 (W+A)
            elseif ($s -and $d) { $cmd = "c" } # 右后 (S+D)
            elseif ($s -and $a) { $cmd = "z" } # 左后 (S+A)
            elseif ($w)         { $cmd = "w" } # 前
            elseif ($s)         { $cmd = "s" } # 后
            elseif ($a)         { $cmd = "a" } # 左平移
            elseif ($d)         { $cmd = "d" } # 右平移

            $now = [DateTime]::Now
            $elapsed = ($now - $lastCommandTime).TotalMilliseconds

            $stateChanged = $cmd -ne $lastState
            if ($stateChanged -or ($cmd -ne "x" -and $elapsed -ge $commandRefreshMs)) {
                $port.Write($cmd)
                $lastState = $cmd
                $lastCommandTime = $now

                if ($stateChanged) {
                    if ($cmd -eq "x") {
                        Write-Host "[Control] Stopped" -ForegroundColor Gray
                    } else {
                        Write-Host "[Control] Moving: $cmd" -ForegroundColor Cyan
                    }
                }
            }
        }

        Start-Sleep -Milliseconds 15
    }
} finally {
    if ($port.IsOpen) {
        $port.Write("x")
        $port.Close()
    }
    Write-Host "`nSerial port closed safely, exiting program." -ForegroundColor Yellow
}