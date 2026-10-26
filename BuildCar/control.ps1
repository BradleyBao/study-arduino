# ===== Config Parameters =====
$portName = "COM6"  
$baudRate = 9600
$keyReleaseTimeoutMs = 600
$commandRefreshMs = 100

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
Write-Host "   WASD" -ForegroundColor Green
Write-Host "   W: Forward | A: Left | S: Backward | D: Right" -ForegroundColor Yellow
Write-Host "   Q: Left Forward | E: Right Forward" -ForegroundColor Yellow
Write-Host "   Hold key to move continuously, release to stop | Press Esc to exit" -ForegroundColor Yellow
Write-Host "==========================================" -ForegroundColor Green

$lastState = "stop"
$lastPressTime = [DateTime]::Now
$lastCommandTime = [DateTime]::MinValue

try {
    while ($true) {
        if ([Console]::KeyAvailable) {
            $keyInfo = [Console]::ReadKey($true)
            
            if ($keyInfo.Key -eq [ConsoleKey]::Escape) { break }

            $key = $keyInfo.KeyChar.ToString().ToLowerInvariant()
            if ($key -in @("w", "a", "s", "d", "q", "e")) {
                $lastPressTime = [DateTime]::Now
                
                if ($lastState -ne $key) {
                    $port.Write($key)
                    $lastState = $key
                    $lastCommandTime = [DateTime]::Now
                    Write-Host "`r[Status] Moving: $key   " -NoNewline -ForegroundColor Cyan
                }
            }
        }

        if ($lastState -ne "stop") {
            $now = [DateTime]::Now
            $elapsedSinceKey = ($now - $lastPressTime).TotalMilliseconds

            if ($elapsedSinceKey -gt $keyReleaseTimeoutMs) {
                $port.Write("x")
                $lastState = "stop"
                Write-Host "`r[Status] Stopped           " -NoNewline -ForegroundColor Gray
            } elseif (($now - $lastCommandTime).TotalMilliseconds -ge $commandRefreshMs) {
                $port.Write($lastState)
                $lastCommandTime = $now
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