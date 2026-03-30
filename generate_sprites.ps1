$lines = @()
$lines += "#pragma once"
$lines += ""

$pngs = Get-ChildItem -Path "resources" -Filter "*.png" | Sort-Object Name

foreach ($png in $pngs) {
    $path  = $png.FullName
    $id    = $png.BaseName
    $bytes = [System.IO.File]::ReadAllBytes($path)
    $hex   = ($bytes | ForEach-Object { "0x{0:X2}" -f $_ }) -join ","
    $lines += "static const unsigned char k_spr_${id}[] = {$hex};"
    $lines += "static const int           k_spr_${id}_size = $($bytes.Count);"
    $lines += ""
    Write-Host "Embedded $($png.Name) ($($bytes.Count) bytes)"
}

[System.IO.File]::WriteAllLines("src\sprites_data.hpp", $lines)
Write-Host "Done -> src\sprites_data.hpp"
